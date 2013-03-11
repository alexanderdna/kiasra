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

//===================================================
// Defines

#define INIT_STACK_SIZE 100

#define GET_PRIMITIVE_TYPE(tag) (KEnvironment::primitiveTypes[(unsigned)tag])

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
	// prepare kcorlib

	std::set<KModuleLoader *> *loadedModules = new std::set<KModuleLoader *>;
	this->loadedModules = loadedModules;

	this->initCorLib();

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
	KModuleLoader *moduleLoader = this->createModuleLoader(KEnvironment::systemLibPath, L"Dkcorlib.dll", 0);
	if (!moduleLoader->load())
	{
		fprintf(stderr, "Cannot load Core Library. Installation may have been corrupt.\n");
		exit(1);
	}
}

//protected
KRESULT KEnvironment::execute(void)
{
start:
	while (this->running)
	{
		unsigned char opcode = this->code[this->ip++];
		if (opcode == OP_RET)
			return KRESULT_OK;
		else
			KEnvironment::executors[opcode](this);
	}

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
				// found an appropriate handler
				if (handler.native)
				{
					KEXCFUNC *func = handler.func;
					this->catchStack->pop();

					this->running = true;
					this->hasException = false;

					this->stackPush(*this->exc);
					(*func)(this);

					this->leaveMethod();
					return KRESULT_OK;
				}
				else
				{
					knuint_t addr = handler.addr;
					this->catchStack->pop();
					
					this->running = true;
					this->hasException = false;

					this->stackPush(*this->exc);
					this->ip = handler.addr;
					
					goto start;
				}
			}

			// handler not found YET
			this->catchStack->pop();
		}

		// exception not handled
		return KRESULT_ERR;
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

	this->module = frame.module;
	this->method = method;
	this->locals = frame.locals;
	this->args   = frame.args;
	this->ip     = frame.ip;
	this->code   = frame.module->code;
	this->strings= frame.module->strings;

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
	this->module = frame->module;
	this->method = frame->method;
	this->locals = frame->locals;
	this->args   = frame->args;
	this->ip     = frame->ip;
	this->code   = frame->module->code;
	this->strings= frame->module->strings;
}

//protected
void KEnvironment::throwException(void)
{
	*this->exc = this->stackPeek();
	this->running = false;
	this->hasException = true;
}

//===================================================
// Stack manipulation

//protected
inline void KEnvironment::stackExpand(void)
{
	knuint_t newSize = this->stackSize * 2;
	KObject *newStack = this->gc->allocForStack(newSize);

	memcpy(newStack, this->stack, this->stackSize);

	this->gc->removeRoot(this->stack);
	this->gc->addRoot(newStack);
	
	this->stack = newStack;
	this->stackSize = newSize;
}

//protected
void KEnvironment::stackPush(const KObject &obj)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer] = obj;
}

//protected
void KEnvironment::stackPushNull(void)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	KObject &obj = this->stack[++this->stackPointer];
	obj.setRaw(NULL);
	obj.type = this->nullType;
}

//protected
void KEnvironment::stackPushAddress(KObject *p)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	KObject &obj = this->stack[++this->stackPointer];
	obj.setRef(p, p->length);
	obj.type = this->makeByRefType(p->type);
}

//protected
void KEnvironment::stackPushBool(kbool_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	KObject &obj = this->stack[++this->stackPointer];
	obj.setBool(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_BOOL);
}

//protected
void KEnvironment::stackPushChar(kchar_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.setChar(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_CHAR);
}

//protected
void KEnvironment::stackPushByte(kbyte_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.setByte(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_BYTE);
}

//protected
void KEnvironment::stackPushSByte(ksbyte_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.setSByte(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_SBYTE);
}

//protected
void KEnvironment::stackPushShort(kshort_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.setShort(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_SHORT);
}

//protected
void KEnvironment::stackPushUShort(kushort_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.setUShort(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_USHORT);
}

//protected
void KEnvironment::stackPushInt(kint_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.setInt(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_INT);
}

//protected
void KEnvironment::stackPushUInt(kuint_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.setUInt(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_UINT);
}

//protected
void KEnvironment::stackPushLong(klong_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.setLong(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_LONG);
}

//protected
void KEnvironment::stackPushULong(kulong_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.setULong(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_ULONG);
}

//protected
void KEnvironment::stackPushFloat(kfloat_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.setFloat(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_FLOAT);
}

//protected
void KEnvironment::stackPushDouble(kdouble_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.setDouble(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_DOUBLE);
}

//protected
void KEnvironment::stackPushString(kstring_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.setString(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_STRING);
}

//protected
void KEnvironment::stackPushStringMoved(kstring_t val, knuint_t len)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.clean();
	obj.vString = val;
	obj.length = len;
	obj.type = GET_PRIMITIVE_TYPE(KT_STRING);
}

//protected
KObject & KEnvironment::stackPeek(void)
{
	return this->stack[this->stackPointer];
}

//protected
KObject & KEnvironment::stackPeek(knuint_t offset)
{
	return this->stack[this->stackPointer - offset + 1];
}

//protected
KObject & KEnvironment::stackPop(void)
{
	KObject &obj = this->stack[this->stackPointer];
	--this->stackPointer;
	return obj;
}

inline void KEnvironment::stackClear(knuint_t count)
{
	this->stackPointer -= count;
}
