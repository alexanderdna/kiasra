#pragma once

#include "krt.h"
#include "kni.h"

//===================================================

struct TypeDef;
struct MetaTypeDef;

struct ModuleDef;
struct MetaModuleDef;

struct ClassDef;
struct MetaClassDef;

struct DelegateDef;
struct MetaDelegateDef;

struct FieldDef;
struct MetaFieldDef;

struct MethodDef;
struct MetaMethodDef;

struct ParamDef;
struct MetaParamDef;

//===================================================

enum ClassAttributes : uint16_t
{
	KCA_PUBLIC  = 0,
	KCA_PRIVATE = 1,
	KCA_STATIC  = 2,
};

enum FieldAttributes : uint16_t
{
	KFA_PUBLIC  = 0,
	KFA_PRIVATE = 1,
	KFA_STATIC  = 2,
	KFA_FINAL   = 4,
};

enum MethodAttributes : uint16_t
{
	KMA_PUBLIC  = 0,
	KMA_PRIVATE = 1,
	KMA_STATIC  = 2,
	KMA_CTOR    = 4,
	KMA_NATIVE  = 8
};

//===================================================

struct TypeDef
{
	ktypetag_t tag;
	kushort_t  dim;
	union
	{
		ClassDef    *cls;
		DelegateDef *del;
	};
};

struct MetaTypeDef
{
	ktypetag_t tag;
	kushort_t  dim;
	ktoken16_t tok;
};

struct ModuleDef
{

};

struct MetaModuleDef
{

};

struct ClassDef
{
	ClassAttributes attrs;
	kstring_t       name;
	
	kushort_t       fieldCount;
	FieldDef      **fieldList;

	kushort_t       methodCount;
	MethodDef     **methodList;

	ktoken16_t     localIndex;
	ktoken16_t     rtIndex;
};

struct MetaClassDef
{
	ClassAttributes attrs;
	ktoken32_t      name;
	ktoken16_t      farIndex;
	ktoken16_t      moduleIndex;
	ktoken16_t      fieldList;
	ktoken16_t      methodList;
};

struct DelegateDef
{
	ClassAttributes attrs;
	kstring_t       name;

	TypeDef        *returnType;
	
	kushort_t       paramCount;
	ParamDef      **paramList;
};

struct MetaDelegateDef
{
	ClassAttributes attrs;
	ktoken32_t      name;
	ktoken16_t      farIndex;
	ktoken16_t      moduleIndex;
	ktoken32_t      returnType;
	ktoken16_t      paramList;
};

struct FieldDef
{
	FieldAttributes attrs;
	kstring_t       name;
	TypeDef        *declType;

	ktoken16_t      localIndex; // position in parent class, instance-static separative
};

struct MetaFieldDef
{
	FieldAttributes attrs;
	ktoken32_t      name;
	ktoken32_t      declType;
};

struct MethodDef
{
	MethodAttributes attrs;
	kstring_t        name;

	TypeDef         *returnType;

	kushort_t        paramCount;
	ParamDef       **paramList;

	kushort_t        localCount;
	TypeDef        **localList;

	union
	{
		knuint_t     addr;
		NFUNC        func;
	};

	ktoken16_t       localIndex; // position in parent class
};

struct MetaMethodDef
{
	MethodAttributes attrs;
	ktoken32_t       name;
	ktoken32_t       returnType;
	ktoken16_t       paramList;
	ktoken16_t       localList;
	ktoken32_t       addr;
};

struct ParamDef
{
	kstring_t name;
	TypeDef  *declType;
	bool      byRef;
};

struct MetaParamDef
{
	ktoken32_t name;
	ktoken32_t declType;
	uint8_t    byRef;
};
