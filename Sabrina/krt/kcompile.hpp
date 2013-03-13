#pragma once

#include "kcs.h"
#include "kmeta.hpp"

struct TypeBuilder : public TypeDef
{
	kuint_t localIndex;
};

struct ModuleBuilder
{
	ModuleDef moduleDef;
};

struct ClassBuilder : public ClassDef
{
};

struct DelegateBuilder : public DelegateDef
{
};

struct FieldBuilder : public FieldDef
{
};

struct MethodBuilder : public MethodDef
{
};

struct LocalBuilder
{
	const TypeDef *declType;
	kushort_t localIndex;
};
