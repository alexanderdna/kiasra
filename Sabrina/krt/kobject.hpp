#pragma once

#include "krt.h"
#include "kenv.hpp"

#include "kmeta.hpp"

class KEnvironment;
class KGC;

class KObject
{
	friend class KEnvironment;
	friend class KGC;

public:
	static KGC *gc;
	static const TypeDef *objectType;
	static const TypeDef *nullType;
	static const TypeDef *stringType;

	static KObject nullObject;

protected:
	const TypeDef *type;

	union
	{
		kbool_t   vBool;
		kchar_t   vChar;
		kbyte_t   vByte;
		ksbyte_t  vSByte;
		kshort_t  vShort;
		kushort_t vUShort;
		kint_t    vInt;
		kuint_t   vUInt;
		klong_t   vLong;
		kulong_t  vULong;
		kfloat_t  vFloat;
		kdouble_t vDouble;

		kstring_t vString;

		KObject  *vObj;	    // for class fields, array elements
		kref_t    vRaw;
	};
	kuint_t length;        // array or string length

public:
	KObject(void);
	~KObject(void);

	KObject(const KObject &obj);
	KObject(const TypeDef *type);

	KObject & operator=(const KObject &obj);

	const TypeDef * getType(void) const;

	kuint_t getLength(void) const;

	kbool_t getBool(void) const;
	kchar_t getChar(void) const;
	kbyte_t getByte(void) const;
	ksbyte_t getSByte(void) const;
	kshort_t getShort(void) const;
	kushort_t getUShort(void) const;
	kint_t getInt(void) const;
	kuint_t getUInt(void) const;
	klong_t getLong(void) const;
	kulong_t getULong(void) const;
	kfloat_t getFloat(void) const;
	kdouble_t getDouble(void) const;
	kstring_t getString(void) const;

	KObject * getRef(void) const;
	kref_t getRaw(void) const;

	KObject & getField(ktoken16_t tok) const;
	KObject & getElement(knuint_t idx) const;

	knint_t getNInt(void) const;
	knuint_t getNUInt(void) const;

	void setBool(kbool_t val);
	void setChar(kchar_t val);
	void setByte(kbyte_t val);
	void setSByte(ksbyte_t val);
	void setShort(kshort_t val);
	void setUShort(kushort_t val);
	void setInt(kint_t val);
	void setUInt(kuint_t val);
	void setLong(klong_t val);
	void setULong(kulong_t val);
	void setFloat(kfloat_t val);
	void setDouble(kdouble_t val);
	void setString(kstring_t val, kuint_t length);

	void setRef(KObject *ref, kuint_t length);
	void setRaw(kref_t raw);

	void setField(ktoken16_t tok, const KObject &obj);
	void setElement(knuint_t idx, const KObject &obj);

protected:
	void clean(void);

	static kstring_t strdup(kstring_t src, knuint_t len);
};
