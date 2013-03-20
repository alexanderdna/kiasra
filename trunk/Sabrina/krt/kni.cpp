#include "kni.h"

#include "krt.h"
#include "kmeta.hpp"
#include "kenv.hpp"
#include "kobject.hpp"
#include "kstringutils.hpp"
#include "kmodule.hpp"

//===================================================
// Defines

#define Env	((KEnvironment *)hKEnv)
#define Cls ((ClassDef *)hKClass)
#define Del ((DelegateDef *)hKDelegate)
#define Fld ((FieldDef *)hKField)
#define Met ((MethodDef *)hKMethod)
#define Typ ((TypeDef *)hKType)

//===================================================

// Gets primitive values

KNI_API kbool_t KniGetLocalBool(kushort_t index)
{
	return KEnvironment::locals[index].getBool();
}

KNI_API kchar_t KniGetLocalChar(kushort_t index)
{
	return KEnvironment::locals[index].getChar();
}

KNI_API kbyte_t KniGetLocalByte(kushort_t index)
{
	return KEnvironment::locals[index].getByte();
}

KNI_API ksbyte_t KniGetLocalSByte(kushort_t index)
{
	return KEnvironment::locals[index].getSByte();
}

KNI_API kshort_t KniGetLocalShort(kushort_t index)
{
	return KEnvironment::locals[index].getShort();
}

KNI_API kushort_t KniGetLocalUShort(kushort_t index)
{
	return KEnvironment::locals[index].getShort();
}

KNI_API kint_t KniGetLocalInt(kushort_t index)
{
	return KEnvironment::locals[index].getInt();
}

KNI_API kuint_t KniGetLocalUInt(kushort_t index)
{
	return KEnvironment::locals[index].getUInt();
}

KNI_API klong_t KniGetLocalLong(kushort_t index)
{
	return KEnvironment::locals[index].getUInt();
}

KNI_API kulong_t KniGetLocalULong(kushort_t index)
{
	return KEnvironment::locals[index].getLong();
}

KNI_API kfloat_t KniGetLocalFloat(kushort_t index)
{
	return KEnvironment::locals[index].getFloat();
}

KNI_API kdouble_t KniGetLocalDouble(kushort_t index)
{
	return KEnvironment::locals[index].getDouble();
}

KNI_API kstring_t KniGetLocalString(kushort_t index)
{
	return KEnvironment::locals[index].getString();
}

KNI_API kref_t KniGetLocalRaw(kushort_t index)
{
	return KEnvironment::locals[index].getRaw();
}

KNI_API knuint_t KniGetLocalLength(kushort_t index)
{
	return KEnvironment::locals[index].getLength();
}

//===================================================

KNI_API kbool_t KniGetArgBool(kushort_t index)
{
	return KEnvironment::args[index].getBool();
}

KNI_API kchar_t KniGetArgChar(kushort_t index)
{
	return KEnvironment::args[index].getChar();
}

KNI_API kbyte_t KniGetArgByte(kushort_t index)
{
	return KEnvironment::args[index].getByte();
}

KNI_API ksbyte_t KniGetArgSByte(kushort_t index)
{
	return KEnvironment::args[index].getSByte();
}

KNI_API kshort_t KniGetArgShort(kushort_t index)
{
	return KEnvironment::args[index].getShort();
}

KNI_API kushort_t KniGetArgUShort(kushort_t index)
{
	return KEnvironment::args[index].getUShort();
}

KNI_API kint_t KniGetArgInt(kushort_t index)
{
	return KEnvironment::args[index].getInt();
}

KNI_API kuint_t KniGetArgUInt(kushort_t index)
{
	return KEnvironment::args[index].getUInt();
}

KNI_API klong_t KniGetArgLong(kushort_t index)
{
	return KEnvironment::args[index].getLong();
}

KNI_API kulong_t KniGetArgULong(kushort_t index)
{
	return KEnvironment::args[index].getULong();
}

KNI_API kfloat_t KniGetArgFloat(kushort_t index)
{
	return KEnvironment::args[index].getFloat();
}

KNI_API kdouble_t KniGetArgDouble(kushort_t index)
{
	return KEnvironment::args[index].getDouble();
}

KNI_API kstring_t KniGetArgString(kushort_t index)
{
	return KEnvironment::args[index].getString();
}

KNI_API kref_t KniGetArgRaw(kushort_t index)
{
	return KEnvironment::args[index].getRaw();
}

KNI_API knuint_t KniGetArgLength(kushort_t index)
{
	return KEnvironment::args[index].getLength();
}

//===================================================

#define CHECK_GET_FIELD(val) if (obj.getType() == KObject::nullType) \
	{ \
		KniThrowException(KEnvironment::exceptions.nullReference); \
		return val; \
	}

KNI_API kbool_t KniGetFieldBool(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(false);

	return obj.getField(Fld->localIndex).getBool();
}

KNI_API kchar_t KniGetFieldChar(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getChar();
}

KNI_API kbyte_t KniGetFieldByte(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getByte();
}

KNI_API ksbyte_t KniGetFieldSByte(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getSByte();
}

KNI_API kshort_t KniGetFieldShort(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getShort();
}

KNI_API kushort_t KniGetFieldUShort(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getUShort();
}

KNI_API kint_t KniGetFieldInt(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getInt();
}

KNI_API kuint_t KniGetFieldUInt(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getUInt();
}

KNI_API klong_t KniGetFieldLong(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getLong();
}

KNI_API kulong_t KniGetFieldULong(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getULong();
}

KNI_API kfloat_t KniGetFieldFloat(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getFloat();
}

KNI_API kdouble_t KniGetFieldDouble(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getDouble();
}

KNI_API kstring_t KniGetFieldString(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(NULL);

	return obj.getField(Fld->localIndex).getString();
}

KNI_API kref_t KniGetFieldRaw(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(NULL);

	return obj.getField(Fld->localIndex).getRaw();
}

KNI_API knuint_t KniGetFieldLength(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getLength();
}

//===================================================

KNI_API kbool_t KniGetStaticFieldBool(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getBool();
}

KNI_API kchar_t KniGetStaticFieldChar(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getChar();
}

KNI_API kbyte_t KniGetStaticFieldByte(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getByte();
}

KNI_API ksbyte_t KniGetStaticFieldSByte(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getSByte();
}

KNI_API kshort_t KniGetStaticFieldShort(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getShort();
}

KNI_API kushort_t KniGetStaticFieldUShort(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getUShort();
}

KNI_API kint_t KniGetStaticFieldInt(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getInt();
}

KNI_API kuint_t KniGetStaticFieldUInt(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getUInt();
}

KNI_API klong_t KniGetStaticFieldLong(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getLong();
}

KNI_API kulong_t KniGetStaticFieldULong(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getULong();
}

KNI_API kfloat_t KniGetStaticFieldFloat(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getFloat();
}

KNI_API kdouble_t KniGetStaticFieldDouble(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getDouble();
}

KNI_API kstring_t KniGetStaticFieldString(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getString();
}

KNI_API kref_t KniGetStaticFieldRaw(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getRaw();
}

KNI_API knuint_t KniGetStaticFieldLength(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getLength();
}

//===================================================

#define CHECK_GET_ELEMENT(val) if (obj.getType() == KObject::nullType) \
	{ \
		KniThrowException(KEnvironment::exceptions.nullReference); \
		return val; \
	}

#define CHECK_GET_INDEX(val) if (index > obj.getLength() - 1) \
	{ \
		KniThrowException(KEnvironment::exceptions.indexOutOfRange); \
		return val; \
	}

KNI_API kbool_t KniGetElementBool(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(false);
	CHECK_GET_INDEX(false);

	return obj.getElement(index).getBool();
}

KNI_API kchar_t KniGetElementChar(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getChar();
}

KNI_API kbyte_t KniGetElementByte(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getByte();
}

KNI_API ksbyte_t KniGetElementSByte(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getSByte();
}

KNI_API kshort_t KniGetElementShort(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getShort();
}

KNI_API kushort_t KniGetElementUShort(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getUShort();
}

KNI_API kint_t KniGetElementInt(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getInt();
}

KNI_API kuint_t KniGetElementUInt(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getUInt();
}

KNI_API klong_t KniGetElementLong(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getLong();
}

KNI_API kulong_t KniGetElementULong(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getULong();
}

KNI_API kfloat_t KniGetElementFloat(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT((kfloat_t)0);
	CHECK_GET_INDEX((kfloat_t)0);

	return obj.getElement(index).getFloat();
}

KNI_API kdouble_t KniGetElementDouble(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT((kdouble_t)0);
	CHECK_GET_INDEX((kdouble_t)0);

	return obj.getElement(index).getDouble();
}

KNI_API kstring_t KniGetElementString(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(NULL);
	CHECK_GET_INDEX(NULL);

	return obj.getElement(index).getString();
}

KNI_API kref_t KniGetElementRaw(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(NULL);
	CHECK_GET_INDEX(NULL);

	return obj.getElement(index).getRaw();
}

KNI_API knuint_t KniGetElementLength(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getLength();
}

//===================================================

KNI_API kbool_t KniGetIndirectBool(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getBool();
}

KNI_API kchar_t KniGetIndirectChar(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getChar();
}

KNI_API kbyte_t KniGetIndirectByte(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getByte();
}

KNI_API ksbyte_t KniGetIndirectSByte(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getSByte();
}

KNI_API kshort_t KniGetIndirectShort(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getShort();
}

KNI_API kushort_t KniGetIndirectUShort(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getUShort();
}

KNI_API kint_t KniGetIndirectInt(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getInt();
}

KNI_API kuint_t KniGetIndirectUInt(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getUInt();
}

KNI_API klong_t KniGetIndirectLong(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getLong();
}

KNI_API kulong_t KniGetIndirectULong(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getULong();
}

KNI_API kfloat_t KniGetIndirectFloat(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getFloat();
}

KNI_API kdouble_t KniGetIndirectDouble(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getDouble();
}

KNI_API kstring_t KniGetIndirectString(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getString();
}

KNI_API kref_t KniGetIndirectRaw(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getRaw();
}

KNI_API knuint_t KniGetIndirectLength(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getLength();
}


//===================================================

// Sets primitive values

KNI_API void KniSetLocalBool(kushort_t index, kbool_t val)
{
	KEnvironment::locals[index].setBool(val);
}

KNI_API void KniSetLocalChar(kushort_t index, kchar_t val)
{
	KEnvironment::locals[index].setChar(val);
}

KNI_API void KniSetLocalByte(kushort_t index, kbyte_t val)
{
	KEnvironment::locals[index].setByte(val);
}

KNI_API void KniSetLocalSByte(kushort_t index, ksbyte_t val)
{
	KEnvironment::locals[index].setSByte(val);
}

KNI_API void KniSetLocalShort(kushort_t index, kshort_t val)
{
	KEnvironment::locals[index].setShort(val);
}

KNI_API void KniSetLocalUShort(kushort_t index, kushort_t val)
{
	KEnvironment::locals[index].setUShort(val);
}

KNI_API void KniSetLocalInt(kushort_t index, kint_t val)
{
	KEnvironment::locals[index].setInt(val);
}

KNI_API void KniSetLocalUInt(kushort_t index, kuint_t val)
{
	KEnvironment::locals[index].setUInt(val);
}

KNI_API void KniSetLocalLong(kushort_t index, klong_t val)
{
	KEnvironment::locals[index].setLong(val);
}

KNI_API void KniSetLocalULong(kushort_t index, kulong_t val)
{
	KEnvironment::locals[index].setULong(val);
}

KNI_API void KniSetLocalFloat(kushort_t index, kfloat_t val)
{
	KEnvironment::locals[index].setFloat(val);
}

KNI_API void KniSetLocalDouble(kushort_t index, kdouble_t val)
{
	KEnvironment::locals[index].setDouble(val);
}

KNI_API void KniSetLocalString(kushort_t index, kstring_t val, kuint_t length)
{
	KEnvironment::locals[index].setString(val, length);
}

KNI_API void KniSetLocalRaw(kushort_t index, kref_t val)
{
	KEnvironment::locals[index].setRaw(val);
}

KNI_API void KniSetLocalNull(kushort_t index)
{
	KEnvironment::locals[index] = KObject::nullObject;
}

//===================================================

KNI_API void KniSetArgBool(kushort_t index, kbool_t val)
{
	KEnvironment::args[index].setBool(val);
}

KNI_API void KniSetArgChar(kushort_t index, kchar_t val)
{
	KEnvironment::args[index].setChar(val);
}

KNI_API void KniSetArgByte(kushort_t index, kbyte_t val)
{
	KEnvironment::args[index].setByte(val);
}

KNI_API void KniSetArgSByte(kushort_t index, ksbyte_t val)
{
	KEnvironment::args[index].setSByte(val);
}

KNI_API void KniSetArgShort(kushort_t index, kshort_t val)
{
	KEnvironment::args[index].setShort(val);
}

KNI_API void KniSetArgUShort(kushort_t index, kushort_t val)
{
	KEnvironment::args[index].setUShort(val);
}

KNI_API void KniSetArgInt(kushort_t index, kint_t val)
{
	KEnvironment::args[index].setInt(val);
}

KNI_API void KniSetArgUInt(kushort_t index, kuint_t val)
{
	KEnvironment::args[index].setUInt(val);
}

KNI_API void KniSetArgLong(kushort_t index, klong_t val)
{
	KEnvironment::args[index].setLong(val);
}

KNI_API void KniSetArgULong(kushort_t index, kulong_t val)
{
	KEnvironment::args[index].setULong(val);
}

KNI_API void KniSetArgFloat(kushort_t index, kfloat_t val)
{
	KEnvironment::args[index].setFloat(val);
}

KNI_API void KniSetArgDouble(kushort_t index, kdouble_t val)
{
	KEnvironment::args[index].setDouble(val);
}

KNI_API void KniSetArgString(kushort_t index, kstring_t val, kuint_t length)
{
	KEnvironment::args[index].setString(val, length);
}

KNI_API void KniSetArgRaw(kushort_t index, kref_t val)
{
	KEnvironment::args[index].setRaw(val);
}

KNI_API void KniSetArgNull(kushort_t index)
{
	KEnvironment::args[index] = KObject::nullObject;
}

//===================================================

#define CHECK_SET_FIELD if (obj.getType() == KObject::nullType) \
	{ \
		KniThrowException(KEnvironment::exceptions.nullReference); \
		return; \
	}

KNI_API void KniSetFieldBool(HKFIELD hKField, kbool_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setBool(val);
}

KNI_API void KniSetFieldChar(HKFIELD hKField, kchar_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setChar(val);
}

KNI_API void KniSetFieldByte(HKFIELD hKField, kbyte_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setByte(val);
}

KNI_API void KniSetFieldSByte(HKFIELD hKField, ksbyte_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setSByte(val);
}

KNI_API void KniSetFieldShort(HKFIELD hKField, kshort_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setShort(val);
}

KNI_API void KniSetFieldUShort(HKFIELD hKField, kushort_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setUShort(val);
}

KNI_API void KniSetFieldInt(HKFIELD hKField, kint_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setUShort(val);
}

KNI_API void KniSetFieldUInt(HKFIELD hKField, kuint_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setUInt(val);
}

KNI_API void KniSetFieldLong(HKFIELD hKField, klong_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setLong(val);
}

KNI_API void KniSetFieldULong(HKFIELD hKField, kulong_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setULong(val);
}

KNI_API void KniSetFieldFloat(HKFIELD hKField, kfloat_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setFloat(val);
}

KNI_API void KniSetFieldDouble(HKFIELD hKField, kdouble_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setDouble(val);
}

KNI_API void KniSetFieldString(HKFIELD hKField, kstring_t val, kuint_t length)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setString(val, length);
}

KNI_API void KniSetFieldRaw(HKFIELD hKField, kref_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setRaw(val);
}

KNI_API void KniSetFieldNull(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex) = KObject::nullObject;
}

//===================================================

KNI_API void KniSetStaticFieldBool(HKCLASS hKClass, HKFIELD hKField, kbool_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setBool(val);
}

KNI_API void KniSetStaticFieldChar(HKCLASS hKClass, HKFIELD hKField, kchar_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setChar(val);
}

KNI_API void KniSetStaticFieldByte(HKCLASS hKClass, HKFIELD hKField, kbyte_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setByte(val);
}

KNI_API void KniSetStaticFieldSByte(HKCLASS hKClass, HKFIELD hKField, ksbyte_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setSByte(val);
}

KNI_API void KniSetStaticFieldShort(HKCLASS hKClass, HKFIELD hKField, kshort_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setShort(val);
}

KNI_API void KniSetStaticFieldUShort(HKCLASS hKClass, HKFIELD hKField, kushort_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setUShort(val);
}

KNI_API void KniSetStaticFieldInt(HKCLASS hKClass, HKFIELD hKField, kint_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setInt(val);
}

KNI_API void KniSetStaticFieldUInt(HKCLASS hKClass, HKFIELD hKField, kuint_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setUInt(val);
}

KNI_API void KniSetStaticFieldLong(HKCLASS hKClass, HKFIELD hKField, klong_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setLong(val);
}

KNI_API void KniSetStaticFieldULong(HKCLASS hKClass, HKFIELD hKField, kulong_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setULong(val);
}

KNI_API void KniSetStaticFieldFloat(HKCLASS hKClass, HKFIELD hKField, kfloat_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setFloat(val);
}

KNI_API void KniSetStaticFieldDouble(HKCLASS hKClass, HKFIELD hKField, kdouble_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setDouble(val);
}

KNI_API void KniSetStaticFieldString(HKCLASS hKClass, HKFIELD hKField, kstring_t val, kuint_t length)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setString(val, length);
}

KNI_API void KniSetStaticFieldRaw(HKCLASS hKClass, HKFIELD hKField, kref_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setRaw(val);
}

KNI_API void KniSetStaticFieldNull(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex) = KObject::nullObject;
}

//===================================================

#define CHECK_SET_ELEMENT if (obj.getType() == KObject::nullType) \
	{ \
		KniThrowException(KEnvironment::exceptions.nullReference); \
		return; \
	}

#define CHECK_SET_INDEX if (index > obj.getLength() - 1) \
	{ \
		KniThrowException(KEnvironment::exceptions.indexOutOfRange); \
		return; \
	}

KNI_API void KniSetElementBool(knuint_t index, kbool_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setBool(val);
}

KNI_API void KniSetElementChar(knuint_t index, kchar_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setChar(val);
}

KNI_API void KniSetElementByte(knuint_t index, kbyte_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setByte(val);
}

KNI_API void KniSetElementSByte(knuint_t index, ksbyte_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setSByte(val);
}

KNI_API void KniSetElementShort(knuint_t index, kshort_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setShort(val);
}

KNI_API void KniSetElementUShort(knuint_t index, kushort_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setUShort(val);
}

KNI_API void KniSetElementInt(knuint_t index, kint_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setInt(val);
}

KNI_API void KniSetElementUInt(knuint_t index, kuint_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setUInt(val);
}

KNI_API void KniSetElementLong(knuint_t index, klong_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setLong(val);
}

KNI_API void KniSetElementULong(knuint_t index, kulong_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setULong(val);
}

KNI_API void KniSetElementFloat(knuint_t index, kfloat_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setFloat(val);
}

KNI_API void KniSetElementDouble(knuint_t index, kdouble_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setDouble(val);
}

KNI_API void KniSetElementString(knuint_t index, kstring_t val, kuint_t length)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setString(val, length);
}

KNI_API void KniSetElementRaw(knuint_t index, kref_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setRaw(val);
}

KNI_API void KniSetElementNull(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index) = KObject::nullObject;
}

//===================================================

KNI_API void KniSetIndirectBool(kbool_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setBool(val);
}

KNI_API void KniSetIndirectChar(kchar_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setChar(val);
}

KNI_API void KniSetIndirectByte(kbyte_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setByte(val);
}

KNI_API void KniSetIndirectSByte(ksbyte_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setSByte(val);
}

KNI_API void KniSetIndirectShort(kshort_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setShort(val);
}

KNI_API void KniSetIndirectUShort(kushort_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setUShort(val);
}

KNI_API void KniSetIndirectInt(kint_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setInt(val);
}

KNI_API void KniSetIndirectUInt(kuint_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setUInt(val);
}

KNI_API void KniSetIndirectLong(klong_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setLong(val);
}

KNI_API void KniSetIndirectULong(kulong_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setULong(val);
}

KNI_API void KniSetIndirectFloat(kfloat_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setFloat(val);
}

KNI_API void KniSetIndirectDouble(kdouble_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setDouble(val);
}

KNI_API void KniSetIndirectString(kstring_t val, kuint_t length)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setString(val, length);
}

KNI_API void KniSetIndirectRaw(kref_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setRaw(val);
}

KNI_API void KniSetIndirectNull(void)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->operator=(KObject::nullObject);
}


//===================================================

// Loads values onto stack

KNI_API void KniLoadBool(kbool_t val)
{
	KEnvironment::stackPushBool(val);
}

KNI_API void KniLoadChar(kchar_t val)
{
	KEnvironment::stackPushChar(val);
}

KNI_API void KniLoadByte(kbyte_t val)
{
	KEnvironment::stackPushByte(val);
}

KNI_API void KniLoadSByte(ksbyte_t val)
{
	KEnvironment::stackPushSByte(val);
}

KNI_API void KniLoadShort(kshort_t val)
{
	KEnvironment::stackPushShort(val);
}

KNI_API void KniLoadUShort(kushort_t val)
{
	KEnvironment::stackPushUShort(val);
}

KNI_API void KniLoadInt(kint_t val)
{
	KEnvironment::stackPushInt(val);
}

KNI_API void KniLoadUInt(kuint_t val)
{
	KEnvironment::stackPushUInt(val);
}

KNI_API void KniLoadLong(klong_t val)
{
	KEnvironment::stackPushLong(val);
}

KNI_API void KniLoadULong(kulong_t val)
{
	KEnvironment::stackPushULong(val);
}

KNI_API void KniLoadFloat(kfloat_t val)
{
	KEnvironment::stackPushFloat(val);
}

KNI_API void KniLoadDouble(kdouble_t val)
{
	KEnvironment::stackPushDouble(val);
}

KNI_API void KniLoadString(kstring_t val, kuint_t length)
{
	KEnvironment::stackPushString(val, length);
}

KNI_API void KniLoadRaw(kref_t val)
{
	KEnvironment::stackPushRaw(val);
}

//===================================================

KNI_API void KniLoadLocal(kushort_t index)
{
	KEnvironment::stackPush(KEnvironment::locals[index]);
}

KNI_API void KniLoadArg(kushort_t index)
{
	KEnvironment::stackPush(KEnvironment::args[index]);
}

KNI_API void KniLoadArgByRef(kushort_t index)
{
	KEnvironment::stackPush(*KEnvironment::args[index].getRef());
}

KNI_API void KniLoadField(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPush(obj.getField(Fld->localIndex));
}

KNI_API void KniLoadStaticField(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	KEnvironment::stackPush(obj.getField(Fld->localIndex));
}

KNI_API void KniLoadElement(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPush(obj.getElement(index));
}

KNI_API void KniLoadIndirect(void)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPush(*obj.getRef());
}

//===================================================

KNI_API void KniLoadLocalLength(kushort_t index)
{
	KEnvironment::stackPushUInt(KEnvironment::locals[index].getLength());
}

KNI_API void KniLoadArgLength(kushort_t index)
{
	KEnvironment::stackPushUInt(KEnvironment::args[index].getLength());
}

KNI_API void KniLoadArgLengthByRef(kushort_t index)
{
	KEnvironment::stackPushUInt(KEnvironment::args[index].getRef()->getLength());
}

KNI_API void KniLoadFieldLength(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPushUInt(obj.getField(Fld->localIndex).getLength());
}

KNI_API void KniLoadStaticFieldLength(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	KEnvironment::stackPushUInt(obj.getField(Fld->localIndex).getLength());
}

KNI_API void KniLoadElementLength(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPushUInt(obj.getElement(index).getLength());
}

KNI_API void KniLoadIndirectLength(void)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPushUInt(obj.getRef()->getLength());
}

//===================================================

KNI_API void KniLoadLocalElement(kushort_t index, knuint_t elemIndex)
{
	KEnvironment::stackPush(KEnvironment::locals[index].getElement(elemIndex));
}

KNI_API void KniLoadArgElement(kushort_t index, knuint_t elemIndex)
{
	KEnvironment::stackPush(KEnvironment::args[index].getElement(elemIndex));
}

KNI_API void KniLoadArgElementByRef(kushort_t index, knuint_t elemIndex)
{
	KEnvironment::stackPush(KEnvironment::args[index].getRef()->getElement(elemIndex));
}

KNI_API void KniLoadFieldElement(HKFIELD hKField, knuint_t elemIndex)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPush(obj.getField(Fld->localIndex).getElement(elemIndex));
}

KNI_API void KniLoadStaticFieldElement(HKCLASS hKClass, HKFIELD hKField, knuint_t elemIndex)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	KEnvironment::stackPush(obj.getField(Fld->localIndex).getElement(elemIndex));
}

KNI_API void KniLoadElementElement(knuint_t index, knuint_t elemIndex)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPush(obj.getElement(index).getElement(elemIndex));
}

KNI_API void KniLoadIndirectElement(knuint_t elemIndex)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPush(obj.getRef()->getElement(elemIndex));
}

//===================================================

KNI_API void KniLoadLocalAddress(kushort_t index)
{
	KEnvironment::stackPushAddress(&KEnvironment::locals[index]);
}

KNI_API void KniLoadArgAddress(kushort_t index)
{
	KEnvironment::stackPushAddress(&KEnvironment::args[index]);
}

KNI_API void KniLoadFieldAddress(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPushAddress(&obj.getField(Fld->localIndex));
}

KNI_API void KniLoadStaticFieldAddress(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	KEnvironment::stackPushAddress(&obj.getField(Fld->localIndex));
}

KNI_API void KniLoadElementAddress(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPushAddress(&obj.getElement(index));
}

KNI_API void KniLoadNull(void)
{
	KEnvironment::stackPushNull();
}

KNI_API void KniLoadNewObject(HKCLASS hKClass)
{
	KObject obj;
	KEnvironment::allocClassInstance(Cls, obj);
	KEnvironment::stackPush(obj);
	KEnvironment::invokeLastThis(Cls->ctor);
}

KNI_API void KniLoadNewDelegate(HKDELEGATE hKDelegate, HKMETHOD hKMethod)
{
	KObject obj;
	if (Met->addr & KMA_STATIC)
		KEnvironment::allocDelegateInstance(Del, NULL, Met, obj);
	else
		KEnvironment::allocDelegateInstance(Del, &KEnvironment::stackPop(), Met, obj);

	KEnvironment::stackPush(obj);
}

KNI_API void KniLoadNewArray(HKTYPE hKType, kuint_t length)
{
	KObject obj;
	KEnvironment::allocArray(Typ, length, obj);
	KEnvironment::stackPush(obj);
}

KNI_API void KniLoadNewArrayBaking(HKTYPE hKType, kuint_t length)
{
	KObject obj;
	KEnvironment::allocArrayBaking(Typ, length, obj);
	KEnvironment::stackPush(obj);
}

//===================================================

// Stores values

KNI_API void KniStoreLocal(kushort_t index)
{
	const KObject &val = KEnvironment::stackPop();
	KEnvironment::locals[index] = val;
}

KNI_API void KniStoreArg(kushort_t index)
{
	const KObject &val = KEnvironment::stackPop();
	KEnvironment::args[index] = val;
}

KNI_API void KniStoreArgByRef(kushort_t index)
{
	const KObject &val = KEnvironment::stackPop();
	KEnvironment::args[index].getRef()->operator=(val);
}

KNI_API void KniStoreField(HKFIELD hKField)
{
	const KObject &val = KEnvironment::stackPop();
	const KObject &obj = KEnvironment::stackPop();
	obj.getField(Fld->localIndex) = val;
}

KNI_API void KniStoreStaticField(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &val = KEnvironment::stackPop();
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex) = val;
}

KNI_API void KniStoreElement(knuint_t index)
{
	const KObject &val = KEnvironment::stackPop();
	const KObject &obj = KEnvironment::stackPop();
	obj.getElement(index) = val;
}

KNI_API void KniStoreIndirect(void)
{
	const KObject &val = KEnvironment::stackPop();
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->operator=(val);
}

//===================================================

// Tests values

KNI_API kbool_t KniIsNull(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef() == NULL;
}

KNI_API kbool_t KniIsNullLocal(kushort_t index)
{
	return KEnvironment::locals[index].getRef() == NULL;
}

KNI_API kbool_t KniIsNullArg(kushort_t index)
{
	return KEnvironment::args[index].getRef() == NULL;
}

KNI_API kbool_t KniIsNullArgByRef(kushort_t index)
{
	return KEnvironment::args[index].getRef()->getRef() == NULL;
}

KNI_API kbool_t KniIsNullField(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getField(Fld->localIndex).getRef() == NULL;
}

KNI_API kbool_t KniIsNullStaticField(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getRef() == NULL;
}

KNI_API kbool_t KniIsNullElement(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getElement(index).getRef() == NULL;
}

KNI_API kbool_t KniIsNullIndirect(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getRef() == NULL;
}

KNI_API kbool_t KniIsReferenceEqual(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();
	return a.getRef() == b.getRef();
}

KNI_API kbool_t KniIsClass(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return (obj.getType()->tag & KT_SCALAR_MASK) == KT_CLASS;
}

KNI_API kbool_t KniIsDelegate(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return (obj.getType()->tag & KT_SCALAR_MASK) == KT_DELEGATE;
}

KNI_API kbool_t KniIsArray(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getType()->dim != 0;
}


KNI_API kbool_t KniIsInstanceOfClass(HKCLASS hKClass)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getType()->cls == Cls && obj.getType()->dim == 0;
}

KNI_API kbool_t KniIsInstanceOfDelegate(HKDELEGATE hKDelegate)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getType()->del == Del && obj.getType()->dim == 0;
}

//===================================================

// Invokes

KNI_API KRESULT KniInvoke(HKMETHOD hKMethod)
{
	return KEnvironment::invoke(Met);
}

KNI_API KRESULT KniInvokeObject(HKDELEGATE hKDelegate)
{
	return KEnvironment::invokeDelegate(Del);
}

KNI_API kbool_t KniHasException(void)
{
	return KEnvironment::hasException;
}

KNI_API void KniPrintExceptionDescription(void)
{
	KEnvironment::printException();
}

KNI_API void KniClearException(void)
{
	KEnvironment::hasException = false;
	KEnvironment::exc->operator=(KObject::nullObject);
}

KNI_API void KniThrowException(HKCLASS excType)
{
	KObject obj;
	KEnvironment::allocClassInstance((ClassDef *)excType, obj);

	KEnvironment::stackPush(obj);
	KEnvironment::invokeExceptionCtor(((ClassDef *)excType)->ctor);

	KEnvironment::throwException();
}

KNI_API void KniThrowExceptionEx(HKCLASS excType)
{
	// stack transition
	// ..., extra, { excType ctor args } -> ..., exc

	KObject objExtra = KEnvironment::stackPop();

	KObject obj;
	KEnvironment::allocClassInstance((ClassDef *)excType, obj);

	KEnvironment::stackPush(obj);
	KEnvironment::invokeExceptionCtor(((ClassDef *)excType)->ctor);

	const FieldDef *extra = KEnvironment::findField((ClassDef *)excType, L"extra");
	if (extra && extra->declType == KEnvironment::objectType)
		obj.setField(extra->localIndex, objExtra);

	KEnvironment::throwException();
}

KNI_API void KniThrowExceptionGeneral(kstring_t message, kuint_t length)
{
	KObject obj;
	KEnvironment::allocClassInstance(KEnvironment::exceptions.general, obj);

	KEnvironment::stackPushString(message, length);
	KEnvironment::stackPush(obj);
	KEnvironment::invokeExceptionCtor(KEnvironment::exceptions.general->ctor);

	KEnvironment::throwException();
}

KNI_API void KniInitLocals(HKTYPE *pHKType, kushort_t count)
{
	KEnvironment::initLocals((const TypeDef **)pHKType, count);
}

//===================================================

// Metadata and types

KNI_API HKTYPE KniGetType(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getType();
}

KNI_API HKTYPE KniGetTypeLocal(kushort_t index)
{
	return KEnvironment::locals[index].getType();
}

KNI_API HKTYPE KniGetTypeArg(kushort_t index)
{
	return KEnvironment::args[index].getType();
}

KNI_API HKTYPE KniGetTypeArgByRef(kushort_t index)
{
	return KEnvironment::args[index].getRef()->getType();
}

KNI_API HKTYPE KniGetTypeField(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getField(Fld->localIndex).getType();
}

KNI_API HKTYPE KniGetTypeStaticField(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Fld->localIndex];
	return obj.getField(Fld->localIndex).getType();
}

KNI_API HKTYPE KniGetTypeElement(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getElement(index).getType();
}

KNI_API HKTYPE KniGetTypeIndirect(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getType();
}


KNI_API HKCLASS KniGetClass(kstring_t ksName)
{
	return KEnvironment::findClass(ksName, KEnvironment::rootModule->getModule());
}

KNI_API HKDELEGATE KniGetDelegate(kstring_t ksName)
{
	return KEnvironment::findDelegate(ksName, KEnvironment::rootModule->getModule());
}

KNI_API HKFIELD KniGetField(HKCLASS hKClass, kstring_t ksName)
{
	return KEnvironment::findField(Cls, ksName);
}

KNI_API HKMETHOD KniGetMethod(HKCLASS hKClass, kstring_t ksName)
{
	return KEnvironment::findMethod(Cls, ksName);
}


KNI_API HKTYPE KniGetPrimitiveType(KTYPETAG tag)
{
	return &KEnvironment::primitiveTypes[tag];
}

KNI_API HKTYPE KniCreateType(KTYPETAG tag, kushort_t dim, HKUSERTYPE hKClassOrDelegate)
{
	if ((tag & KT_SCALAR_MASK) == KT_CLASS)
		return KEnvironment::createType(tag, dim, (ClassDef *)hKClassOrDelegate);
	else if ((tag & KT_SCALAR_MASK) == KT_DELEGATE)
		return KEnvironment::createType(tag, dim, (DelegateDef *)hKClassOrDelegate);
	else
		return KEnvironment::createType(tag, dim);
}

KNI_API kbool_t KniIsClassType(HKTYPE hKType)
{
	return (Typ->tag & KT_SCALAR_MASK) == KT_CLASS && !Typ->dim;
}

KNI_API kbool_t KniIsDelegateType(HKTYPE hKType)
{
	return (Typ->tag & KT_SCALAR_MASK) == KT_DELEGATE && !Typ->dim;
}

KNI_API kbool_t KniIsArrayType(HKTYPE hKType)
{
	return Typ->dim != 0;
}
