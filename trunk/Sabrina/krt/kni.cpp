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

kbool_t KNI_API KniGetLocalBool(kushort_t index)
{
	return KEnvironment::locals[index].getBool();
}

kchar_t KNI_API KniGetLocalChar(kushort_t index)
{
	return KEnvironment::locals[index].getChar();
}

kbyte_t KNI_API KniGetLocalByte(kushort_t index)
{
	return KEnvironment::locals[index].getByte();
}

ksbyte_t KNI_API KniGetLocalSByte(kushort_t index)
{
	return KEnvironment::locals[index].getSByte();
}

kshort_t KNI_API KniGetLocalShort(kushort_t index)
{
	return KEnvironment::locals[index].getShort();
}

kushort_t KNI_API KniGetLocalUShort(kushort_t index)
{
	return KEnvironment::locals[index].getShort();
}

kint_t KNI_API KniGetLocalInt(kushort_t index)
{
	return KEnvironment::locals[index].getInt();
}

kuint_t KNI_API KniGetLocalUInt(kushort_t index)
{
	return KEnvironment::locals[index].getUInt();
}

klong_t KNI_API KniGetLocalLong(kushort_t index)
{
	return KEnvironment::locals[index].getUInt();
}

kulong_t KNI_API KniGetLocalULong(kushort_t index)
{
	return KEnvironment::locals[index].getLong();
}

kfloat_t KNI_API KniGetLocalFloat(kushort_t index)
{
	return KEnvironment::locals[index].getFloat();
}

kdouble_t KNI_API KniGetLocalDouble(kushort_t index)
{
	return KEnvironment::locals[index].getDouble();
}

kstring_t KNI_API KniGetLocalString(kushort_t index)
{
	return KEnvironment::locals[index].getString();
}

kref_t KNI_API KniGetLocalRaw(kushort_t index)
{
	return KEnvironment::locals[index].getRaw();
}

knuint_t KNI_API KniGetLocalLength(kushort_t index)
{
	return KEnvironment::locals[index].getLength();
}

//===================================================

kbool_t KNI_API KniGetArgBool(kushort_t index)
{
	return KEnvironment::args[index].getBool();
}

kchar_t KNI_API KniGetArgChar(kushort_t index)
{
	return KEnvironment::args[index].getChar();
}

kbyte_t KNI_API KniGetArgByte(kushort_t index)
{
	return KEnvironment::args[index].getByte();
}

ksbyte_t KNI_API KniGetArgSByte(kushort_t index)
{
	return KEnvironment::args[index].getSByte();
}

kshort_t KNI_API KniGetArgShort(kushort_t index)
{
	return KEnvironment::args[index].getShort();
}

kushort_t KNI_API KniGetArgUShort(kushort_t index)
{
	return KEnvironment::args[index].getUShort();
}

kint_t KNI_API KniGetArgInt(kushort_t index)
{
	return KEnvironment::args[index].getInt();
}

kuint_t KNI_API KniGetArgUInt(kushort_t index)
{
	return KEnvironment::args[index].getUInt();
}

klong_t KNI_API KniGetArgLong(kushort_t index)
{
	return KEnvironment::args[index].getLong();
}

kulong_t KNI_API KniGetArgULong(kushort_t index)
{
	return KEnvironment::args[index].getULong();
}

kfloat_t KNI_API KniGetArgFloat(kushort_t index)
{
	return KEnvironment::args[index].getFloat();
}

kdouble_t KNI_API KniGetArgDouble(kushort_t index)
{
	return KEnvironment::args[index].getDouble();
}

kstring_t KNI_API KniGetArgString(kushort_t index)
{
	return KEnvironment::args[index].getString();
}

kref_t KNI_API KniGetArgRaw(kushort_t index)
{
	return KEnvironment::args[index].getRaw();
}

knuint_t KNI_API KniGetArgLength(kushort_t index)
{
	return KEnvironment::args[index].getLength();
}

//===================================================

#define CHECK_GET_FIELD(val) if (obj.getType() == KObject::nullType) \
	{ \
		KniThrowException(KEnvironment::exceptions.nullReference); \
		return val; \
	}

kbool_t KNI_API KniGetFieldBool(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(false);

	return obj.getField(Fld->localIndex).getBool();
}

kchar_t KNI_API KniGetFieldChar(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getChar();
}

kbyte_t KNI_API KniGetFieldByte(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getByte();
}

ksbyte_t KNI_API KniGetFieldSByte(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getSByte();
}

kshort_t KNI_API KniGetFieldShort(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getShort();
}

kushort_t KNI_API KniGetFieldUShort(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getUShort();
}

kint_t KNI_API KniGetFieldInt(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getInt();
}

kuint_t KNI_API KniGetFieldUInt(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getUInt();
}

klong_t KNI_API KniGetFieldLong(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getLong();
}

kulong_t KNI_API KniGetFieldULong(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getULong();
}

kfloat_t KNI_API KniGetFieldFloat(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getFloat();
}

kdouble_t KNI_API KniGetFieldDouble(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(NULL);

	return obj.getField(Fld->localIndex).getDouble();
}

kstring_t KNI_API KniGetFieldString(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(NULL);

	return obj.getField(Fld->localIndex).getString();
}

kref_t KNI_API KniGetFieldRaw(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(NULL);

	return obj.getField(Fld->localIndex).getRaw();
}

knuint_t KNI_API KniGetFieldLength(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_FIELD(0);

	return obj.getField(Fld->localIndex).getLength();
}

//===================================================

kbool_t KNI_API KniGetStaticFieldBool(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getBool();
}

kchar_t KNI_API KniGetStaticFieldChar(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getChar();
}

kbyte_t KNI_API KniGetStaticFieldByte(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getByte();
}

ksbyte_t KNI_API KniGetStaticFieldSByte(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getSByte();
}

kshort_t KNI_API KniGetStaticFieldShort(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getShort();
}

kushort_t KNI_API KniGetStaticFieldUShort(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getUShort();
}

kint_t KNI_API KniGetStaticFieldInt(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getInt();
}

kuint_t KNI_API KniGetStaticFieldUInt(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getUInt();
}

klong_t KNI_API KniGetStaticFieldLong(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getLong();
}

kulong_t KNI_API KniGetStaticFieldULong(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getULong();
}

kfloat_t KNI_API KniGetStaticFieldFloat(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getFloat();
}

kdouble_t KNI_API KniGetStaticFieldDouble(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getDouble();
}

kstring_t KNI_API KniGetStaticFieldString(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getString();
}

kref_t KNI_API KniGetStaticFieldRaw(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getRaw();
}

knuint_t KNI_API KniGetStaticFieldLength(HKCLASS hKClass, HKFIELD hKField)
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

kbool_t KNI_API KniGetElementBool(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(false);
	CHECK_GET_INDEX(false);

	return obj.getElement(index).getBool();
}

kchar_t KNI_API KniGetElementChar(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getChar();
}

kbyte_t KNI_API KniGetElementByte(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getByte();
}

ksbyte_t KNI_API KniGetElementSByte(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getSByte();
}

kshort_t KNI_API KniGetElementShort(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getShort();
}

kushort_t KNI_API KniGetElementUShort(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getUShort();
}

kint_t KNI_API KniGetElementInt(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getInt();
}

kuint_t KNI_API KniGetElementUInt(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getUInt();
}

klong_t KNI_API KniGetElementLong(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getLong();
}

kulong_t KNI_API KniGetElementULong(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getULong();
}

kfloat_t KNI_API KniGetElementFloat(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT((kfloat_t)0);
	CHECK_GET_INDEX((kfloat_t)0);

	return obj.getElement(index).getFloat();
}

kdouble_t KNI_API KniGetElementDouble(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT((kdouble_t)0);
	CHECK_GET_INDEX((kdouble_t)0);

	return obj.getElement(index).getDouble();
}

kstring_t KNI_API KniGetElementString(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(NULL);
	CHECK_GET_INDEX(NULL);

	return obj.getElement(index).getString();
}

kref_t KNI_API KniGetElementRaw(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(NULL);
	CHECK_GET_INDEX(NULL);

	return obj.getElement(index).getRaw();
}

knuint_t KNI_API KniGetElementLength(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_GET_ELEMENT(0);
	CHECK_GET_INDEX(0);

	return obj.getElement(index).getLength();
}

//===================================================

kbool_t KNI_API KniGetIndirectBool(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getBool();
}

kchar_t KNI_API KniGetIndirectChar(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getChar();
}

kbyte_t KNI_API KniGetIndirectByte(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getByte();
}

ksbyte_t KNI_API KniGetIndirectSByte(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getSByte();
}

kshort_t KNI_API KniGetIndirectShort(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getShort();
}

kushort_t KNI_API KniGetIndirectUShort(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getUShort();
}

kint_t KNI_API KniGetIndirectInt(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getInt();
}

kuint_t KNI_API KniGetIndirectUInt(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getUInt();
}

klong_t KNI_API KniGetIndirectLong(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getLong();
}

kulong_t KNI_API KniGetIndirectULong(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getULong();
}

kfloat_t KNI_API KniGetIndirectFloat(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getFloat();
}

kdouble_t KNI_API KniGetIndirectDouble(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getDouble();
}

kstring_t KNI_API KniGetIndirectString(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getString();
}

kref_t KNI_API KniGetIndirectRaw(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getRaw();
}

knuint_t KNI_API KniGetIndirectLength(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getLength();
}


//===================================================

// Sets primitive values

void KNI_API KniSetLocalBool(kushort_t index, kbool_t val)
{
	KEnvironment::locals[index].setBool(val);
}

void KNI_API KniSetLocalChar(kushort_t index, kchar_t val)
{
	KEnvironment::locals[index].setChar(val);
}

void KNI_API KniSetLocalByte(kushort_t index, kbyte_t val)
{
	KEnvironment::locals[index].setByte(val);
}

void KNI_API KniSetLocalSByte(kushort_t index, ksbyte_t val)
{
	KEnvironment::locals[index].setSByte(val);
}

void KNI_API KniSetLocalShort(kushort_t index, kshort_t val)
{
	KEnvironment::locals[index].setShort(val);
}

void KNI_API KniSetLocalUShort(kushort_t index, kushort_t val)
{
	KEnvironment::locals[index].setUShort(val);
}

void KNI_API KniSetLocalInt(kushort_t index, kint_t val)
{
	KEnvironment::locals[index].setInt(val);
}

void KNI_API KniSetLocalUInt(kushort_t index, kuint_t val)
{
	KEnvironment::locals[index].setUInt(val);
}

void KNI_API KniSetLocalLong(kushort_t index, klong_t val)
{
	KEnvironment::locals[index].setLong(val);
}

void KNI_API KniSetLocalULong(kushort_t index, kulong_t val)
{
	KEnvironment::locals[index].setULong(val);
}

void KNI_API KniSetLocalFloat(kushort_t index, kfloat_t val)
{
	KEnvironment::locals[index].setFloat(val);
}

void KNI_API KniSetLocalDouble(kushort_t index, kdouble_t val)
{
	KEnvironment::locals[index].setDouble(val);
}

void KNI_API KniSetLocalString(kushort_t index, kstring_t val, knuint_t length)
{
	KEnvironment::locals[index].setString(val, length);
}

void KNI_API KniSetLocalRaw(kushort_t index, kref_t val)
{
	KEnvironment::locals[index].setRaw(val);
}

void KNI_API KniSetLocalNull(kushort_t index)
{
	KEnvironment::locals[index] = KObject::nullObject;
}

//===================================================

void KNI_API KniSetArgBool(kushort_t index, kbool_t val)
{
	KEnvironment::args[index].setBool(val);
}

void KNI_API KniSetArgChar(kushort_t index, kchar_t val)
{
	KEnvironment::args[index].setChar(val);
}

void KNI_API KniSetArgByte(kushort_t index, kbyte_t val)
{
	KEnvironment::args[index].setByte(val);
}

void KNI_API KniSetArgSByte(kushort_t index, ksbyte_t val)
{
	KEnvironment::args[index].setSByte(val);
}

void KNI_API KniSetArgShort(kushort_t index, kshort_t val)
{
	KEnvironment::args[index].setShort(val);
}

void KNI_API KniSetArgUShort(kushort_t index, kushort_t val)
{
	KEnvironment::args[index].setUShort(val);
}

void KNI_API KniSetArgInt(kushort_t index, kint_t val)
{
	KEnvironment::args[index].setInt(val);
}

void KNI_API KniSetArgUInt(kushort_t index, kuint_t val)
{
	KEnvironment::args[index].setUInt(val);
}

void KNI_API KniSetArgLong(kushort_t index, klong_t val)
{
	KEnvironment::args[index].setLong(val);
}

void KNI_API KniSetArgULong(kushort_t index, kulong_t val)
{
	KEnvironment::args[index].setULong(val);
}

void KNI_API KniSetArgFloat(kushort_t index, kfloat_t val)
{
	KEnvironment::args[index].setFloat(val);
}

void KNI_API KniSetArgDouble(kushort_t index, kdouble_t val)
{
	KEnvironment::args[index].setDouble(val);
}

void KNI_API KniSetArgString(kushort_t index, kstring_t val, knuint_t length)
{
	KEnvironment::args[index].setString(val, length);
}

void KNI_API KniSetArgRaw(kushort_t index, kref_t val)
{
	KEnvironment::args[index].setRaw(val);
}

void KNI_API KniSetArgNull(kushort_t index)
{
	KEnvironment::args[index] = KObject::nullObject;
}

//===================================================

#define CHECK_SET_FIELD if (obj.getType() == KObject::nullType) \
	{ \
		KniThrowException(KEnvironment::exceptions.nullReference); \
		return; \
	}

void KNI_API KniSetFieldBool(HKFIELD hKField, kbool_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setBool(val);
}

void KNI_API KniSetFieldChar(HKFIELD hKField, kchar_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setChar(val);
}

void KNI_API KniSetFieldByte(HKFIELD hKField, kbyte_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setByte(val);
}

void KNI_API KniSetFieldSByte(HKFIELD hKField, ksbyte_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setSByte(val);
}

void KNI_API KniSetFieldShort(HKFIELD hKField, kshort_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setShort(val);
}

void KNI_API KniSetFieldUShort(HKFIELD hKField, kushort_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setUShort(val);
}

void KNI_API KniSetFieldInt(HKFIELD hKField, kint_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setUShort(val);
}

void KNI_API KniSetFieldUInt(HKFIELD hKField, kuint_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setUInt(val);
}

void KNI_API KniSetFieldLong(HKFIELD hKField, klong_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setLong(val);
}

void KNI_API KniSetFieldULong(HKFIELD hKField, kulong_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setULong(val);
}

void KNI_API KniSetFieldFloat(HKFIELD hKField, kfloat_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setFloat(val);
}

void KNI_API KniSetFieldDouble(HKFIELD hKField, kdouble_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setDouble(val);
}

void KNI_API KniSetFieldString(HKFIELD hKField, kstring_t val, knuint_t length)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setString(val, length);
}

void KNI_API KniSetFieldRaw(HKFIELD hKField, kref_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex).setRaw(val);
}

void KNI_API KniSetFieldNull(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_FIELD;

	obj.getField(Fld->localIndex) = KObject::nullObject;
}

//===================================================

void KNI_API KniSetStaticFieldBool(HKCLASS hKClass, HKFIELD hKField, kbool_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setBool(val);
}

void KNI_API KniSetStaticFieldChar(HKCLASS hKClass, HKFIELD hKField, kchar_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setChar(val);
}

void KNI_API KniSetStaticFieldByte(HKCLASS hKClass, HKFIELD hKField, kbyte_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setByte(val);
}

void KNI_API KniSetStaticFieldSByte(HKCLASS hKClass, HKFIELD hKField, ksbyte_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setSByte(val);
}

void KNI_API KniSetStaticFieldShort(HKCLASS hKClass, HKFIELD hKField, kshort_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setShort(val);
}

void KNI_API KniSetStaticFieldUShort(HKCLASS hKClass, HKFIELD hKField, kushort_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setUShort(val);
}

void KNI_API KniSetStaticFieldInt(HKCLASS hKClass, HKFIELD hKField, kint_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setInt(val);
}

void KNI_API KniSetStaticFieldUInt(HKCLASS hKClass, HKFIELD hKField, kuint_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setUInt(val);
}

void KNI_API KniSetStaticFieldLong(HKCLASS hKClass, HKFIELD hKField, klong_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setLong(val);
}

void KNI_API KniSetStaticFieldULong(HKCLASS hKClass, HKFIELD hKField, kulong_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setULong(val);
}

void KNI_API KniSetStaticFieldFloat(HKCLASS hKClass, HKFIELD hKField, kfloat_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setFloat(val);
}

void KNI_API KniSetStaticFieldDouble(HKCLASS hKClass, HKFIELD hKField, kdouble_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setDouble(val);
}

void KNI_API KniSetStaticFieldString(HKCLASS hKClass, HKFIELD hKField, kstring_t val, knuint_t length)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setString(val, length);
}

void KNI_API KniSetStaticFieldRaw(HKCLASS hKClass, HKFIELD hKField, kref_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setRaw(val);
}

void KNI_API KniSetStaticFieldNull(HKCLASS hKClass, HKFIELD hKField)
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

void KNI_API KniSetElementBool(knuint_t index, kbool_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setBool(val);
}

void KNI_API KniSetElementChar(knuint_t index, kchar_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setChar(val);
}

void KNI_API KniSetElementByte(knuint_t index, kbyte_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setByte(val);
}

void KNI_API KniSetElementSByte(knuint_t index, ksbyte_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setSByte(val);
}

void KNI_API KniSetElementShort(knuint_t index, kshort_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setShort(val);
}

void KNI_API KniSetElementUShort(knuint_t index, kushort_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setUShort(val);
}

void KNI_API KniSetElementInt(knuint_t index, kint_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setInt(val);
}

void KNI_API KniSetElementUInt(knuint_t index, kuint_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setUInt(val);
}

void KNI_API KniSetElementLong(knuint_t index, klong_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setLong(val);
}

void KNI_API KniSetElementULong(knuint_t index, kulong_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setULong(val);
}

void KNI_API KniSetElementFloat(knuint_t index, kfloat_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setFloat(val);
}

void KNI_API KniSetElementDouble(knuint_t index, kdouble_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setDouble(val);
}

void KNI_API KniSetElementString(knuint_t index, kstring_t val, knuint_t length)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setString(val, length);
}

void KNI_API KniSetElementRaw(knuint_t index, kref_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index).setRaw(val);
}

void KNI_API KniSetElementNull(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	CHECK_SET_ELEMENT;
	CHECK_SET_INDEX;

	obj.getElement(index) = KObject::nullObject;
}

//===================================================

void KNI_API KniSetIndirectBool(kbool_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setBool(val);
}

void KNI_API KniSetIndirectChar(kchar_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setChar(val);
}

void KNI_API KniSetIndirectByte(kbyte_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setByte(val);
}

void KNI_API KniSetIndirectSByte(ksbyte_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setSByte(val);
}

void KNI_API KniSetIndirectShort(kshort_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setShort(val);
}

void KNI_API KniSetIndirectUShort(kushort_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setUShort(val);
}

void KNI_API KniSetIndirectInt(kint_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setInt(val);
}

void KNI_API KniSetIndirectUInt(kuint_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setUInt(val);
}

void KNI_API KniSetIndirectLong(klong_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setLong(val);
}

void KNI_API KniSetIndirectULong(kulong_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setULong(val);
}

void KNI_API KniSetIndirectFloat(kfloat_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setFloat(val);
}

void KNI_API KniSetIndirectDouble(kdouble_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setDouble(val);
}

void KNI_API KniSetIndirectString(kstring_t val, knuint_t length)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setString(val, length);
}

void KNI_API KniSetIndirectRaw(kref_t val)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->setRaw(val);
}

void KNI_API KniSetIndirectNull(void)
{
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->operator=(KObject::nullObject);
}


//===================================================

// Loads values onto stack

void KNI_API KniLoadBool(kbool_t val)
{
	KEnvironment::stackPushBool(val);
}

void KNI_API KniLoadChar(kchar_t val)
{
	KEnvironment::stackPushChar(val);
}

void KNI_API KniLoadByte(kbyte_t val)
{
	KEnvironment::stackPushByte(val);
}

void KNI_API KniLoadSByte(ksbyte_t val)
{
	KEnvironment::stackPushSByte(val);
}

void KNI_API KniLoadShort(kshort_t val)
{
	KEnvironment::stackPushShort(val);
}

void KNI_API KniLoadUShort(kushort_t val)
{
	KEnvironment::stackPushUShort(val);
}

void KNI_API KniLoadInt(kint_t val)
{
	KEnvironment::stackPushInt(val);
}

void KNI_API KniLoadUInt(kuint_t val)
{
	KEnvironment::stackPushUInt(val);
}

void KNI_API KniLoadLong(klong_t val)
{
	KEnvironment::stackPushLong(val);
}

void KNI_API KniLoadULong(kulong_t val)
{
	KEnvironment::stackPushULong(val);
}

void KNI_API KniLoadFloat(kfloat_t val)
{
	KEnvironment::stackPushFloat(val);
}

void KNI_API KniLoadDouble(kdouble_t val)
{
	KEnvironment::stackPushDouble(val);
}

void KNI_API KniLoadString(kstring_t val, knuint_t length)
{
	KEnvironment::stackPushString(val, length);
}

void KNI_API KniLoadRaw(kref_t val)
{
	KEnvironment::stackPushRaw(val);
}

//===================================================

void KNI_API KniLoadLocal(kushort_t index)
{
	KEnvironment::stackPush(KEnvironment::locals[index]);
}

void KNI_API KniLoadArg(kushort_t index)
{
	KEnvironment::stackPush(KEnvironment::args[index]);
}

void KNI_API KniLoadArgByRef(kushort_t index)
{
	KEnvironment::stackPush(*KEnvironment::args[index].getRef());
}

void KNI_API KniLoadField(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPush(obj.getField(Fld->localIndex));
}

void KNI_API KniLoadStaticField(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	KEnvironment::stackPush(obj.getField(Fld->localIndex));
}

void KNI_API KniLoadElement(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPush(obj.getElement(index));
}

void KNI_API KniLoadIndirect(void)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPush(*obj.getRef());
}

//===================================================

void KNI_API KniLoadLocalLength(kushort_t index)
{
	KEnvironment::stackPushUInt(KEnvironment::locals[index].getLength());
}

void KNI_API KniLoadArgLength(kushort_t index)
{
	KEnvironment::stackPushUInt(KEnvironment::args[index].getLength());
}

void KNI_API KniLoadArgLengthByRef(kushort_t index)
{
	KEnvironment::stackPushUInt(KEnvironment::args[index].getRef()->getLength());
}

void KNI_API KniLoadFieldLength(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPushUInt(obj.getField(Fld->localIndex).getLength());
}

void KNI_API KniLoadStaticFieldLength(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	KEnvironment::stackPushUInt(obj.getField(Fld->localIndex).getLength());
}

void KNI_API KniLoadElementLength(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPushUInt(obj.getElement(index).getLength());
}

void KNI_API KniLoadIndirectLength(void)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPushUInt(obj.getRef()->getLength());
}

//===================================================

void KNI_API KniLoadLocalElement(kushort_t index, knuint_t elemIndex)
{
	KEnvironment::stackPush(KEnvironment::locals[index].getElement(elemIndex));
}

void KNI_API KniLoadArgElement(kushort_t index, knuint_t elemIndex)
{
	KEnvironment::stackPush(KEnvironment::args[index].getElement(elemIndex));
}

void KNI_API KniLoadArgElementByRef(kushort_t index, knuint_t elemIndex)
{
	KEnvironment::stackPush(KEnvironment::args[index].getRef()->getElement(elemIndex));
}

void KNI_API KniLoadFieldElement(HKFIELD hKField, knuint_t elemIndex)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPush(obj.getField(Fld->localIndex).getElement(elemIndex));
}

void KNI_API KniLoadStaticFieldElement(HKCLASS hKClass, HKFIELD hKField, knuint_t elemIndex)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	KEnvironment::stackPush(obj.getField(Fld->localIndex).getElement(elemIndex));
}

void KNI_API KniLoadElementElement(knuint_t index, knuint_t elemIndex)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPush(obj.getElement(index).getElement(elemIndex));
}

void KNI_API KniLoadIndirectElement(knuint_t elemIndex)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPush(obj.getRef()->getElement(elemIndex));
}

//===================================================

void KNI_API KniLoadLocalAddress(kushort_t index)
{
	KEnvironment::stackPushAddress(&KEnvironment::locals[index]);
}

void KNI_API KniLoadArgAddress(kushort_t index)
{
	KEnvironment::stackPushAddress(&KEnvironment::args[index]);
}

void KNI_API KniLoadFieldAddress(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPushAddress(&obj.getField(Fld->localIndex));
}

void KNI_API KniLoadStaticFieldAddress(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	KEnvironment::stackPushAddress(&obj.getField(Fld->localIndex));
}

void KNI_API KniLoadElementAddress(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPushAddress(&obj.getElement(index));
}

void KNI_API KniLoadNull(void)
{
	KEnvironment::stackPushNull();
}

void KNI_API KniLoadNewObject(HKCLASS hKClass)
{
	KObject obj;
	KEnvironment::allocClassInstance(Cls, obj);
	KEnvironment::stackPush(obj);
	KEnvironment::invokeLastThis(Cls->ctor);
}

void KNI_API KniLoadNewDelegate(HKDELEGATE hKDelegate, HKMETHOD hKMethod)
{
	KObject obj;
	if (Met->addr & KMA_STATIC)
		KEnvironment::allocDelegateInstance(Del, NULL, Met, obj);
	else
		KEnvironment::allocDelegateInstance(Del, &KEnvironment::stackPop(), Met, obj);

	KEnvironment::stackPush(obj);
}

void KNI_API KniLoadNewArray(HKTYPE hKType, knuint_t length)
{
	KObject obj;
	KEnvironment::allocArray(Typ, length, obj);
	KEnvironment::stackPush(obj);
}

void KNI_API KniLoadNewArrayBaking(HKTYPE hKType, knuint_t length)
{
	KObject obj;
	KEnvironment::allocArrayBaking(Typ, length, obj);
	KEnvironment::stackPush(obj);
}

//===================================================

// Stores values

void KNI_API KniStoreLocal(kushort_t index)
{
	const KObject &val = KEnvironment::stackPop();
	KEnvironment::locals[index].accept(val);
}

void KNI_API KniStoreArg(kushort_t index)
{
	const KObject &val = KEnvironment::stackPop();
	KEnvironment::args[index].accept(val);
}

void KNI_API KniStoreArgByRef(kushort_t index)
{
	const KObject &val = KEnvironment::stackPop();
	KEnvironment::args[index].getRef()->accept(val);
}

void KNI_API KniStoreField(HKFIELD hKField)
{
	const KObject &val = KEnvironment::stackPop();
	const KObject &obj = KEnvironment::stackPop();
	obj.getField(Fld->localIndex).accept(val);
}

void KNI_API KniStoreStaticField(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &val = KEnvironment::stackPop();
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).accept(val);
}

void KNI_API KniStoreElement(knuint_t index)
{
	const KObject &val = KEnvironment::stackPop();
	const KObject &obj = KEnvironment::stackPop();
	obj.getElement(index).accept(val);
}

void KNI_API KniStoreIndirect(void)
{
	const KObject &val = KEnvironment::stackPop();
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->accept(val);
}

//===================================================

// Tests values

kbool_t KNI_API KniIsNull(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef() == NULL;
}

kbool_t KNI_API KniIsNullLocal(kushort_t index)
{
	return KEnvironment::locals[index].getRef() == NULL;
}

kbool_t KNI_API KniIsNullArg(kushort_t index)
{
	return KEnvironment::args[index].getRef() == NULL;
}

kbool_t KNI_API KniIsNullArgByRef(kushort_t index)
{
	return KEnvironment::args[index].getRef()->getRef() == NULL;
}

kbool_t KNI_API KniIsNullField(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getField(Fld->localIndex).getRef() == NULL;
}

kbool_t KNI_API KniIsNullStaticField(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getRef() == NULL;
}

kbool_t KNI_API KniIsNullElement(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getElement(index).getRef() == NULL;
}

kbool_t KNI_API KniIsNullIndirect(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getRef() == NULL;
}

kbool_t KNI_API KniIsReferenceEqual(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();
	return a.getRef() == b.getRef();
}

kbool_t KNI_API KniIsClass(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return (obj.getType()->tag & KT_SCALAR_MASK) == KT_CLASS;
}

kbool_t KNI_API KniIsDelegate(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return (obj.getType()->tag & KT_SCALAR_MASK) == KT_DELEGATE;
}

kbool_t KNI_API KniIsArray(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getType()->dim != 0;
}


kbool_t KNI_API KniIsInstanceOfClass(HKCLASS hKClass)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getType()->cls == Cls && obj.getType()->dim == 0;
}

kbool_t KNI_API KniIsInstanceOfDelegate(HKDELEGATE hKDelegate)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getType()->del == Del && obj.getType()->dim == 0;
}

//===================================================

// Invokes

KRESULT KNI_API KniInvoke(HKMETHOD hKMethod)
{
	KEnvironment::invoke(Met);

	if (KEnvironment::exc->getRef() == NULL)
		return KRESULT_OK;
	else
		return KRESULT_ERR;
}

KRESULT KNI_API KniInvokeObject(void)
{
	//ASSUMED: delegate instance is in the form { [0] = thisObj, [1] = methodDef }.

	const KObject &obj = KEnvironment::stackPop();
	const MethodDef *met = (MethodDef *) (obj.getRef() + 1)->getRaw();
	if (met->attrs & KMA_STATIC)
	{
		KEnvironment::invoke(met);
	}
	else
	{
		KEnvironment::stackPush(*obj.getRef());
		KEnvironment::invokeLastThis(met);
	}

	if (KEnvironment::exc->getRef() == NULL)
		return KRESULT_OK;
	else
		return KRESULT_ERR;
}

kbool_t KNI_API KniHasException(void)
{
	return KEnvironment::hasException;
}

void KNI_API KniPrintExceptionDescription(void)
{
	KEnvironment::printException();
}

void KNI_API KniClearException(void)
{
	KEnvironment::hasException = false;
	KEnvironment::exc->accept(KObject::nullObject);
}

void KNI_API KniThrowException(HKFIELD hKFCode)
{
	const ClassDef *cls = KEnvironment::exceptions.klass;
	
	KEnvironment::stackPush(cls->module->staticData[cls->localIndex].getField(((FieldDef *)hKFCode)->localIndex));
	KEnvironment::invoke(KEnvironment::exceptions.fromCode);
	
	KEnvironment::throwException();
}

void KNI_API KniThrowExceptionEx(kstring_t message, knuint_t length)
{
	const ClassDef *cls = KEnvironment::exceptions.klass;
	
	KEnvironment::stackPushString(message, length);
	KEnvironment::invoke(KEnvironment::exceptions.fromMessage);

	KEnvironment::throwException();
}

void KNI_API KniInitLocals(HKTYPE *pHKType, kushort_t count)
{
	KEnvironment::initLocals((const TypeDef **)pHKType, count);
}

//===================================================

// Metadata and types

HKTYPE KNI_API KniGetType(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getType();
}

HKTYPE KNI_API KniGetTypeLocal(kushort_t index)
{
	return KEnvironment::locals[index].getType();
}

HKTYPE KNI_API KniGetTypeArg(kushort_t index)
{
	return KEnvironment::args[index].getType();
}

HKTYPE KNI_API KniGetTypeArgByRef(kushort_t index)
{
	return KEnvironment::args[index].getRef()->getType();
}

HKTYPE KNI_API KniGetTypeField(HKFIELD hKField)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getField(Fld->localIndex).getType();
}

HKTYPE KNI_API KniGetTypeStaticField(HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Fld->localIndex];
	return obj.getField(Fld->localIndex).getType();
}

HKTYPE KNI_API KniGetTypeElement(knuint_t index)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getElement(index).getType();
}

HKTYPE KNI_API KniGetTypeIndirect(void)
{
	const KObject &obj = KEnvironment::stackPop();
	return obj.getRef()->getType();
}


HKCLASS KNI_API KniGetClass(kstring_t ksName)
{
	return KEnvironment::findClass(ksName, KEnvironment::rootModule->getModule());
}

HKDELEGATE KNI_API KniGetDelegate(kstring_t ksName)
{
	return KEnvironment::findDelegate(ksName, KEnvironment::rootModule->getModule());
}

HKFIELD KNI_API KniGetField(HKCLASS hKClass, kstring_t ksName)
{
	return KEnvironment::findField(Cls, ksName);
}

HKMETHOD KNI_API KniGetMethod(HKCLASS hKClass, kstring_t ksName)
{
	return KEnvironment::findMethod(Cls, ksName);
}


HKTYPE KNI_API KniGetPrimitiveType(ktypetag_t tag)
{
	return &KEnvironment::primitiveTypes[tag];
}

HKTYPE KNI_API KniCreateType(ktypetag_t tag, kushort_t dim, HKUSERTYPE hKClassOrDelegate)
{
	if ((tag & KT_SCALAR_MASK) == KT_CLASS)
		return KEnvironment::createType(tag, dim, (ClassDef *)hKClassOrDelegate);
	else if ((tag & KT_SCALAR_MASK) == KT_DELEGATE)
		return KEnvironment::createType(tag, dim, (DelegateDef *)hKClassOrDelegate);
	else
		return KEnvironment::createType(tag, dim);
}

kbool_t KNI_API KniIsClassType(HKTYPE hKType)
{
	return (Typ->tag & KT_SCALAR_MASK) == KT_CLASS && !Typ->dim;
}

kbool_t KNI_API KniIsDelegateType(HKTYPE hKType)
{
	return (Typ->tag & KT_SCALAR_MASK) == KT_DELEGATE && !Typ->dim;
}

kbool_t KNI_API KniIsArrayType(HKTYPE hKType)
{
	return Typ->dim != 0;
}
