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

	KT_BYREF     = 0x4000,	/* a flag to determine address object */

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

/* Native extern function prototype */
typedef NINVOKE_RESULT (*NFUNC)(void);

typedef unsigned char  kbool_t;
typedef wchar_t        kchar_t;
typedef uint8_t        kbyte_t;
typedef int8_t         ksbyte_t;
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
kbool_t KNI_API KniGetLocalBool(kushort_t index);
/* Gets local variable as char value */
kchar_t KNI_API KniGetLocalChar(kushort_t index);
/* Gets local variable as byte value */
kbyte_t KNI_API KniGetLocalByte(kushort_t index);
/* Gets local variable as sbool value */
ksbyte_t KNI_API KniGetLocalSByte(kushort_t index);
/* Gets local variable as short value */
kshort_t KNI_API KniGetLocalShort(kushort_t index);
/* Gets local variable as ushort value */
kushort_t KNI_API KniGetLocalUShort(kushort_t index);
/* Gets local variable as int value */
kint_t KNI_API KniGetLocalInt(kushort_t index);
/* Gets local variable as uint value */
kuint_t KNI_API KniGetLocalUInt(kushort_t index);
/* Gets local variable as long value */
klong_t KNI_API KniGetLocalLong(kushort_t index);
/* Gets local variable as ulong value */
kulong_t KNI_API KniGetLocalULong(kushort_t index);
/* Gets local variable as float value */
kfloat_t KNI_API KniGetLocalFloat(kushort_t index);
/* Gets local variable as double value */
kdouble_t KNI_API KniGetLocalDouble(kushort_t index);
/* Gets local variable as string value */
kstring_t KNI_API KniGetLocalString(kushort_t index);
/* Gets local variable as raw value */
kref_t KNI_API KniGetLocalRaw(kushort_t index);
/* Gets length of string or array local variable */
knuint_t KNI_API KniGetLocalLength(kushort_t index);


/* Gets argument as bool value */
kbool_t KNI_API KniGetArgBool(kushort_t index);
/* Gets argument as char value */
kchar_t KNI_API KniGetArgChar(kushort_t index);
/* Gets argument as byte value */
kbyte_t KNI_API KniGetArgByte(kushort_t index);
/* Gets argument as sbyte value */
ksbyte_t KNI_API KniGetArgSByte(kushort_t index);
/* Gets argument as short value */
kshort_t KNI_API KniGetArgShort(kushort_t index);
/* Gets argument as ushort value */
kushort_t KNI_API KniGetArgUShort(kushort_t index);
/* Gets argument as int value */
kint_t KNI_API KniGetArgInt(kushort_t index);
/* Gets argument as uint value */
kuint_t KNI_API KniGetArgUInt(kushort_t index);
/* Gets argument as long value */
klong_t KNI_API KniGetArgLong(kushort_t index);
/* Gets argument as ulong value */
kulong_t KNI_API KniGetArgULong(kushort_t index);
/* Gets argument as float value */
kfloat_t KNI_API KniGetArgFloat(kushort_t index);
/* Gets argument as double value */
kdouble_t KNI_API KniGetArgDouble(kushort_t index);
/* Gets argument as string value */
kstring_t KNI_API KniGetArgString(kushort_t index);
/* Gets argument as raw value */
kref_t KNI_API KniGetArgRaw(kushort_t index);
/* Gets length of string or array argument */
knuint_t KNI_API KniGetArgLength(kushort_t index);


/* Gets instance field as bool value */
kbool_t KNI_API KniGetFieldBool(HKFIELD hKField);
/* Gets instance field as char value */
kchar_t KNI_API KniGetFieldChar(HKFIELD hKField);
/* Gets instance field as byte value */
kbyte_t KNI_API KniGetFieldByte(HKFIELD hKField);
/* Gets instance field as sbyte value */
ksbyte_t KNI_API KniGetFieldSByte(HKFIELD hKField);
/* Gets instance field as short value */
kshort_t KNI_API KniGetFieldShort(HKFIELD hKField);
/* Gets instance field as ushort value */
kushort_t KNI_API KniGetFieldUShort(HKFIELD hKField);
/* Gets instance field as int value */
kint_t KNI_API KniGetFieldInt(HKFIELD hKField);
/* Gets instance field as uint value */
kuint_t KNI_API KniGetFieldUInt(HKFIELD hKField);
/* Gets instance field as long value */
klong_t KNI_API KniGetFieldLong(HKFIELD hKField);
/* Gets instance field as ulong value */
kulong_t KNI_API KniGetFieldULong(HKFIELD hKField);
/* Gets instance field as float value */
kfloat_t KNI_API KniGetFieldFloat(HKFIELD hKField);
/* Gets instance field as double value */
kdouble_t KNI_API KniGetFieldDouble(HKFIELD hKField);
/* Gets instance field as string value */
kstring_t KNI_API KniGetFieldString(HKFIELD hKField);
/* Gets instance field as raw value */
kref_t KNI_API KniGetFieldRaw(HKFIELD hKField);
/* Gets length of string or array instance field */
knuint_t KNI_API KniGetFieldLength(HKFIELD hKField);

/* Gets static field as bool value */
kbool_t KNI_API KniGetStaticFieldBool(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as char value */
kchar_t KNI_API KniGetStaticFieldChar(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as byte value */
kbyte_t KNI_API KniGetStaticFieldByte(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as sbyte value */
ksbyte_t KNI_API KniGetStaticFieldSByte(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as short value */
kshort_t KNI_API KniGetStaticFieldShort(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as ushort value */
kushort_t KNI_API KniGetStaticFieldUShort(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as int value */
kint_t KNI_API KniGetStaticFieldInt(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as uint value */
kuint_t KNI_API KniGetStaticFieldUInt(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as long value */
klong_t KNI_API KniGetStaticFieldLong(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as ulong value */
kulong_t KNI_API KniGetStaticFieldULong(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as float value */
kfloat_t KNI_API KniGetStaticFieldFloat(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as double value */
kdouble_t KNI_API KniGetStaticFieldDouble(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as string value */
kstring_t KNI_API KniGetStaticFieldString(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as raw value */
kref_t KNI_API KniGetStaticFieldRaw(HKCLASS hKClass, HKFIELD hKField);
/* Gets length of string or array static field */
knuint_t KNI_API KniGetStaticFieldLength(HKCLASS hKClass, HKFIELD hKField);


/* Gets element as bool value */
kbool_t KNI_API KniGetElementBool(knuint_t index);
/* Gets element as char value */
kchar_t KNI_API KniGetElementChar(knuint_t index);
/* Gets element as byte value */
kbyte_t KNI_API KniGetElementByte(knuint_t index);
/* Gets element as sbyte value */
ksbyte_t KNI_API KniGetElementSByte(knuint_t index);
/* Gets element as short value */
kshort_t KNI_API KniGetElementShort(knuint_t index);
/* Gets element as ushort value */
kushort_t KNI_API KniGetElementUShort(knuint_t index);
/* Gets element as int value */
kint_t KNI_API KniGetElementInt(knuint_t index);
/* Gets element as uint value */
kuint_t KNI_API KniGetElementUInt(knuint_t index);
/* Gets element as long value */
klong_t KNI_API KniGetElementLong(knuint_t index);
/* Gets element as ulong value */
kulong_t KNI_API KniGetElementULong(knuint_t index);
/* Gets element as float value */
kfloat_t KNI_API KniGetElementFloat(knuint_t index);
/* Gets element as double value */
kdouble_t KNI_API KniGetElementDouble(knuint_t index);
/* Gets element as string value */
kstring_t KNI_API KniGetElementString(knuint_t index);
/* Gets element as raw value */
kref_t KNI_API KniGetElementRaw(knuint_t index);
/* Gets length of string or array typed element */
knuint_t KNI_API KniGetElementLength(knuint_t index);


/* Gets indirect object as bool value */
kbool_t KNI_API KniGetIndirectBool(void);
/* Gets indirect object as char value */
kchar_t KNI_API KniGetIndirectChar(void);
/* Gets indirect object as byte value */
kbyte_t KNI_API KniGetIndirectByte(void);
/* Gets indirect object as sbyte value */
ksbyte_t KNI_API KniGetIndirectSByte(void);
/* Gets indirect object as short value */
kshort_t KNI_API KniGetIndirectShort(void);
/* Gets indirect object as ushort value */
kushort_t KNI_API KniGetIndirectUShort(void);
/* Gets indirect object as int value */
kint_t KNI_API KniGetIndirectInt(void);
/* Gets indirect object as uint value */
kuint_t KNI_API KniGetIndirectUInt(void);
/* Gets indirect object as long value */
klong_t KNI_API KniGetIndirectLong(void);
/* Gets indirect object as ulong value */
kulong_t KNI_API KniGetIndirectULong(void);
/* Gets indirect object as float value */
kfloat_t KNI_API KniGetIndirectFloat(void);
/* Gets indirect object as double value */
kdouble_t KNI_API KniGetIndirectDouble(void);
/* Gets indirect object as string value */
kstring_t KNI_API KniGetIndirectString(void);
/* Gets indirect object as raw value */
kref_t KNI_API KniGetIndirectRaw(void);
/* Gets length of string or array indirect object */
knuint_t KNI_API KniGetIndirectLength(void);

/*===================================================*/

/* Sets primitive values */

/* Sets bool value to local variable */
void KNI_API KniSetLocalBool(kushort_t index, kbool_t val);
/* Sets char value to local variable */
void KNI_API KniSetLocalChar(kushort_t index, kchar_t val);
/* Sets byte value to local variable */
void KNI_API KniSetLocalByte(kushort_t index, kbyte_t val);
/* Sets sbyte value to local variable */
void KNI_API KniSetLocalSByte(kushort_t index, ksbyte_t val);
/* Sets short value to local variable */
void KNI_API KniSetLocalShort(kushort_t index, kshort_t val);
/* Sets ushort value to local variable */
void KNI_API KniSetLocalUShort(kushort_t index, kushort_t val);
/* Sets int value to local variable */
void KNI_API KniSetLocalInt(kushort_t index, kint_t val);
/* Sets uint value to local variable */
void KNI_API KniSetLocalUInt(kushort_t index, kuint_t val);
/* Sets long value to local variable */
void KNI_API KniSetLocalLong(kushort_t index, klong_t val);
/* Sets ulong value to local variable */
void KNI_API KniSetLocalULong(kushort_t index, kulong_t val);
/* Sets float value to local variable */
void KNI_API KniSetLocalFloat(kushort_t index, kfloat_t val);
/* Sets double value to local variable */
void KNI_API KniSetLocalDouble(kushort_t index, kdouble_t val);
/* Sets string value to local variable */
void KNI_API KniSetLocalString(kushort_t index, kstring_t val, knuint_t length);
/* Sets raw value to local variable */
void KNI_API KniSetLocalRaw(kushort_t index, kref_t val);
/* Set null value to local variable */
void KNI_API KniSetLocalNull(kushort_t index);

/* Sets bool value to argument */
void KNI_API KniSetArgBool(kushort_t index, kbool_t val);
/* Sets char value to argument */
void KNI_API KniSetArgChar(kushort_t index, kchar_t val);
/* Sets byte value to argument */
void KNI_API KniSetArgByte(kushort_t index, kbyte_t val);
/* Sets sbyte value to argument */
void KNI_API KniSetArgSByte(kushort_t index, ksbyte_t val);
/* Sets short value to argument */
void KNI_API KniSetArgShort(kushort_t index, kshort_t val);
/* Sets ushort value to argument */
void KNI_API KniSetArgUShort(kushort_t index, kushort_t val);
/* Sets int value to argument */
void KNI_API KniSetArgInt(kushort_t index, kint_t val);
/* Sets uint value to argument */
void KNI_API KniSetArgUInt(kushort_t index, kuint_t val);
/* Sets long value to argument */
void KNI_API KniSetArgLong(kushort_t index, klong_t val);
/* Sets ulong value to argument */
void KNI_API KniSetArgULong(kushort_t index, kulong_t val);
/* Sets float value to argument */
void KNI_API KniSetArgFloat(kushort_t index, kfloat_t val);
/* Sets double value to argument */
void KNI_API KniSetArgDouble(kushort_t index, kdouble_t val);
/* Sets string value to argument */
void KNI_API KniSetArgString(kushort_t index, kstring_t val, knuint_t length);
/* Sets raw value to argument */
void KNI_API KniSetArgRaw(kushort_t index, kref_t val);
/* Set null value to argument */
void KNI_API KniSetArgNull(kushort_t index);


/* Sets bool value to instance field */
void KNI_API KniSetFieldBool(HKFIELD hKField, kbool_t val);
/* Sets char value to instance field */
void KNI_API KniSetFieldChar(HKFIELD hKField, kchar_t val);
/* Sets byte value to instance field */
void KNI_API KniSetFieldByte(HKFIELD hKField, kbyte_t val);
/* Sets sbyte value to instance field */
void KNI_API KniSetFieldSByte(HKFIELD hKField, ksbyte_t val);
/* Sets short value to instance field */
void KNI_API KniSetFieldShort(HKFIELD hKField, kshort_t val);
/* Sets ushort value to instance field */
void KNI_API KniSetFieldUShort(HKFIELD hKField, kushort_t val);
/* Sets int value to instance field */
void KNI_API KniSetFieldInt(HKFIELD hKField, kint_t val);
/* Sets uint value to instance field */
void KNI_API KniSetFieldUInt(HKFIELD hKField, kuint_t val);
/* Sets long value to instance field */
void KNI_API KniSetFieldLong(HKFIELD hKField, klong_t val);
/* Sets ulong value to instance field */
void KNI_API KniSetFieldULong(HKFIELD hKField, kulong_t val);
/* Sets float value to instance field */
void KNI_API KniSetFieldFloat(HKFIELD hKField, kfloat_t val);
/* Sets double value to instance field */
void KNI_API KniSetFieldDouble(HKFIELD hKField, kdouble_t val);
/* Sets string value to instance field */
void KNI_API KniSetFieldString(HKFIELD hKField, kstring_t val, knuint_t length);
/* Sets raw value to instannce field */
void KNI_API KniSetFieldRaw(HKFIELD hKField, kref_t val);
/* Set null value to instance field */
void KNI_API KniSetFieldNull(HKFIELD hKField);


/* Sets bool value to static field */
void KNI_API KniSetStaticFieldBool(HKCLASS hKClass, HKFIELD hKField, kbool_t val);
/* Sets char value to static field */
void KNI_API KniSetStaticFieldChar(HKCLASS hKClass, HKFIELD hKField, kchar_t val);
/* Sets byte value to static field */
void KNI_API KniSetStaticFieldByte(HKCLASS hKClass, HKFIELD hKField, kbyte_t val);
/* Sets sbyte value to static field */
void KNI_API KniSetStaticFieldSByte(HKCLASS hKClass, HKFIELD hKField, ksbyte_t val);
/* Sets short value to static field */
void KNI_API KniSetStaticFieldShort(HKCLASS hKClass, HKFIELD hKField, kshort_t val);
/* Sets ushort value to static field */
void KNI_API KniSetStaticFieldUShort(HKCLASS hKClass, HKFIELD hKField, kushort_t val);
/* Sets int value to static field */
void KNI_API KniSetStaticFieldInt(HKCLASS hKClass, HKFIELD hKField, kint_t val);
/* Sets uint value to static field */
void KNI_API KniSetStaticFieldUInt(HKCLASS hKClass, HKFIELD hKField, kuint_t val);
/* Sets long value to static field */
void KNI_API KniSetStaticFieldLong(HKCLASS hKClass, HKFIELD hKField, klong_t val);
/* Sets ulong value to static field */
void KNI_API KniSetStaticFieldULong(HKCLASS hKClass, HKFIELD hKField, kulong_t val);
/* Sets float value to static field */
void KNI_API KniSetStaticFieldFloat(HKCLASS hKClass, HKFIELD hKField, kfloat_t val);
/* Sets double value to static field */
void KNI_API KniSetStaticFieldDouble(HKCLASS hKClass, HKFIELD hKField, kdouble_t val);
/* Sets string value to static field */
void KNI_API KniSetStaticFieldString(HKCLASS hKClass, HKFIELD hKField, kstring_t val, knuint_t length);
/* Sets raw value to static field */
void KNI_API KniSetStaticFieldRaw(HKCLASS hKClass, HKFIELD hKField, kref_t val);
/* Set null value to static field */
void KNI_API KniSetStaticFieldNull(HKCLASS hKClass, HKFIELD hKField);


/* Sets bool value to array element */
void KNI_API KniSetElementBool(knuint_t index, kbool_t val);
/* Sets char value to array element */
void KNI_API KniSetElementChar(knuint_t index, kchar_t val);
/* Sets byte value to array element */
void KNI_API KniSetElementByte(knuint_t index, kbyte_t val);
/* Sets sbyte value to array element */
void KNI_API KniSetElementSByte(knuint_t index, ksbyte_t val);
/* Sets short value to array element */
void KNI_API KniSetElementShort(knuint_t index, kshort_t val);
/* Sets ushort value to array element */
void KNI_API KniSetElementUShort(knuint_t index, kushort_t val);
/* Sets int value to array element */
void KNI_API KniSetElementInt(knuint_t index, kint_t val);
/* Sets uint value to array element */
void KNI_API KniSetElementUInt(knuint_t index, kuint_t val);
/* Sets long value to array element */
void KNI_API KniSetElementLong(knuint_t index, klong_t val);
/* Sets ulong value to array element */
void KNI_API KniSetElementULong(knuint_t index, kulong_t val);
/* Sets float value to array element */
void KNI_API KniSetElementFloat(knuint_t index, kfloat_t val);
/* Sets double value to array element */
void KNI_API KniSetElementDouble(knuint_t index, kdouble_t val);
/* Sets string value to array element */
void KNI_API KniSetElementString(knuint_t index, kstring_t val, knuint_t length);
/* Sets raw value to array element */
void KNI_API KniSetElementRaw(knuint_t index, kref_t val);
/* Sets null value to array element */
void KNI_API KniSetElementNull(knuint_t index);


/* Sets bool value to indirect object */
void KNI_API KniSetIndirectBool(kbool_t val);
/* Sets char value to indirect object */
void KNI_API KniSetIndirectChar(kchar_t val);
/* Sets byte value to indirect object */
void KNI_API KniSetIndirectByte(kbyte_t val);
/* Sets sbyte value to indirect object */
void KNI_API KniSetIndirectSByte(ksbyte_t val);
/* Sets short value to indirect object */
void KNI_API KniSetIndirectShort(kshort_t val);
/* Sets ushort value to indirect object */
void KNI_API KniSetIndirectUShort(kushort_t val);
/* Sets int value to indirect object */
void KNI_API KniSetIndirectInt(kint_t val);
/* Sets uint value to indirect object */
void KNI_API KniSetIndirectUInt(kuint_t val);
/* Sets long value to indirect object */
void KNI_API KniSetIndirectLong(klong_t val);
/* Sets ulong value to indirect object */
void KNI_API KniSetIndirectULong(kulong_t val);
/* Sets float value to indirect object */
void KNI_API KniSetIndirectFloat(kfloat_t val);
/* Sets double value to indirect object */
void KNI_API KniSetIndirectDouble(kdouble_t val);
/* Sets string value to indirect object */
void KNI_API KniSetIndirectString(kstring_t val, knuint_t length);
/* Sets raw value to indirect object */
void KNI_API KniSetIndirectRaw(kref_t val);
/* Sets null value to indirect object */
void KNI_API KniSetIndirectNull(void);

/*===================================================*/

/* Loads values onto stack */

/* Loads bool value onto stack */
void KNI_API KniLoadBool(kbool_t val);
/* Loads char value onto stack */
void KNI_API KniLoadChar(kchar_t val);
/* Loads byte value onto stack */
void KNI_API KniLoadByte(kbyte_t val);
/* Loads sbyte value onto stack */
void KNI_API KniLoadSByte(ksbyte_t val);
/* Loads short value onto stack */
void KNI_API KniLoadShort(kshort_t val);
/* Loads ushort value onto stack */
void KNI_API KniLoadUShort(kushort_t val);
/* Loads int value onto stack */
void KNI_API KniLoadInt(kint_t val);
/* Loads uint value onto stack */
void KNI_API KniLoadUInt(kuint_t val);
/* Loads long value onto stack */
void KNI_API KniLoadLong(klong_t val);
/* Loads ulong value onto stack */
void KNI_API KniLoadULong(kulong_t val);
/* Loads float value onto stack */
void KNI_API KniLoadFloat(kfloat_t val);
/* Loads double value onto stack */
void KNI_API KniLoadDouble(kdouble_t val);
/* Loads string value onto stack */
void KNI_API KniLoadString(kstring_t val, knuint_t length);
/* Loads raw value onto stack */
void KNI_API KniLoadRaw(kref_t val);

/* Loads local variable onto stack */
void KNI_API KniLoadLocal(kushort_t index);
/* Loads argument onto stack */
void KNI_API KniLoadArg(kushort_t index);
/* Loads ByRef argument onto stack */
void KNI_API KniLoadArgByRef(kushort_t index);
/* Loads instance field onto stack */
void KNI_API KniLoadField(HKFIELD hKField);
/* Loads static field onto stack */
void KNI_API KniLoadStaticField(HKCLASS hKClass, HKFIELD hKField);
/* Loads array element onto stack */
void KNI_API KniLoadElement(knuint_t index);
/* Loads indirect object onto stack */
void KNI_API KniLoadIndirect(void);

/* Loads length of string or array local variable onto stack */
void KNI_API KniLoadLocalLength(kushort_t index);
/* Loads length of string or array argument onto stack */
void KNI_API KniLoadArgLength(kushort_t index);
/* Loads length of string or array ByRef argument onto stack */
void KNI_API KniLoadArgLengthByRef(kushort_t index);
/* Loads length of string or array instance field onto stack */
void KNI_API KniLoadFieldLength(HKFIELD hKField);
/* Loads length of string or array static field onto stack */
void KNI_API KniLoadStaticFieldLength(HKCLASS hKClass, HKFIELD hKField);
/* Loads length of string or array typed array element onto stack */
void KNI_API KniLoadElementLength(knuint_t index);
/* Loads length of string or array indirect object onto stack */
void KNI_API KniLoadIndirectLength(void);

/* Loads element of array typed local variable onto stack */
void KNI_API KniLoadLocalElement(kushort_t index, knuint_t elemIndex);
/* Loads element of array typed argument onto stack */
void KNI_API KniLoadArgElement(kushort_t index, knuint_t elemIndex);
/* Loads element of array typed ByRef argument onto stack */
void KNI_API KniLoadArgElementByRef(kushort_t index, knuint_t elemIndex);
/* Loads element of array typed instance field onto stack */
void KNI_API KniLoadFieldElement(HKFIELD hKField, knuint_t elemIndex);
/* Loads element of array typed static field onto stack */
void KNI_API KniLoadStaticFieldElement(HKCLASS hKClass, HKFIELD hKField, knuint_t elemIndex);
/* Loads element of array typed array element onto stack */
void KNI_API KniLoadElementElement(knuint_t index, knuint_t elemIndex);
/* Loads element of array typed  onto stack */
void KNI_API KniLoadIndirectElement(knuint_t elemIndex);

/* Loads address of local variable onto stack */
void KNI_API KniLoadLocalAddress(kushort_t index);
/* Loads address of argument onto stack */
void KNI_API KniLoadArgAddress(kushort_t index);
/* Loads address of instance field onto stack */
void KNI_API KniLoadFieldAddress(HKFIELD hKField);
/* Loads address of static field onto stack */
void KNI_API KniLoadStaticFieldAddress(HKCLASS hKClass, HKFIELD hKField);
/* Loads address of array element onto stack */
void KNI_API KniLoadElementAddress(knuint_t index);

/* Loads null value onto stack */
void KNI_API KniLoadNull(void);

/* Loads new class instance onto stack */
void KNI_API KniLoadNewObject(HKCLASS hKClass);
/* Loads new delegate instance onto stack */
void KNI_API KniLoadNewDelegate(HKDELEGATE hKDelegate, HKMETHOD hKMethod);
/* Loads new array onto stack */
void KNI_API KniLoadNewArray(HKTYPE hKType, knuint_t length);
/* Loads new array onto stack, elements are already loaded */
void KNI_API KniLoadNewArrayBaking(HKTYPE hKType, knuint_t length);

/*===================================================*/

/* Stores values */

/* Stores stack top value into local variable */
void KNI_API KniStoreLocal(kushort_t index);
/* Stores stack top value into argument */
void KNI_API KniStoreArg(kushort_t index);
/* Stores stack top value into ByRef argument */
void KNI_API KniStoreArgByRef(kushort_t index);
/* Stores stack top value into instance field */
void KNI_API KniStoreField(HKFIELD hKField);
/* Stores stack top value into static field */
void KNI_API KniStoreStaticField(HKCLASS hKClass, HKFIELD hKField);
/* Stores stack top value into array element */
void KNI_API KniStoreElement(knuint_t index);
/* Stores stack top value into indirect object */
void KNI_API KniStoreIndirect(void);

/*===================================================*/

/* Tests values */

/* Checks if stack top value is null */
kbool_t KNI_API KniIsNull(void);
/* Checks if local variable is null */
kbool_t KNI_API KniIsNullLocal(kushort_t index);
/* Checks if argument is null */
kbool_t KNI_API KniIsNullArg(kushort_t index);
/* Checks if ByRef argument is null */
kbool_t KNI_API KniIsNullArgByRef(kushort_t index);
/* Checks if instance field is null */
kbool_t KNI_API KniIsNullField(HKFIELD hKField);
/* Checks if static field is null */
kbool_t KNI_API KniIsNullStaticField(HKCLASS hKClass, HKFIELD hKField);
/* Checks if array element is null */
kbool_t KNI_API KniIsNullElement(knuint_t index);
/* Checks if indirect object is null */
kbool_t KNI_API KniIsNullIndirect(void);

/* Checks if two stack top values are equal by reference */
kbool_t KNI_API KniIsReferenceEqual(void);

/* Checks if stack top value is class instance */
kbool_t KNI_API KniIsClass(void);
/* Checks if stack top value is delegate instance */
kbool_t KNI_API KniIsDelegate(void);
/* Checks if stack top value is array */
kbool_t KNI_API KniIsArray(void);

/* Checks if stack top value is instance of the given class */
kbool_t KNI_API KniIsInstanceOfClass(HKCLASS hKClass);
/* Checks if stack top value is instance of the given delegate */
kbool_t KNI_API KniIsInstanceOfDelegate(HKDELEGATE hKDelegate);

/*===================================================*/

/* Invokes */

/* Invokes a method */
KRESULT KNI_API KniInvoke(HKMETHOD hKMethod);
/* Invokes a delegate object on stack top */
KRESULT KNI_API KniInvokeObject(void);

/* Checks if an exception has been thrown */
kbool_t KNI_API KniHasException(void);
/* Prints the stack trace of the current exception to the standard error output (stderr) */
void KNI_API KniPrintExceptionDescription(void);
/* Clears the current exception */
void KNI_API KniClearException(void);
/* Throws an exception loaded onto stack */
void KNI_API KniThrowException(HKFIELD hKFCode);
/* Throws an exception loaded onto stack */
void KNI_API KniThrowExceptionEx(kstring_t message, knuint_t length);

/* Initializes local variables for current native method */
void KNI_API KniInitLocals(HKTYPE *pHKTypes, kushort_t count);

/*===================================================*/

/* Metadata and types */

/* Gets datatype of stack top value */
HKTYPE KNI_API KniGetType(void);

/* Gets datatype of local variable */
HKTYPE KNI_API KniGetTypeLocal(kushort_t index);
/* Gets datatype of argument */
HKTYPE KNI_API KniGetTypeArg(kushort_t index);
/* Gets datatype of ByRef argument */
HKTYPE KNI_API KniGetTypeArgByRef(kushort_t index);
/* Gets datatype of instance field */
HKTYPE KNI_API KniGetTypeField(HKFIELD hKField);
/* Gets datatype of static field */
HKTYPE KNI_API KniGetTypeStaticField(HKCLASS hKClass, HKFIELD hKField);
/* Gets datatype of array element */
HKTYPE KNI_API KniGetTypeElement(knuint_t index);
/* Gets datatype of indirect object */
HKTYPE KNI_API KniGetTypeIndirect(void);

/* Gets ClassDef having the given name */
HKCLASS KNI_API KniGetClass(kstring_t ksName);
/* Gets DelegateDef having the given name */
HKDELEGATE KNI_API KniGetDelegate(kstring_t ksName);
/* Gets FieldDef having the given name */
HKFIELD KNI_API KniGetField(HKCLASS hKClass, kstring_t ksName);
/* Gets MethodDef having the given name */
HKMETHOD KNI_API KniGetMethod(HKCLASS hKClass, kstring_t ksName);

/* Gets handle of a primitive type */
HKTYPE KNI_API KniGetPrimitiveType(ktypetag_t tag);

/* Creates TypeDef from the given attributes */
HKTYPE KNI_API KniCreateType(ktypetag_t tag, kushort_t dim, HKUSERTYPE hKClassOrDelegate);

/* Checks if the given type is class type */
kbool_t KNI_API KniIsClassType(HKTYPE hKType);
/* Checks if the given type is delegate type */
kbool_t KNI_API KniIsDelegateType(HKTYPE hKType);
/* Checks if the given type is array type */
kbool_t KNI_API KniIsArrayType(HKTYPE hKType);

/*===================================================*/

#ifdef __cplusplus
}
#endif

#endif/*_KNI_H*/
