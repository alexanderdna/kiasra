#pragma once

#include "krt.h"
#include "kmachine.h"

class KObject
{
private:
	const TypeDef *type;

	union
	{
		kbool_t   vBool;
		kint_t    vInt;
		kfloat_t  vFloat;
		kstring_t vString;
		KObject  *vObj;	    // for class fields, array elements
	};
	uint32_t length;        // array or string length

	KMachine *vm;

public:
	KObject();
	~KObject();

	KObject(const KObject &src);

	KObject & operator =(const KObject &src);

	KObject(KMachine *vm, const TypeDef *type);

	const TypeDef * getType() const;
	const kbool_t getBool() const;
	const kint_t getInt() const;
	const kfloat_t & getFloat() const;
	const kstring_t getString() const;

	KObject & getElement(uint32_t index);
	KObject & getField(ktoken_t token);

	const uint32_t getLength() const;

	void init(KMachine *vm, const TypeDef *type);

private:
	void cleanUp();
	void accept(const KObject &src);
};
