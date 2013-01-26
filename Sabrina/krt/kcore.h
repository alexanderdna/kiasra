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

// token for fields, methods, variables
typedef uint16_t ktoken_t;

// token for types (in the Type Table)
typedef uint32_t ktypetoken_t;

enum class NativeFunctionResult
{
	OK = 0,
	Error = 1,
};

typedef NativeFunctionResult (*NativeFunction)();

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
	K_UDT       = K_CLASS | K_DELEGATE,
};

// module attributes
enum ModuleAttribute : uint16_t
{
	KMODA_SYS  = 0x01,
	KMODA_USER = 0x02,
	KMODA_DLL  = 0x04,
	KMODA_KASM = 0x08,
};

// class attributes
enum ClassAttribute : uint16_t
{
	KCA_PUBLIC  = 0x00,
	KCA_PRIVATE = 0x01,
	KCA_STATIC  = 0x02,
};

// field attributes
enum FieldAttribute : uint16_t
{
	KFA_PUBLIC  = 0x00,
	KFA_PRIVATE = 0x01,
	KFA_STATIC  = 0x02,
	KFA_FINAL   = 0x04,
};

// method attributes
enum MethodAttribute : uint16_t
{
	KMA_PUBLIC  = 0x00,
	KMA_PRIVATE = 0x01,
	KMA_STATIC  = 0x02,
	KMA_CTOR    = 0x04,
	KMA_NATIVE  = 0x0100,
	KMA_SPECIAL = 0x1000,
};

//===================================================================
// Metadata
//===================================================================

struct MetaModuleDef;
struct MetaTypeDef;
struct MetaClassDef;
struct MetaDelegateDef;
struct MetaFieldDef;
struct MetaMethodDef;
struct MetaParamDef;

struct MetaModuleDef
{
	kcstring_t path;
	uint32_t hash;

	ktoken_t globalIndex;

	MetaModuleDef() : path(NULL), hash(0), globalIndex(0) { }

	MetaModuleDef(kcstring_t path, uint32_t hash)
		: path(path), hash(hash), globalIndex(0) { }
};

struct MetaTypeDef
{
	TypeTag tag;
	uint16_t dim;
	ktoken_t token;

	ktoken_t globalToken;

	MetaTypeDef() : tag(K_VOID), dim(0), token(0) { }

	MetaTypeDef(TypeTag tag)
		: tag(tag), dim(0), token(0) { }

	MetaTypeDef(TypeTag tag, uint16_t dim)
		: tag(tag), dim(dim), token(0) { }

	MetaTypeDef(TypeTag tag, uint16_t dim, ktoken_t token)
		: tag(tag), dim(dim), token(token) { }
};

struct MetaClassDef
{
	kcstring_t name;
	ClassAttribute attrs;

	ktoken_t firstField;
	ktoken_t firstMethod;

	ktoken_t moduleIndex;
	ktoken_t farIndex;

	ktoken_t globalToken;

	MetaClassDef() : name(NULL), attrs(KCA_PUBLIC), firstField(0), firstMethod(0),
		moduleIndex(0), farIndex(0), globalToken(0) { }

	MetaClassDef(uint16_t attrs, kcstring_t name, ktoken_t firstField, ktoken_t firstMethod)
		: attrs((ClassAttribute)attrs), name(name), firstField(firstField), firstMethod(firstMethod),
		moduleIndex(0), farIndex(0), globalToken(0) { }

	MetaClassDef(uint16_t attrs, kcstring_t name, ktoken_t moduleIndex, ktoken_t farIndex, int farDummy)
		: attrs((ClassAttribute)attrs), name(name), moduleIndex(moduleIndex), farIndex(farIndex),
		firstField(0), firstMethod(0), globalToken(0) { }
};

struct MetaDelegateDef
{
	kcstring_t name;
	ClassAttribute attrs;

	ktypetoken_t returnType;
	ktoken_t firstParam;

	ktoken_t moduleIndex;
	ktoken_t farIndex;

	ktoken_t globalToken;

	MetaDelegateDef() : name(NULL), attrs(KCA_PUBLIC), returnType(0), firstParam(0),
		moduleIndex(0), farIndex(0), globalToken(0) { }

	MetaDelegateDef(uint16_t attrs, kcstring_t name, ktypetoken_t returnType, ktoken_t firstParam)
		: attrs((ClassAttribute)attrs), name(name), returnType(returnType), firstParam(firstParam),
		moduleIndex(0), farIndex(0), globalToken(0) { }

	MetaDelegateDef(uint16_t attrs, kcstring_t name, ktoken_t moduleIndex, ktoken_t farIndex, int farDummy)
		: attrs((ClassAttribute)attrs), name(name), moduleIndex(moduleIndex), farIndex(farIndex),
		returnType(0), firstParam(0), globalToken(0) { }
};

struct MetaFieldDef
{
	kcstring_t name;
	FieldAttribute attrs;

	ktypetoken_t declType;

	ktoken_t localToken;

	MetaFieldDef() : name(NULL), attrs(KFA_PUBLIC), declType(0), localToken(0) { }

	MetaFieldDef(uint16_t attrs, kcstring_t name, ktypetoken_t declType)
		: attrs((FieldAttribute)attrs), name(name), declType(declType),
		localToken(0) { }
};

struct MetaMethodDef
{
	kcstring_t name;
	MethodAttribute attrs;

	ktypetoken_t returnType;
	ktoken_t firstParam;
	ktoken_t firstLocal;

	union
	{
		uint32_t address;
		NativeFunction nativeFunction;
	};

	ktoken_t localToken;

	MetaMethodDef() : name(NULL), attrs(KMA_PUBLIC), returnType(0), firstParam(0), firstLocal(0), address(0), localToken(0) { }

	MetaMethodDef(uint16_t attrs, kcstring_t name, ktypetoken_t returnType, ktoken_t firstParam, NativeFunction nativeFunction)
		: attrs((MethodAttribute)attrs), name(name), returnType(returnType), firstParam(firstParam), nativeFunction(nativeFunction),
		firstLocal(0), localToken(0) { }
};

struct MetaParamDef
{
	kcstring_t name;
	ktypetoken_t declType;
	bool byRef;

	ktoken_t localToken;

	MetaParamDef() : name(NULL), declType(0), byRef(false) { }

	MetaParamDef(bool byRef, kcstring_t name, ktypetoken_t declType)
		: byRef(byRef), name(name), declType(declType),
		localToken(0) { }
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
	virtual void allocLocals(const ktoken_t *localTypes) =0;
};
