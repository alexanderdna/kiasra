#ifndef _KNI_H
#define _KNI_H

#include <stdint.h>
#include <wchar.h>

/*===================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
	#ifdef KRT_EXPORTS
		#define KNI_API __declspec(dllexport)
	#else
		#define KNI_API __declspec(dllimport)
	#endif
#else
	#define KNI_API
#endif

/*===================================================*/

/* Type tags */
typedef enum ktypetag_t
{
	KT_VOID    = 0x0000,
	KT_BOOL    = 0x0001,
	KT_CHAR    = 0x0002,
	KT_BYTE    = 0x0003,
	KT_SBYTE   = 0x0004,
	KT_SHORT   = 0x0005,
	KT_USHORT  = 0x0006,
	KT_INT     = 0x0007,
	KT_UINT    = 0x0008,
	KT_LONG    = 0x0009,
	KT_ULONG   = 0x000A,
	KT_FLOAT   = 0x000B,
	KT_DOUBLE  = 0x000C,
	KT_STRING  = 0x000D,
	KT_CLASS   = 0x000E,
	KT_DELEGATE= 0x000F,
	KT_ARRAY   = 0x0100,
	KT_OBJECT  = 0x1000,
	KT_RAW     = 0x2000,
	KT_REF     = 0x4000,

	KT_SCALAR_MASK = KT_VOID | KT_BOOL | KT_CHAR | KT_BYTE | KT_SBYTE |
					KT_SHORT | KT_USHORT | KT_INT | KT_UINT | KT_LONG | KT_ULONG |
					KT_FLOAT | KT_DOUBLE | KT_STRING |
					KT_CLASS | KT_DELEGATE,

	KT_REF_MASK = KT_ARRAY | KT_OBJECT | KT_RAW
} ktypetag_t;

/*===================================================*/

typedef enum KRESULT
{
	KRESULT_OK,
	KRESULT_ERR
} KRESULT;

typedef enum NINVOKE_RESULT
{
	NINVOKE_OK = 0,
	NINVOKE_ERR = 1
} NINVOKE_RESULT;

/* Handle of Execution Environment */
typedef void * HKENV;

/* Native extern function prototype */
typedef NINVOKE_RESULT (*NFUNC)(HKENV hKEnv);

/* Exception handling function prototype */
typedef void (*KEXCFUNC)(HKENV hKEnv);

typedef unsigned char  kbool_t;
typedef wchar_t        kchar_t;
typedef int8_t         kbyte_t;
typedef uint8_t        ksbyte_t;
typedef int16_t        kshort_t;
typedef uint16_t       kushort_t;
typedef int32_t        kint_t;
typedef uint32_t       kuint_t;
typedef int64_t        klong_t;
typedef uint64_t       kulong_t;

typedef float          kfloat_t;
typedef double         kdouble_t;

typedef const kchar_t *kstring_t;

/* Reference to userdata */
typedef void          *kref_t;

/* User-defined type (class or delegate) */
typedef const void    *HKUSERTYPE;

/* Handle of ClassDef */
typedef HKUSERTYPE     HKCLASS;

/* Handle of DelegateDef */
typedef HKUSERTYPE     HKDELEGATE;

/* Handle of MethodDef */
typedef const void    *HKMETHOD;

/* Handle of FieldDef */
typedef const void    *HKFIELD;

/* Handle of TypeDef */
typedef const void    *HKTYPE;

/* Native integer */
typedef ptrdiff_t     knint_t;
/* Native unsigned integer */
typedef size_t        knuint_t;

/* 16-bit token */
typedef uint16_t      ktoken16_t;
/* 32-bit token */
typedef uint32_t      ktoken32_t;

/*===================================================*/

/* Gets primitive values */

/* Gets local variable as bool value */
kbool_t KNI_API KniGetLocalBool(HKENV hKEnv, kushort_t index);
/* Gets local variable as char value */
kchar_t KNI_API KniGetLocalChar(HKENV hKEnv, kushort_t index);
/* Gets local variable as byte value */
kbyte_t KNI_API KniGetLocalByte(HKENV hKEnv, kushort_t index);
/* Gets local variable as sbool value */
ksbyte_t KNI_API KniGetLocalSByte(HKENV hKEnv, kushort_t index);
/* Gets local variable as short value */
kshort_t KNI_API KniGetLocalShort(HKENV hKEnv, kushort_t index);
/* Gets local variable as ushort value */
kushort_t KNI_API KniGetLocalUShort(HKENV hKEnv, kushort_t index);
/* Gets local variable as int value */
kint_t KNI_API KniGetLocalInt(HKENV hKEnv, kushort_t index);
/* Gets local variable as uint value */
kuint_t KNI_API KniGetLocalUInt(HKENV hKEnv, kushort_t index);
/* Gets local variable as long value */
klong_t KNI_API KniGetLocalLong(HKENV hKEnv, kushort_t index);
/* Gets local variable as ulong value */
kulong_t KNI_API KniGetLocalULong(HKENV hKEnv, kushort_t index);
/* Gets local variable as float value */
kfloat_t KNI_API KniGetLocalFloat(HKENV hKEnv, kushort_t index);
/* Gets local variable as double value */
kdouble_t KNI_API KniGetLocalDouble(HKENV hKEnv, kushort_t index);
/* Gets local variable as string value */
kstring_t KNI_API KniGetLocalString(HKENV hKEnv, kushort_t index);
/* Gets length of string or array local variable */
knuint_t KNI_API KniGetLocalLength(HKENV hKEnv, kushort_t index);


/* Gets argument as bool value */
kbool_t KNI_API KniGetArgBool(HKENV hKEnv, kushort_t index);
/* Gets argument as char value */
kchar_t KNI_API KniGetArgChar(HKENV hKEnv, kushort_t index);
/* Gets argument as byte value */
kbyte_t KNI_API KniGetArgByte(HKENV hKEnv, kushort_t index);
/* Gets argument as sbyte value */
ksbyte_t KNI_API KniGetArgSByte(HKENV hKEnv, kushort_t index);
/* Gets argument as short value */
kshort_t KNI_API KniGetArgShort(HKENV hKEnv, kushort_t index);
/* Gets argument as ushort value */
kushort_t KNI_API KniGetArgUShort(HKENV hKEnv, kushort_t index);
/* Gets argument as int value */
kint_t KNI_API KniGetArgInt(HKENV hKEnv, kushort_t index);
/* Gets argument as uint value */
kuint_t KNI_API KniGetArgUInt(HKENV hKEnv, kushort_t index);
/* Gets argument as long value */
klong_t KNI_API KniGetArgLong(HKENV hKEnv, kushort_t index);
/* Gets argument as ulong value */
kulong_t KNI_API KniGetArgULong(HKENV hKEnv, kushort_t index);
/* Gets argument as float value */
kfloat_t KNI_API KniGetArgFloat(HKENV hKEnv, kushort_t index);
/* Gets argument as double value */
kdouble_t KNI_API KniGetArgDouble(HKENV hKEnv, kushort_t index);
/* Gets argument as string value */
kstring_t KNI_API KniGetArgString(HKENV hKEnv, kushort_t index);
/* Gets length of string or array argument */
knuint_t KNI_API KniGetArgLength(HKENV hKEnv, kushort_t index);


/* Gets instance field as bool value */
kbool_t KNI_API KniGetFieldBool(HKENV hKEnv, HKFIELD hKField);
/* Gets instance field as char value */
kchar_t KNI_API KniGetFieldChar(HKENV hKEnv, HKFIELD hKField);
/* Gets instance field as byte value */
kbyte_t KNI_API KniGetFieldByte(HKENV hKEnv, HKFIELD hKField);
/* Gets instance field as sbyte value */
ksbyte_t KNI_API KniGetFieldSByte(HKENV hKEnv, HKFIELD hKField);
/* Gets instance field as short value */
kshort_t KNI_API KniGetFieldShort(HKENV hKEnv, HKFIELD hKField);
/* Gets instance field as ushort value */
kushort_t KNI_API KniGetFieldUShort(HKENV hKEnv, HKFIELD hKField);
/* Gets instance field as int value */
kint_t KNI_API KniGetFieldInt(HKENV hKEnv, HKFIELD hKField);
/* Gets instance field as uint value */
kuint_t KNI_API KniGetFieldUInt(HKENV hKEnv, HKFIELD hKField);
/* Gets instance field as long value */
klong_t KNI_API KniGetFieldLong(HKENV hKEnv, HKFIELD hKField);
/* Gets instance field as ulong value */
kulong_t KNI_API KniGetFieldULong(HKENV hKEnv, HKFIELD hKField);
/* Gets instance field as float value */
kfloat_t KNI_API KniGetFieldFloat(HKENV hKEnv, HKFIELD hKField);
/* Gets instance field as double value */
kdouble_t KNI_API KniGetFieldDouble(HKENV hKEnv, HKFIELD hKField);
/* Gets instance field as string value */
kstring_t KNI_API KniGetFieldString(HKENV hKEnv, HKFIELD hKField);
/* Gets length of string or array instance field */
knuint_t KNI_API KniGetFieldLength(HKENV hKEnv, HKFIELD hKField);

/* Gets static field as bool value */
kbool_t KNI_API KniGetStaticFieldBool(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as char value */
kchar_t KNI_API KniGetStaticFieldChar(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as byte value */
kbyte_t KNI_API KniGetStaticFieldByte(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as sbyte value */
ksbyte_t KNI_API KniGetStaticFieldSByte(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as short value */
kshort_t KNI_API KniGetStaticFieldShort(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as ushort value */
kushort_t KNI_API KniGetStaticFieldUShort(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as int value */
kint_t KNI_API KniGetStaticFieldInt(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as uint value */
kuint_t KNI_API KniGetStaticFieldUInt(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as long value */
klong_t KNI_API KniGetStaticFieldLong(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as ulong value */
kulong_t KNI_API KniGetStaticFieldULong(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as float value */
kfloat_t KNI_API KniGetStaticFieldFloat(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as double value */
kdouble_t KNI_API KniGetStaticFieldDouble(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as string value */
kstring_t KNI_API KniGetStaticFieldString(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Gets length of string or array static field */
knuint_t KNI_API KniGetStaticFieldLength(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);


/* Gets element as bool value */
kbool_t KNI_API KniGetElementBool(HKENV hKEnv, knuint_t index);
/* Gets element as char value */
kchar_t KNI_API KniGetElementChar(HKENV hKEnv, knuint_t index);
/* Gets element as byte value */
kbyte_t KNI_API KniGetElementByte(HKENV hKEnv, knuint_t index);
/* Gets element as sbyte value */
ksbyte_t KNI_API KniGetElementSByte(HKENV hKEnv, knuint_t index);
/* Gets element as short value */
kshort_t KNI_API KniGetElementShort(HKENV hKEnv, knuint_t index);
/* Gets element as ushort value */
kushort_t KNI_API KniGetElementUShort(HKENV hKEnv, knuint_t index);
/* Gets element as int value */
kint_t KNI_API KniGetElementInt(HKENV hKEnv, knuint_t index);
/* Gets element as uint value */
kuint_t KNI_API KniGetElementUInt(HKENV hKEnv, knuint_t index);
/* Gets element as long value */
klong_t KNI_API KniGetElementLong(HKENV hKEnv, knuint_t index);
/* Gets element as ulong value */
kulong_t KNI_API KniGetElementULong(HKENV hKEnv, knuint_t index);
/* Gets element as float value */
kfloat_t KNI_API KniGetElementFloat(HKENV hKEnv, knuint_t index);
/* Gets element as double value */
kdouble_t KNI_API KniGetElementDouble(HKENV hKEnv, knuint_t index);
/* Gets element as string value */
kstring_t KNI_API KniGetElementString(HKENV hKEnv, knuint_t index);
/* Gets length of string or array typed element */
knuint_t KNI_API KniGetElementLength(HKENV hKEnv, knuint_t index);


/* Gets indirect object as bool value */
kbool_t KNI_API KniGetIndirectBool(HKENV hKEnv);
/* Gets indirect object as char value */
kchar_t KNI_API KniGetIndirectChar(HKENV hKEnv);
/* Gets indirect object as byte value */
kbyte_t KNI_API KniGetIndirectByte(HKENV hKEnv);
/* Gets indirect object as sbyte value */
ksbyte_t KNI_API KniGetIndirectSByte(HKENV hKEnv);
/* Gets indirect object as short value */
kshort_t KNI_API KniGetIndirectShort(HKENV hKEnv);
/* Gets indirect object as ushort value */
kushort_t KNI_API KniGetIndirectUShort(HKENV hKEnv);
/* Gets indirect object as int value */
kint_t KNI_API KniGetIndirectInt(HKENV hKEnv);
/* Gets indirect object as uint value */
kuint_t KNI_API KniGetIndirectUInt(HKENV hKEnv);
/* Gets indirect object as long value */
klong_t KNI_API KniGetIndirectLong(HKENV hKEnv);
/* Gets indirect object as ulong value */
kulong_t KNI_API KniGetIndirectULong(HKENV hKEnv);
/* Gets indirect object as float value */
kfloat_t KNI_API KniGetIndirectFloat(HKENV hKEnv);
/* Gets indirect object as double value */
kdouble_t KNI_API KniGetIndirectDouble(HKENV hKEnv);
/* Gets indirect object as string value */
kstring_t KNI_API KniGetIndirectString(HKENV hKEnv);
/* Gets length of string or array indirect object */
knuint_t KNI_API KniGetIndirectLength(HKENV hKEnv);

/*===================================================*/

/* Sets primitive values */

/* Sets bool value to local variable */
void KNI_API KniSetLocalBool(HKENV hKEnv, kushort_t index, kbool_t val);
/* Sets char value to local variable */
void KNI_API KniSetLocalChar(HKENV hKEnv, kushort_t index, kchar_t val);
/* Sets byte value to local variable */
void KNI_API KniSetLocalByte(HKENV hKEnv, kushort_t index, kbyte_t val);
/* Sets sbyte value to local variable */
void KNI_API KniSetLocalSByte(HKENV hKEnv, kushort_t index, ksbyte_t val);
/* Sets short value to local variable */
void KNI_API KniSetLocalShort(HKENV hKEnv, kushort_t index, kshort_t val);
/* Sets ushort value to local variable */
void KNI_API KniSetLocalUShort(HKENV hKEnv, kushort_t index, kushort_t val);
/* Sets int value to local variable */
void KNI_API KniSetLocalInt(HKENV hKEnv, kushort_t index, kint_t val);
/* Sets uint value to local variable */
void KNI_API KniSetLocalUInt(HKENV hKEnv, kushort_t index, kuint_t val);
/* Sets long value to local variable */
void KNI_API KniSetLocalLong(HKENV hKEnv, kushort_t index, klong_t val);
/* Sets ulong value to local variable */
void KNI_API KniSetLocalULong(HKENV hKEnv, kushort_t index, kulong_t val);
/* Sets float value to local variable */
void KNI_API KniSetLocalFloat(HKENV hKEnv, kushort_t index, kfloat_t val);
/* Sets double value to local variable */
void KNI_API KniSetLocalDouble(HKENV hKEnv, kushort_t index, kdouble_t val);
/* Sets string value to local variable */
void KNI_API KniSetLocalString(HKENV hKEnv, kushort_t index, kstring_t val);

/* Sets bool value to argument */
void KNI_API KniSetArgBool(HKENV hKEnv, kushort_t index, kbool_t val);
/* Sets char value to argument */
void KNI_API KniSetArgChar(HKENV hKEnv, kushort_t index, kchar_t val);
/* Sets byte value to argument */
void KNI_API KniSetArgByte(HKENV hKEnv, kushort_t index, kbyte_t val);
/* Sets sbyte value to argument */
void KNI_API KniSetArgSByte(HKENV hKEnv, kushort_t index, ksbyte_t val);
/* Sets short value to argument */
void KNI_API KniSetArgShort(HKENV hKEnv, kushort_t index, kshort_t val);
/* Sets ushort value to argument */
void KNI_API KniSetArgUShort(HKENV hKEnv, kushort_t index, kushort_t val);
/* Sets int value to argument */
void KNI_API KniSetArgInt(HKENV hKEnv, kushort_t index, kint_t val);
/* Sets uint value to argument */
void KNI_API KniSetArgUInt(HKENV hKEnv, kushort_t index, kuint_t val);
/* Sets long value to argument */
void KNI_API KniSetArgLong(HKENV hKEnv, kushort_t index, klong_t val);
/* Sets ulong value to argument */
void KNI_API KniSetArgULong(HKENV hKEnv, kushort_t index, kulong_t val);
/* Sets float value to argument */
void KNI_API KniSetArgFloat(HKENV hKEnv, kushort_t index, kfloat_t val);
/* Sets double value to argument */
void KNI_API KniSetArgDouble(HKENV hKEnv, kushort_t index, kdouble_t val);
/* Sets string value to argument */
void KNI_API KniSetArgString(HKENV hKEnv, kushort_t index, kstring_t val);


/* Sets bool value to instance field */
void KNI_API KniSetFieldBool(HKENV hKEnv, HKFIELD hKField, kbool_t val);
/* Sets char value to instance field */
void KNI_API KniSetFieldChar(HKENV hKEnv, HKFIELD hKField, kchar_t val);
/* Sets byte value to instance field */
void KNI_API KniSetFieldByte(HKENV hKEnv, HKFIELD hKField, kbyte_t val);
/* Sets sbyte value to instance field */
void KNI_API KniSetFieldSByte(HKENV hKEnv, HKFIELD hKField, ksbyte_t val);
/* Sets short value to instance field */
void KNI_API KniSetFieldShort(HKENV hKEnv, HKFIELD hKField, kshort_t val);
/* Sets ushort value to instance field */
void KNI_API KniSetFieldUShort(HKENV hKEnv, HKFIELD hKField, kushort_t val);
/* Sets int value to instance field */
void KNI_API KniSetFieldInt(HKENV hKEnv, HKFIELD hKField, kint_t val);
/* Sets uint value to instance field */
void KNI_API KniSetFieldUInt(HKENV hKEnv, HKFIELD hKField, kuint_t val);
/* Sets long value to instance field */
void KNI_API KniSetFieldLong(HKENV hKEnv, HKFIELD hKField, klong_t val);
/* Sets ulong value to instance field */
void KNI_API KniSetFieldULong(HKENV hKEnv, HKFIELD hKField, kulong_t val);
/* Sets float value to instance field */
void KNI_API KniSetFieldFloat(HKENV hKEnv, HKFIELD hKField, kfloat_t val);
/* Sets double value to instance field */
void KNI_API KniSetFieldDouble(HKENV hKEnv, HKFIELD hKField, kdouble_t val);
/* Sets string value to instance field */
void KNI_API KniSetFieldString(HKENV hKEnv, HKFIELD hKField, kstring_t val);


/* Sets bool value to static field */
void KNI_API KniSetStaticFieldBool(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kbool_t val);
/* Sets char value to static field */
void KNI_API KniSetStaticFieldChar(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kchar_t val);
/* Sets byte value to static field */
void KNI_API KniSetStaticFieldByte(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kbyte_t val);
/* Sets sbyte value to static field */
void KNI_API KniSetStaticFieldSByte(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, ksbyte_t val);
/* Sets short value to static field */
void KNI_API KniSetStaticFieldShort(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kshort_t val);
/* Sets ushort value to static field */
void KNI_API KniSetStaticFieldUShort(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kushort_t val);
/* Sets int value to static field */
void KNI_API KniSetStaticFieldInt(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kint_t val);
/* Sets uint value to static field */
void KNI_API KniSetStaticFieldUInt(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kuint_t val);
/* Sets long value to static field */
void KNI_API KniSetStaticFieldLong(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, klong_t val);
/* Sets ulong value to static field */
void KNI_API KniSetStaticFieldULong(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kulong_t val);
/* Sets float value to static field */
void KNI_API KniSetStaticFieldFloat(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kfloat_t val);
/* Sets double value to static field */
void KNI_API KniSetStaticFieldDouble(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kdouble_t val);
/* Sets string value to static field */
void KNI_API KniSetStaticFieldString(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kstring_t val);


/* Sets bool value to array element */
void KNI_API KniSetElementBool(HKENV hKEnv, knuint_t index, kbool_t val);
/* Sets char value to array element */
void KNI_API KniSetElementChar(HKENV hKEnv, knuint_t index, kchar_t val);
/* Sets byte value to array element */
void KNI_API KniSetElementByte(HKENV hKEnv, knuint_t index, kbyte_t val);
/* Sets sbyte value to array element */
void KNI_API KniSetElementSByte(HKENV hKEnv, knuint_t index, ksbyte_t val);
/* Sets short value to array element */
void KNI_API KniSetElementShort(HKENV hKEnv, knuint_t index, kshort_t val);
/* Sets ushort value to array element */
void KNI_API KniSetElementUShort(HKENV hKEnv, knuint_t index, kushort_t val);
/* Sets int value to array element */
void KNI_API KniSetElementInt(HKENV hKEnv, knuint_t index, kint_t val);
/* Sets uint value to array element */
void KNI_API KniSetElementUInt(HKENV hKEnv, knuint_t index, kuint_t val);
/* Sets long value to array element */
void KNI_API KniSetElementLong(HKENV hKEnv, knuint_t index, klong_t val);
/* Sets ulong value to array element */
void KNI_API KniSetElementULong(HKENV hKEnv, knuint_t index, kulong_t val);
/* Sets float value to array element */
void KNI_API KniSetElementFloat(HKENV hKEnv, knuint_t index, kfloat_t val);
/* Sets double value to array element */
void KNI_API KniSetElementDouble(HKENV hKEnv, knuint_t index, kdouble_t val);
/* Sets string value to array element */
void KNI_API KniSetElementString(HKENV hKEnv, knuint_t index, kstring_t val);


/* Sets bool value to indirect object */
void KNI_API KniSetIndirectBool(HKENV hKEnv, kbool_t val);
/* Sets char value to indirect object */
void KNI_API KniSetIndirectChar(HKENV hKEnv, kchar_t val);
/* Sets byte value to indirect object */
void KNI_API KniSetIndirectByte(HKENV hKEnv, kbyte_t val);
/* Sets sbyte value to indirect object */
void KNI_API KniSetIndirectSByte(HKENV hKEnv, ksbyte_t val);
/* Sets short value to indirect object */
void KNI_API KniSetIndirectShort(HKENV hKEnv, kshort_t val);
/* Sets ushort value to indirect object */
void KNI_API KniSetIndirectUShort(HKENV hKEnv, kushort_t val);
/* Sets int value to indirect object */
void KNI_API KniSetIndirectInt(HKENV hKEnv, kint_t val);
/* Sets uint value to indirect object */
void KNI_API KniSetIndirectUInt(HKENV hKEnv, kuint_t val);
/* Sets long value to indirect object */
void KNI_API KniSetIndirectLong(HKENV hKEnv, klong_t val);
/* Sets ulong value to indirect object */
void KNI_API KniSetIndirectULong(HKENV hKEnv, kulong_t val);
/* Sets float value to indirect object */
void KNI_API KniSetIndirectFloat(HKENV hKEnv, kfloat_t val);
/* Sets double value to indirect object */
void KNI_API KniSetIndirectDouble(HKENV hKEnv, kdouble_t val);
/* Sets string value to indirect object */
void KNI_API KniSetIndirectString(HKENV hKEnv, kstring_t val);

/*===================================================*/

/* Loads values onto stack */

/* Loads bool value onto stack */
void KNI_API KniLoadBool(HKENV hKEnv, kbool_t val);
/* Loads char value onto stack */
void KNI_API KniLoadChar(HKENV hKEnv, kchar_t val);
/* Loads byte value onto stack */
void KNI_API KniLoadByte(HKENV hKEnv, kbyte_t val);
/* Loads sbyte value onto stack */
void KNI_API KniLoadSByte(HKENV hKEnv, ksbyte_t val);
/* Loads short value onto stack */
void KNI_API KniLoadShort(HKENV hKEnv, kshort_t val);
/* Loads ushort value onto stack */
void KNI_API KniLoadUShort(HKENV hKEnv, kushort_t val);
/* Loads int value onto stack */
void KNI_API KniLoadInt(HKENV hKEnv, kint_t val);
/* Loads uint value onto stack */
void KNI_API KniLoadUInt(HKENV hKEnv, kuint_t val);
/* Loads long value onto stack */
void KNI_API KniLoadLong(HKENV hKEnv, klong_t val);
/* Loads ulong value onto stack */
void KNI_API KniLoadULong(HKENV hKEnv, kulong_t val);
/* Loads float value onto stack */
void KNI_API KniLoadFloat(HKENV hKEnv, kfloat_t val);
/* Loads double value onto stack */
void KNI_API KniLoadDouble(HKENV hKEnv, kdouble_t val);
/* Loads string value onto stack */
void KNI_API KniLoadString(HKENV hKEnv, kstring_t val);

/* Loads local variable onto stack */
void KNI_API KniLoadLocal(HKENV hKEnv, kushort_t index);
/* Loads argument onto stack */
void KNI_API KniLoadArg(HKENV hKEnv, kushort_t index);
/* Loads ByRef argument onto stack */
void KNI_API KniLoadArgByRef(HKENV hKEnv, kushort_t index);
/* Loads instance field onto stack */
void KNI_API KniLoadField(HKENV hKEnv, HKFIELD hKField);
/* Loads static field onto stack */
void KNI_API KniLoadStaticField(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Loads array element onto stack */
void KNI_API KniLoadElement(HKENV hKEnv, knuint_t index);
/* Loads indirect object onto stack */
void KNI_API KniLoadIndirect(HKENV hKEnv);

/* Loads length of string or array local variable onto stack */
void KNI_API KniLoadLocalLength(HKENV hKEnv, kushort_t index);
/* Loads length of string or array argument onto stack */
void KNI_API KniLoadArgLength(HKENV hKEnv, kushort_t index);
/* Loads length of string or array ByRef argument onto stack */
void KNI_API KniLoadArgLengthByRef(HKENV hKEnv, kushort_t index);
/* Loads length of string or array instance field onto stack */
void KNI_API KniLoadFieldLength(HKENV hKEnv, HKFIELD hKField);
/* Loads length of string or array static field onto stack */
void KNI_API KniLoadStaticFieldLength(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Loads length of string or array typed array element onto stack */
void KNI_API KniLoadElementLength(HKENV hKEnv, knuint_t index);
/* Loads length of string or array indirect object onto stack */
void KNI_API KniLoadIndirectLength(HKENV hKEnv);

/* Loads element of array typed local variable onto stack */
void KNI_API KniLoadLocalElement(HKENV hKEnv, kushort_t index, knuint_t elemIndex);
/* Loads element of array typed argument onto stack */
void KNI_API KniLoadArgElement(HKENV hKEnv, kushort_t index, knuint_t elemIndex);
/* Loads element of array typed ByRef argument onto stack */
void KNI_API KniLoadArgElementByRef(HKENV hKEnv, kushort_t index, knuint_t elemIndex);
/* Loads element of array typed instance field onto stack */
void KNI_API KniLoadFieldElement(HKENV hKEnv, HKFIELD hKField, knuint_t elemIndex);
/* Loads element of array typed static field onto stack */
void KNI_API KniLoadStaticFieldElement(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, knuint_t elemIndex);
/* Loads element of array typed array element onto stack */
void KNI_API KniLoadElementElement(HKENV hKEnv, knuint_t index, knuint_t elemIndex);
/* Loads element of array typed  onto stack */
void KNI_API KniLoadIndirectElement(HKENV hKEnv, knuint_t elemIndex);

/* Loads address of local variable onto stack */
void KNI_API KniLoadLocalAddress(HKENV hKEnv, kushort_t index);
/* Loads address of argument onto stack */
void KNI_API KniLoadArgAddress(HKENV hKEnv, kushort_t index);
/* Loads address of instance field onto stack */
void KNI_API KniLoadFieldAddress(HKENV hKEnv, HKFIELD hKField);
/* Loads address of static field onto stack */
void KNI_API KniLoadStaticFieldAddress(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Loads address of array element onto stack */
void KNI_API KniLoadElementAddress(HKENV hKEnv, knuint_t index);

/* Loads null value onto stack */
void KNI_API KniLoadNull(HKENV hKEnv);

/* Loads new class instance onto stack */
void KNI_API KniLoadNewObject(HKENV hKEnv, HKCLASS hKClass);
/* Loads new delegate instance onto stack */
void KNI_API KniLoadNewDelegate(HKENV hKEnv, HKDELEGATE hKDelegate, HKMETHOD hKMethod);
/* Loads new array onto stack */
void KNI_API KniLoadNewArray(HKENV hKEnv, HKTYPE hKType, knuint_t length);
/* Loads new array onto stack, elements are already loaded */
void KNI_API KniLoadNewArrayBaking(HKENV hKEnv, HKTYPE hKType, knuint_t length);

/*===================================================*/

/* Stores values */

/* Stores stack top value into local variable */
void KNI_API KniStoreLocal(HKENV hKEnv, kushort_t index);
/* Stores stack top value into argument */
void KNI_API KniStoreArg(HKENV hKEnv, kushort_t index);
/* Stores stack top value into ByRef argument */
void KNI_API KniStoreArgByRef(HKENV hKEnv, kushort_t index);
/* Stores stack top value into instance field */
void KNI_API KniStoreField(HKENV hKEnv, HKFIELD hKField);
/* Stores stack top value into static field */
void KNI_API KniStoreStaticField(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Stores stack top value into array element */
void KNI_API KniStoreElement(HKENV hKEnv, knuint_t index);
/* Stores stack top value into indirect object */
void KNI_API KniStoreIndirect(HKENV hKEnv);

/*===================================================*/

/* Tests values */

/* Checks if stack top value is null */
kbool_t KNI_API KniIsNull(HKENV hKEnv);
/* Checks if local variable is null */
kbool_t KNI_API KniIsNullLocal(HKENV hKEnv, kushort_t index);
/* Checks if argument is null */
kbool_t KNI_API KniIsNullArg(HKENV hKEnv, kushort_t index);
/* Checks if ByRef argument is null */
kbool_t KNI_API KniIsNullArgByRef(HKENV hKEnv, kushort_t index);
/* Checks if instance field is null */
kbool_t KNI_API KniIsNullField(HKENV hKEnv, HKFIELD hKField);
/* Checks if static field is null */
kbool_t KNI_API KniIsNullStaticField(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Checks if array element is null */
kbool_t KNI_API KniIsNullElement(HKENV hKEnv, knuint_t index);
/* Checks if indirect object is null */
kbool_t KNI_API KniIsNullIndirect(HKENV hKEnv);

/* Checks if two stack top values are equal by reference */
kbool_t KNI_API KniIsReferenceEqual(HKENV hKEnv);

/* Checks if stack top value is class instance */
kbool_t KNI_API KniIsClass(HKENV hKEnv);
/* Checks if stack top value is delegate instance */
kbool_t KNI_API KniIsDelegate(HKENV hKEnv);
/* Checks if stack top value is array */
kbool_t KNI_API KniIsArray(HKENV hKEnv);

/* Checks if stack top value is instance of the given class */
kbool_t KNI_API KniIsInstanceOfClass(HKENV hKEnv, HKCLASS hKClass);
/* Checks if stack top value is instance of the given delegate */
kbool_t KNI_API KniIsInstanceOfDelegate(HKENV hKEnv, HKDELEGATE hKDelegate);

/*===================================================*/

/* Invokes */

/* Invokes a method */
KRESULT KNI_API KniInvoke(HKENV hKEnv, HKMETHOD hKMethod);
/* Invokes a delegate object on stack top */
KRESULT KNI_API KniInvokeObject(HKENV hKEnv);

/* Marks the beginning of a protected region */
void KNI_API KniEnterProtectedRegion(HKENV hKEnv, HKTYPE hKTypeExc, KEXCFUNC *pKExcFunc);
/* Marks the end of a protected region */
void KNI_API KniLeaveProtectedRegion(HKENV hKEnv);
/* Throws an exception loaded onto stack */
void KNI_API KniThrowException(HKENV hKEnv);

/* Initializes local variables for current native method */
void KNI_API KniInitLocals(HKENV hKEnv, HKTYPE *pHKTypes, kushort_t count);

/*===================================================*/

/* Metadata and types */

/* Gets datatype of stack top value */
HKTYPE KNI_API KniGetType(HKENV hKEnv);

/* Gets datatype of local variable */
HKTYPE KNI_API KniGetTypeLocal(HKENV hKEnv, kushort_t index);
/* Gets datatype of argument */
HKTYPE KNI_API KniGetTypeArg(HKENV hKEnv, kushort_t index);
/* Gets datatype of ByRef argument */
HKTYPE KNI_API KniGetTypeArgByRef(HKENV hKEnv, kushort_t index);
/* Gets datatype of instance field */
HKTYPE KNI_API KniGetTypeField(HKENV hKEnv, HKFIELD hKField);
/* Gets datatype of static field */
HKTYPE KNI_API KniGetTypeStaticField(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
/* Gets datatype of array element */
HKTYPE KNI_API KniGetTypeElement(HKENV hKEnv, knuint_t index);
/* Gets datatype of indirect object */
HKTYPE KNI_API KniGetTypeIndirect(HKENV hKEnv);

/* Gets ClassDef having the given name */
HKCLASS KNI_API KniGetClass(HKENV hKEnv, kstring_t ksName);
/* Gets DelegateDef having the given name */
HKDELEGATE KNI_API KniGetDelegate(HKENV hKEnv, kstring_t ksName);
/* Gets FieldDef having the given name */
HKFIELD KNI_API KniGetField(HKENV hKEnv, HKCLASS hKClass, kstring_t ksName);
/* Gets MethodDef having the given name */
HKMETHOD KNI_API KniGetMethod(HKENV hKEnv, HKCLASS hKClass, kstring_t ksName);

/* Gets handle of a primitive type */
HKTYPE KNI_API KniGetPrimitiveType(HKENV hKEnv, ktypetag_t tag);

/* Creates TypeDef from the given attributes */
HKTYPE KNI_API KniCreateType(HKENV hKEnv, ktypetag_t tag, kushort_t dim, HKUSERTYPE hKClassOrDelegate);

/* Checks if the given type is class type */
kbool_t KNI_API KniIsClassType(HKENV hKEnv, HKTYPE hKType);
/* Checks if the given type is delegate type */
kbool_t KNI_API KniIsDelegateType(HKENV hKEnv, HKTYPE hKType);
/* Checks if the given type is array type */
kbool_t KNI_API KniIsArrayType(HKENV hKEnv, HKTYPE hKType);

/*===================================================*/

#ifdef __cplusplus
}
#endif

#endif/*_KNI_H*/
