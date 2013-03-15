#pragma once

#include "kni.h"
#include <stack>

struct TypeDef;
struct CallFrame;

struct ExceptionHandler
{
	const TypeDef *excType;
	CallFrame     *frame;
	knuint_t       addr;
	knuint_t       addrEnd;
	knuint_t       stackPointer; //at the moment of 'enter' instruction
};

typedef std::stack<ExceptionHandler> catchstack_t;
