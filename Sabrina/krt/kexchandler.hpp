#pragma once

#include "kni.h"
#include <stack>

struct TypeDef;
struct KFrame;

struct KExceptionHandler
{
	const TypeDef *excType;
	KFrame        *frame;
	knuint_t       addr;
	knuint_t       addrEnd;
};

typedef std::stack<KExceptionHandler> kcatchstack_t;
