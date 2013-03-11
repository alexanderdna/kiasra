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
	union
	{
		knuint_t   addr;
		KEXCFUNC  *func;
	};
};

typedef std::stack<KExceptionHandler> kcatchstack_t;
