#pragma once

#include <stack>

struct ModuleDef;
struct MethodDef;

struct CallFrame
{
	ModuleDef       *module;
	const MethodDef *method;
	KObject         *args;
	KObject         *locals;
	knuint_t         ip;
	knuint_t         stackPointer; // for stack unwinding
};

typedef std::stack<CallFrame> callstack_t;
