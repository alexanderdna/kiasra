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
#include <cstdlib>
#include <cstdio>
#include <cwchar>

//===================================================
// Defines

#define INIT_STACK_SIZE 100

#define GC_ALLOC(len)		(KEnvironment::gc->alloc(len))
#define GC_REGISTER(p)		KEnvironment::gc->addRoot(p)
#define GC_UNREGISTER(p)	KEnvironment::gc->removeRoot(p)

//===================================================
// Static fields

bool KEnvironment::isInitialized = false;

kstring_t KEnvironment::systemLibPath;

KTypeTree *KEnvironment::typeTree;

const TypeDef **KEnvironment::primitiveTypes;
const TypeDef  *KEnvironment::voidType;
const TypeDef  *KEnvironment::arrayType;
const TypeDef  *KEnvironment::objectType;
const TypeDef  *KEnvironment::rawType;
const TypeDef  *KEnvironment::excType;
const TypeDef  *KEnvironment::nullType;

KGC *KEnvironment::gc;

KModuleLoader             *KEnvironment::rootModule;
std::set<KModuleLoader *> *KEnvironment::loadedModules;
KModuleLoader             *KEnvironment::corlibModule;

KObject *KEnvironment::stack;
knuint_t KEnvironment::stackSize;
knuint_t KEnvironment::stackPointer;

kcallstack_t    *KEnvironment::callStack;
KFrame          *KEnvironment::frame;		// current call frame
ModuleDef       *KEnvironment::module;	// current method's module
const MethodDef *KEnvironment::method;	// current method
KObject         *KEnvironment::locals;	// current local variables
KObject         *KEnvironment::args;		// current arguments
kstring_t       *KEnvironment::strings;	// current string table
kuint_t         *KEnvironment::stringLengths;

const unsigned char *KEnvironment::code;	// code to execute
knuint_t             KEnvironment::ip;	// pointer to next instruction

bool KEnvironment::running;
bool KEnvironment::hasException;

KObject                     *KEnvironment::exc;			// latest unhandled exception
kcatchstack_t               *KEnvironment::catchStack;	// stack of exception handlers
KEnvironment::ktracedeque_t *KEnvironment::traceDeque;	// deque of methods traced from exception

KEnvironment::KExceptions    KEnvironment::exceptions;	// bundle of common exceptions

//===================================================

//public
void KEnvironment::Initialize(void)
{
	if (KEnvironment::isInitialized)
		return;

	KEnvironment::rootModule = NULL;
	KEnvironment::stackSize = INIT_STACK_SIZE;
	KEnvironment::stackPointer = 0;
	KEnvironment::frame = NULL;
	KEnvironment::module = NULL;
	KEnvironment::method = NULL;
	KEnvironment::locals = NULL;
	KEnvironment::args = NULL;
	KEnvironment::code = NULL;
	KEnvironment::ip = NULL;
	KEnvironment::exc = NULL;

	//===================================

	KEnvironment::initSystemLibPath();

	//===================================
	// prepare executors

	for (knuint_t i = 0; i < OP_OPCODE_COUNT; ++i)
		KEnvironment::executors[i] = KEnvironment::defaultExecutors[i];

	for (knuint_t i = OP_OPCODE_COUNT; i < 256; ++i)
		KEnvironment::executors[i] = KEnvironment::do_InvalidOpcode;

	//===================================
	// prepare basic types

	KTypeTree *typeTree = new KTypeTree;
	KEnvironment::typeTree = typeTree;

	const TypeDef **primitiveTypes = new const TypeDef * [(knuint_t)KT_STRING + 1];
	KEnvironment::primitiveTypes = primitiveTypes;

	for (knuint_t i = 0; i <= (knuint_t)KT_STRING; ++i)
		primitiveTypes[i] = typeTree->add((ktypetag_t)i, 0, NULL);

	KEnvironment::voidType = typeTree->add(KT_VOID, 0, NULL);
	KEnvironment::arrayType = typeTree->add(KT_ARRAY, 0, NULL);
	KEnvironment::objectType = typeTree->add(KT_OBJECT, 0, NULL);
	KEnvironment::rawType = typeTree->add(KT_RAW, 0, NULL);
	
	KEnvironment::nullType = new TypeDef; // nothing special, just a unique reference

	//===================================
	// prepare module loaders

	std::set<KModuleLoader *> *loadedModules = new std::set<KModuleLoader *>;
	KEnvironment::loadedModules = loadedModules;

	//===================================
	// prepare GC

	KEnvironment::gc = new KGC;

	KEnvironment::stack = KEnvironment::gc->allocForStack(INIT_STACK_SIZE);
	GC_REGISTER(KEnvironment::stack);

	//===================================
	// prepare other stacks

	KEnvironment::callStack = new kcallstack_t;
	KEnvironment::catchStack = new kcatchstack_t;

	KEnvironment::traceDeque = new ktracedeque_t;
	
	//===================================
	// misc

	KObject::objectType = KEnvironment::objectType;
	KObject::nullType = KEnvironment::nullType;

	KObject::nullObject.type = KObject::nullType;
	KObject::nullObject.setRef(NULL, 0);
	
	//===================================

	KEnvironment::isInitialized = true;
}

//public
void KEnvironment::Finalize(void)
{
	if (!KEnvironment::isInitialized)
		return;

	if (KEnvironment::loadedModules)
	{
		for (std::set<KModuleLoader *>::iterator it = KEnvironment::loadedModules->begin();
			it != KEnvironment::loadedModules->end(); ++it)
		{
			KModuleLoader *moduleLoader = *it;
			moduleLoader->onDispose();
			delete moduleLoader;
		}

		delete KEnvironment::loadedModules;
		KEnvironment::loadedModules = NULL;
	}

	if (KEnvironment::typeTree)
	{
		delete KEnvironment::typeTree;
		KEnvironment::typeTree = NULL;
	}

	if (KEnvironment::primitiveTypes)
	{
		delete []KEnvironment::primitiveTypes;
		KEnvironment::primitiveTypes = NULL;
	}

	if (KEnvironment::nullType)
	{
		delete KEnvironment::nullType;
		KEnvironment::nullType = NULL;
	}

	if (KEnvironment::callStack)
	{
		delete KEnvironment::callStack;
		KEnvironment::callStack = NULL;
	}

	if (KEnvironment::catchStack)
	{
		delete KEnvironment::catchStack;
		KEnvironment::catchStack = NULL;
	}

	if (KEnvironment::traceDeque)
	{
		delete KEnvironment::traceDeque;
		KEnvironment::traceDeque = NULL;
	}

	if (KEnvironment::gc)
	{
		delete KEnvironment::gc;
		KEnvironment::gc = NULL;
	}

	if (KEnvironment::systemLibPath)
	{
		delete [] KEnvironment::systemLibPath;
		KEnvironment::systemLibPath = NULL;
	}

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
		knuint_t len = lastSlash - buffer + 1;
		
		buffer[lastSlash - buffer + 1] = 0;

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
		
		buffer[lastSlash - buffer + 1] = 0;

		kchar_t *s = new kchar_t[len + 1];
		for (knuint_t i = 0; i < len; ++i)
			s[i] = buffer[i];
		s[len] = 0;
		KEnvironment::systemLibPath = s;
	}
#endif
}

//protected
void KEnvironment::initCorLib(void)
{
	KModuleLoader *corlibLoader = KEnvironment::createModuleLoader(NULL, KCORLIB_NAME, 0);
	if (!corlibLoader->load())
	{
		// corlib not found => panic exit
		fwprintf(stderr, L"Cannot load %s. Installation might be corrupted.\n", KCORLIB_NAME);
		exit(1);
	}

	KEnvironment::corlibModule = corlibLoader;
}

//protected
void KEnvironment::initExceptions(void)
{
	KExceptions &exc = KEnvironment::exceptions;

	const ClassDef *excClass = KEnvironment::findClass(L"System.Exception", KEnvironment::corlibModule->getModule());
	exc.klass = excClass;

	exc.ctor = (MethodDef *) findMethod(excClass, L".ctor");
	exc.fromCode = (MethodDef *) findMethod(excClass, L"fromCode");
	exc.fromMessage = (MethodDef *) findMethod(excClass, L"fromMessage");

	exc.custom = (FieldDef *) findField(excClass, L"Custom");
	exc.invalidCast = (FieldDef *) findField(excClass, L"InvalidCast");
	exc.invalidOperation = (FieldDef *) findField(excClass, L"InvalidOperation");
	exc.invalidArgument = (FieldDef *) findField(excClass, L"InvalidArgument");
	exc.nullArgument = (FieldDef *) findField(excClass, L"NullArgument");
	exc.nullReference = (FieldDef *) findField(excClass, L"NullReference");
	exc.indexOutOfRange = (FieldDef *) findField(excClass, L"IndexOutOfRange");
	exc.divisionByZero = (FieldDef *) findField(excClass, L"DivisionByZero");
	exc.invalidOpCode = (FieldDef *) findField(excClass, L"InvalidOpCode");
}

//===================================================

//protected
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
				KExceptionHandler &handler = KEnvironment::catchStack->top();
				KFrame *frame = handler.frame;

				for (; KEnvironment::frame != frame; )
				{
					// push current method for tracing
					KEnvironment::traceDeque->push_back(KEnvironment::method);
					// then leave
					KEnvironment::leaveMethod();
				}

				if (handler.excType == KEnvironment::exc->type)
				{
					knuint_t addr = handler.addr;
					KEnvironment::catchStack->pop();
					
					KEnvironment::running = true;
					KEnvironment::hasException = false;

					KEnvironment::stackPush(*KEnvironment::exc);
					KEnvironment::ip = handler.addr;

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

//public
KModuleLoader * KEnvironment::createModuleLoader(kstring_t importerPath, kstring_t path, uint32_t hash)
{
	for (std::set<KModuleLoader *>::iterator it = KEnvironment::loadedModules->begin();
		it != KEnvironment::loadedModules->end(); ++it)
	{
		if (krt_strequ((*it)->getPath(), path))
			return (*it);
	}

	KModuleLoader *moduleLoader = new KModuleLoader(importerPath, path, hash);
	KEnvironment::loadedModules->insert(moduleLoader);

	return moduleLoader;
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
	else
	{
		kushort_t classCount = module->classCount;
		ClassDef **classList = module->classList;
		MetaClassDef *metaClassList = module->metaClassList;
		for (kushort_t i = 0; i < classCount; ++i)
			if (!metaClassList[i].farIndex && krt_strequ(classList[i]->name, name))
				return classList[i];
	}

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
	else
	{
		kushort_t delegateCount = module->delegateCount;
		DelegateDef **delegateList = module->delegateList;
		MetaDelegateDef *metaDelegateList = module->metaDelegateList;
		for (kushort_t i = 0; i < delegateCount; ++i)
			if (!metaDelegateList[i].farIndex && krt_strequ(delegateList[i]->name, name))
				return delegateList[i];
	}

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

//public
const TypeDef * KEnvironment::createType(ktypetag_t tag, kushort_t dim)
{
	return KEnvironment::typeTree->add(tag, dim, NULL);
}

//public
const TypeDef * KEnvironment::createType(ktypetag_t tag, kushort_t dim, ClassDef *cls)
{
	return KEnvironment::typeTree->add(tag, dim, cls);
}

//public
const TypeDef * KEnvironment::createType(ktypetag_t tag, kushort_t dim, DelegateDef *del)
{
	return KEnvironment::typeTree->add(tag, dim, del);
}


//public
const TypeDef * KEnvironment::makeByRefType(const TypeDef *typeDef)
{
	return KEnvironment::typeTree->add((ktypetag_t)(typeDef->tag | KT_REF), typeDef->dim, typeDef->cls);
}

//public
const TypeDef * KEnvironment::makeByValType(const TypeDef *typeDef)
{
	return KEnvironment::typeTree->add((ktypetag_t)(typeDef->tag & ~KT_REF), typeDef->dim, typeDef->cls);
}

//public
const TypeDef * KEnvironment::makeArrayType(const TypeDef *typeDef)
{
	return KEnvironment::typeTree->add((ktypetag_t)typeDef->tag, typeDef->dim + 1, typeDef->cls);
}

//public
const TypeDef * KEnvironment::makeElementType(const TypeDef *typeDef)
{
	return KEnvironment::typeTree->add((ktypetag_t)typeDef->tag, typeDef->dim - 1, typeDef->cls);
}

//===================================================
// Objects

//protected
void KEnvironment::allocClassInstance(const ClassDef *classDef, KObject &outObj)
{
	knuint_t fieldCount = classDef->iFieldCount;
	FieldDef **fieldList = classDef->iFieldList;

	KObject *flds = GC_ALLOC(fieldCount);
	for (knuint_t i = 0; i < fieldCount; ++i)
		INIT_DEFAULT_VALUE(flds[i], fieldList[i]->declType);

	outObj.type = KEnvironment::typeTree->add(KT_CLASS, 0, classDef);
	outObj.setRef(flds, fieldCount);
}

//protected
void KEnvironment::allocDelegateInstance(const DelegateDef *delegateDef, KObject *objThis, const MethodDef *method, KObject &outObj)
{
	KObject *flds = GC_ALLOC(2);
	if (objThis)
		flds[0] = *objThis;

	flds[1].type = KEnvironment::rawType;
	flds[1].vRaw = (void *) method;
	flds[1].length = 0;

	outObj.type = KEnvironment::typeTree->add(KT_DELEGATE, 0, delegateDef);
	outObj.vObj = flds;
	outObj.length = 2;
}

//protected
void KEnvironment::allocArray(const TypeDef *arrayType, knuint_t length, KObject &outObj)
{
	const TypeDef *elemType = KEnvironment::makeElementType(arrayType);
	KObject *elems = GC_ALLOC(length);

	for (knuint_t i = 0; i < length; ++i)
		INIT_DEFAULT_VALUE(elems[i], elemType);

	outObj.type = arrayType;
	outObj.vObj = elems;
	outObj.length = length;
}

//protected
void KEnvironment::allocArrayBaking(const TypeDef *arrayType, knuint_t length, KObject &outObj)
{
	const TypeDef *elemType = KEnvironment::makeElementType(arrayType);
	KObject *elems = GC_ALLOC(length);

	for (klong_t i = length - 1; i >= 0; --i)
		elems[i] = KEnvironment::stackPeek((knuint_t)((klong_t)length - i));

	outObj.type = arrayType;
	outObj.vObj = elems;
	outObj.length = length;
}

//protected
void KEnvironment::initLocals(const TypeDef **types, knuint_t count)
{
	KObject *locals = GC_ALLOC(count);
	GC_REGISTER(locals);

	for (knuint_t i = 0; i < count; ++i)
		INIT_DEFAULT_VALUE(locals[i], types[i]);

	KEnvironment::frame->locals = locals;
}

//===================================================
// Calls

//protected
KRESULT KEnvironment::invoke(const MethodDef *methodDef)
{
	kushort_t paramCount = method->paramCount;
	ParamDef **paramList = method->paramList;

	kushort_t argCount = (methodDef->attrs & KMA_STATIC) ? paramCount : paramCount + 1;

	KObject *args = GC_ALLOC(argCount);
	GC_REGISTER(args);

	for (knuint_t i = 0; i < argCount; ++i)
		args[i] = KEnvironment::stackPeek(argCount - i);

	KEnvironment::stackClear(argCount);

	KEnvironment::enterMethod(methodDef, args);

	if (methodDef->attrs & KMA_NATIVE)
	{
		if (method->func() == KRESULT_ERR)
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

//protected
KRESULT KEnvironment::invokeLastThis(const MethodDef *methodDef)
{
	kushort_t paramCount = method->paramCount;
	ParamDef **paramList = method->paramList;

	kushort_t argCount = paramCount + 1;

	KObject *args = GC_ALLOC(argCount);
	GC_REGISTER(args);

	args[0] = KEnvironment::stackPeek(1);

	for (knuint_t i = 1; i < argCount; ++i)
		args[i] = KEnvironment::stackPeek(argCount - i + 1);

	KEnvironment::stackClear(argCount);

	KEnvironment::enterMethod(methodDef, args);

	if (methodDef->attrs & KMA_NATIVE)
	{
		if (method->func() == KRESULT_ERR)
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

//protected
void KEnvironment::enterMethod(const MethodDef *method, KObject *args)
{
	KFrame frame = { };
	frame.module = method->klass->module;
	frame.method = method;

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

//protected
void KEnvironment::leaveMethod()
{
	KFrame *frame = KEnvironment::frame;

	GC_UNREGISTER(frame->args);
	GC_UNREGISTER(frame->locals);

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

//protected
void KEnvironment::throwException(void)
{
	*KEnvironment::exc = KEnvironment::stackPeek();
	KEnvironment::running = false;
	KEnvironment::hasException = true;
}

//protected
void KEnvironment::printException(void)
{
	// TODO: dump stack trace
}
