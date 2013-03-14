#include "kobject.hpp"

#include "kgc.hpp"
#include "kconfig.hpp"

#include <cstring>

#define IS_STRING(type) (type && ((type->tag & KT_SCALAR_MASK) == KT_STRING && !type->dim))

// Checks if `type` is array, class or delegate, while not ByRef
#define IS_REF(type)    (type && ((type->dim || (type->cls)) && ((type->tag & KT_BYREF) == 0)))

KGC          *KObject::gc;

const TypeDef *KObject::objectType;
const TypeDef *KObject::nullType;

KObject KObject::nullObject;

//public
KObject::KObject(void)
	: type(NULL), vLong(0), length(0)
{
}

//public
KObject::~KObject(void)
{
	this->clean();
}

//public
KObject::KObject(const KObject &obj)
	: type(obj.type), length(obj.length)
{
	if (IS_STRING(obj.type))
		this->vString = KObject::strdup(obj.vString, obj.length);
}

//public
KObject::KObject(const TypeDef *type)
	: type(type), vLong(0), length(0)
{
}

//public
KObject & KObject::operator=(const KObject &obj)
{
	if (this != &obj)
	{
		this->clean();

		this->type = obj.type;
		this->length = obj.length;
		this->vLong = obj.vLong;
		if (IS_STRING(obj.type))
			this->vString = KObject::strdup(obj.vString, obj.length);
	}

	return *this;
}

//public
const TypeDef * KObject::getType(void) const
{
	return this->type;
}

//public
kuint_t KObject::getLength(void) const
{
	return this->length;
}

//public
kbool_t KObject::getBool(void) const
{
	return this->vBool;
}

//public
kchar_t KObject::getChar(void) const
{
	return this->vChar;
}

//public
kbyte_t KObject::getByte(void) const
{
	return this->vByte;
}

//public
ksbyte_t KObject::getSByte(void) const
{
	return this->vSByte;
}

//public
kshort_t KObject::getShort(void) const
{
	return this->vShort;
}

//public
kushort_t KObject::getUShort(void) const
{
	return this->vUShort;
}

//public
kint_t KObject::getInt(void) const
{
	return this->vInt;
}

//public
kuint_t KObject::getUInt(void) const
{
	return this->vUInt;
}

//public
klong_t KObject::getLong(void) const
{
	return this->vLong;
}

//public
kulong_t KObject::getULong(void) const
{
	return this->vULong;
}

//public
kfloat_t KObject::getFloat(void) const
{
	return this->vFloat;
}

//public
kdouble_t KObject::getDouble(void) const
{
	return this->vDouble;
}

//public
kstring_t KObject::getString(void) const
{
	return this->vString;
}

//public
KObject * KObject::getRef(void) const
{
	return this->vObj;
}

//public
kref_t KObject::getRaw(void) const
{
	return this->vRaw;
}

//public
KObject & KObject::getField(ktoken16_t tok) const
{
	return this->vObj[tok];
}

//public
KObject & KObject::getElement(knuint_t idx) const
{
	return this->vObj[idx];
}

//public
knint_t KObject::getNInt(void) const
{
#ifdef IS64BIT
	return (knint_t)this->vLong;
#else
	return (knint_t)this->vInt;
#endif
}

//public
knuint_t KObject::getNUInt(void) const
{
#ifdef IS64BIT
	return (knuint_t)this->vULong;
#else
	return (knuint_t)this->vUInt;
#endif
}

//public
void KObject::setBool(kbool_t val)
{
	this->clean();
	this->vBool = val;
}

//public
void KObject::setChar(kchar_t val)
{
	this->clean();
	this->vChar = val;
}

//public
void KObject::setByte(kbyte_t val)
{
	this->clean();
	this->vByte = val;
}

//public
void KObject::setSByte(ksbyte_t val)
{
	this->clean();
	this->vSByte = val;
}

//public
void KObject::setShort(kshort_t val)
{
	this->clean();
	this->vShort = val;
}

//public
void KObject::setUShort(kushort_t val)
{
	this->clean();
	this->vUShort = val;
}

//public
void KObject::setInt(kint_t val)
{
	this->clean();
	this->vInt = val;
}

//public
void KObject::setUInt(kuint_t val)
{
	this->clean();
	this->vUInt = val;
}

//public
void KObject::setLong(klong_t val)
{
	this->clean();
	this->vLong = val;
}

//public
void KObject::setULong(kulong_t val)
{
	this->clean();
	this->vULong = val;
}

//public
void KObject::setFloat(kfloat_t val)
{
	this->clean();
	this->vFloat = val;
}

//public
void KObject::setDouble(kdouble_t val)
{
	this->clean();
	this->vDouble = val;
}

//public
void KObject::setString(kstring_t val, kuint_t length)
{
	this->clean();

	this->length = length;
	this->vString = KObject::strdup(val, this->length);
}

//public
void KObject::setRef(KObject *ref, kuint_t length)
{
	this->clean();

	this->length = length;
	this->vObj = ref;
}

//public
void KObject::setRaw(kref_t raw)
{
	this->clean();
	this->vRaw = raw;
}

//public
void KObject::setField(ktoken16_t tok, const KObject &obj)
{
	this->vObj[tok] = obj;

	if (IS_REF(obj.type) && obj.vObj)
		KObject::gc->mark(obj.vObj);
}

//public
void KObject::setElement(knuint_t idx, const KObject &obj)
{
	this->vObj[idx] = obj;

	if (IS_REF(obj.type) && obj.vObj)
		KObject::gc->mark(obj.vObj);
}


//protected
void KObject::clean(void)
{
	if (IS_STRING(this->type) && this->vString)
	{
		delete [] this->vString;
		this->vLong = NULL; //long to ensure 64 bits are unset
	}
}

//protected static
knuint_t KObject::strlen(kstring_t s)
{
	if (!s)
		return 0;

	knuint_t len;
	for (len = 0; s[len]; ++len)
		;
	return len;
}

//protected static
kstring_t KObject::strdup(kstring_t src, knuint_t len)
{
	if (!src)
		return NULL;

	kchar_t *s = new kchar_t[len + 1];
	for (knuint_t i = 0; i < len; ++i)
		s[i] = src[i];
	s[len] = 0;
	return s;
}
