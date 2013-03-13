#include "kconfig.hpp"
#include "krt.h"
#include "kgc.hpp"
#include "kobject.hpp"
#include "kenv.hpp"

//===================================================
// Stack manipulation

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
void KEnvironment::stackPushString(kstring_t val, knuint_t length)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.setString(val, length);
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
void KEnvironment::stackPushRaw(kref_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();
	
	KObject &obj = this->stack[++this->stackPointer];
	obj.clean();
	obj.vRaw = val;
	obj.length = 0;
	obj.type = this->rawType;
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
