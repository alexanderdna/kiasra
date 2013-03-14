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
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();

	KEnvironment::stack[++KEnvironment::stackPointer] = obj;
}

//protected
void KEnvironment::stackPushNull(void)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();

	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setRaw(NULL);
	obj.type = KEnvironment::nullType;
}

//protected
void KEnvironment::stackPushAddress(KObject *p)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();

	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setRef(p, p->length);
	obj.type = KEnvironment::makeByRefType(p->type);
}

//protected
void KEnvironment::stackPushBool(kbool_t val)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();

	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setBool(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_BOOL);
}

//protected
void KEnvironment::stackPushChar(kchar_t val)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();
	
	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setChar(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_CHAR);
}

//protected
void KEnvironment::stackPushByte(kbyte_t val)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();
	
	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setByte(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_BYTE);
}

//protected
void KEnvironment::stackPushSByte(ksbyte_t val)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();
	
	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setSByte(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_SBYTE);
}

//protected
void KEnvironment::stackPushShort(kshort_t val)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();
	
	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setShort(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_SHORT);
}

//protected
void KEnvironment::stackPushUShort(kushort_t val)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();
	
	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setUShort(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_USHORT);
}

//protected
void KEnvironment::stackPushInt(kint_t val)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();
	
	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setInt(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_INT);
}

//protected
void KEnvironment::stackPushUInt(kuint_t val)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();
	
	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setUInt(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_UINT);
}

//protected
void KEnvironment::stackPushLong(klong_t val)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();
	
	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setLong(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_LONG);
}

//protected
void KEnvironment::stackPushULong(kulong_t val)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();
	
	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setULong(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_ULONG);
}

//protected
void KEnvironment::stackPushFloat(kfloat_t val)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();
	
	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setFloat(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_FLOAT);
}

//protected
void KEnvironment::stackPushDouble(kdouble_t val)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();
	
	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setDouble(val);
	obj.type = GET_PRIMITIVE_TYPE(KT_DOUBLE);
}

//protected
void KEnvironment::stackPushString(kstring_t val, kuint_t length)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();
	
	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.setString(val, length);
	obj.type = GET_PRIMITIVE_TYPE(KT_STRING);
}

//protected
void KEnvironment::stackPushStringMoved(kstring_t val, kuint_t len)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();
	
	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.clean();
	obj.vString = val;
	obj.length = len;
	obj.type = GET_PRIMITIVE_TYPE(KT_STRING);
}

//protected
void KEnvironment::stackPushRaw(kref_t val)
{
	if (KEnvironment::stackPointer >= KEnvironment::stackSize)
		KEnvironment::stackExpand();
	
	KObject &obj = KEnvironment::stack[++KEnvironment::stackPointer];
	obj.clean();
	obj.vRaw = val;
	obj.length = 0;
	obj.type = KEnvironment::rawType;
}

//protected
KObject & KEnvironment::stackPeek(void)
{
	return KEnvironment::stack[KEnvironment::stackPointer];
}

//protected
KObject & KEnvironment::stackPeek(knuint_t offset)
{
	return KEnvironment::stack[KEnvironment::stackPointer - offset + 1];
}

//protected
KObject & KEnvironment::stackPop(void)
{
	KObject &obj = KEnvironment::stack[KEnvironment::stackPointer];
	--KEnvironment::stackPointer;
	return obj;
}

//protected
inline void KEnvironment::stackExpand(void)
{
	knuint_t newSize = KEnvironment::stackSize * 2;
	KObject *newStack = KEnvironment::gc->allocForStack(newSize);

	memcpy(newStack, KEnvironment::stack, KEnvironment::stackSize);

	KEnvironment::gc->removeRoot(KEnvironment::stack);
	KEnvironment::gc->addRoot(newStack);
	
	KEnvironment::stack = newStack;
	KEnvironment::stackSize = newSize;
}
