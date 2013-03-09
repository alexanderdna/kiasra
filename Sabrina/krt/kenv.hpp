#pragma once

#include "kmeta.hpp"

class KGC;
class KObject;

class KEnvironment
{
	friend class KGC;

protected:
	KGC *gc;

	KObject *stack;
	knuint_t stackSize;
	knuint_t stackPointer;

public:
	KEnvironment(void);
	~KEnvironment(void);

	const TypeDef * getScalarType(ktypetag_t tag) const;

protected:
	inline void stackExpand(void);
	void stackPush(const KObject &obj);
	void stackPushBool(kbool_t val);
	void stackPushChar(kchar_t val);
	void stackPushByte(kbyte_t val);
	void stackPushSByte(ksbyte_t val);
	void stackPushShort(kshort_t val);
	void stackPushUShort(kushort_t val);
	void stackPushInt(kint_t val);
	void stackPushUInt(kuint_t val);
	void stackPushLong(klong_t val);
	void stackPushULong(kulong_t val);
	void stackPushFloat(kfloat_t val);
	void stackPushDouble(kdouble_t val);
	void stackPushString(kstring_t val);
	KObject & stackPeek(void);
	KObject & stackPop(void);

};
