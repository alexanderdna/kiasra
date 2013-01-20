#pragma once

#include <cstdint>
#include <cwchar>

//===================================================================
// Types
//===================================================================

typedef bool           kbool_t;
typedef int32_t        kint_t;
typedef double         kfloat_t;

typedef       wchar_t *kstring_t;
typedef const wchar_t *kcstring_t;

typedef uint16_t ktoken_t;

enum TypeTag : uint16_t
{
	K_VOID     = 0x0001,
	K_BOOL     = 0x0002,
	K_INT      = 0x0004,
	K_FLOAT    = 0x0008,
	K_STRING   = 0x0010,
	K_CLASS    = 0x0020,
	K_DELEGATE = 0x0040,
	K_OBJECT   = 0x1000,

	K_PRIMITIVE = K_BOOL | K_INT | K_FLOAT | K_STRING,
	K_VALUE     = K_BOOL | K_INT | K_FLOAT,
	K_REFERENCE = K_STRING | K_CLASS | K_DELEGATE,
};

// module attributes
typedef int kmoduleattr_t;
#define KMODA_SYS  0x01
#define KMODA_USER 0x02
#define KMODA_DLL  0x04
#define KMODA_KASM 0x08

// class attributes
typedef int kclassattr_t;
#define KCA_PUBLIC  0x00
#define KCA_PRIVATE 0x01
#define KCA_STATIC  0x02

// field attributes
typedef int kfieldattr_t;
#define KFA_PUBLIC  0x00
#define KFA_PRIVATE 0x01
#define KFA_STATIC  0x02
#define KFA_FINAL   0x04

// method attributes
typedef int kmethodattr_t;
#define KMA_PUBLIC  0x00
#define KMA_PRIVATE 0x01
#define KMA_STATIC  0x02
#define KMA_CTOR    0x04
#define KMA_NATIVE  0x0100
#define KMA_SPECIAL 0x1000

//===================================================================
// Runtime metadata
//===================================================================

struct ModuleDef;
struct ClassDef;
struct DelegateDef;
struct FieldDef;
struct MethodDef;
struct ParamDef;

struct TypeDef
{
	TypeTag tag;
	uint16_t dim;	// dimensions count
	union
	{
		const ClassDef *classDef;
		const DelegateDef *delegateDef;
	};
};

struct ModuleDef
{
	kmoduleattr_t attrs;
	ClassDef *classes;
	DelegateDef *delegates;
};

struct ClassDef
{
	kcstring_t name;
	kclassattr_t attrs;

	FieldDef *fields;
	MethodDef *methods;

	MethodDef *ctor;
	MethodDef *cctor;
};

struct DelegateDef
{
	kcstring_t name;
	kclassattr_t attrs;

	TypeDef *returnType;
	ParamDef *params;
};

struct FieldDef
{
	kcstring_t name;
	kfieldattr_t attrs;

	TypeDef *declType;
};

struct MethodDef
{
	kcstring_t name;
	kmethodattr_t attrs;

	TypeDef *returnType;
	ParamDef *params;
};

struct ParamDef
{
	kcstring_t name;
	TypeDef *declType;
	bool byRef;
};

//===================================================================
// Compile time metadata
//===================================================================

struct MetaTypeDef;
struct MetaClassDef;
struct MetaDelegateDef;
struct MetaFieldDef;
struct MetaMethodDef;
struct MetaParamDef;

struct MetaTypeDef
{
	TypeTag tag;
	uint16_t dim;
	ktoken_t token;

	TypeDef *rt;
};

struct MetaClassDef
{
	kcstring_t name;
	kclassattr_t attrs;

	ktoken_t firstField;
	uint16_t fieldCount;

	ktoken_t fieldMethod;
	uint16_t methodCount;

	ClassDef *rt;
};

struct MetaDelegateDef
{
	kcstring_t name;
	kclassattr_t attrs;

	ktoken_t returnType;
	ktoken_t firstParam;
	uint16_t paramCount;

	DelegateDef *rt;
};

struct MetaFieldDef
{
	kcstring_t name;
	kfieldattr_t attrs;

	ktoken_t declType;

	FieldDef *rt;
};

struct MetaMethodDef
{
	kcstring_t name;
	kmethodattr_t attrs;

	ktoken_t returnType;
	ktoken_t firstParam;
	uint16_t paramCount;

	MethodDef *rt;
};

struct MetaParamDef
{
	kcstring_t name;
	ktoken_t declType;
	bool byRef;

	ParamDef *rt;
};

//===================================================================
// Machine
//===================================================================

class KVM
{
public:
	// conversions
	virtual void convBool() =0;
	virtual void convInt() =0;
	virtual void convFloat() =0;
	virtual void convString() =0;

	virtual void box() =0;
	virtual void unbox(const ktoken_t toType) =0;
	virtual void cast(const ktoken_t toType) =0;

	// gets data from storage units
	virtual kbool_t getBool_loc(const ktoken_t token) =0;
	virtual kbool_t getBool_arg(const ktoken_t token) =0;
	virtual kbool_t getBool_argind(const ktoken_t token) =0;
	virtual kbool_t getBool_fld(const ktoken_t token) =0;
	virtual kbool_t getBool_sfld(const ktoken_t token) =0;
	virtual kbool_t getBool_elem(const kint_t index) =0;
	
	virtual kint_t getInt_loc(const ktoken_t token) =0;
	virtual kint_t getInt_argind(const ktoken_t token) =0;
	virtual kint_t getInt_arg(const ktoken_t token) =0;
	virtual kint_t getInt_fld(const ktoken_t token) =0;
	virtual kint_t getInt_sfld(const ktoken_t token) =0;
	virtual kint_t getInt_elem(const kint_t index) =0;

	virtual const kfloat_t & getFloat_loc(const ktoken_t token) =0;
	virtual const kfloat_t & getFloat_arg(const ktoken_t token) =0;
	virtual const kfloat_t & getFloat_argind(const ktoken_t token) =0;
	virtual const kfloat_t & getFloat_fld(const ktoken_t token) =0;
	virtual const kfloat_t & getFloat_sfld(const ktoken_t token) =0;
	virtual const kfloat_t & getFloat_elem(const kint_t index) =0;

	virtual kcstring_t getString_loc(const ktoken_t token) =0;
	virtual kcstring_t getString_arg(const ktoken_t token) =0;
	virtual kcstring_t getString_argind(const ktoken_t token) =0;
	virtual kcstring_t getString_fld(const ktoken_t token) =0;
	virtual kcstring_t getString_sfld(const ktoken_t token) =0;

	// changes data in storage units
	virtual void setBool_loc(const ktoken_t token, const kbool_t value) =0;
	virtual void setBool_arg(const ktoken_t token, const kbool_t value) =0;
	virtual void setBool_argind(const ktoken_t token, const kbool_t value) =0;
	virtual void setBool_fld(const ktoken_t token, const kbool_t value) =0;
	virtual void setBool_sfld(const ktoken_t token, const kbool_t value) =0;
	virtual void setBool_elem(const kint_t index, const kbool_t value) =0;

	virtual void setInt_loc(const ktoken_t token, const kint_t value) =0;
	virtual void setInt_arg(const ktoken_t token, const kint_t value) =0;
	virtual void setInt_argind(const ktoken_t token, const kint_t value) =0;
	virtual void setInt_fld(const ktoken_t token, const kint_t value) =0;
	virtual void setInt_sfld(const ktoken_t token, const kint_t value) =0;
	virtual void setInt_elem(const kint_t index, const kint_t value) =0;

	virtual void setFloat_loc(const ktoken_t token, const kfloat_t &value) =0;
	virtual void setFloat_arg(const ktoken_t token, const kfloat_t &value) =0;
	virtual void setFloat_argind(const ktoken_t token, const kfloat_t &value) =0;
	virtual void setFloat_fld(const ktoken_t token, const kfloat_t &value) =0;
	virtual void setFloat_sfld(const ktoken_t token, const kfloat_t &value) =0;
	virtual void setFloat_elem(const kint_t index, const kfloat_t &value) =0;

	virtual void setString_loc(const ktoken_t token, kcstring_t value) =0;
	virtual void setString_arg(const ktoken_t token, kcstring_t value) =0;
	virtual void setString_argind(const ktoken_t token, kcstring_t value) =0;
	virtual void setString_fld(const ktoken_t token, kcstring_t value) =0;
	virtual void setString_sfld(const ktoken_t token, kcstring_t value) =0;
	virtual void setString_elem(const kint_t index, kcstring_t value) =0;

	// gets string or array length
	virtual kint_t getLength_loc(const ktoken_t token) =0;
	virtual kint_t getLength_arg(const ktoken_t token) =0;
	virtual kint_t getLength_argind(const ktoken_t token) =0;
	virtual kint_t getLength_fld(const ktoken_t token) =0;
	virtual kint_t getLength_sfld(const ktoken_t token) =0;
	virtual kint_t getLength_elem(const kint_t index) =0;

	// loads data onto the stack
	virtual void loadBool(const kbool_t value) =0;
	virtual void loadInt(const kint_t value) =0;
	virtual void loadFloat(const kfloat_t &value) =0;
	virtual void loadString(kcstring_t value) =0;
	
	virtual void load_loc(const ktoken_t token) =0;
	virtual void load_arg(const ktoken_t token) =0;
	virtual void load_fld(const ktoken_t token) =0;
	virtual void load_sfld(const ktoken_t token) =0;
	virtual void load_elem(const kint_t index) =0;

	virtual void loadLength_loc(const ktoken_t token) =0;
	virtual void loadLength_arg(const ktoken_t token) =0;
	virtual void loadLength_fld(const ktoken_t token) =0;
	virtual void loadLength_sfld(const ktoken_t token) =0;
	virtual void loadLength_elem(const kint_t index) =0;

	virtual void loadAddress_loc(const ktoken_t token) =0;
	virtual void loadAddress_arg(const ktoken_t token) =0;
	virtual void loadAddress_fld(const ktoken_t token) =0;
	virtual void loadAddress_sfld(const ktoken_t token) =0;
	virtual void loadAddress_elem(const kint_t index) =0;

	virtual void loadIndirect() =0;

	virtual void loadNewObject(const ktoken_t classToken) =0;
	virtual void loadNewDelegate(const ktoken_t delegateTypeToken) =0;
	virtual void loadNewArray(const TypeDef &arrType) =0;
	virtual void loadNewArray(const ktoken_t arrTypeToken) =0;

	virtual void loadNull() =0;
	virtual void loadThis() =0;

	// stores data on the stack into storage units
	virtual void store_loc(const ktoken_t token) =0;
	virtual void store_arg(const ktoken_t token) =0;
	virtual void store_fld(const ktoken_t token) =0;
	virtual void store_sfld(const ktoken_t token) =0;
	virtual void store_elem(const kint_t index) =0;

	virtual void storeIndirect() =0;

	// checks for null object
	virtual kbool_t isNull() =0;
	virtual kbool_t isNull_loc(const ktoken_t token) =0;
	virtual kbool_t isNull_arg(const ktoken_t token) =0;
	virtual kbool_t isNull_fld(const ktoken_t token) =0;
	virtual kbool_t isNull_sfld(const ktoken_t token) =0;
	virtual kbool_t isNull_elem(const kint_t index) =0;

	// compares two references on the stack
	virtual kbool_t referenceEquals() =0;

	// invokes a method
	virtual void invokeMethod(const ktoken_t token) =0;
	virtual void invokeObject() =0; //for delegate instance
	
	// allocates and initializes local variable
	virtual void allocLocals(const TypeDef *localTypes) =0;
};
