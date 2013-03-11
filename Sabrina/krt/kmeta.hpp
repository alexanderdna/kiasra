#pragma once

#include "krt.h"
#include "kni.h"

class KObject;

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

enum ModuleAttributes : uint16_t
{
	KMODA_SYSTEM,
	KMODA_USER,
	KMODA_KIASRA,
	KMODA_NATIVE,
};

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
	uint16_t tag;
	kushort_t  dim;
	union
	{
		ClassDef    *cls;
		DelegateDef *del;
	};
};

struct MetaTypeDef
{
	uint16_t   tag;
	kushort_t  dim;
	ktoken16_t tok;
};

struct ModuleDef
{
	ModuleAttributes attrs;
	kstring_t        path;

	kuint_t          stringCount;
	kstring_t       *strings;

	kushort_t        moduleCount;
	ModuleDef       *moduleList;	// array of imported modules

	kuint_t          typeCount;
	TypeDef        **typeList;		// array of pointers to types

	kushort_t        classCount;
	ClassDef       **classList;		// array of pointers to internal and external classes
	MetaClassDef    *metaClassList;	// for internal/external detection

	kushort_t        delegateCount;
	DelegateDef    **delegateList;	// array of pointers to internal and external delegates
	MetaDelegateDef *metaDelegateList;	// for internal/external detection

	kushort_t        fieldCount;
	FieldDef        *fieldList;		// array of class fields

	kushort_t        methodCount;
	MethodDef       *methodList;	// array of class methods

	kushort_t        paramCount;
	ParamDef        *paramList;		// array of method parameters

	kushort_t        dparamCount;
	ParamDef        *dparamList;	// array of delegate parameters

	kushort_t        localCount;
	TypeDef        **localList;		// array of pointers to elements of `types`

	ktoken16_t       rtIndex;		// position in importing module

	KObject         *staticData;	// objects to store static fields
	const unsigned char *code;		// module bytecode
};

struct MetaModuleDef
{
	ModuleAttributes attrs;
	ktoken32_t       path;
};

struct ClassDef
{
	ClassAttributes attrs;
	kstring_t       name;
	
	kushort_t       fieldCount;
	FieldDef      **fieldList;	// array of pointers to elements of `fields` in ModuleDef

	kushort_t       methodCount;
	MethodDef     **methodList;	// array of pointers to elements of `methods` in ModuleDef

	ModuleDef      *module;		// pointer to importing module

	ktoken16_t      localIndex;	// position in defining module
	ktoken16_t      rtIndex;	// position in importing module

	kushort_t       iFieldCount;
	FieldDef      **iFieldList;	// instance fields
	
	kushort_t       sFieldCount;
	FieldDef      **sFieldList; // static fields

	MethodDef     **iMethodList;// instance method
	MethodDef     **sMethodList;// static method
	MethodDef      *ctor;		// instance constructor
	MethodDef      *cctor;		// static constructor
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

	const TypeDef  *returnType;
	
	kushort_t       paramCount;
	ParamDef      **paramList;	// array of pointers to elements of `dparams` in ModuleDef

	ModuleDef      *module;		// pointer to importing module

	ktoken16_t      localIndex;	// position in defining module
	ktoken16_t      rtIndex;	// position in importing module
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
	const TypeDef  *declType;

	ClassDef       *klass;		// pointer to parent class
	ktoken16_t      localIndex;	// position in parent class, instance-static separative
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

	const TypeDef   *returnType;

	kushort_t        paramCount;
	ParamDef       **paramList;

	kushort_t        localCount;
	const TypeDef  **localList;

	union
	{
		knuint_t     addr;
		NFUNC        func;
	};

	ClassDef        *klass;			// pointer to parent class
	ktoken16_t       localIndex;	// position in parent class
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
	kstring_t       name;
	const TypeDef  *declType;
	bool            byRef;
};

struct MetaParamDef
{
	ktoken32_t name;
	ktoken32_t declType;
	uint8_t    byRef;
};
