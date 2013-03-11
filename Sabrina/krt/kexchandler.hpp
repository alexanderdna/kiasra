#pragma once

#include "kni.h"
#include <stack>

struct TypeDef;
struct KFrame;

struct KExceptionHandler
{
	bool           native;
	const TypeDef *excType;
	KFrame        *frame;
	knuint_t       addrEnd;
	union
	{
		knuint_t   addr;
		KEXCFUNC  *func;
	};
};

typedef std::stack<KExceptionHandler> kcatchstack_t;
