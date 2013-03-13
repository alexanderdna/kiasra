#include "kenv.hpp"

#include "kconfig.hpp"
#include "krt.h"
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

#define GC_ALLOC(len)		(this->gc->alloc(len))
#define GC_REGISTER(p)		this->gc->addRoot(p)
#define GC_UNREGISTER(p)	this->gc->removeRoot(p)

//===================================================
// Static fields

kstring_t KEnvironment::systemLibPath;

const TypeDef **KEnvironment::primitiveTypes;
const TypeDef  *KEnvironment::voidType;
const TypeDef  *KEnvironment::arrayType;
const TypeDef  *KEnvironment::objectType;
const TypeDef  *KEnvironment::rawType;
const TypeDef  *KEnvironment::excType;
const TypeDef  *KEnvironment::nullType;

//===================================================
// Constructor, destructor

//public
KEnvironment::KEnvironment(void)
	: rootModule(NULL),
	stackSize(INIT_STACK_SIZE), stackPointer(0),
	frame(NULL), module(NULL), method(NULL), locals(NULL), args(NULL),
	code(NULL), ip(0),
	exc(NULL)
{
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
	this->typeTree = typeTree;

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
	this->loadedModules = loadedModules;

	//===================================
	// prepare GC

	this->gc = new KGC;

	this->stack = this->gc->allocForStack(INIT_STACK_SIZE);
	GC_REGISTER(this->stack);

	//===================================
	// prepare other stacks

	this->callStack = new kcallstack_t;
	this->catchStack = new kcatchstack_t;

	this->traceDeque = new ktracedeque_t;
	
	//===================================
	// misc

	KObject::env = const_cast<KEnvironment *>(this);
	KObject::objectType = KEnvironment::objectType;
	KObject::nullType = KEnvironment::nullType;

	KObject::nullObject.type = KObject::nullType;
	KObject::nullObject.setRef(NULL, 0);
}

//public
KEnvironment::~KEnvironment(void)
{
	if (this->loadedModules)
	{
		for (std::set<KModuleLoader *>::iterator it = this->loadedModules->begin();
			it != this->loadedModules->end(); ++it)
		{
			KModuleLoader *moduleLoader = *it;
			moduleLoader->onDispose();
			delete moduleLoader;
		}

		delete this->loadedModules;
		this->loadedModules = NULL;
	}

	if (this->typeTree)
	{
		delete this->typeTree;
		this->typeTree = NULL;
	}

	if (KEnvironment::primitiveTypes)
	{
		delete []KEnvironment::primitiveTypes;
		KEnvironment::primitiveTypes = NULL;
	}

	if (this->nullType)
	{
		delete KEnvironment::nullType;
		KEnvironment::nullType = NULL;
	}

	if (this->callStack)
	{
		delete this->callStack;
		this->callStack = NULL;
	}

	if (this->catchStack)
	{
		delete this->catchStack;
		this->catchStack = NULL;
	}

	if (this->traceDeque)
	{
		delete this->traceDeque;
		this->traceDeque = NULL;
	}

	if (this->gc)
	{
		delete this->gc;
		this->gc = NULL;
	}

	if (KEnvironment::systemLibPath)
	{
		delete [] KEnvironment::systemLibPath;
		KEnvironment::systemLibPath = NULL;
	}
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
	KModuleLoader *corlibLoader = this->createModuleLoader(NULL, KCORLIB_NAME, 0);
	if (!corlibLoader->load())
	{
		// corlib not found => panic exit
		fwprintf(stderr, L"Cannot load %s. Installation might be corrupted.\n", KCORLIB_NAME);
		exit(1);
	}

	this->corlibModule = corlibLoader;
}

//protected
void KEnvironment::initExceptions(void)
{
	KExceptions &exc = this->exceptions;

	const ClassDef *excClass = this->findClass(L"System.Exception", this->corlibModule->getModule());
	exc.klass = excClass;

	exc.ctor = (MethodDef *) findMethod(excClass, L".ctor");
	exc.fromCode = (MethodDef *) findMethod(excClass, L"fromCode");
	exc.fromMessage = (MethodDef *) findMethod(excClass, L"fromMessage");

	exc.custom = (FieldDef *) findField(excClass, L"Custom");
	exc.invalidCast = (FieldDef *) findField(excClass, L"InvalidCast");
	exc.invalidOperation = (FieldDef *) findField(excClass, L"InvalidOperation");
	exc.invalidArgument = (FieldDef *) findField(excClass, L"InvalidArgument");
	exc.nullArgument = (FieldDef *) findField(excClass, L"NullArgument");
	exc.indexOutOfRange = (FieldDef *) findField(excClass, L"IndexOutOfRange");
	exc.divisionByZero = (FieldDef *) findField(excClass, L"DivisionByZero");
}

//===================================================

//protected
KRESULT KEnvironment::execute(void)
{
	while (this->running)
	{
		unsigned char opcode = this->code[this->ip++];
		if (opcode == OP_RET)
			return KRESULT_OK;
		else
			KEnvironment::executors[opcode](this);

		if (this->hasException)
		{
			this->traceDeque->clear();

			while (!this->catchStack->empty())
			{
				KExceptionHandler &handler = this->catchStack->top();
				KFrame *frame = handler.frame;

				for (; this->frame != frame; )
				{
					// push current method for tracing
					this->traceDeque->push_back(this->method);
					// then leave
					this->leaveMethod();
				}

				if (handler.excType == this->exc->type)
				{
					knuint_t addr = handler.addr;
					this->catchStack->pop();
					
					this->running = true;
					this->hasException = false;

					this->stackPush(*this->exc);
					this->ip = handler.addr;

					break;
				}

				// handler not found YET
				this->catchStack->pop();
			}

			if (this->hasException)
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
	for (std::set<KModuleLoader *>::iterator it = this->loadedModules->begin();
		it != this->loadedModules->end(); ++it)
	{
		if (krt_strequ((*it)->getPath(), path))
			return (*it);
	}

	KModuleLoader *moduleLoader = new KModuleLoader(const_cast<KEnvironment *>(this), importerPath, path, hash);
	this->loadedModules->insert(moduleLoader);

	return moduleLoader;
}

//public
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
	return this->typeTree->add(tag, dim, NULL);
}

//public
const TypeDef * KEnvironment::createType(ktypetag_t tag, kushort_t dim, ClassDef *cls)
{
	return this->typeTree->add(tag, dim, cls);
}

//public
const TypeDef * KEnvironment::createType(ktypetag_t tag, kushort_t dim, DelegateDef *del)
{
	return this->typeTree->add(tag, dim, del);
}


//public
const TypeDef * KEnvironment::makeByRefType(const TypeDef *typeDef)
{
	return this->typeTree->add((ktypetag_t)(typeDef->tag | KT_REF), typeDef->dim, typeDef->cls);
}

//public
const TypeDef * KEnvironment::makeArrayType(const TypeDef *typeDef)
{
	return this->typeTree->add((ktypetag_t)typeDef->tag, typeDef->dim + 1, typeDef->cls);
}

//public
const TypeDef * KEnvironment::makeElementType(const TypeDef *typeDef)
{
	return this->typeTree->add((ktypetag_t)typeDef->tag, typeDef->dim - 1, typeDef->cls);
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

	outObj.type = this->typeTree->add(KT_CLASS, 0, classDef);
	outObj.setRef(flds, fieldCount);
}

//protected
void KEnvironment::allocDelegateInstance(const DelegateDef *delegateDef, KObject *objThis, const MethodDef *method, KObject &outObj)
{
	KObject *flds = GC_ALLOC(2);
	if (objThis)
		flds[0] = *objThis;

	flds[1].type = this->rawType;
	flds[1].vRaw = (void *) method;
	flds[1].length = 0;

	outObj.type = this->typeTree->add(KT_DELEGATE, 0, delegateDef);
	outObj.vObj = flds;
	outObj.length = 2;
}

//protected
void KEnvironment::allocArray(const TypeDef *arrayType, knuint_t length, KObject &outObj)
{
	const TypeDef *elemType = this->makeElementType(arrayType);
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
	const TypeDef *elemType = this->makeElementType(arrayType);
	KObject *elems = GC_ALLOC(length);

	for (klong_t i = length - 1; i >= 0; --i)
		elems[i] = this->stackPeek((knuint_t)((klong_t)length - i));

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

	this->frame->locals = locals;
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
		args[i] = this->stackPeek(argCount - i);

	this->stackClear(argCount);

	this->enterMethod(methodDef, args);

	if (methodDef->attrs & KMA_NATIVE)
	{
		if (method->func(this) == KRESULT_ERR)
		{
			this->throwException();
			return KRESULT_ERR;
		}
	}
	else
	{
		if (this->execute() == KRESULT_ERR)
			return KRESULT_ERR;
	}

	this->leaveMethod();

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

	args[0] = this->stackPeek(1);

	for (knuint_t i = 1; i < argCount; ++i)
		args[i] = this->stackPeek(argCount - i + 1);

	this->stackClear(argCount);

	this->enterMethod(methodDef, args);

	if (methodDef->attrs & KMA_NATIVE)
	{
		if (method->func(this) == KRESULT_ERR)
		{
			this->throwException();
			return KRESULT_ERR;
		}
	}
	else
	{
		if (this->execute() == KRESULT_ERR)
			return KRESULT_ERR;
	}

	this->leaveMethod();

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

	this->method = method;
	this->locals = frame.locals;
	this->args   = frame.args;
	this->ip     = frame.ip;

	// only update when module change
	if (this->module != frame.module)
	{
		this->module = frame.module;
		this->code = frame.module->code;
		this->strings = frame.module->strings;
		this->stringLengths = frame.module->stringLengths;
	}

	this->callStack->push(frame);
	this->frame = &this->callStack->top();
}

//protected
void KEnvironment::leaveMethod()
{
	KFrame *frame = this->frame;

	GC_UNREGISTER(frame->args);
	GC_UNREGISTER(frame->locals);

	this->callStack->pop();
	frame = &this->callStack->top();

	this->frame = frame;
	this->method = frame->method;
	this->locals = frame->locals;
	this->args   = frame->args;
	this->ip     = frame->ip;

	if (this->module != frame->module)
	{
		this->module = frame->module;
		this->code   = frame->module->code;
		this->strings= frame->module->strings;
		this->stringLengths = frame->module->stringLengths;
	}
}

//protected
void KEnvironment::throwException(void)
{
	*this->exc = this->stackPeek();
	this->running = false;
	this->hasException = true;
}

//protected
void KEnvironment::printException(void)
{
	// TODO: dump stack trace
}
