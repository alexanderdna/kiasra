#include "kni.h"

#include "krt.h"
#include "kmeta.hpp"
#include "kenv.hpp"
#include "kobject.hpp"
#include "kstringutils.hpp"

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

kbool_t KNI_API KniGetLocalBool(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getBool();
}

kchar_t KNI_API KniGetLocalChar(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getChar();
}

kbyte_t KNI_API KniGetLocalByte(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getByte();
}

ksbyte_t KNI_API KniGetLocalSByte(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getSByte();
}

kshort_t KNI_API KniGetLocalShort(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getShort();
}

kushort_t KNI_API KniGetLocalUShort(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getShort();
}

kint_t KNI_API KniGetLocalInt(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getInt();
}

kuint_t KNI_API KniGetLocalUInt(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getUInt();
}

klong_t KNI_API KniGetLocalLong(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getUInt();
}

kulong_t KNI_API KniGetLocalULong(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getLong();
}

kfloat_t KNI_API KniGetLocalFloat(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getFloat();
}

kdouble_t KNI_API KniGetLocalDouble(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getDouble();
}

kstring_t KNI_API KniGetLocalString(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getString();
}

knuint_t KNI_API KniGetLocalLength(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getLength();
}

//===================================================

kbool_t KNI_API KniGetArgBool(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getBool();
}

kchar_t KNI_API KniGetArgChar(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getChar();
}

kbyte_t KNI_API KniGetArgByte(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getByte();
}

ksbyte_t KNI_API KniGetArgSByte(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getSByte();
}

kshort_t KNI_API KniGetArgShort(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getShort();
}

kushort_t KNI_API KniGetArgUShort(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getUShort();
}

kint_t KNI_API KniGetArgInt(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getInt();
}

kuint_t KNI_API KniGetArgUInt(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getUInt();
}

klong_t KNI_API KniGetArgLong(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getLong();
}

kulong_t KNI_API KniGetArgULong(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getULong();
}

kfloat_t KNI_API KniGetArgFloat(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getFloat();
}

kdouble_t KNI_API KniGetArgDouble(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getDouble();
}

kstring_t KNI_API KniGetArgString(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getString();
}

knuint_t KNI_API KniGetArgLength(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getLength();
}

//===================================================

kbool_t KNI_API KniGetFieldBool(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getBool();
}

kchar_t KNI_API KniGetFieldChar(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getChar();
}

kbyte_t KNI_API KniGetFieldByte(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getByte();
}

ksbyte_t KNI_API KniGetFieldSByte(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getSByte();
}

kshort_t KNI_API KniGetFieldShort(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getShort();
}

kushort_t KNI_API KniGetFieldUShort(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getUShort();
}

kint_t KNI_API KniGetFieldInt(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getInt();
}

kuint_t KNI_API KniGetFieldUInt(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getUInt();
}

klong_t KNI_API KniGetFieldLong(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getLong();
}

kulong_t KNI_API KniGetFieldULong(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getULong();
}

kfloat_t KNI_API KniGetFieldFloat(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getFloat();
}

kdouble_t KNI_API KniGetFieldDouble(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getDouble();
}

kstring_t KNI_API KniGetFieldString(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getString();
}

knuint_t KNI_API KniGetFieldLength(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getLength();
}

//===================================================

kbool_t KNI_API KniGetStaticFieldBool(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getBool();
}

kchar_t KNI_API KniGetStaticFieldChar(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getChar();
}

kbyte_t KNI_API KniGetStaticFieldByte(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getByte();
}

ksbyte_t KNI_API KniGetStaticFieldSByte(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getSByte();
}

kshort_t KNI_API KniGetStaticFieldShort(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getShort();
}

kushort_t KNI_API KniGetStaticFieldUShort(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getUShort();
}

kint_t KNI_API KniGetStaticFieldInt(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getInt();
}

kuint_t KNI_API KniGetStaticFieldUInt(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getUInt();
}

klong_t KNI_API KniGetStaticFieldLong(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getLong();
}

kulong_t KNI_API KniGetStaticFieldULong(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getULong();
}

kfloat_t KNI_API KniGetStaticFieldFloat(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getFloat();
}

kdouble_t KNI_API KniGetStaticFieldDouble(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getDouble();
}

kstring_t KNI_API KniGetStaticFieldString(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getString();
}

knuint_t KNI_API KniGetStaticFieldLength(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getLength();
}

//===================================================

kbool_t KNI_API KniGetElementBool(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getBool();
}

kchar_t KNI_API KniGetElementChar(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getChar();
}

kbyte_t KNI_API KniGetElementByte(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getByte();
}

ksbyte_t KNI_API KniGetElementSByte(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getSByte();
}

kshort_t KNI_API KniGetElementShort(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getShort();
}

kushort_t KNI_API KniGetElementUShort(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getUShort();
}

kint_t KNI_API KniGetElementInt(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getInt();
}

kuint_t KNI_API KniGetElementUInt(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getUInt();
}

klong_t KNI_API KniGetElementLong(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getLong();
}

kulong_t KNI_API KniGetElementULong(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getULong();
}

kfloat_t KNI_API KniGetElementFloat(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getFloat();
}

kdouble_t KNI_API KniGetElementDouble(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getDouble();
}

kstring_t KNI_API KniGetElementString(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getString();
}

knuint_t KNI_API KniGetElementLength(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getLength();
}

//===================================================

kbool_t KNI_API KniGetIndirectBool(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getBool();
}

kchar_t KNI_API KniGetIndirectChar(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getChar();
}

kbyte_t KNI_API KniGetIndirectByte(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getByte();
}

ksbyte_t KNI_API KniGetIndirectSByte(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getSByte();
}

kshort_t KNI_API KniGetIndirectShort(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getShort();
}

kushort_t KNI_API KniGetIndirectUShort(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getUShort();
}

kint_t KNI_API KniGetIndirectInt(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getInt();
}

kuint_t KNI_API KniGetIndirectUInt(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getUInt();
}

klong_t KNI_API KniGetIndirectLong(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getLong();
}

kulong_t KNI_API KniGetIndirectULong(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getULong();
}

kfloat_t KNI_API KniGetIndirectFloat(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getFloat();
}

kdouble_t KNI_API KniGetIndirectDouble(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getDouble();
}

kstring_t KNI_API KniGetIndirectString(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getString();
}

knuint_t KNI_API KniGetIndirectLength(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getLength();
}


//===================================================

// Sets primitive values

void KNI_API KniSetLocalBool(HKENV hKEnv, kushort_t index, kbool_t val)
{
	Env->locals[index].setBool(val);
}

void KNI_API KniSetLocalChar(HKENV hKEnv, kushort_t index, kchar_t val)
{
	Env->locals[index].setChar(val);
}

void KNI_API KniSetLocalByte(HKENV hKEnv, kushort_t index, kbyte_t val)
{
	Env->locals[index].setByte(val);
}

void KNI_API KniSetLocalSByte(HKENV hKEnv, kushort_t index, ksbyte_t val)
{
	Env->locals[index].setSByte(val);
}

void KNI_API KniSetLocalShort(HKENV hKEnv, kushort_t index, kshort_t val)
{
	Env->locals[index].setShort(val);
}

void KNI_API KniSetLocalUShort(HKENV hKEnv, kushort_t index, kushort_t val)
{
	Env->locals[index].setUShort(val);
}

void KNI_API KniSetLocalInt(HKENV hKEnv, kushort_t index, kint_t val)
{
	Env->locals[index].setInt(val);
}

void KNI_API KniSetLocalUInt(HKENV hKEnv, kushort_t index, kuint_t val)
{
	Env->locals[index].setUInt(val);
}

void KNI_API KniSetLocalLong(HKENV hKEnv, kushort_t index, klong_t val)
{
	Env->locals[index].setLong(val);
}

void KNI_API KniSetLocalULong(HKENV hKEnv, kushort_t index, kulong_t val)
{
	Env->locals[index].setULong(val);
}

void KNI_API KniSetLocalFloat(HKENV hKEnv, kushort_t index, kfloat_t val)
{
	Env->locals[index].setFloat(val);
}

void KNI_API KniSetLocalDouble(HKENV hKEnv, kushort_t index, kdouble_t val)
{
	Env->locals[index].setDouble(val);
}

void KNI_API KniSetLocalString(HKENV hKEnv, kushort_t index, kstring_t val)
{
	Env->locals[index].setString(val);
}

//===================================================

void KNI_API KniSetArgBool(HKENV hKEnv, kushort_t index, kbool_t val)
{
	Env->args[index].setBool(val);
}

void KNI_API KniSetArgChar(HKENV hKEnv, kushort_t index, kchar_t val)
{
	Env->args[index].setChar(val);
}

void KNI_API KniSetArgByte(HKENV hKEnv, kushort_t index, kbyte_t val)
{
	Env->args[index].setByte(val);
}

void KNI_API KniSetArgSByte(HKENV hKEnv, kushort_t index, ksbyte_t val)
{
	Env->args[index].setSByte(val);
}

void KNI_API KniSetArgShort(HKENV hKEnv, kushort_t index, kshort_t val)
{
	Env->args[index].setShort(val);
}

void KNI_API KniSetArgUShort(HKENV hKEnv, kushort_t index, kushort_t val)
{
	Env->args[index].setUShort(val);
}

void KNI_API KniSetArgInt(HKENV hKEnv, kushort_t index, kint_t val)
{
	Env->args[index].setInt(val);
}

void KNI_API KniSetArgUInt(HKENV hKEnv, kushort_t index, kuint_t val)
{
	Env->args[index].setUInt(val);
}

void KNI_API KniSetArgLong(HKENV hKEnv, kushort_t index, klong_t val)
{
	Env->args[index].setLong(val);
}

void KNI_API KniSetArgULong(HKENV hKEnv, kushort_t index, kulong_t val)
{
	Env->args[index].setULong(val);
}

void KNI_API KniSetArgFloat(HKENV hKEnv, kushort_t index, kfloat_t val)
{
	Env->args[index].setFloat(val);
}

void KNI_API KniSetArgDouble(HKENV hKEnv, kushort_t index, kdouble_t val)
{
	Env->args[index].setDouble(val);
}

void KNI_API KniSetArgString(HKENV hKEnv, kushort_t index, kstring_t val)
{
	Env->args[index].setString(val);
}

//===================================================

void KNI_API KniSetFieldBool(HKENV hKEnv, HKFIELD hKField, kbool_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getField(Fld->localIndex).setBool(val);
}

void KNI_API KniSetFieldChar(HKENV hKEnv, HKFIELD hKField, kchar_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getField(Fld->localIndex).setChar(val);
}

void KNI_API KniSetFieldByte(HKENV hKEnv, HKFIELD hKField, kbyte_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getField(Fld->localIndex).setByte(val);
}

void KNI_API KniSetFieldSByte(HKENV hKEnv, HKFIELD hKField, ksbyte_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getField(Fld->localIndex).setSByte(val);
}

void KNI_API KniSetFieldShort(HKENV hKEnv, HKFIELD hKField, kshort_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getField(Fld->localIndex).setShort(val);
}

void KNI_API KniSetFieldUShort(HKENV hKEnv, HKFIELD hKField, kushort_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getField(Fld->localIndex).setUShort(val);
}

void KNI_API KniSetFieldInt(HKENV hKEnv, HKFIELD hKField, kint_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getField(Fld->localIndex).setUShort(val);
}

void KNI_API KniSetFieldUInt(HKENV hKEnv, HKFIELD hKField, kuint_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getField(Fld->localIndex).setUInt(val);
}

void KNI_API KniSetFieldLong(HKENV hKEnv, HKFIELD hKField, klong_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getField(Fld->localIndex).setLong(val);
}

void KNI_API KniSetFieldULong(HKENV hKEnv, HKFIELD hKField, kulong_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getField(Fld->localIndex).setULong(val);
}

void KNI_API KniSetFieldFloat(HKENV hKEnv, HKFIELD hKField, kfloat_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getField(Fld->localIndex).setFloat(val);
}

void KNI_API KniSetFieldDouble(HKENV hKEnv, HKFIELD hKField, kdouble_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getField(Fld->localIndex).setDouble(val);
}

void KNI_API KniSetFieldString(HKENV hKEnv, HKFIELD hKField, kstring_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getField(Fld->localIndex).setString(val);
}

//===================================================

void KNI_API KniSetStaticFieldBool(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kbool_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setBool(val);
}

void KNI_API KniSetStaticFieldChar(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kchar_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setChar(val);
}

void KNI_API KniSetStaticFieldByte(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kbyte_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setByte(val);
}

void KNI_API KniSetStaticFieldSByte(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, ksbyte_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setSByte(val);
}

void KNI_API KniSetStaticFieldShort(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kshort_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setShort(val);
}

void KNI_API KniSetStaticFieldUShort(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kushort_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setUShort(val);
}

void KNI_API KniSetStaticFieldInt(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kint_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setInt(val);
}

void KNI_API KniSetStaticFieldUInt(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kuint_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setUInt(val);
}

void KNI_API KniSetStaticFieldLong(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, klong_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setLong(val);
}

void KNI_API KniSetStaticFieldULong(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kulong_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setULong(val);
}

void KNI_API KniSetStaticFieldFloat(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kfloat_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setFloat(val);
}

void KNI_API KniSetStaticFieldDouble(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kdouble_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setDouble(val);
}

void KNI_API KniSetStaticFieldString(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kstring_t val)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).setString(val);
}

//===================================================

void KNI_API KniSetElementBool(HKENV hKEnv, knuint_t index, kbool_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getElement(index).setBool(val);
}

void KNI_API KniSetElementChar(HKENV hKEnv, knuint_t index, kchar_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getElement(index).setChar(val);
}

void KNI_API KniSetElementByte(HKENV hKEnv, knuint_t index, kbyte_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getElement(index).setByte(val);
}

void KNI_API KniSetElementSByte(HKENV hKEnv, knuint_t index, ksbyte_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getElement(index).setSByte(val);
}

void KNI_API KniSetElementShort(HKENV hKEnv, knuint_t index, kshort_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getElement(index).setShort(val);
}

void KNI_API KniSetElementUShort(HKENV hKEnv, knuint_t index, kushort_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getElement(index).setUShort(val);
}

void KNI_API KniSetElementInt(HKENV hKEnv, knuint_t index, kint_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getElement(index).setInt(val);
}

void KNI_API KniSetElementUInt(HKENV hKEnv, knuint_t index, kuint_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getElement(index).setUInt(val);
}

void KNI_API KniSetElementLong(HKENV hKEnv, knuint_t index, klong_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getElement(index).setLong(val);
}

void KNI_API KniSetElementULong(HKENV hKEnv, knuint_t index, kulong_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getElement(index).setULong(val);
}

void KNI_API KniSetElementFloat(HKENV hKEnv, knuint_t index, kfloat_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getElement(index).setFloat(val);
}

void KNI_API KniSetElementDouble(HKENV hKEnv, knuint_t index, kdouble_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getElement(index).setDouble(val);
}

void KNI_API KniSetElementString(HKENV hKEnv, knuint_t index, kstring_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getElement(index).setString(val);
}

//===================================================

void KNI_API KniSetIndirectBool(HKENV hKEnv, kbool_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getRef()->setBool(val);
}

void KNI_API KniSetIndirectChar(HKENV hKEnv, kchar_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getRef()->setChar(val);
}

void KNI_API KniSetIndirectByte(HKENV hKEnv, kbyte_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getRef()->setByte(val);
}

void KNI_API KniSetIndirectSByte(HKENV hKEnv, ksbyte_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getRef()->setSByte(val);
}

void KNI_API KniSetIndirectShort(HKENV hKEnv, kshort_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getRef()->setShort(val);
}

void KNI_API KniSetIndirectUShort(HKENV hKEnv, kushort_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getRef()->setUShort(val);
}

void KNI_API KniSetIndirectInt(HKENV hKEnv, kint_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getRef()->setInt(val);
}

void KNI_API KniSetIndirectUInt(HKENV hKEnv, kuint_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getRef()->setUInt(val);
}

void KNI_API KniSetIndirectLong(HKENV hKEnv, klong_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getRef()->setLong(val);
}

void KNI_API KniSetIndirectULong(HKENV hKEnv, kulong_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getRef()->setULong(val);
}

void KNI_API KniSetIndirectFloat(HKENV hKEnv, kfloat_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getRef()->setFloat(val);
}

void KNI_API KniSetIndirectDouble(HKENV hKEnv, kdouble_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getRef()->setDouble(val);
}

void KNI_API KniSetIndirectString(HKENV hKEnv, kstring_t val)
{
	const KObject &obj = Env->stackPop();
	obj.getRef()->setString(val);
}


//===================================================

// Loads values onto stack

void KNI_API KniLoadBool(HKENV hKEnv, kbool_t val)
{
	Env->stackPushBool(val);
}

void KNI_API KniLoadChar(HKENV hKEnv, kchar_t val)
{
	Env->stackPushChar(val);
}

void KNI_API KniLoadByte(HKENV hKEnv, kbyte_t val)
{
	Env->stackPushByte(val);
}

void KNI_API KniLoadSByte(HKENV hKEnv, ksbyte_t val)
{
	Env->stackPushSByte(val);
}

void KNI_API KniLoadShort(HKENV hKEnv, kshort_t val)
{
	Env->stackPushShort(val);
}

void KNI_API KniLoadUShort(HKENV hKEnv, kushort_t val)
{
	Env->stackPushUShort(val);
}

void KNI_API KniLoadInt(HKENV hKEnv, kint_t val)
{
	Env->stackPushInt(val);
}

void KNI_API KniLoadUInt(HKENV hKEnv, kuint_t val)
{
	Env->stackPushUInt(val);
}

void KNI_API KniLoadLong(HKENV hKEnv, klong_t val)
{
	Env->stackPushLong(val);
}

void KNI_API KniLoadULong(HKENV hKEnv, kulong_t val)
{
	Env->stackPushULong(val);
}

void KNI_API KniLoadFloat(HKENV hKEnv, kfloat_t val)
{
	Env->stackPushFloat(val);
}

void KNI_API KniLoadDouble(HKENV hKEnv, kdouble_t val)
{
	Env->stackPushDouble(val);
}

void KNI_API KniLoadString(HKENV hKEnv, kstring_t val)
{
	Env->stackPushString(val);
}

//===================================================

void KNI_API KniLoadLocal(HKENV hKEnv, kushort_t index)
{
	Env->stackPush(Env->locals[index]);
}

void KNI_API KniLoadArg(HKENV hKEnv, kushort_t index)
{
	Env->stackPush(Env->args[index]);
}

void KNI_API KniLoadArgByRef(HKENV hKEnv, kushort_t index)
{
	Env->stackPush(*Env->args[index].getRef());
}

void KNI_API KniLoadField(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	Env->stackPush(obj.getField(Fld->localIndex));
}

void KNI_API KniLoadStaticField(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	Env->stackPush(obj.getField(Fld->localIndex));
}

void KNI_API KniLoadElement(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	Env->stackPush(obj.getElement(index));
}

void KNI_API KniLoadIndirect(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	Env->stackPush(*obj.getRef());
}

//===================================================

void KNI_API KniLoadLocalLength(HKENV hKEnv, kushort_t index)
{
	Env->stackPushUInt(Env->locals[index].getLength());
}

void KNI_API KniLoadArgLength(HKENV hKEnv, kushort_t index)
{
	Env->stackPushUInt(Env->args[index].getLength());
}

void KNI_API KniLoadArgLengthByRef(HKENV hKEnv, kushort_t index)
{
	Env->stackPushUInt(Env->args[index].getRef()->getLength());
}

void KNI_API KniLoadFieldLength(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	Env->stackPushUInt(obj.getField(Fld->localIndex).getLength());
}

void KNI_API KniLoadStaticFieldLength(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	Env->stackPushUInt(obj.getField(Fld->localIndex).getLength());
}

void KNI_API KniLoadElementLength(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	Env->stackPushUInt(obj.getElement(index).getLength());
}

void KNI_API KniLoadIndirectLength(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	Env->stackPushUInt(obj.getRef()->getLength());
}

//===================================================

void KNI_API KniLoadLocalElement(HKENV hKEnv, kushort_t index, knuint_t elemIndex)
{
	Env->stackPush(Env->locals[index].getElement(elemIndex));
}

void KNI_API KniLoadArgElement(HKENV hKEnv, kushort_t index, knuint_t elemIndex)
{
	Env->stackPush(Env->args[index].getElement(elemIndex));
}

void KNI_API KniLoadArgElementByRef(HKENV hKEnv, kushort_t index, knuint_t elemIndex)
{
	Env->stackPush(Env->args[index].getRef()->getElement(elemIndex));
}

void KNI_API KniLoadFieldElement(HKENV hKEnv, HKFIELD hKField, knuint_t elemIndex)
{
	const KObject &obj = Env->stackPop();
	Env->stackPush(obj.getField(Fld->localIndex).getElement(elemIndex));
}

void KNI_API KniLoadStaticFieldElement(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, knuint_t elemIndex)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	Env->stackPush(obj.getField(Fld->localIndex).getElement(elemIndex));
}

void KNI_API KniLoadElementElement(HKENV hKEnv, knuint_t index, knuint_t elemIndex)
{
	const KObject &obj = Env->stackPop();
	Env->stackPush(obj.getElement(index).getElement(elemIndex));
}

void KNI_API KniLoadIndirectElement(HKENV hKEnv, knuint_t elemIndex)
{
	const KObject &obj = Env->stackPop();
	Env->stackPush(obj.getRef()->getElement(elemIndex));
}

//===================================================

void KNI_API KniLoadLocalAddress(HKENV hKEnv, kushort_t index)
{
	Env->stackPushAddress(&Env->locals[index]);
}

void KNI_API KniLoadArgAddress(HKENV hKEnv, kushort_t index)
{
	Env->stackPushAddress(&Env->args[index]);
}

void KNI_API KniLoadFieldAddress(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	Env->stackPushAddress(&obj.getField(Fld->localIndex));
}

void KNI_API KniLoadStaticFieldAddress(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	Env->stackPushAddress(&obj.getField(Fld->localIndex));
}

void KNI_API KniLoadElementAddress(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	Env->stackPushAddress(&obj.getElement(index));
}

void KNI_API KniLoadNull(HKENV hKEnv)
{
	Env->stackPushNull();
}

void KNI_API KniLoadNewObject(HKENV hKEnv, HKCLASS hKClass)
{
	KObject obj;
	Env->allocClassInstance(Cls, obj);
	Env->stackPush(obj);
	Env->invokeLastThis(Cls->ctor);
}

void KNI_API KniLoadNewDelegate(HKENV hKEnv, HKDELEGATE hKDelegate, HKMETHOD hKMethod)
{
	KObject obj;
	if (Met->addr & KMA_STATIC)
		Env->allocDelegateInstance(Del, NULL, Met, obj);
	else
		Env->allocDelegateInstance(Del, &Env->stackPop(), Met, obj);

	Env->stackPush(obj);
}

void KNI_API KniLoadNewArray(HKENV hKEnv, HKTYPE hKType, knuint_t length)
{
	KObject obj;
	Env->allocArray(Typ, length, obj);
	Env->stackPush(obj);
}

void KNI_API KniLoadNewArrayBaking(HKENV hKEnv, HKTYPE hKType, knuint_t length)
{
	KObject obj;
	Env->allocArrayBaking(Typ, length, obj);
	Env->stackPush(obj);
}

//===================================================

// Stores values

void KNI_API KniStoreLocal(HKENV hKEnv, kushort_t index)
{
	const KObject &val = Env->stackPop();
	Env->locals[index].accept(val);
}

void KNI_API KniStoreArg(HKENV hKEnv, kushort_t index)
{
	const KObject &val = Env->stackPop();
	Env->args[index].accept(val);
}

void KNI_API KniStoreArgByRef(HKENV hKEnv, kushort_t index)
{
	const KObject &val = Env->stackPop();
	Env->args[index].getRef()->accept(val);
}

void KNI_API KniStoreField(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &val = Env->stackPop();
	const KObject &obj = Env->stackPop();
	obj.getField(Fld->localIndex).accept(val);
}

void KNI_API KniStoreStaticField(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &val = Env->stackPop();
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	obj.getField(Fld->localIndex).accept(val);
}

void KNI_API KniStoreElement(HKENV hKEnv, knuint_t index)
{
	const KObject &val = Env->stackPop();
	const KObject &obj = Env->stackPop();
	obj.getElement(index).accept(val);
}

void KNI_API KniStoreIndirect(HKENV hKEnv)
{
	const KObject &val = Env->stackPop();
	const KObject &obj = Env->stackPop();
	obj.getRef()->accept(val);
}

//===================================================

// Tests values

kbool_t KNI_API KniIsNull(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef() == NULL;
}

kbool_t KNI_API KniIsNullLocal(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getRef() == NULL;
}

kbool_t KNI_API KniIsNullArg(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getRef() == NULL;
}

kbool_t KNI_API KniIsNullArgByRef(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getRef()->getRef() == NULL;
}

kbool_t KNI_API KniIsNullField(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getRef() == NULL;
}

kbool_t KNI_API KniIsNullStaticField(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Cls->localIndex];
	return obj.getField(Fld->localIndex).getRef() == NULL;
}

kbool_t KNI_API KniIsNullElement(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getRef() == NULL;
}

kbool_t KNI_API KniIsNullIndirect(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getRef() == NULL;
}

kbool_t KNI_API KniIsReferenceEqual(HKENV hKEnv)
{
	const KObject &b = Env->stackPop();
	const KObject &a = Env->stackPop();
	return a.getRef() == b.getRef();
}

kbool_t KNI_API KniIsClass(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return (obj.getType()->tag & KT_SCALAR_MASK) == KT_CLASS;
}

kbool_t KNI_API KniIsDelegate(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return (obj.getType()->tag & KT_SCALAR_MASK) == KT_DELEGATE;
}

kbool_t KNI_API KniIsArray(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getType()->dim != 0;
}


kbool_t KNI_API KniIsInstanceOfClass(HKENV hKEnv, HKCLASS hKClass)
{
	const KObject &obj = Env->stackPop();
	return obj.getType()->cls == Cls && obj.getType()->dim == 0;
}

kbool_t KNI_API KniIsInstanceOfDelegate(HKENV hKEnv, HKDELEGATE hKDelegate)
{
	const KObject &obj = Env->stackPop();
	return obj.getType()->del == Del && obj.getType()->dim == 0;
}

//===================================================

// Invokes

KRESULT KNI_API KniInvoke(HKENV hKEnv, HKMETHOD hKMethod)
{
	Env->invoke(Met);

	if (Env->exc->getRef() == NULL)
		return KRESULT_OK;
	else
		return KRESULT_ERR;
}

KRESULT KNI_API KniInvokeObject(HKENV hKEnv)
{
	//ASSUMED: delegate instance is in the form { [0] = thisObj, [1] = methodDef }.

	const KObject &obj = Env->stackPop();
	const MethodDef *met = (MethodDef *) (obj.getRef() + 1)->getRaw();
	if (met->attrs & KMA_STATIC)
	{
		Env->invoke(met);
	}
	else
	{
		Env->stackPush(*obj.getRef());
		Env->invokeLastThis(met);
	}

	if (Env->exc->getRef() == NULL)
		return KRESULT_OK;
	else
		return KRESULT_ERR;
}

void KNI_API KniEnterProtectedRegion(HKENV hKEnv, HKTYPE hKTypeExc, KEXCFUNC pKExcFunc)
{
}

void KNI_API KniLeaveProtectedRegion(HKENV hKEnv, KEXCFUNC pKExcFunc)
{
}

void KNI_API KniThrowException(HKENV hKEnv)
{
	Env->throwException();
}

void KNI_API KniInitLocals(HKENV hKEnv, HKTYPE *pHKType)
{
}

//===================================================

// Metadata and types

HKTYPE KNI_API KniGetType(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getType();
}

HKTYPE KNI_API KniGetTypeLocal(HKENV hKEnv, kushort_t index)
{
	return Env->locals[index].getType();
}

HKTYPE KNI_API KniGetTypeArg(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getType();
}

HKTYPE KNI_API KniGetTypeArgByRef(HKENV hKEnv, kushort_t index)
{
	return Env->args[index].getRef()->getType();
}

HKTYPE KNI_API KniGetTypeField(HKENV hKEnv, HKFIELD hKField)
{
	const KObject &obj = Env->stackPop();
	return obj.getField(Fld->localIndex).getType();
}

HKTYPE KNI_API KniGetTypeStaticField(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField)
{
	const KObject &obj = Cls->module->staticData[Fld->localIndex];
	return obj.getField(Fld->localIndex).getType();
}

HKTYPE KNI_API KniGetTypeElement(HKENV hKEnv, knuint_t index)
{
	const KObject &obj = Env->stackPop();
	return obj.getElement(index).getType();
}

HKTYPE KNI_API KniGetTypeIndirect(HKENV hKEnv)
{
	const KObject &obj = Env->stackPop();
	return obj.getRef()->getType();
}


HKCLASS KNI_API KniGetClass(HKENV hKEnv, kstring_t ksName)
{
	return Env->findClass(ksName, Env->rootModule);
}

HKDELEGATE KNI_API KniGetDelegate(HKENV hKEnv, kstring_t ksName)
{
	return Env->findDelegate(ksName, Env->rootModule);
}

HKFIELD KNI_API KniGetField(HKENV HKEnv, HKCLASS hKClass, kstring_t ksName)
{
	knint_t fieldCount = Cls->fieldCount;
	FieldDef **fieldList = Cls->fieldList;
	for (knint_t i = fieldCount - 1; i >= 0; --i)
		if (krt_strequ(fieldList[i]->name, ksName))
			return fieldList[i];

	return NULL;
}

HKMETHOD KNI_API KniGetMethod(HKENV hKEnv, HKCLASS hKClass, kstring_t ksName)
{
	knint_t methodCount = Cls->methodCount;
	MethodDef **methodList = Cls->methodList;
	for (knint_t i = methodCount - 1; i >= 0; --i)
		if (krt_strequ(methodList[i]->name, ksName))
			return methodList[i];

	return NULL;
}


HKTYPE KNI_API KniGetPrimitiveType(HKENV hKEnv, ktypetag_t tag)
{
	return &Env->primitiveTypes[tag];
}

HKTYPE KNI_API KniCreateType(HKENV hKEnv, ktypetag_t tag, kushort_t dim, HKUSERTYPE hKClassOrDelegate)
{
	if ((tag & KT_SCALAR_MASK) == KT_CLASS)
		return Env->createType(tag, dim, (ClassDef *)hKClassOrDelegate);
	else if ((tag & KT_SCALAR_MASK) == KT_DELEGATE)
		return Env->createType(tag, dim, (DelegateDef *)hKClassOrDelegate);
	else
		return Env->createType(tag, dim);
}

kbool_t KNI_API KniIsClassType(HKENV hKEnv, HKTYPE hKType)
{
	(void)hKEnv;
	return (Typ->tag & KT_SCALAR_MASK) == KT_CLASS && !Typ->dim;
}

kbool_t KNI_API KniIsDelegateType(HKENV hKEnv, HKTYPE hKType)
{
	(void)hKEnv;
	return (Typ->tag & KT_SCALAR_MASK) == KT_DELEGATE && !Typ->dim;
}

kbool_t KNI_API KniIsArrayType(HKENV hKEnv, HKTYPE hKType)
{
	(void)hKEnv;
	return Typ->dim != 0;
}
