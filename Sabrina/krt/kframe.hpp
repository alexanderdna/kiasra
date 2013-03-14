#pragma once

#include <stack>

struct ModuleDef;
struct MethodDef;

struct KFrame
{
	ModuleDef       *module;
	const MethodDef *method;
	KObject         *args;
	KObject         *locals;
	knuint_t         ip;
	knuint_t         stackPointer; // for stack unwinding
};

typedef std::stack<KFrame> kcallstack_t;
