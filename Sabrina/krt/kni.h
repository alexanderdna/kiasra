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
typedef enum KTYPETAG
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

	KT_ARRAY   = 0x0100,	/* `array` storage type */
	KT_OBJECT  = 0x1000,	/* `object` storage type */
	KT_RAW     = 0x2000,	/* `raw` storage type */

	KT_BYREF   = 0x4000,	/*!bit flag! address type (ByRef argument) */

	KT_SCALAR_MASK = KT_VOID | KT_BOOL | KT_CHAR | KT_BYTE | KT_SBYTE |
					KT_SHORT | KT_USHORT | KT_INT | KT_UINT | KT_LONG | KT_ULONG |
					KT_FLOAT | KT_DOUBLE | KT_STRING |
					KT_CLASS | KT_DELEGATE,

	KT_REF_MASK = KT_ARRAY | KT_OBJECT | KT_RAW
} KTYPETAG;

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
KNI_API kbool_t KniGetLocalBool(kushort_t index);
/* Gets local variable as char value */
KNI_API kchar_t KniGetLocalChar(kushort_t index);
/* Gets local variable as byte value */
KNI_API kbyte_t KniGetLocalByte(kushort_t index);
/* Gets local variable as sbool value */
KNI_API ksbyte_t KniGetLocalSByte(kushort_t index);
/* Gets local variable as short value */
KNI_API kshort_t KniGetLocalShort(kushort_t index);
/* Gets local variable as ushort value */
KNI_API kushort_t KniGetLocalUShort(kushort_t index);
/* Gets local variable as int value */
KNI_API kint_t KniGetLocalInt(kushort_t index);
/* Gets local variable as uint value */
KNI_API kuint_t KniGetLocalUInt(kushort_t index);
/* Gets local variable as long value */
KNI_API klong_t KniGetLocalLong(kushort_t index);
/* Gets local variable as ulong value */
KNI_API kulong_t KniGetLocalULong(kushort_t index);
/* Gets local variable as float value */
KNI_API kfloat_t KniGetLocalFloat(kushort_t index);
/* Gets local variable as double value */
KNI_API kdouble_t KniGetLocalDouble(kushort_t index);
/* Gets local variable as string value */
KNI_API kstring_t KniGetLocalString(kushort_t index);
/* Gets local variable as raw value */
KNI_API kref_t KniGetLocalRaw(kushort_t index);
/* Gets length of string or array local variable */
KNI_API knuint_t KniGetLocalLength(kushort_t index);


/* Gets argument as bool value */
KNI_API kbool_t KniGetArgBool(kushort_t index);
/* Gets argument as char value */
KNI_API kchar_t KniGetArgChar(kushort_t index);
/* Gets argument as byte value */
KNI_API kbyte_t KniGetArgByte(kushort_t index);
/* Gets argument as sbyte value */
KNI_API ksbyte_t KniGetArgSByte(kushort_t index);
/* Gets argument as short value */
KNI_API kshort_t KniGetArgShort(kushort_t index);
/* Gets argument as ushort value */
KNI_API kushort_t KniGetArgUShort(kushort_t index);
/* Gets argument as int value */
KNI_API kint_t KniGetArgInt(kushort_t index);
/* Gets argument as uint value */
KNI_API kuint_t KniGetArgUInt(kushort_t index);
/* Gets argument as long value */
KNI_API klong_t KniGetArgLong(kushort_t index);
/* Gets argument as ulong value */
KNI_API kulong_t KniGetArgULong(kushort_t index);
/* Gets argument as float value */
KNI_API kfloat_t KniGetArgFloat(kushort_t index);
/* Gets argument as double value */
KNI_API kdouble_t KniGetArgDouble(kushort_t index);
/* Gets argument as string value */
KNI_API kstring_t KniGetArgString(kushort_t index);
/* Gets argument as raw value */
KNI_API kref_t KniGetArgRaw(kushort_t index);
/* Gets length of string or array argument */
KNI_API knuint_t KniGetArgLength(kushort_t index);


/* Gets instance field as bool value */
KNI_API kbool_t KniGetFieldBool(HKFIELD hKField);
/* Gets instance field as char value */
KNI_API kchar_t KniGetFieldChar(HKFIELD hKField);
/* Gets instance field as byte value */
KNI_API kbyte_t KniGetFieldByte(HKFIELD hKField);
/* Gets instance field as sbyte value */
KNI_API ksbyte_t KniGetFieldSByte(HKFIELD hKField);
/* Gets instance field as short value */
KNI_API kshort_t KniGetFieldShort(HKFIELD hKField);
/* Gets instance field as ushort value */
KNI_API kushort_t KniGetFieldUShort(HKFIELD hKField);
/* Gets instance field as int value */
KNI_API kint_t KniGetFieldInt(HKFIELD hKField);
/* Gets instance field as uint value */
KNI_API kuint_t KniGetFieldUInt(HKFIELD hKField);
/* Gets instance field as long value */
KNI_API klong_t KniGetFieldLong(HKFIELD hKField);
/* Gets instance field as ulong value */
KNI_API kulong_t KniGetFieldULong(HKFIELD hKField);
/* Gets instance field as float value */
KNI_API kfloat_t KniGetFieldFloat(HKFIELD hKField);
/* Gets instance field as double value */
KNI_API kdouble_t KniGetFieldDouble(HKFIELD hKField);
/* Gets instance field as string value */
KNI_API kstring_t KniGetFieldString(HKFIELD hKField);
/* Gets instance field as raw value */
KNI_API kref_t KniGetFieldRaw(HKFIELD hKField);
/* Gets length of string or array instance field */
KNI_API knuint_t KniGetFieldLength(HKFIELD hKField);

/* Gets static field as bool value */
KNI_API kbool_t KniGetStaticFieldBool(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as char value */
KNI_API kchar_t KniGetStaticFieldChar(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as byte value */
KNI_API kbyte_t KniGetStaticFieldByte(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as sbyte value */
KNI_API ksbyte_t KniGetStaticFieldSByte(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as short value */
KNI_API kshort_t KniGetStaticFieldShort(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as ushort value */
KNI_API kushort_t KniGetStaticFieldUShort(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as int value */
KNI_API kint_t KniGetStaticFieldInt(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as uint value */
KNI_API kuint_t KniGetStaticFieldUInt(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as long value */
KNI_API klong_t KniGetStaticFieldLong(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as ulong value */
KNI_API kulong_t KniGetStaticFieldULong(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as float value */
KNI_API kfloat_t KniGetStaticFieldFloat(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as double value */
KNI_API kdouble_t KniGetStaticFieldDouble(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as string value */
KNI_API kstring_t KniGetStaticFieldString(HKCLASS hKClass, HKFIELD hKField);
/* Gets static field as raw value */
KNI_API kref_t KniGetStaticFieldRaw(HKCLASS hKClass, HKFIELD hKField);
/* Gets length of string or array static field */
KNI_API knuint_t KniGetStaticFieldLength(HKCLASS hKClass, HKFIELD hKField);


/* Gets element as bool value */
KNI_API kbool_t KniGetElementBool(knuint_t index);
/* Gets element as char value */
KNI_API kchar_t KniGetElementChar(knuint_t index);
/* Gets element as byte value */
KNI_API kbyte_t KniGetElementByte(knuint_t index);
/* Gets element as sbyte value */
KNI_API ksbyte_t KniGetElementSByte(knuint_t index);
/* Gets element as short value */
KNI_API kshort_t KniGetElementShort(knuint_t index);
/* Gets element as ushort value */
KNI_API kushort_t KniGetElementUShort(knuint_t index);
/* Gets element as int value */
KNI_API kint_t KniGetElementInt(knuint_t index);
/* Gets element as uint value */
KNI_API kuint_t KniGetElementUInt(knuint_t index);
/* Gets element as long value */
KNI_API klong_t KniGetElementLong(knuint_t index);
/* Gets element as ulong value */
KNI_API kulong_t KniGetElementULong(knuint_t index);
/* Gets element as float value */
KNI_API kfloat_t KniGetElementFloat(knuint_t index);
/* Gets element as double value */
KNI_API kdouble_t KniGetElementDouble(knuint_t index);
/* Gets element as string value */
KNI_API kstring_t KniGetElementString(knuint_t index);
/* Gets element as raw value */
KNI_API kref_t KniGetElementRaw(knuint_t index);
/* Gets length of string or array typed element */
KNI_API knuint_t KniGetElementLength(knuint_t index);


/* Gets indirect object as bool value */
KNI_API kbool_t KniGetIndirectBool(void);
/* Gets indirect object as char value */
KNI_API kchar_t KniGetIndirectChar(void);
/* Gets indirect object as byte value */
KNI_API kbyte_t KniGetIndirectByte(void);
/* Gets indirect object as sbyte value */
KNI_API ksbyte_t KniGetIndirectSByte(void);
/* Gets indirect object as short value */
KNI_API kshort_t KniGetIndirectShort(void);
/* Gets indirect object as ushort value */
KNI_API kushort_t KniGetIndirectUShort(void);
/* Gets indirect object as int value */
KNI_API kint_t KniGetIndirectInt(void);
/* Gets indirect object as uint value */
KNI_API kuint_t KniGetIndirectUInt(void);
/* Gets indirect object as long value */
KNI_API klong_t KniGetIndirectLong(void);
/* Gets indirect object as ulong value */
KNI_API kulong_t KniGetIndirectULong(void);
/* Gets indirect object as float value */
KNI_API kfloat_t KniGetIndirectFloat(void);
/* Gets indirect object as double value */
KNI_API kdouble_t KniGetIndirectDouble(void);
/* Gets indirect object as string value */
KNI_API kstring_t KniGetIndirectString(void);
/* Gets indirect object as raw value */
KNI_API kref_t KniGetIndirectRaw(void);
/* Gets length of string or array indirect object */
KNI_API knuint_t KniGetIndirectLength(void);

/*===================================================*/

/* Sets primitive values */

/* Sets bool value to local variable */
KNI_API void KniSetLocalBool(kushort_t index, kbool_t val);
/* Sets char value to local variable */
KNI_API void KniSetLocalChar(kushort_t index, kchar_t val);
/* Sets byte value to local variable */
KNI_API void KniSetLocalByte(kushort_t index, kbyte_t val);
/* Sets sbyte value to local variable */
KNI_API void KniSetLocalSByte(kushort_t index, ksbyte_t val);
/* Sets short value to local variable */
KNI_API void KniSetLocalShort(kushort_t index, kshort_t val);
/* Sets ushort value to local variable */
KNI_API void KniSetLocalUShort(kushort_t index, kushort_t val);
/* Sets int value to local variable */
KNI_API void KniSetLocalInt(kushort_t index, kint_t val);
/* Sets uint value to local variable */
KNI_API void KniSetLocalUInt(kushort_t index, kuint_t val);
/* Sets long value to local variable */
KNI_API void KniSetLocalLong(kushort_t index, klong_t val);
/* Sets ulong value to local variable */
KNI_API void KniSetLocalULong(kushort_t index, kulong_t val);
/* Sets float value to local variable */
KNI_API void KniSetLocalFloat(kushort_t index, kfloat_t val);
/* Sets double value to local variable */
KNI_API void KniSetLocalDouble(kushort_t index, kdouble_t val);
/* Sets string value to local variable */
KNI_API void KniSetLocalString(kushort_t index, kstring_t val, kuint_t length);
/* Sets raw value to local variable */
KNI_API void KniSetLocalRaw(kushort_t index, kref_t val);
/* Set null value to local variable */
KNI_API void KniSetLocalNull(kushort_t index);

/* Sets bool value to argument */
KNI_API void KniSetArgBool(kushort_t index, kbool_t val);
/* Sets char value to argument */
KNI_API void KniSetArgChar(kushort_t index, kchar_t val);
/* Sets byte value to argument */
KNI_API void KniSetArgByte(kushort_t index, kbyte_t val);
/* Sets sbyte value to argument */
KNI_API void KniSetArgSByte(kushort_t index, ksbyte_t val);
/* Sets short value to argument */
KNI_API void KniSetArgShort(kushort_t index, kshort_t val);
/* Sets ushort value to argument */
KNI_API void KniSetArgUShort(kushort_t index, kushort_t val);
/* Sets int value to argument */
KNI_API void KniSetArgInt(kushort_t index, kint_t val);
/* Sets uint value to argument */
KNI_API void KniSetArgUInt(kushort_t index, kuint_t val);
/* Sets long value to argument */
KNI_API void KniSetArgLong(kushort_t index, klong_t val);
/* Sets ulong value to argument */
KNI_API void KniSetArgULong(kushort_t index, kulong_t val);
/* Sets float value to argument */
KNI_API void KniSetArgFloat(kushort_t index, kfloat_t val);
/* Sets double value to argument */
KNI_API void KniSetArgDouble(kushort_t index, kdouble_t val);
/* Sets string value to argument */
KNI_API void KniSetArgString(kushort_t index, kstring_t val, kuint_t length);
/* Sets raw value to argument */
KNI_API void KniSetArgRaw(kushort_t index, kref_t val);
/* Set null value to argument */
KNI_API void KniSetArgNull(kushort_t index);


/* Sets bool value to instance field */
KNI_API void KniSetFieldBool(HKFIELD hKField, kbool_t val);
/* Sets char value to instance field */
KNI_API void KniSetFieldChar(HKFIELD hKField, kchar_t val);
/* Sets byte value to instance field */
KNI_API void KniSetFieldByte(HKFIELD hKField, kbyte_t val);
/* Sets sbyte value to instance field */
KNI_API void KniSetFieldSByte(HKFIELD hKField, ksbyte_t val);
/* Sets short value to instance field */
KNI_API void KniSetFieldShort(HKFIELD hKField, kshort_t val);
/* Sets ushort value to instance field */
KNI_API void KniSetFieldUShort(HKFIELD hKField, kushort_t val);
/* Sets int value to instance field */
KNI_API void KniSetFieldInt(HKFIELD hKField, kint_t val);
/* Sets uint value to instance field */
KNI_API void KniSetFieldUInt(HKFIELD hKField, kuint_t val);
/* Sets long value to instance field */
KNI_API void KniSetFieldLong(HKFIELD hKField, klong_t val);
/* Sets ulong value to instance field */
KNI_API void KniSetFieldULong(HKFIELD hKField, kulong_t val);
/* Sets float value to instance field */
KNI_API void KniSetFieldFloat(HKFIELD hKField, kfloat_t val);
/* Sets double value to instance field */
KNI_API void KniSetFieldDouble(HKFIELD hKField, kdouble_t val);
/* Sets string value to instance field */
KNI_API void KniSetFieldString(HKFIELD hKField, kstring_t val, kuint_t length);
/* Sets raw value to instannce field */
KNI_API void KniSetFieldRaw(HKFIELD hKField, kref_t val);
/* Set null value to instance field */
KNI_API void KniSetFieldNull(HKFIELD hKField);


/* Sets bool value to static field */
KNI_API void KniSetStaticFieldBool(HKCLASS hKClass, HKFIELD hKField, kbool_t val);
/* Sets char value to static field */
KNI_API void KniSetStaticFieldChar(HKCLASS hKClass, HKFIELD hKField, kchar_t val);
/* Sets byte value to static field */
KNI_API void KniSetStaticFieldByte(HKCLASS hKClass, HKFIELD hKField, kbyte_t val);
/* Sets sbyte value to static field */
KNI_API void KniSetStaticFieldSByte(HKCLASS hKClass, HKFIELD hKField, ksbyte_t val);
/* Sets short value to static field */
KNI_API void KniSetStaticFieldShort(HKCLASS hKClass, HKFIELD hKField, kshort_t val);
/* Sets ushort value to static field */
KNI_API void KniSetStaticFieldUShort(HKCLASS hKClass, HKFIELD hKField, kushort_t val);
/* Sets int value to static field */
KNI_API void KniSetStaticFieldInt(HKCLASS hKClass, HKFIELD hKField, kint_t val);
/* Sets uint value to static field */
KNI_API void KniSetStaticFieldUInt(HKCLASS hKClass, HKFIELD hKField, kuint_t val);
/* Sets long value to static field */
KNI_API void KniSetStaticFieldLong(HKCLASS hKClass, HKFIELD hKField, klong_t val);
/* Sets ulong value to static field */
KNI_API void KniSetStaticFieldULong(HKCLASS hKClass, HKFIELD hKField, kulong_t val);
/* Sets float value to static field */
KNI_API void KniSetStaticFieldFloat(HKCLASS hKClass, HKFIELD hKField, kfloat_t val);
/* Sets double value to static field */
KNI_API void KniSetStaticFieldDouble(HKCLASS hKClass, HKFIELD hKField, kdouble_t val);
/* Sets string value to static field */
KNI_API void KniSetStaticFieldString(HKCLASS hKClass, HKFIELD hKField, kstring_t val, kuint_t length);
/* Sets raw value to static field */
KNI_API void KniSetStaticFieldRaw(HKCLASS hKClass, HKFIELD hKField, kref_t val);
/* Set null value to static field */
KNI_API void KniSetStaticFieldNull(HKCLASS hKClass, HKFIELD hKField);


/* Sets bool value to array element */
KNI_API void KniSetElementBool(knuint_t index, kbool_t val);
/* Sets char value to array element */
KNI_API void KniSetElementChar(knuint_t index, kchar_t val);
/* Sets byte value to array element */
KNI_API void KniSetElementByte(knuint_t index, kbyte_t val);
/* Sets sbyte value to array element */
KNI_API void KniSetElementSByte(knuint_t index, ksbyte_t val);
/* Sets short value to array element */
KNI_API void KniSetElementShort(knuint_t index, kshort_t val);
/* Sets ushort value to array element */
KNI_API void KniSetElementUShort(knuint_t index, kushort_t val);
/* Sets int value to array element */
KNI_API void KniSetElementInt(knuint_t index, kint_t val);
/* Sets uint value to array element */
KNI_API void KniSetElementUInt(knuint_t index, kuint_t val);
/* Sets long value to array element */
KNI_API void KniSetElementLong(knuint_t index, klong_t val);
/* Sets ulong value to array element */
KNI_API void KniSetElementULong(knuint_t index, kulong_t val);
/* Sets float value to array element */
KNI_API void KniSetElementFloat(knuint_t index, kfloat_t val);
/* Sets double value to array element */
KNI_API void KniSetElementDouble(knuint_t index, kdouble_t val);
/* Sets string value to array element */
KNI_API void KniSetElementString(knuint_t index, kstring_t val, kuint_t length);
/* Sets raw value to array element */
KNI_API void KniSetElementRaw(knuint_t index, kref_t val);
/* Sets null value to array element */
KNI_API void KniSetElementNull(knuint_t index);


/* Sets bool value to indirect object */
KNI_API void KniSetIndirectBool(kbool_t val);
/* Sets char value to indirect object */
KNI_API void KniSetIndirectChar(kchar_t val);
/* Sets byte value to indirect object */
KNI_API void KniSetIndirectByte(kbyte_t val);
/* Sets sbyte value to indirect object */
KNI_API void KniSetIndirectSByte(ksbyte_t val);
/* Sets short value to indirect object */
KNI_API void KniSetIndirectShort(kshort_t val);
/* Sets ushort value to indirect object */
KNI_API void KniSetIndirectUShort(kushort_t val);
/* Sets int value to indirect object */
KNI_API void KniSetIndirectInt(kint_t val);
/* Sets uint value to indirect object */
KNI_API void KniSetIndirectUInt(kuint_t val);
/* Sets long value to indirect object */
KNI_API void KniSetIndirectLong(klong_t val);
/* Sets ulong value to indirect object */
KNI_API void KniSetIndirectULong(kulong_t val);
/* Sets float value to indirect object */
KNI_API void KniSetIndirectFloat(kfloat_t val);
/* Sets double value to indirect object */
KNI_API void KniSetIndirectDouble(kdouble_t val);
/* Sets string value to indirect object */
KNI_API void KniSetIndirectString(kstring_t val, kuint_t length);
/* Sets raw value to indirect object */
KNI_API void KniSetIndirectRaw(kref_t val);
/* Sets null value to indirect object */
KNI_API void KniSetIndirectNull(void);

/*===================================================*/

/* Loads values onto stack */

/* Loads bool value onto stack */
KNI_API void KniLoadBool(kbool_t val);
/* Loads char value onto stack */
KNI_API void KniLoadChar(kchar_t val);
/* Loads byte value onto stack */
KNI_API void KniLoadByte(kbyte_t val);
/* Loads sbyte value onto stack */
KNI_API void KniLoadSByte(ksbyte_t val);
/* Loads short value onto stack */
KNI_API void KniLoadShort(kshort_t val);
/* Loads ushort value onto stack */
KNI_API void KniLoadUShort(kushort_t val);
/* Loads int value onto stack */
KNI_API void KniLoadInt(kint_t val);
/* Loads uint value onto stack */
KNI_API void KniLoadUInt(kuint_t val);
/* Loads long value onto stack */
KNI_API void KniLoadLong(klong_t val);
/* Loads ulong value onto stack */
KNI_API void KniLoadULong(kulong_t val);
/* Loads float value onto stack */
KNI_API void KniLoadFloat(kfloat_t val);
/* Loads double value onto stack */
KNI_API void KniLoadDouble(kdouble_t val);
/* Loads string value onto stack */
KNI_API void KniLoadString(kstring_t val, kuint_t length);
/* Loads raw value onto stack */
KNI_API void KniLoadRaw(kref_t val);

/* Loads local variable onto stack */
KNI_API void KniLoadLocal(kushort_t index);
/* Loads argument onto stack */
KNI_API void KniLoadArg(kushort_t index);
/* Loads ByRef argument onto stack */
KNI_API void KniLoadArgByRef(kushort_t index);
/* Loads instance field onto stack */
KNI_API void KniLoadField(HKFIELD hKField);
/* Loads static field onto stack */
KNI_API void KniLoadStaticField(HKCLASS hKClass, HKFIELD hKField);
/* Loads array element onto stack */
KNI_API void KniLoadElement(knuint_t index);
/* Loads indirect object onto stack */
KNI_API void KniLoadIndirect(void);

/* Loads length of string or array local variable onto stack */
KNI_API void KniLoadLocalLength(kushort_t index);
/* Loads length of string or array argument onto stack */
KNI_API void KniLoadArgLength(kushort_t index);
/* Loads length of string or array ByRef argument onto stack */
KNI_API void KniLoadArgLengthByRef(kushort_t index);
/* Loads length of string or array instance field onto stack */
KNI_API void KniLoadFieldLength(HKFIELD hKField);
/* Loads length of string or array static field onto stack */
KNI_API void KniLoadStaticFieldLength(HKCLASS hKClass, HKFIELD hKField);
/* Loads length of string or array typed array element onto stack */
KNI_API void KniLoadElementLength(knuint_t index);
/* Loads length of string or array indirect object onto stack */
KNI_API void KniLoadIndirectLength(void);

/* Loads element of array typed local variable onto stack */
KNI_API void KniLoadLocalElement(kushort_t index, knuint_t elemIndex);
/* Loads element of array typed argument onto stack */
KNI_API void KniLoadArgElement(kushort_t index, knuint_t elemIndex);
/* Loads element of array typed ByRef argument onto stack */
KNI_API void KniLoadArgElementByRef(kushort_t index, knuint_t elemIndex);
/* Loads element of array typed instance field onto stack */
KNI_API void KniLoadFieldElement(HKFIELD hKField, knuint_t elemIndex);
/* Loads element of array typed static field onto stack */
KNI_API void KniLoadStaticFieldElement(HKCLASS hKClass, HKFIELD hKField, knuint_t elemIndex);
/* Loads element of array typed array element onto stack */
KNI_API void KniLoadElementElement(knuint_t index, knuint_t elemIndex);
/* Loads element of array typed  onto stack */
KNI_API void KniLoadIndirectElement(knuint_t elemIndex);

/* Loads address of local variable onto stack */
KNI_API void KniLoadLocalAddress(kushort_t index);
/* Loads address of argument onto stack */
KNI_API void KniLoadArgAddress(kushort_t index);
/* Loads address of instance field onto stack */
KNI_API void KniLoadFieldAddress(HKFIELD hKField);
/* Loads address of static field onto stack */
KNI_API void KniLoadStaticFieldAddress(HKCLASS hKClass, HKFIELD hKField);
/* Loads address of array element onto stack */
KNI_API void KniLoadElementAddress(knuint_t index);

/* Loads null value onto stack */
KNI_API void KniLoadNull(void);

/* Loads new class instance onto stack */
KNI_API void KniLoadNewObject(HKCLASS hKClass);
/* Loads new delegate instance onto stack */
KNI_API void KniLoadNewDelegate(HKDELEGATE hKDelegate, HKMETHOD hKMethod);
/* Loads new array onto stack */
KNI_API void KniLoadNewArray(HKTYPE hKType, kuint_t length);
/* Loads new array onto stack, elements are already loaded */
KNI_API void KniLoadNewArrayBaking(HKTYPE hKType, kuint_t length);

/*===================================================*/

/* Stores values */

/* Stores stack top value into local variable */
KNI_API void KniStoreLocal(kushort_t index);
/* Stores stack top value into argument */
KNI_API void KniStoreArg(kushort_t index);
/* Stores stack top value into ByRef argument */
KNI_API void KniStoreArgByRef(kushort_t index);
/* Stores stack top value into instance field */
KNI_API void KniStoreField(HKFIELD hKField);
/* Stores stack top value into static field */
KNI_API void KniStoreStaticField(HKCLASS hKClass, HKFIELD hKField);
/* Stores stack top value into array element */
KNI_API void KniStoreElement(knuint_t index);
/* Stores stack top value into indirect object */
KNI_API void KniStoreIndirect(void);

/*===================================================*/

/* Tests values */

/* Checks if stack top value is null */
KNI_API kbool_t KniIsNull(void);
/* Checks if local variable is null */
KNI_API kbool_t KniIsNullLocal(kushort_t index);
/* Checks if argument is null */
KNI_API kbool_t KniIsNullArg(kushort_t index);
/* Checks if ByRef argument is null */
KNI_API kbool_t KniIsNullArgByRef(kushort_t index);
/* Checks if instance field is null */
KNI_API kbool_t KniIsNullField(HKFIELD hKField);
/* Checks if static field is null */
KNI_API kbool_t KniIsNullStaticField(HKCLASS hKClass, HKFIELD hKField);
/* Checks if array element is null */
KNI_API kbool_t KniIsNullElement(knuint_t index);
/* Checks if indirect object is null */
KNI_API kbool_t KniIsNullIndirect(void);

/* Checks if two stack top values are equal by reference */
KNI_API kbool_t KniIsReferenceEqual(void);

/* Checks if stack top value is class instance */
KNI_API kbool_t KniIsClass(void);
/* Checks if stack top value is delegate instance */
KNI_API kbool_t KniIsDelegate(void);
/* Checks if stack top value is array */
KNI_API kbool_t KniIsArray(void);

/* Checks if stack top value is instance of the given class */
KNI_API kbool_t KniIsInstanceOfClass(HKCLASS hKClass);
/* Checks if stack top value is instance of the given delegate */
KNI_API kbool_t KniIsInstanceOfDelegate(HKDELEGATE hKDelegate);

/*===================================================*/

/* Invokes */

/* Invokes a method */
KNI_API KRESULT KniInvoke(HKMETHOD hKMethod);
/* Invokes a delegate object */
KNI_API KRESULT KniInvokeObject(HKDELEGATE hKDelegate);

/* Checks if an exception has been thrown */
KNI_API kbool_t KniHasException(void);
/* Prints the stack trace of the current exception to the standard error output (stderr) */
KNI_API void KniPrintExceptionDescription(void);
/* Clears the current exception */
KNI_API void KniClearException(void);
/* Throws an exception */
KNI_API void KniThrowException(HKCLASS excType);
/* Throws an exception with extra data loaded on the stack */
KNI_API void KniThrowExceptionEx(HKCLASS excType);
/* Throws a System.Exception object */
KNI_API void KniThrowExceptionGeneral(kstring_t message, kuint_t length);

/* Initializes local variables for current native method */
KNI_API void KniInitLocals(HKTYPE *pHKTypes, kushort_t count);

/*===================================================*/

/* Metadata and types */

/* Gets datatype of stack top value */
KNI_API HKTYPE KniGetType(void);

/* Gets datatype of local variable */
KNI_API HKTYPE KniGetTypeLocal(kushort_t index);
/* Gets datatype of argument */
KNI_API HKTYPE KniGetTypeArg(kushort_t index);
/* Gets datatype of ByRef argument */
KNI_API HKTYPE KniGetTypeArgByRef(kushort_t index);
/* Gets datatype of instance field */
KNI_API HKTYPE KniGetTypeField(HKFIELD hKField);
/* Gets datatype of static field */
KNI_API HKTYPE KniGetTypeStaticField(HKCLASS hKClass, HKFIELD hKField);
/* Gets datatype of array element */
KNI_API HKTYPE KniGetTypeElement(knuint_t index);
/* Gets datatype of indirect object */
KNI_API HKTYPE KniGetTypeIndirect(void);

/* Gets ClassDef having the given name */
KNI_API HKCLASS KniGetClass(kstring_t ksName);
/* Gets DelegateDef having the given name */
KNI_API HKDELEGATE KniGetDelegate(kstring_t ksName);
/* Gets FieldDef having the given name */
KNI_API HKFIELD KniGetField(HKCLASS hKClass, kstring_t ksName);
/* Gets MethodDef having the given name */
KNI_API HKMETHOD KniGetMethod(HKCLASS hKClass, kstring_t ksName);

/* Gets handle of a primitive type */
KNI_API HKTYPE KniGetPrimitiveType(KTYPETAG tag);

/* Creates TypeDef from the given attributes */
KNI_API HKTYPE KniCreateType(KTYPETAG tag, kushort_t dim, HKUSERTYPE hKClassOrDelegate);

/* Checks if the given type is class type */
KNI_API kbool_t KniIsClassType(HKTYPE hKType);
/* Checks if the given type is delegate type */
KNI_API kbool_t KniIsDelegateType(HKTYPE hKType);
/* Checks if the given type is array type */
KNI_API kbool_t KniIsArrayType(HKTYPE hKType);

/*===================================================*/

#ifdef __cplusplus
}
#endif

#endif/*_KNI_H*/
