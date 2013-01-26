#pragma once

#include "kcore.h"

#ifdef KRT_EXPORTS
#define KRT_API __declspec(dllexport)
#else
#define KRT_API __declspec(dllimport)
#endif

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
	ModuleAttribute attrs;
	ClassDef *classes;
	DelegateDef *delegates;
};

struct ClassDef
{
	kcstring_t name;
	ClassAttribute attrs;

	FieldDef *fields;
	MethodDef *methods;

	MethodDef *ctor;
	MethodDef *cctor;
};

struct DelegateDef
{
	kcstring_t name;
	ClassAttribute attrs;

	TypeDef *returnType;
	ParamDef *params;
};

struct FieldDef
{
	kcstring_t name;
	FieldAttribute attrs;

	TypeDef *declType;
};

struct MethodDef
{
	kcstring_t name;
	MethodAttribute attrs;

	TypeDef *returnType;
	ParamDef *params;
};

struct ParamDef
{
	kcstring_t name;
	TypeDef *declType;
	bool byRef;
};
