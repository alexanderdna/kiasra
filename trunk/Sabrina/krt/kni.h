#ifndef _KNI_H
#define _KNI_H

#include <stdint.h>

enum ktypetag_t : uint16_t
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
};

struct KVM;

enum NINVOKE_RESULT
{
	NINVOKE_OK = 0,
	NINVOKE_ERR = 1
};

typedef NINVOKE_RESULT (*NFUNC)(KVM *vm);

typedef unsigned char kbool_t;
typedef wchar_t       kchar_t;
typedef int8_t        kbyte_t;
typedef uint8_t       ksbyte_t;
typedef int16_t       kshort_t;
typedef uint16_t      kushort_t;
typedef int32_t       kint_t;
typedef uint32_t      kuint_t;
typedef int64_t       klong_t;
typedef uint64_t      kulong_t;

typedef float         kfloat_t;
typedef double        kdouble_t;

typedef kchar_t      *kstring_t;

// reference to a variable or a field
typedef void         *kref_t;

// user-defined type (class or delegate)
typedef const void    *UdtId;
typedef UdtId          ClassId;
typedef UdtId          DelegateId;
typedef const void    *MethodId;
typedef const void    *FieldId;
typedef const void    *TypeId;

// native int
typedef int           knint_t;
// native unsigned int
typedef unsigned int  knuint_t;

// 16-bit token
typedef uint16_t      ktoken16_t;
// 32-bit token
typedef uint32_t      ktoken32_t;

struct KVM
{
	// getting data
	
	virtual kbool_t getBool_loc(kushort_t index) =0;
	virtual kbool_t getBool_arg(kushort_t index) =0;
	virtual kbool_t getBool_fld(FieldId id) =0;
	virtual kbool_t getBool_sfld(FieldId id) =0;
	virtual kbool_t getBool_elem(knuint_t index) =0;
	virtual kbool_t getBool_ind() =0;

	virtual kchar_t getChar_loc(kushort_t index) =0;
	virtual kchar_t getChar_arg(kushort_t index) =0;
	virtual kchar_t getChar_fld(FieldId id) =0;
	virtual kchar_t getChar_sfld(FieldId id) =0;
	virtual kchar_t getChar_elem(knuint_t index) =0;
	virtual kchar_t getChar_ind() =0;

	virtual kbyte_t getByte_loc(kushort_t index) =0;
	virtual kbyte_t getByte_arg(kushort_t index) =0;
	virtual kbyte_t getByte_fld(FieldId id) =0;
	virtual kbyte_t getByte_sfld(FieldId id) =0;
	virtual kbyte_t getByte_elem(knuint_t index) =0;
	virtual kbyte_t getByte_ind() =0;

	virtual ksbyte_t getSByte_loc(kushort_t index) =0;
	virtual ksbyte_t getSByte_arg(kushort_t index) =0;
	virtual ksbyte_t getSByte_fld(FieldId id) =0;
	virtual ksbyte_t getSByte_sfld(FieldId id) =0;
	virtual ksbyte_t getSByte_elem(knuint_t index) =0;
	virtual ksbyte_t getSByte_ind() =0;
	
	virtual kshort_t getShort_loc(kushort_t index) =0;
	virtual kshort_t getShort_arg(kushort_t index) =0;
	virtual kshort_t getShort_fld(FieldId id) =0;
	virtual kshort_t getShort_sfld(FieldId id) =0;
	virtual kshort_t getShort_elem(knuint_t index) =0;
	virtual kshort_t getShort_ind() =0;

	virtual kushort_t getUShort_loc(kushort_t index) =0;
	virtual kushort_t getUShort_arg(kushort_t index) =0;
	virtual kushort_t getUShort_fld(FieldId id) =0;
	virtual kushort_t getUShort_sfld(FieldId id) =0;
	virtual kushort_t getUShort_elem(knuint_t index) =0;
	virtual kushort_t getUShort_ind() =0;
	
	virtual kint_t getInt_loc(kushort_t index) =0;
	virtual kint_t getInt_arg(kushort_t index) =0;
	virtual kint_t getInt_fld(FieldId id) =0;
	virtual kint_t getInt_sfld(FieldId id) =0;
	virtual kint_t getInt_elem(knuint_t index) =0;
	virtual kint_t getInt_ind() =0;
	
	virtual kuint_t getUInt_loc(kushort_t index) =0;
	virtual kuint_t getUInt_arg(kushort_t index) =0;
	virtual kuint_t getUInt_fld(FieldId id) =0;
	virtual kuint_t getUInt_sfld(FieldId id) =0;
	virtual kuint_t getUInt_elem(knuint_t index) =0;
	virtual kuint_t getUInt_ind() =0;
	
	virtual klong_t getLong_loc(kushort_t index) =0;
	virtual klong_t getLong_arg(kushort_t index) =0;
	virtual klong_t getLong_fld(FieldId id) =0;
	virtual klong_t getLong_sfld(FieldId id) =0;
	virtual klong_t getLong_elem(knuint_t index) =0;
	virtual klong_t getLong_ind() =0;
	
	virtual kulong_t getULong_loc(kushort_t index) =0;
	virtual kulong_t getULong_arg(kushort_t index) =0;
	virtual kulong_t getULong_fld(FieldId id) =0;
	virtual kulong_t getULong_sfld(FieldId id) =0;
	virtual kulong_t getULong_elem(knuint_t index) =0;
	virtual kulong_t getULong_ind() =0;
	
	virtual kfloat_t getFloat_loc(kushort_t index) =0;
	virtual kfloat_t getFloat_arg(kushort_t index) =0;
	virtual kfloat_t getFloat_fld(FieldId id) =0;
	virtual kfloat_t getFloat_sfld(FieldId id) =0;
	virtual kfloat_t getFloat_elem(knuint_t index) =0;
	virtual kfloat_t getFloat_ind() =0;
	
	virtual kdouble_t getDouble_loc(kushort_t index) =0;
	virtual kdouble_t getDouble_arg(kushort_t index) =0;
	virtual kdouble_t getDouble_fld(FieldId id) =0;
	virtual kdouble_t getDouble_sfld(FieldId id) =0;
	virtual kdouble_t getDouble_elem(knuint_t index) =0;
	virtual kdouble_t getDouble_ind() =0;

	virtual const kstring_t getString_loc(kushort_t index) =0;
	virtual const kstring_t getString_arg(kushort_t index) =0;
	virtual const kstring_t getString_fld(FieldId id) =0;
	virtual const kstring_t getString_sfld(FieldId id) =0;
	virtual const kstring_t getString_elem(knuint_t index) =0;
	virtual const kstring_t getString_ind() =0;

	virtual knuint_t getLength_loc(kushort_t index) =0;
	virtual knuint_t getLength_arg(kushort_t index) =0;
	virtual knuint_t getLength_fld(FieldId id) =0;
	virtual knuint_t getLength_sfld(FieldId id) =0;
	virtual knuint_t getLength_elem(knuint_t index) =0;
	virtual knuint_t getLength();

	// setting data

	virtual void setBool_loc(kushort_t index, kbool_t value) =0;
	virtual void setBool_arg(kushort_t index, kbool_t value) =0;
	virtual void setBool_fld(FieldId id, kbool_t value) =0;
	virtual void setBool_sfld(FieldId id, kbool_t value) =0;
	virtual void setBool_elem(knuint_t index, kbool_t value) =0;
	virtual void setBool_ind(kbool_t value) =0;

	virtual void setChar_loc(kushort_t index, kchar_t value) =0;
	virtual void setChar_arg(kushort_t index, kchar_t value) =0;
	virtual void setChar_fld(FieldId id, kchar_t value) =0;
	virtual void setChar_sfld(FieldId id, kchar_t value) =0;
	virtual void setChar_elem(knuint_t index, kchar_t value) =0;
	virtual void setChar_ind(kchar_t value) =0;

	virtual void setByte_loc(kushort_t index, kbyte_t value) =0;
	virtual void setByte_arg(kushort_t index, kbyte_t value) =0;
	virtual void setByte_fld(FieldId id, kbyte_t value) =0;
	virtual void setByte_sfld(FieldId id, kbyte_t value) =0;
	virtual void setByte_elem(knuint_t index, kbyte_t value) =0;
	virtual void setByte_ind(kbyte_t value) =0;

	virtual void setSByte_loc(kushort_t index, ksbyte_t value) =0;
	virtual void setSByte_arg(kushort_t index, ksbyte_t value) =0;
	virtual void setSByte_fld(FieldId id, ksbyte_t value) =0;
	virtual void setSByte_sfld(FieldId id, ksbyte_t value) =0;
	virtual void setSByte_elem(knuint_t index, ksbyte_t value) =0;
	virtual void setSByte_ind(ksbyte_t value) =0;

	virtual void setShort_loc(kushort_t index, kshort_t value) =0;
	virtual void setShort_arg(kushort_t index, kshort_t value) =0;
	virtual void setShort_fld(FieldId id, kshort_t value) =0;
	virtual void setShort_sfld(FieldId id, kshort_t value) =0;
	virtual void setShort_elem(knuint_t index, kshort_t value) =0;
	virtual void setShort_ind(kshort_t value) =0;

	virtual void setUShort_loc(kushort_t index, kushort_t value) =0;
	virtual void setUShort_arg(kushort_t index, kushort_t value) =0;
	virtual void setUShort_fld(FieldId id, kushort_t value) =0;
	virtual void setUShort_sfld(FieldId id, kushort_t value) =0;
	virtual void setUShort_elem(knuint_t index, kushort_t value) =0;
	virtual void setUShort_ind(kushort_t value) =0;

	virtual void setInt_loc(kushort_t index, kint_t value) =0;
	virtual void setInt_arg(kushort_t index, kint_t value) =0;
	virtual void setInt_fld(FieldId id, kint_t value) =0;
	virtual void setInt_sfld(FieldId id, kint_t value) =0;
	virtual void setInt_elem(knuint_t index, kint_t value) =0;
	virtual void setInt_ind(kint_t value) =0;

	virtual void setUInt_loc(kushort_t index, kuint_t value) =0;
	virtual void setUInt_arg(kushort_t index, kuint_t value) =0;
	virtual void setUInt_fld(FieldId id, kuint_t value) =0;
	virtual void setUInt_sfld(FieldId id, kuint_t value) =0;
	virtual void setUInt_elem(knuint_t index, kuint_t value) =0;
	virtual void setUInt_ind(kuint_t value) =0;

	virtual void setLong_loc(kushort_t index, klong_t value) =0;
	virtual void setLong_arg(kushort_t index, klong_t value) =0;
	virtual void setLong_fld(FieldId id, klong_t value) =0;
	virtual void setLong_sfld(FieldId id, klong_t value) =0;
	virtual void setLong_elem(knuint_t index, klong_t value) =0;
	virtual void setLong_ind(klong_t value) =0;

	virtual void setULong_loc(kushort_t index, kulong_t value) =0;
	virtual void setULong_arg(kushort_t index, kulong_t value) =0;
	virtual void setULong_fld(FieldId id, kulong_t value) =0;
	virtual void setULong_sfld(FieldId id, kulong_t value) =0;
	virtual void setULong_elem(knuint_t index, kulong_t value) =0;
	virtual void setULong_ind(kulong_t value) =0;

	virtual void setFloat_loc(kushort_t index, kfloat_t value) =0;
	virtual void setFloat_arg(kushort_t index, kfloat_t value) =0;
	virtual void setFloat_fld(FieldId id, kfloat_t value) =0;
	virtual void setFloat_sfld(FieldId id, kfloat_t value) =0;
	virtual void setFloat_elem(knuint_t index, kfloat_t value) =0;
	virtual void setFloat_ind(kfloat_t value) =0;

	virtual void setDouble_loc(kushort_t index, kdouble_t value) =0;
	virtual void setDouble_arg(kushort_t index, kdouble_t value) =0;
	virtual void setDouble_fld(FieldId id, kdouble_t value) =0;
	virtual void setDouble_sfld(FieldId id, kdouble_t value) =0;
	virtual void setDouble_elem(knuint_t index, kdouble_t value) =0;
	virtual void setDouble_ind(kdouble_t value) =0;

	virtual void setString_loc(kushort_t index, const kstring_t value) =0;
	virtual void setString_arg(kushort_t index, const kstring_t value) =0;
	virtual void setString_fld(FieldId id, const kstring_t value) =0;
	virtual void setString_sfld(FieldId id, const kstring_t value) =0;
	virtual void setString_elem(knuint_t index, const kstring_t value) =0;
	virtual void setString_ind(const kstring_t value) =0;

	// loading data onto stack

	virtual void loadBool(kbool_t value) =0;
	virtual void loadChar(kchar_t value) =0;
	virtual void loadByte(kbyte_t value) =0;
	virtual void loadSByte(ksbyte_t value) =0;
	virtual void loadShort(kshort_t value) =0;
	virtual void loadUShort(kushort_t value) =0;
	virtual void loadInt(kint_t value) =0;
	virtual void loadUInt(kuint_t value) =0;
	virtual void loadLong(klong_t value) =0;
	virtual void loadULong(kulong_t value) =0;
	virtual void loadFloat(kfloat_t value) =0;
	virtual void loadDouble(kdouble_t value) =0;
	virtual void loadString(const kstring_t value) =0;
	
	virtual void load_loc(kushort_t index) =0;
	virtual void load_arg(kushort_t index) =0;
	virtual void load_fld(FieldId id) =0;
	virtual void load_sfld(FieldId id) =0;
	virtual void load_elem(knuint_t index) =0;

	virtual void loadLength_loc(kushort_t index) =0;
	virtual void loadLength_arg(kushort_t index) =0;
	virtual void loadLength_fld(FieldId id) =0;
	virtual void loadLength_sfld(FieldId id) =0;
	virtual void loadLength_elem(knuint_t index) =0;

	virtual void loadElem_loc(kushort_t index, knuint_t elemIndex) =0;
	virtual void loadElem_arg(kushort_t index, knuint_t elemIndex) =0;
	virtual void loadElem_fld(FieldId id, knuint_t elemIndex) =0;
	virtual void loadElem_sfld(FieldId id, knuint_t elemIndex) =0;
	virtual void loadElem_elem(knuint_t index, knuint_t elemIndex) =0;

	virtual void loadAddress_loc(kushort_t index) =0;
	virtual void loadAddress_arg(kushort_t index) =0;
	virtual void loadAddress_fld(FieldId id) =0;
	virtual void loadAddress_sfld(FieldId id) =0;
	virtual void loadAddress_elem(knuint_t index) =0;

	virtual void loadNull() =0;
	virtual void loadThis() =0;

	virtual void loadNewClassInstance(ClassId cls) =0;
	virtual void loadNewDelegateInstance(DelegateId del, MethodId met) =0;
	virtual void loadNewDelegateInstance(DelegateId del, NFUNC nativeFunction) =0;
	virtual void loadNewArray(TypeId arrType, knuint_t length) =0;

	// reference checking

	virtual bool isNull() =0;
	virtual bool isNull_loc(kushort_t index) =0;
	virtual bool isNull_arg(kushort_t index) =0;
	virtual bool isNull_fld(FieldId id) =0;
	virtual bool isNull_sfld(FieldId id) =0;
	virtual bool isNull_ind() =0;

	virtual bool referenceEquals() =0;

	// creating and deleting object

	virtual void createClassInstance(ClassId cls, ...) =0;
	virtual void createDelegateInstance(DelegateId del, MethodId met) =0;
	virtual void createDelegateInstance(DelegateId del, NFUNC nativeFunction) =0;
	virtual void createArray(TypeId arrType, knuint_t length) =0;

	// calling function

	virtual void invokeVoid(MethodId id) =0;
	virtual void invokeVoid() =0;

	virtual kbool_t invokeBool(MethodId id) =0;
	virtual kbool_t invokeBool();

	virtual kchar_t invokeChar(MethodId id) =0;
	virtual kchar_t invokeChar();
	
	virtual kbyte_t invokeByte(MethodId id) =0;
	virtual kbyte_t invokeByte();

	virtual ksbyte_t invokeSByte(MethodId id) =0;
	virtual ksbyte_t invokeSByte() =0;

	virtual kshort_t invokeShort(MethodId id) =0;
	virtual kshort_t invokeShort() =0;

	virtual kushort_t invokeUShort(MethodId id) =0;
	virtual kushort_t invokeUShort() =0;

	virtual kint_t invokeInt(MethodId id) =0;
	virtual kint_t invokeInt() =0;

	virtual kuint_t invokeUInt(MethodId id) =0;
	virtual kuint_t invokeUInt() =0;

	virtual klong_t invokeLong(MethodId id) =0;
	virtual klong_t invokeLong() =0;

	virtual kulong_t invokeULong(MethodId id) =0;
	virtual kulong_t invokeULong() =0;

	virtual kfloat_t invokeFloat(MethodId id) =0;
	virtual kfloat_t invokeFloat() =0;

	virtual kdouble_t invokeDouble(MethodId id) =0;
	virtual kdouble_t invokeDouble() =0;

	virtual const kstring_t invokeString(MethodId id) =0;
	virtual const kstring_t invokeString() =0;

	virtual void invokeObject(MethodId id) =0;
	virtual void invokeObject() =0;

	// metadata

	virtual ClassId getClass(const kstring_t name) =0;
	virtual DelegateId getDelegate(const kstring_t name) =0;
	virtual MethodId getMethod(ClassId cls, const kstring_t name) =0;
	virtual FieldId getField(ClassId cls, const kstring_t name) =0;
	
	virtual TypeId getType(ktypetag_t tag) =0;
	virtual TypeId getType(ktypetag_t tag, kushort_t dim) =0;
	virtual TypeId getType(ktypetag_t tag, kushort_t dim, UdtId id) =0;

	virtual TypeId getObjectType() =0;

	virtual bool isClassInstance() =0;
	virtual bool isDelegateInstance() =0;
	virtual bool isArray() =0;

	virtual bool isInstanceOf(UdtId id) =0;
};

#endif//_KNI_H
