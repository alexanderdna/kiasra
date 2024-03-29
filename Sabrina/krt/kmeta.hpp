#pragma once

#include "kconfig.hpp"
#include "krt.h"
#include "kni.h"

#ifdef ISWIN
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#include <dlfcn.h>
#endif

//===================================================

// Tests to differentiate baked info from builder

#define IS_TYPEDEF(p)		(p->size == sizeof(TypeDef))
#define IS_CLASSDEF(p)		(p->size == sizeof(ClassDef))
#define IS_DELEGATEDEF(p)	(p->size == sizeof(DelegateDef))
#define IS_FIELDDEF(p)		(p->size == sizeof(FieldDef))
#define IS_METHODDEF(p)		(p->size == sizeof(MethodDef))
#define IS_PARAMDEF(p)		(p->size == sizeof(ParamDef))

//===================================================

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

struct TypeDef
{
	uint16_t  tag;
	kushort_t dim;
	union
	{
		ClassDef    *cls;
		DelegateDef *del;
	};

	ktoken32_t localIndex; // position in type list, only used by Compiler Services
};

struct MetaTypeDef
{
	kushort_t  tag;
	kushort_t  dim;
	ktoken16_t tok;
};

struct ModuleDef
{
	uint16_t         attrs;
	kstring_t        path;

	kuint_t          stringCount;
	kstring_t       *strings;
	kuint_t         *stringLengths;

	kushort_t        moduleCount;
	ModuleDef      **moduleList;	// array of imported modules

	kuint_t          typeCount;
	const TypeDef  **typeList;		// array of pointers to types

	kushort_t        classCount;
	ClassDef       **classList;		// array of pointers to internal and external classes
	bool            *extClassFlags;	// for internal/external detection

	kushort_t        delegateCount;
	DelegateDef    **delegateList;	// array of pointers to internal and external delegates
	bool            *extDelegateFlags;	// for internal/external detection

	kuint_t          fieldCount;
	FieldDef        *fieldList;		// array of class fields

	kuint_t          methodCount;
	MethodDef       *methodList;	// array of class methods

	kuint_t          paramCount;
	ParamDef        *paramList;		// array of method parameters

	kuint_t          dparamCount;
	ParamDef        *dparamList;	// array of delegate parameters

	kuint_t          localCount;
	const TypeDef  **localList;		// array of pointers to elements of `types`

	KObject         *staticData;	// objects to store static fields
	const unsigned char *code;		// module bytecode

	const MethodDef *entryMethod;	// execution starts here

#ifdef ISWIN
		HMODULE      libHandle;
#else
		void        *libHandle;
#endif

	ModuleDef(void);  // definition in kmodule.cpp
	~ModuleDef(void); // definition in kmodule.cpp
};

struct MetaModuleDef
{
	uint16_t         attrs;
	ktoken32_t       path;
	uint32_t         hash;
};

struct ClassDef
{
	size_t size;

	uint16_t        attrs;
	kstring_t       name;
	
	kushort_t       fieldCount;
	FieldDef      **fieldList;	// array of pointers to elements of `fields` in ModuleDef

	kushort_t       methodCount;
	MethodDef     **methodList;	// array of pointers to elements of `methods` in ModuleDef

	ModuleDef      *module;		// pointer to importing module

	ktoken16_t      localIndex;	// position in defining module

	kushort_t       iFieldCount;
	FieldDef      **iFieldList;	// instance fields
	
	kushort_t       sFieldCount;
	FieldDef      **sFieldList; // static fields

	MethodDef      *ctor;		// instance constructor
	MethodDef      *cctor;		// static constructor

	ClassDef(void);
	~ClassDef(void);
};

struct MetaClassDef
{
	uint16_t        attrs;
	ktoken32_t      name;
	ktoken16_t      farIndex;
	ktoken16_t      moduleIndex;
	ktoken32_t      fieldList;
	ktoken32_t      methodList;
};

struct DelegateDef
{
	size_t size;

	uint16_t        attrs;
	kstring_t       name;

	const TypeDef  *returnType;
	
	kushort_t       paramCount;
	ParamDef      **paramList;	// array of pointers to elements of `dparams` in ModuleDef

	ModuleDef      *module;		// pointer to importing module

	ktoken16_t      localIndex;	// position in defining module

	DelegateDef(void);
	~DelegateDef(void);
};

struct MetaDelegateDef
{
	uint16_t        attrs;
	ktoken32_t      name;
	ktoken16_t      farIndex;
	ktoken16_t      moduleIndex;
	ktoken32_t      returnType;
	ktoken32_t      paramList;
};

struct FieldDef
{
	size_t size;

	uint16_t        attrs;
	kstring_t       name;
	const TypeDef  *declType;

	ClassDef       *klass;		// pointer to parent class
	ktoken16_t      localIndex;	// position in parent class, instance-static separative
};

struct MetaFieldDef
{
	uint16_t        attrs;
	ktoken32_t      name;
	ktoken32_t      declType;
};

struct MethodDef
{
	size_t size;

	uint16_t         attrs;
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

	MethodDef(void);
	~MethodDef(void);
};

struct MetaMethodDef
{
	uint16_t         attrs;
	ktoken32_t       name;
	ktoken32_t       returnType;
	ktoken32_t       paramList;
	ktoken32_t       localList;
	kuint_t          addr;
};

struct ParamDef
{
	kstring_t       name;
	const TypeDef  *declType;
	bool            byRef;

	ktoken32_t      globalIndex;
};

struct MetaParamDef
{
	ktoken32_t name;
	ktoken32_t declType;
	uint8_t    byRef;
};
