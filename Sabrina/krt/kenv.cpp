#include "kenv.hpp"

#include <cstdlib>

#include "krt.h"
#include "kmeta.hpp"
#include "kgc.hpp"
#include "kobject.hpp"

#define INIT_STACK_SIZE 100

//public
KEnvironment::KEnvironment(void)
	: stackSize(INIT_STACK_SIZE), stackPointer(0)
{
	this->gc = new KGC;

	this->stack = this->gc->allocForStack(INIT_STACK_SIZE);
	this->gc->addRoot(this->stack);

	KObject::env = const_cast<KEnvironment *>(this);
}

//public
KEnvironment::~KEnvironment(void)
{
	if (this->gc)
	{
		this->gc->removeRoot(this->stack);

		delete this->gc;
		this->gc = NULL;
	}
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

//protected
void KEnvironment::stackPush(const KObject &obj)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer] = obj;
}

//protected
void KEnvironment::stackPushBool(kbool_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer].setBool(val);
}

//protected
void KEnvironment::stackPushChar(kchar_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer].setChar(val);
}

//protected
void KEnvironment::stackPushByte(kbyte_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer].setByte(val);
}

//protected
void KEnvironment::stackPushSByte(ksbyte_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer].setSByte(val);
}

//protected
void KEnvironment::stackPushShort(kshort_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer].setShort(val);
}

//protected
void KEnvironment::stackPushUShort(kushort_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer].setUShort(val);
}

//protected
void KEnvironment::stackPushInt(kint_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer].setInt(val);
}

//protected
void KEnvironment::stackPushUInt(kuint_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer].setUInt(val);
}

//protected
void KEnvironment::stackPushLong(klong_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer].setLong(val);
}

//protected
void KEnvironment::stackPushULong(kulong_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer].setULong(val);
}

//protected
void KEnvironment::stackPushFloat(kfloat_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer].setFloat(val);
}

//protected
void KEnvironment::stackPushDouble(kdouble_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer].setDouble(val);
}

//protected
void KEnvironment::stackPushString(kstring_t val)
{
	if (this->stackPointer >= this->stackSize)
		this->stackExpand();

	this->stack[++this->stackPointer].setString(val);
}

//protected
KObject & KEnvironment::stackPeek(void)
{
	return this->stack[this->stackPointer];
}

//protected
KObject & KEnvironment::stackPop(void)
{
	KObject &obj = this->stack[this->stackPointer];
	--this->stackPointer;
	return obj;
}
