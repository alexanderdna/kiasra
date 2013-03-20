#include "kenv.hpp"

#include "kconfig.hpp"
#include "krt.h"
#include "kopcodes.h"
#include "kmeta.hpp"
#include "kgc.hpp"
#include "kframe.hpp"
#include "kobject.hpp"
#include "ktypetree.hpp"
#include "kexchandler.hpp"
#include "kstringutils.hpp"
#include "kmodule.hpp"

#include <set>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cwchar>

#ifndef ISWIN
#include <unistd.h>
#endif

//===================================================
// Defines

#define INIT_STACK_SIZE 100

#define GC_ALLOC(len)		(KEnvironment::gc->alloc(len))
#define GC_REGISTER(p)		KEnvironment::gc->addRoot(p)
#define GC_UNREGISTER(p)	KEnvironment::gc->removeRoot(p)

//===================================================
// Static fields

bool KEnvironment::isInitialized = false;
bool KEnvironment::isForExecution = false;

kstring_t KEnvironment::systemLibPath;

TypeTree *KEnvironment::typeTree;

const TypeDef **KEnvironment::primitiveTypes;
const TypeDef  *KEnvironment::voidType;
const TypeDef  *KEnvironment::arrayType;
const TypeDef  *KEnvironment::objectType;
const TypeDef  *KEnvironment::rawType;
const TypeDef  *KEnvironment::excType;
const TypeDef  *KEnvironment::nullType;

KGC *KEnvironment::gc;

ModuleLoader             *KEnvironment::rootModule;
std::set<ModuleLoader *> *KEnvironment::loadedModules;
ModuleLoader             *KEnvironment::corlibModule;

KObject *KEnvironment::stack;
knuint_t KEnvironment::stackSize;
knuint_t KEnvironment::stackPointer;

callstack_t     *KEnvironment::callStack;
CallFrame       *KEnvironment::frame;         // current call frame
ModuleDef       *KEnvironment::module;        // current method's module
const MethodDef *KEnvironment::method;        // current method
KObject         *KEnvironment::locals;        // current local variables
KObject         *KEnvironment::args;          // current arguments
kstring_t       *KEnvironment::strings;	      // current string table
kuint_t         *KEnvironment::stringLengths; // length of constant strings

const unsigned char *KEnvironment::code;	  // code to execute
knuint_t             KEnvironment::ip;        // pointer to next instruction

bool KEnvironment::running;
bool KEnvironment::hasException;

KObject                    *KEnvironment::exc;        // latest unhandled exception
catchstack_t               *KEnvironment::catchStack; // stack of exception handlers
KEnvironment::tracedeque_t *KEnvironment::traceDeque; // deque of methods traced from exception

KEnvironment::Exceptions    KEnvironment::exceptions; // bundle of common exceptions

//===================================================

//public static
void KEnvironment::initialize(bool isForExecution)
{
	if (KEnvironment::isInitialized)
	{
		if (isForExecution != KEnvironment::isForExecution)
		{
			// not the same reason => re-initialize
			KEnvironment::finalize();
			KEnvironment::initialize(isForExecution);
		}
		return;
	}

	KEnvironment::rootModule = NULL;
	KEnvironment::stackSize = INIT_STACK_SIZE;
	KEnvironment::stackPointer = 0;
	KEnvironment::frame = NULL;
	KEnvironment::module = NULL;
	KEnvironment::method = NULL;
	KEnvironment::locals = NULL;
	KEnvironment::args = NULL;
	KEnvironment::code = NULL;
	KEnvironment::ip = 0;
	KEnvironment::exc = NULL;

	//===================================

	KEnvironment::initSystemLibPath();

	//===================================
	// prepare executors

	if (isForExecution)
	{
		for (knuint_t i = 0; i < OP_OPCODE_COUNT; ++i)
			KEnvironment::executors[i] = KEnvironment::defaultExecutors[i];

		for (knuint_t i = OP_OPCODE_COUNT; i < 256; ++i)
			KEnvironment::executors[i] = KEnvironment::do_InvalidOpcode;
	}

	//===================================
	// prepare basic types

	TypeTree *typeTree = new TypeTree;
	KEnvironment::typeTree = typeTree;

	const TypeDef **primitiveTypes = new const TypeDef * [(knuint_t)KT_STRING + 1];
	KEnvironment::primitiveTypes = primitiveTypes;

	for (knuint_t i = 0; i <= (knuint_t)KT_STRING; ++i)
		primitiveTypes[i] = typeTree->add((KTYPETAG)i, 0, NULL);

	KEnvironment::voidType = typeTree->add(KT_VOID, 0, NULL);
	KEnvironment::arrayType = typeTree->add(KT_ARRAY, 0, NULL);
	KEnvironment::objectType = typeTree->add(KT_OBJECT, 0, NULL);
	KEnvironment::rawType = typeTree->add(KT_RAW, 0, NULL);
	
	KEnvironment::nullType = new TypeDef; // nothing special, just a unique reference

	//===================================
	// prepare module loaders

	std::set<ModuleLoader *> *loadedModules = new std::set<ModuleLoader *>;
	KEnvironment::loadedModules = loadedModules;

	//===================================
	// prepare GC

	if (isForExecution)
	{
		KEnvironment::gc = new KGC;

		KEnvironment::stack = KEnvironment::gc->allocForStack(INIT_STACK_SIZE);
		GC_REGISTER(KEnvironment::stack);

		// element 0 is unused
		KEnvironment::stack[0] = KObject::nullObject;
	}
	else
	{
		KEnvironment::gc = NULL;
	}

	//===================================
	// prepare other stacks

	if (isForExecution)
	{
		KEnvironment::callStack = new callstack_t;
		KEnvironment::catchStack = new catchstack_t;
		KEnvironment::traceDeque = new tracedeque_t;
	}
	else
	{
		KEnvironment::callStack = NULL;
		KEnvironment::catchStack = NULL;
		KEnvironment::traceDeque = NULL;
	}

	//===================================
	// misc

	KObject::objectType = KEnvironment::objectType;
	KObject::nullType = KEnvironment::nullType;
	KObject::stringType = KEnvironment::primitiveTypes[KT_STRING];

	KObject::nullObject.type = KObject::nullType;
	KObject::nullObject.setRef(NULL, 0);
	
	//===================================

	KEnvironment::isInitialized = true;
	KEnvironment::isForExecution = isForExecution;
}

//public static
void KEnvironment::finalize(void)
{
	if (!KEnvironment::isInitialized)
		return;

	if (KEnvironment::loadedModules)
	{
		for (std::set<ModuleLoader *>::iterator it = KEnvironment::loadedModules->begin();
			it != KEnvironment::loadedModules->end(); ++it)
		{
			ModuleLoader *moduleLoader = *it;
			moduleLoader->onDispose();
			delete moduleLoader;
		}

		delete KEnvironment::loadedModules;
		KEnvironment::loadedModules = NULL;
	}

#define DELETE_IF_NOT_NULL(p) if (p) { delete p; p = NULL; }
#define ADELETE_IF_NOT_NULL(p) if (p) { delete []p; p = NULL; }

	DELETE_IF_NOT_NULL(KEnvironment::typeTree);
	ADELETE_IF_NOT_NULL(KEnvironment::primitiveTypes);
	DELETE_IF_NOT_NULL(KEnvironment::nullType);
	DELETE_IF_NOT_NULL(KEnvironment::callStack);
	DELETE_IF_NOT_NULL(KEnvironment::catchStack);
	DELETE_IF_NOT_NULL(KEnvironment::traceDeque);
	DELETE_IF_NOT_NULL(KEnvironment::gc);
	ADELETE_IF_NOT_NULL(KEnvironment::systemLibPath);

	KEnvironment::isInitialized = false;
}

//===================================================

//protected static
void KEnvironment::initSystemLibPath(void)
{
#ifdef ISWIN
	{
		LPSTR buffer = (LPSTR)malloc(1024 * sizeof(CHAR));
		DWORD result = GetModuleFileNameA(NULL, buffer, 1024);
		if (result == ERROR_INSUFFICIENT_BUFFER)
		{
			buffer[1023] = 0;
		}

		CHAR * lastSlash = strrchr(buffer, '\\');
		knuint_t len = lastSlash - buffer + 1; //we will take the slash too

		kchar_t *s = new kchar_t[len + 1];
		for (knuint_t i = 0; i < len; ++i)
			s[i] = buffer[i];
		s[len] = 0;
		KEnvironment::systemLibPath = s;
	}
#else
	{
		char *buffer = (char *)malloc(1024 * sizeof(char));
		ssize_t _len = readlink("/proc/self/exe", buffer, 1023);
		if (_len != -1)
			buffer[_len] = 0;
		else
			buffer[0] = 0;

		char * lastSlash = strrchr(buffer, '/');
		knuint_t len = lastSlash - buffer + 1;

		kchar_t *s = new kchar_t[len + 1];
		for (knuint_t i = 0; i < len; ++i)
			s[i] = buffer[i];
		s[len] = 0;
		KEnvironment::systemLibPath = s;
	}
#endif
}

//protected static
void KEnvironment::initCorLib(void)
{
	ModuleLoader *corlibLoader = KEnvironment::createModuleLoader((KMODULEATTRIBUTES)(KMODA_NATIVE | KMODA_SYSTEM),
		KEnvironment::systemLibPath, KCORLIB_NAME);

	if (!corlibLoader->load())
	{
		// corlib not found => panic exit
		fwprintf(stderr, L"Cannot load %s. Installation might be corrupted.\n", KCORLIB_NAME);
		exit(1);
	}

	KEnvironment::corlibModule = corlibLoader;
}

//protected static
void KEnvironment::initExceptions(void)
{
	Exceptions &exc = KEnvironment::exceptions;

	const ModuleDef *corlib = KEnvironment::corlibModule->getModule();

	// exception classes, in no particular order
	enum KEXCEPTION_CLASSES
	{
		KEXC_GENERAL,
		KEXC_INVALIDCAST,
		KEXC_INVALIDOPERATION,
		KEXC_INVALIDARGUMENT,
		KEXC_NULLARGUMENT,
		KEXC_NULLREFERENCE,
		KEXC_INDEXOUTOFRANGE,
		KEXC_DIVISIONBYZERO,
		KEXC_STACKOVERFLOW,
		KEXC_STACKEMPTY,
		KEXC_INVALIDOPCODE,

		KEXCEPTION_COUNT,
	};

	static kstring_t exceptionNames[] =
	{
		L"System.Exception",
		L"System.InvalidCastException",
		L"System.InvalidOperationException",
		L"System.InvalidArgumentException",
		L"System.NullArgumentException",
		L"System.NullReferenceException",
		L"System.IndexOutOfRangeException",
		L"System.DivisionByZeroException",
		L"System.StackOverflowException",
		L"System.StackEmptyException",
		L"System.InvalidOpCodeException",
	};

	static const ClassDef *exceptionClasses[KEXCEPTION_COUNT];
	
	knuint_t classCount = corlib->classCount;
	ClassDef **classList = corlib->classList;
	knuint_t nFoundExceptions = 0;

	for (knuint_t i = 0; i < classCount; ++i)
	{
		for (knuint_t j = 0; j < KEXCEPTION_COUNT; ++j)
		{
			if (krt_strequ(exceptionNames[j], classList[i]->name))
			{
				exceptionClasses[j] = classList[i];
				++nFoundExceptions;
				break;
			}
		}

		if (nFoundExceptions == KEXCEPTION_COUNT)
			break;
	}

	exc.general          = exceptionClasses[KEXC_GENERAL];
	exc.invalidCast      = exceptionClasses[KEXC_INVALIDCAST];
	exc.invalidOperation = exceptionClasses[KEXC_INVALIDOPERATION];
	exc.invalidArgument  = exceptionClasses[KEXC_INVALIDARGUMENT];
	exc.nullArgument     = exceptionClasses[KEXC_NULLARGUMENT];
	exc.nullReference    = exceptionClasses[KEXC_NULLREFERENCE];
	exc.indexOutOfRange  = exceptionClasses[KEXC_INDEXOUTOFRANGE];
	exc.divisionByZero   = exceptionClasses[KEXC_DIVISIONBYZERO];
	exc.stackOverflow    = exceptionClasses[KEXC_STACKOVERFLOW];
	exc.stackEmpty       = exceptionClasses[KEXC_STACKEMPTY];
	exc.invalidOpCode    = exceptionClasses[KEXC_INVALIDOPCODE];
}

//===================================================

//protected static
KRESULT KEnvironment::execute(void)
{
	while (KEnvironment::running)
	{
		unsigned char opcode = KEnvironment::code[KEnvironment::ip++];
		if (opcode == OP_RET)
			return KRESULT_OK;
		else
			KEnvironment::executors[opcode]();

		if (KEnvironment::hasException)
		{
			KEnvironment::traceDeque->clear();

			while (!KEnvironment::catchStack->empty())
			{
				ExceptionHandler &handler = KEnvironment::catchStack->top();
				CallFrame *frame = handler.frame;

				for (; KEnvironment::frame != frame; )
				{
					// push current method for tracing
					KEnvironment::traceDeque->push_back(KEnvironment::method);
					// then leave
					KEnvironment::leaveMethod();
				}

				if (handler.excType == KEnvironment::exc->type)
				{
					// unwind stack
					KEnvironment::stackPointer = handler.stackPointer;
					// and load exception object
					KEnvironment::stackPush(*KEnvironment::exc);

					knuint_t addr = handler.addr;
					KEnvironment::catchStack->pop();
					
					KEnvironment::running = true;
					KEnvironment::hasException = false;

					KEnvironment::ip = addr;

					break;
				}

				// handler not found YET
				KEnvironment::catchStack->pop();
			}

			if (KEnvironment::hasException)
				// exception not handled
				return KRESULT_ERR;
		}
	}

	return KRESULT_OK;
}

//===================================================

//public static
ModuleLoader * KEnvironment::createModuleLoader(KMODULEATTRIBUTES attrs, kstring_t importerPath, kstring_t filename)
{
	for (std::set<ModuleLoader *>::iterator it = KEnvironment::loadedModules->begin();
		it != KEnvironment::loadedModules->end(); ++it)
	{
		if (krt_strequ((*it)->getFilename(), filename)
			&& ((attrs & KMODA_USER) ? krt_strequ((*it)->getImporterPath(), importerPath) : true))
			return (*it);
	}

	ModuleLoader *loader = ModuleLoader::create(attrs, importerPath, filename);
	KEnvironment::loadedModules->insert(loader);
	return loader;
}

//public static
kstring_t KEnvironment::getSystemLibPath(void)
{
	return KEnvironment::systemLibPath;
}

//===================================================
// Meta

//public static
const ClassDef * KEnvironment::findClass(kstring_t name, const ModuleDef *module)
{
	const ClassDef *res;

	kushort_t moduleCount = module->moduleCount;
	if (moduleCount)
	{
		ModuleDef **moduleList = module->moduleList;
		for (kushort_t i = 0; i < moduleCount; ++i)
			if (res = KEnvironment::findClass(name, moduleList[i]))
				return res;
	}
	
	kushort_t classCount = module->classCount;
	ClassDef **classList = module->classList;
	MetaClassDef *metaClassList = module->metaClassList;
	for (kushort_t i = 0; i < classCount; ++i)
		if (!metaClassList[i].farIndex && krt_strequ(classList[i]->name, name))
			return classList[i];

	return NULL;
}

//public static
const DelegateDef * KEnvironment::findDelegate(kstring_t name, const ModuleDef *module)
{
	const DelegateDef *res;

	kushort_t moduleCount = module->moduleCount;
	if (moduleCount)
	{
		ModuleDef **moduleList = module->moduleList;
		for (kushort_t i = 0; i < moduleCount; ++i)
			if (res = KEnvironment::findDelegate(name, moduleList[i]))
				return res;
	}

	kushort_t delegateCount = module->delegateCount;
	DelegateDef **delegateList = module->delegateList;
	MetaDelegateDef *metaDelegateList = module->metaDelegateList;
	for (kushort_t i = 0; i < delegateCount; ++i)
		if (!metaDelegateList[i].farIndex && krt_strequ(delegateList[i]->name, name))
			return delegateList[i];

	return NULL;
}

//public static
const FieldDef * KEnvironment::findField(const ClassDef *cls, kstring_t name)
{
	knint_t fieldCount = cls->fieldCount;
	FieldDef **fieldList = cls->fieldList;
	for (knint_t i = fieldCount - 1; i >= 0; --i)
		if (krt_strequ(fieldList[i]->name, name))
			return fieldList[i];

	return NULL;
}

//public static
const MethodDef * KEnvironment::findMethod(const ClassDef *cls, kstring_t name)
{
	knint_t methodCount = cls->methodCount;
	MethodDef **methodList = cls->methodList;
	for (knint_t i = methodCount - 1; i >= 0; --i)
		if (krt_strequ(methodList[i]->name, name))
			return methodList[i];

	return NULL;
}

//public static
const TypeDef * KEnvironment::createType(KTYPETAG tag, kushort_t dim)
{
	return KEnvironment::typeTree->add(tag, dim, NULL);
}

//public static
const TypeDef * KEnvironment::createType(KTYPETAG tag, kushort_t dim, const ClassDef *cls)
{
	return KEnvironment::typeTree->add(tag, dim, cls);
}

//public static
const TypeDef * KEnvironment::createType(KTYPETAG tag, kushort_t dim, const DelegateDef *del)
{
	return KEnvironment::typeTree->add(tag, dim, del);
}


//public static
const TypeDef * KEnvironment::makeByRefType(const TypeDef *typeDef)
{
	return KEnvironment::typeTree->add((KTYPETAG)(typeDef->tag | KT_BYREF), typeDef->dim, typeDef->cls);
}

//public static
const TypeDef * KEnvironment::makeByValType(const TypeDef *typeDef)
{
	return KEnvironment::typeTree->add((KTYPETAG)(typeDef->tag & ~KT_BYREF), typeDef->dim, typeDef->cls);
}

//public static
const TypeDef * KEnvironment::makeArrayType(const TypeDef *typeDef)
{
	return KEnvironment::typeTree->add((KTYPETAG)typeDef->tag, typeDef->dim + 1, typeDef->cls);
}

//public static
const TypeDef * KEnvironment::makeElementType(const TypeDef *typeDef)
{
	return KEnvironment::typeTree->add((KTYPETAG)typeDef->tag, typeDef->dim - 1, typeDef->cls);
}

//===================================================
// Objects

//protected static
void KEnvironment::allocClassInstance(const ClassDef *classDef, KObject &outObj)
{
	kuint_t fieldCount = classDef->iFieldCount;
	FieldDef **fieldList = classDef->iFieldList;

	KObject *flds = GC_ALLOC(fieldCount);
	for (knuint_t i = 0; i < fieldCount; ++i)
		INIT_DEFAULT_VALUE(flds[i], fieldList[i]->declType);

	outObj.type = KEnvironment::createType(KT_CLASS, 0, classDef);
	outObj.setRef(flds, fieldCount);
}

//protected static
void KEnvironment::allocDelegateInstance(const DelegateDef *delegateDef, KObject *objThis, const MethodDef *method, KObject &outObj)
{
	if (!(method->attrs & KMA_STATIC) && objThis->type == KObject::nullType)
	{
		KniThrowException(KEnvironment::exceptions.nullReference);
		return;
	}

	KObject *flds = GC_ALLOC(2);
	if (objThis)
	{
		flds[0] = *objThis;
		KEnvironment::gc->mark(objThis->vObj);
	}

	flds[1].type = KEnvironment::rawType;
	flds[1].vRaw = (void *) method;
	flds[1].length = 0;

	outObj.type = KEnvironment::createType(KT_DELEGATE, 0, delegateDef);
	outObj.vObj = flds;
	outObj.length = 2;
}

//protected static
void KEnvironment::allocArray(const TypeDef *arrayType, kuint_t length, KObject &outObj)
{
	const TypeDef *elemType = KEnvironment::makeElementType(arrayType);
	KObject *elems = GC_ALLOC(length);

	for (knuint_t i = 0; i < length; ++i)
		INIT_DEFAULT_VALUE(elems[i], elemType);

	outObj.type = arrayType;
	outObj.vObj = elems;
	outObj.length = length;
}

//protected static
void KEnvironment::allocArrayBaking(const TypeDef *arrayType, kuint_t length, KObject &outObj)
{
	const TypeDef *elemType = KEnvironment::makeElementType(arrayType);
	KObject *elems = GC_ALLOC(length);

	for (klong_t i = length - 1; i >= 0; --i)
		elems[i] = KEnvironment::stackPeek((kuint_t)((klong_t)length - i));

	outObj.type = arrayType;
	outObj.vObj = elems;
	outObj.length = length;
}

//protected static
void KEnvironment::initLocals(const TypeDef **types, kuint_t count)
{
	KObject *locals = GC_ALLOC(count);
	GC_REGISTER(locals);

	for (kuint_t i = 0; i < count; ++i)
		INIT_DEFAULT_VALUE(locals[i], types[i]);

	KEnvironment::frame->locals = locals;
}

//===================================================
// Calls

#define MAX_CALL_STACK	50000
#define CHECK_CALLSTACK_OVERFLOW                                                           \
        if (KEnvironment::callStack->size() == MAX_CALL_STACK)                             \
        {                                                                                  \
            KniThrowException(KEnvironment::exceptions.stackOverflow);                     \
            return KRESULT_ERR;                                                            \
        }

//protected static
KRESULT KEnvironment::invoke(const MethodDef *method)
{
	CHECK_CALLSTACK_OVERFLOW;

	kushort_t paramCount = method->paramCount;
	ParamDef **paramList = method->paramList;

	kushort_t argCount = (method->attrs & KMA_STATIC) ? paramCount : paramCount + 1;

	if (KEnvironment::stackPointer < argCount)
	{
		KniThrowException(KEnvironment::exceptions.stackEmpty);
		return KRESULT_ERR;
	}

	KObject *args = GC_ALLOC(argCount);
	GC_REGISTER(args);

	for (knuint_t i = 0; i < argCount; ++i)
		args[i] = KEnvironment::stackPeek(argCount - i);

	KEnvironment::stackClear(argCount);

	if (!(method->attrs & KMA_STATIC) && args[0].type == KObject::nullType)
	{
		KniThrowException(KEnvironment::exceptions.nullReference);
		return KRESULT_ERR;
	}

	KEnvironment::enterMethod(method, args);

	if (method->attrs & KMA_NATIVE)
	{
		if (method->func() == NINVOKE_ERR)
		{
			KEnvironment::throwException();
			return KRESULT_ERR;
		}
	}
	else
	{
		if (KEnvironment::execute() == KRESULT_ERR)
			return KRESULT_ERR;
	}

	KEnvironment::leaveMethod();

	return KRESULT_OK;
}

//protected static
KRESULT KEnvironment::invokeDelegate(const DelegateDef *dele)
{
	CHECK_CALLSTACK_OVERFLOW;

	kushort_t paramCount = dele->paramCount;
	ParamDef **paramList = dele->paramList;

	if (KEnvironment::stackPointer < paramCount + 1u)
	{
		KniThrowException(KEnvironment::exceptions.stackEmpty);
		return KRESULT_ERR;
	}

	const KObject &objCallable = KEnvironment::stackPeek(paramCount + 1);
	const MethodDef *method = (MethodDef *)objCallable.getField(1).getRaw();

	bool isStatic = (method->attrs & KMA_STATIC) != 0;

	kushort_t argCount = isStatic ? paramCount : paramCount + 1;

	KObject *args = GC_ALLOC(argCount);
	GC_REGISTER(args);

	if (isStatic)
	{
		for (knuint_t i = 0; i < argCount; ++i)
			args[i] = KEnvironment::stackPeek(argCount - i);
		
		KEnvironment::stackClear(paramCount + 1);
	}
	else
	{
		args[0] = objCallable.getField(0);
		for (knuint_t i = 1; i < argCount; ++i)
			args[i] = KEnvironment::stackPeek(argCount - i);

		if (args[0].type == KObject::nullType)
		{
			KniThrowException(KEnvironment::exceptions.nullReference);
			return KRESULT_ERR;
		}
		
		KEnvironment::stackClear(paramCount + 1);
	}

	KEnvironment::enterMethod(method, args);

	if (method->attrs & KMA_NATIVE)
	{
		if (method->func() == NINVOKE_ERR)
		{
			KEnvironment::throwException();
			return KRESULT_ERR;
		}
	}
	else
	{
		if (KEnvironment::execute() == KRESULT_ERR)
			return KRESULT_ERR;
	}

	KEnvironment::leaveMethod();

	return KRESULT_OK;
}

//protected static
KRESULT KEnvironment::invokeLastThis(const MethodDef *method)
{
	CHECK_CALLSTACK_OVERFLOW;

	kushort_t paramCount = method->paramCount;
	ParamDef **paramList = method->paramList;

	kushort_t argCount = paramCount + 1;

	if (KEnvironment::stackPointer < argCount)
	{
		KniThrowException(KEnvironment::exceptions.stackEmpty);
		return KRESULT_ERR;
	}

	KObject *args = GC_ALLOC(argCount);
	GC_REGISTER(args);

	args[0] = KEnvironment::stackPeek(1);

	for (knuint_t i = 1; i < argCount; ++i)
		args[i] = KEnvironment::stackPeek(argCount - i + 1);

	KEnvironment::stackClear(argCount);

	KEnvironment::enterMethod(method, args);

	if (method->attrs & KMA_NATIVE)
	{
		if (method->func() == NINVOKE_ERR)
		{
			KEnvironment::throwException();
			return KRESULT_ERR;
		}
	}
	else
	{
		if (KEnvironment::execute() == KRESULT_ERR)
			return KRESULT_ERR;
	}

	KEnvironment::leaveMethod();

	return KRESULT_OK;
}

KRESULT KEnvironment::invokeExceptionCtor(const MethodDef *ctor)
{
	kushort_t paramCount = ctor->paramCount;
	ParamDef **paramList = ctor->paramList;

	kushort_t argCount = paramCount + 1;

	KObject *args = GC_ALLOC(argCount);
	GC_REGISTER(args);

	args[0] = KEnvironment::stackPeek(1);

	for (knuint_t i = 1; i < argCount; ++i)
		args[i] = KEnvironment::stackPeek(argCount - i + 1);

	KEnvironment::stackClear(argCount);

	KEnvironment::enterMethod(ctor, args);

	if (ctor->attrs & KMA_NATIVE)
	{
		if (method->func() == NINVOKE_ERR)
		{
			KEnvironment::throwException();
			return KRESULT_ERR;
		}
	}
	else
	{
		if (KEnvironment::execute() == KRESULT_ERR)
			return KRESULT_ERR;
	}

	KEnvironment::leaveMethod();

	return KRESULT_OK;
}

//protected static
void KEnvironment::enterMethod(const MethodDef *method, KObject *args)
{
	CallFrame frame = { };
	frame.module = method->klass->module;
	frame.method = method;
	frame.stackPointer = KEnvironment::stackPointer;

	frame.args = args;

	if (method->attrs & KMODA_KIASRA)
	{
		kushort_t localCount = method->localCount;
		const TypeDef **localList = method->localList;

		KObject *locals = GC_ALLOC(localCount);
		GC_REGISTER(locals);

		for (knuint_t i = 0; i < localCount; ++i)
			INIT_DEFAULT_VALUE(locals[i], localList[i]);

		frame.locals = locals;
	}
	else
	{
		frame.locals = NULL;
	}

	KEnvironment::method = method;
	KEnvironment::locals = frame.locals;
	KEnvironment::args   = frame.args;
	KEnvironment::ip     = frame.ip;

	// only update when module change
	if (KEnvironment::module != frame.module)
	{
		KEnvironment::module = frame.module;
		KEnvironment::code = frame.module->code;
		KEnvironment::strings = frame.module->strings;
		KEnvironment::stringLengths = frame.module->stringLengths;
	}

	KEnvironment::callStack->push(frame);
	KEnvironment::frame = &KEnvironment::callStack->top();
}

//protected static
void KEnvironment::leaveMethod()
{
	CallFrame *frame = KEnvironment::frame;

	GC_UNREGISTER(frame->args);
	GC_UNREGISTER(frame->locals);

	if (KEnvironment::callStack->empty())
	{
		KEnvironment::frame = NULL;
		KEnvironment::method = NULL;
		KEnvironment::locals = NULL;
		KEnvironment::args = NULL;
		KEnvironment::ip = 0;
	}
	else
	{
		KEnvironment::callStack->pop();
		frame = &KEnvironment::callStack->top();

		KEnvironment::frame = frame;
		KEnvironment::method = frame->method;
		KEnvironment::locals = frame->locals;
		KEnvironment::args   = frame->args;
		KEnvironment::ip     = frame->ip;

		if (KEnvironment::module != frame->module)
		{
			KEnvironment::module = frame->module;
			KEnvironment::code   = frame->module->code;
			KEnvironment::strings= frame->module->strings;
			KEnvironment::stringLengths = frame->module->stringLengths;
		}
	}
}

//protected static
void KEnvironment::throwException(void)
{
	*KEnvironment::exc = KEnvironment::stackPeek();
	KEnvironment::running = false;
	KEnvironment::hasException = true;
}

//protected static
void KEnvironment::printException(void)
{
	// TODO: dump stack trace
}
