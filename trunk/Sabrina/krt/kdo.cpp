#include "kenv.hpp"

#include "kconfig.hpp"
#include "krt.h"
#include "kmeta.hpp"
#include "kgc.hpp"
#include "kframe.hpp"
#include "kobject.hpp"
#include "ktypetree.hpp"
#include "kexchandler.hpp"
#include "kstringutils.hpp"

#include <set>
#include <cstdlib>

//===================================================
// Defines

#define GET_PRIMITIVE_TYPE(tag) (env->primitiveTypes[(unsigned)tag])

#define GC_ALLOC(len)		(env->gc->alloc(len))
#define GC_REGISTER(p)		env->gc->addRoot(p)
#define GC_UNREGISTER(p)	env->gc->removeRoot(p)

#define BCREAD(v, t)		v = *(t *)(env->code + env->ip); env->ip += sizeof(t)

#define COND_JUMP(OP) \
	{ \
		if (a.type == GET_PRIMITIVE_TYPE(KT_DOUBLE)) \
		{ \
			if (a.vDouble OP b.vDouble) env->ip += offset; \
		} \
		else if (a.type == GET_PRIMITIVE_TYPE(KT_FLOAT)) \
		{ \
			if (a.vFloat OP b.vFloat) env->ip += offset; \
		} \
		else \
		{ \
			switch (a.type->tag & KT_SCALAR_MASK) \
			{ \
			case KT_CHAR: case KT_BYTE: case KT_USHORT: case KT_UINT: \
				if (a.vUInt OP b.vUInt) env->ip += offset; \
				break; \
			case KT_ULONG: \
				if (a.vULong OP b.vULong) env->ip += offset; \
				break; \
			case KT_SBYTE: \
				if (a.vSByte OP b.vSByte) env->ip += offset; \
				break; \
			case KT_SHORT: \
				if (a.vShort OP b.vShort) env->ip += offset; \
				break; \
			case KT_INT: \
				if (a.vInt OP b.vInt) env->ip += offset; \
				break; \
			case KT_LONG: \
				if (a.vLong OP b.vLong) env->ip += offset; \
				break; \
			} \
		} \
	}

#define ARITH_OP(OP) \
	{ \
		switch (a.type->tag & KT_SCALAR_MASK) \
		{ \
		case KT_CHAR: \
			env->stackPushChar(a.vChar OP b.vChar); \
			break; \
		case KT_BYTE: \
			env->stackPushByte(a.vByte OP b.vByte); \
			break; \
		case KT_SBYTE: \
			env->stackPushSByte(a.vSByte OP b.vSByte); \
			break; \
		case KT_SHORT: \
			env->stackPushShort(a.vShort OP b.vShort); \
			break; \
		case KT_USHORT: \
			env->stackPushUShort(a.vUShort OP b.vUShort); \
			break; \
		case KT_INT: \
			env->stackPushInt(a.vInt OP b.vInt); \
			break; \
		case KT_UINT: \
			env->stackPushUInt(a.vUInt OP b.vUInt); \
			break; \
		case KT_LONG: \
			env->stackPushLong(a.vLong OP b.vLong); \
			break; \
		case KT_ULONG: \
			env->stackPushULong(a.vULong OP b.vULong); \
			break; \
		case KT_FLOAT: \
			env->stackPushFloat(a.vFloat OP b.vFloat); \
			break; \
		case KT_DOUBLE: \
			env->stackPushDouble(a.vDouble OP + b.vDouble); \
			break; \
		} \
	}

#define COMP_OP(OP) \
	{ \
		switch (a.type->tag & KT_SCALAR_MASK) \
		{ \
		case KT_CHAR: \
			env->stackPushBool(a.vChar OP b.vChar); \
			break; \
		case KT_BYTE: \
			env->stackPushBool(a.vByte OP b.vByte); \
			break; \
		case KT_SBYTE: \
			env->stackPushBool(a.vSByte OP b.vSByte); \
			break; \
		case KT_SHORT: \
			env->stackPushBool(a.vShort OP b.vShort); \
			break; \
		case KT_USHORT: \
			env->stackPushBool(a.vUShort OP b.vUShort); \
			break; \
		case KT_INT: \
			env->stackPushBool(a.vInt OP b.vInt); \
			break; \
		case KT_UINT: \
			env->stackPushBool(a.vUInt OP b.vUInt); \
			break; \
		case KT_LONG: \
			env->stackPushBool(a.vLong OP b.vLong); \
			break; \
		case KT_ULONG: \
			env->stackPushBool(a.vULong OP b.vULong); \
			break; \
		case KT_FLOAT: \
			env->stackPushBool(a.vFloat OP b.vFloat); \
			break; \
		case KT_DOUBLE: \
			env->stackPushBool(a.vDouble OP + b.vDouble); \
			break; \
		} \
	}

#define LOGIC_OP(LOP, BOP) \
	{ \
		switch (a.type->tag & KT_SCALAR_MASK) \
		{ \
		case KT_BOOL: \
			env->stackPushChar(a.vBool LOP b.vBool); \
			break; \
		case KT_CHAR: \
			env->stackPushChar(a.vChar BOP b.vChar); \
			break; \
		case KT_BYTE: \
			env->stackPushByte(a.vByte BOP b.vByte); \
			break; \
		case KT_SBYTE: \
			env->stackPushSByte(a.vSByte BOP b.vSByte); \
			break; \
		case KT_SHORT: \
			env->stackPushShort(a.vShort BOP b.vShort); \
			break; \
		case KT_USHORT: \
			env->stackPushUShort(a.vUShort BOP b.vUShort); \
			break; \
		case KT_INT: \
			env->stackPushInt(a.vInt BOP b.vInt); \
			break; \
		case KT_UINT: \
			env->stackPushUInt(a.vUInt BOP b.vUInt); \
			break; \
		case KT_LONG: \
			env->stackPushLong(a.vLong BOP b.vLong); \
			break; \
		case KT_ULONG: \
			env->stackPushULong(a.vULong BOP b.vULong); \
			break; \
		} \
	}

#define CONV_OP(t, T) \
	{ \
		if (obj.type->dim == 0) \
		{ \
			switch (obj.type->tag & KT_SCALAR_MASK) \
			{ \
			case KT_BOOL: \
				env->stackPush##T(obj.vBool ? (t)1 : (t)0); break; \
			case KT_CHAR: \
				env->stackPush##T((t)obj.vChar); break; \
			case KT_BYTE: \
				env->stackPush##T((t)obj.vByte); break; \
			case KT_SBYTE: \
				env->stackPush##T((t)obj.vSByte); break; \
			case KT_SHORT: \
				env->stackPush##T((t)obj.vShort); break; \
			case KT_USHORT: \
				env->stackPush##T((t)obj.vUShort); break; \
			case KT_INT: \
				env->stackPush##T((t)obj.vInt); break; \
			case KT_UINT: \
				env->stackPush##T((t)obj.vUInt); break; \
			case KT_LONG: \
				env->stackPush##T((t)obj.vLong); break; \
			case KT_ULONG: \
				env->stackPush##T((t)obj.vULong); break; \
			case KT_FLOAT: \
				env->stackPush##T((t)obj.vFloat); break; \
			case KT_DOUBLE: \
				env->stackPush##T((t)obj.vDouble); break; \
			case KT_STRING: \
				/* InvalidCast exception */ env->throwException(); break; \
			default: \
				if (obj.type == KObject::nullType) \
				{ env->stackPush##T((t)0); } \
				else \
				{ /* InvalidCast exception */ env->throwException(); } \
				break; \
			} \
		} \
		else \
		{ \
			/* InvalidCast exception */ env->throwException(); \
		} \
	}

#define CAST_OP(t, T) \
	{ \
		switch (srcType->tag & KT_SCALAR_MASK) \
		{ \
		case KT_BOOL: \
			env->stackPush##T(obj.vBool ? (t)1 : (t)0); break; \
		case KT_CHAR: \
			env->stackPush##T((t)obj.vChar); break; \
		case KT_BYTE: \
			env->stackPush##T((t)obj.vByte); break; \
		case KT_SBYTE: \
			env->stackPush##T((t)obj.vSByte); break; \
		case KT_SHORT: \
			env->stackPush##T((t)obj.vShort); break; \
		case KT_USHORT: \
			env->stackPush##T((t)obj.vUShort); break; \
		case KT_INT: \
			env->stackPush##T((t)obj.vInt); break; \
		case KT_UINT: \
			env->stackPush##T((t)obj.vUInt); break; \
		case KT_LONG: \
			env->stackPush##T((t)obj.vLong); break; \
		case KT_ULONG: \
			env->stackPush##T((t)obj.vULong); break; \
		case KT_FLOAT: \
			env->stackPush##T((t)obj.vFloat); break; \
		case KT_DOUBLE: \
			env->stackPush##T((t)obj.vDouble); break; \
		case KT_STRING: \
			/* InvalidCast exception */ env->throwException(); break; \
		default: \
			if (obj.type == KEnvironment::nullType) \
			{ env->stackPush##T((t)0); } \
			else \
			{ /* InvalidCast exception */ env->throwException(); } \
			break; \
		} \
	}

//===================================================
// Static fields

KEnvironment::EXECUTOR * KEnvironment::executors[256];
KEnvironment::EXECUTOR * KEnvironment::defaultExecutors[] =
{
	KEnvironment::do_nop,

	KEnvironment::do_ldtrue,
	KEnvironment::do_ldfalse,
	KEnvironment::do_ldc_ch,
	KEnvironment::do_ldc_i1,
	KEnvironment::do_ldc_u1,
	KEnvironment::do_ldc_i2,
	KEnvironment::do_ldc_u2,
	KEnvironment::do_ldc_i4,
	KEnvironment::do_ldc_u4,
	KEnvironment::do_ldc_i8,
	KEnvironment::do_ldc_u8,
	KEnvironment::do_ldc_r4,
	KEnvironment::do_ldc_r8,
	KEnvironment::do_ldstr,

	KEnvironment::do_ldthis,
	KEnvironment::do_ldnull,

	KEnvironment::do_ldloc,
	KEnvironment::do_ldloca,
	KEnvironment::do_ldarg,
	KEnvironment::do_ldarga,
	KEnvironment::do_ldfld,
	KEnvironment::do_ldflda,
	KEnvironment::do_ldsfld,
	KEnvironment::do_ldsflda,
	KEnvironment::do_ldelem,
	KEnvironment::do_ldelema,
	KEnvironment::do_ldind,

	KEnvironment::do_stloc,
	KEnvironment::do_starg,
	KEnvironment::do_stfld,
	KEnvironment::do_stsfld,
	KEnvironment::do_stelem,
	KEnvironment::do_stind,
	
	KEnvironment::do_pop,
	KEnvironment::do_dup,
	
	KEnvironment::do_jmp,
	KEnvironment::do_je,
	KEnvironment::do_jne,
	KEnvironment::do_jl,
	KEnvironment::do_jle,
	KEnvironment::do_jg,
	KEnvironment::do_jge,
	KEnvironment::do_jtrue,
	KEnvironment::do_jfalse,
	
	KEnvironment::do_enter,
	KEnvironment::do_leave,
	KEnvironment::do_throw,
	
	KEnvironment::do_calli,
	KEnvironment::do_calls,
	KEnvironment::do_callo,
	KEnvironment::do_ret,
	
	KEnvironment::do_add,
	KEnvironment::do_sub,
	KEnvironment::do_mul,
	KEnvironment::do_div,
	KEnvironment::do_rem,
	KEnvironment::do_neg,
	KEnvironment::do_equ,
	KEnvironment::do_neq,
	KEnvironment::do_les,
	KEnvironment::do_leq,
	KEnvironment::do_grt,
	KEnvironment::do_geq,
	KEnvironment::do_cat,
	KEnvironment::do_and,
	KEnvironment::do_or,
	KEnvironment::do_xor,
	KEnvironment::do_not,
	KEnvironment::do_shl,
	KEnvironment::do_shr,

	KEnvironment::do_istrue,
	KEnvironment::do_isnull,
	
	KEnvironment::do_newobj,
	KEnvironment::do_newdel,
	KEnvironment::do_newarr,
	KEnvironment::do_makearr,
	
	KEnvironment::do_conv_ch,
	KEnvironment::do_conv_i1,
	KEnvironment::do_conv_u1,
	KEnvironment::do_conv_i2,
	KEnvironment::do_conv_u2,
	KEnvironment::do_conv_i4,
	KEnvironment::do_conv_u4,
	KEnvironment::do_conv_i8,
	KEnvironment::do_conv_u8,
	KEnvironment::do_conv_r4,
	KEnvironment::do_conv_r8,
	KEnvironment::do_conv_s,
	
	KEnvironment::do_cast,
	KEnvironment::do_isinst,

	KEnvironment::do_hlt,
	KEnvironment::do_brk,
	KEnvironment::do_trace,
};

//===================================================
// Opcode executors

void KEnvironment::do_InvalidOpcode(KEnvironment *env)
{
	//throw System.Exception.fromCode(System.Exception.InvalidOpCode);
	env->throwException();
}

void KEnvironment::do_nop(KEnvironment *env)
{
	(void)0;
}

void KEnvironment::do_ldtrue(KEnvironment *env)
{
	env->stackPushBool(true);
}

void KEnvironment::do_ldfalse(KEnvironment *env)
{
	env->stackPushBool(false);
}

void KEnvironment::do_ldc_ch(KEnvironment *env)
{
	kchar_t v;
	BCREAD(v, kchar_t);
	env->stackPushChar(v);
}

void KEnvironment::do_ldc_i1(KEnvironment *env)
{
	ksbyte_t v;
	BCREAD(v, ksbyte_t);
	env->stackPushSByte(v);
}

void KEnvironment::do_ldc_u1(KEnvironment *env)
{
	kbyte_t v;
	BCREAD(v, kbyte_t);
	env->stackPushByte(v);
}

void KEnvironment::do_ldc_i2(KEnvironment *env)
{
	kshort_t v;
	BCREAD(v, kshort_t);
	env->stackPushShort(v);
}

void KEnvironment::do_ldc_u2(KEnvironment *env)
{
	kushort_t v;
	BCREAD(v, kushort_t);
	env->stackPushUShort(v);
}

void KEnvironment::do_ldc_i4(KEnvironment *env)
{
	kint_t v;
	BCREAD(v, kint_t);
	env->stackPushInt(v);
}

void KEnvironment::do_ldc_u4(KEnvironment *env)
{
	kuint_t v;
	BCREAD(v, kuint_t);
	env->stackPushUInt(v);
}

void KEnvironment::do_ldc_i8(KEnvironment *env)
{
	klong_t v;
	BCREAD(v, klong_t);
	env->stackPushLong(v);
}

void KEnvironment::do_ldc_u8(KEnvironment *env)
{
	kulong_t v;
	BCREAD(v, kulong_t);
	env->stackPushULong(v);
}

void KEnvironment::do_ldc_r4(KEnvironment *env)
{
	kfloat_t v;
	BCREAD(v, kfloat_t);
	env->stackPushFloat(v);
}

void KEnvironment::do_ldc_r8(KEnvironment *env)
{
	kdouble_t v;
	BCREAD(v, kdouble_t);
	env->stackPushDouble(v);
}

void KEnvironment::do_ldstr(KEnvironment *env)
{
	ktoken32_t tok;
	BCREAD(tok, ktoken32_t);
	env->stackPushString(env->strings[tok]);
}


void KEnvironment::do_ldthis(KEnvironment *env)
{
	env->stackPush(env->args[0]);
}

void KEnvironment::do_ldnull(KEnvironment *env)
{
	env->stackPushNull();
}


void KEnvironment::do_ldloc(KEnvironment *env)
{
	kushort_t idx;
	BCREAD(idx, kushort_t);
	env->stackPush(env->locals[idx]);
}

void KEnvironment::do_ldloca(KEnvironment *env)
{
	kushort_t idx;
	BCREAD(idx, kushort_t);
	env->stackPushAddress(&env->locals[idx]);
}

void KEnvironment::do_ldarg(KEnvironment *env)
{
	kushort_t idx;
	BCREAD(idx, kushort_t);
	env->stackPush(env->args[idx]);
}

void KEnvironment::do_ldarga(KEnvironment *env)
{
	kushort_t idx;
	BCREAD(idx, kushort_t);
	env->stackPushAddress(&env->args[idx]);
}

void KEnvironment::do_ldfld(KEnvironment *env)
{
	ktoken16_t tok;
	BCREAD(tok, ktoken16_t);

	const KObject &obj = env->stackPop();
	env->stackPush(obj.getField(tok));
}

void KEnvironment::do_ldflda(KEnvironment *env)
{
	ktoken16_t tok;
	BCREAD(tok, ktoken16_t);

	const KObject &obj = env->stackPop();
	env->stackPushAddress(&obj.getField(tok));
}

void KEnvironment::do_ldsfld(KEnvironment *env)
{
	ktoken16_t clstok, fldtok;
	BCREAD(clstok, ktoken16_t);
	BCREAD(fldtok, ktoken16_t);

	const ClassDef *cls = env->module->classList[clstok];
	
	const KObject &obj = cls->module->staticData[clstok];
	env->stackPush(obj.getField(fldtok));
}

void KEnvironment::do_ldsflda(KEnvironment *env)
{
	ktoken16_t clstok, fldtok;
	BCREAD(clstok, ktoken16_t);
	BCREAD(fldtok, ktoken16_t);

	const ClassDef *cls = env->module->classList[clstok];

	const KObject &obj = cls->module->staticData[clstok];
	env->stackPushAddress(&obj.getField(fldtok));
}

void KEnvironment::do_ldelem(KEnvironment *env)
{
	const KObject &objIdx = env->stackPop();
	const KObject &objArr = env->stackPop();

	knint_t idx = objIdx.getNInt();
	knuint_t len = objArr.length;

	if (idx < 0 || idx + 1 > (knint_t)len)
	{
		//TODO: throw System.Exception.fromCode(System.Exception.IndexOutOfRange);
		env->throwException();
		return;
	}

	env->stackPush(objArr.getElement(idx));
}

void KEnvironment::do_ldelema(KEnvironment *env)
{
	const KObject &objIdx = env->stackPop();
	const KObject &objArr = env->stackPop();

	knint_t idx = objIdx.getNInt();
	knuint_t len = objArr.length;

	if (idx < 0 || idx + 1 > (knint_t)len)
	{
		//TODO: throw System.Exception.fromCode(System.Exception.IndexOutOfRange);
		env->throwException();
		return;
	}

	env->stackPushAddress(&objArr.getElement(idx));
}

void KEnvironment::do_ldind(KEnvironment *env)
{
	const KObject &objAddr = env->stackPop();
	env->stackPush(*objAddr.getRef());
}


void KEnvironment::do_stloc(KEnvironment *env)
{
	kushort_t idx;
	BCREAD(idx, kushort_t);

	const KObject &obj = env->stackPop();
	env->locals[idx].accept(obj);
}

void KEnvironment::do_starg(KEnvironment *env)
{
	kushort_t idx;
	BCREAD(idx, kushort_t);

	const KObject &obj = env->stackPop();
	env->args[idx].accept(obj);
}

void KEnvironment::do_stfld(KEnvironment *env)
{
	ktoken16_t tok;
	BCREAD(tok, ktoken16_t);

	const KObject &val = env->stackPop();
	KObject &obj = env->stackPop();
	obj.setField(tok, val);
}

void KEnvironment::do_stsfld(KEnvironment *env)
{
	ktoken16_t clstok, fldtok;
	BCREAD(clstok, ktoken16_t);
	BCREAD(fldtok, ktoken16_t);

	const ClassDef *cls = env->module->classList[clstok];

	const KObject &val = env->stackPop();
	KObject &obj = cls->module->staticData[clstok];
	obj.setField(fldtok, val);
}

void KEnvironment::do_stelem(KEnvironment *env)
{
	const KObject &objVal = env->stackPop();
	const KObject &objIdx = env->stackPop();
	KObject &objArr = env->stackPop();

	knint_t idx = objIdx.getNInt();
	knuint_t len = objArr.length;

	if (idx < 0 || idx + 1 > (knint_t)len)
	{
		//TODO: throw System.Exception.fromCode(System.Exception.IndexOutOfRange);
		env->throwException();
		return;
	}

	objArr.setElement(idx, objVal);
}

void KEnvironment::do_stind(KEnvironment *env)
{
	const KObject &val = env->stackPop();
	const KObject &obj = env->stackPop();
	obj.getRef()->accept(val);
}

	
void KEnvironment::do_pop(KEnvironment *env)
{
	env->stackPop();
}

void KEnvironment::do_dup(KEnvironment *env)
{
	env->stackPush(env->stackPeek());
}

	
void KEnvironment::do_jmp(KEnvironment *env)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	env->ip += offset;
}

void KEnvironment::do_je(KEnvironment *env)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	if (a.vRaw == b.vRaw)
		env->ip += offset;
}

void KEnvironment::do_jne(KEnvironment *env)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	if (a.vRaw != b.vRaw)
		env->ip += offset;
}

void KEnvironment::do_jl(KEnvironment *env)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	COND_JUMP(<);
}

void KEnvironment::do_jle(KEnvironment *env)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	COND_JUMP(<=);
}

void KEnvironment::do_jg(KEnvironment *env)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	COND_JUMP(>);
}

void KEnvironment::do_jge(KEnvironment *env)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	COND_JUMP(>=);
}

void KEnvironment::do_jtrue(KEnvironment *env)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &obj = env->stackPop();
	if (obj.vBool)
		env->ip += offset;
}

void KEnvironment::do_jfalse(KEnvironment *env)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &obj = env->stackPop();
	if (!obj.vBool)
		env->ip += offset;
}

	
void KEnvironment::do_enter(KEnvironment *env)
{
	ktoken32_t tok;
	kshort_t offset, offsetEnd;
	BCREAD(tok, ktoken32_t);
	BCREAD(offset, kshort_t);
	BCREAD(offsetEnd, kshort_t);

	const TypeDef *excType = env->module->typeList[tok];

	KExceptionHandler handler = { };
	handler.native = false;
	handler.excType = excType;
	handler.frame = env->frame;
	handler.addr = env->ip + offset;
	handler.addrEnd = env->ip + offsetEnd;

	env->catchStack->push(handler);
}

void KEnvironment::do_leave(KEnvironment *env)
{
	knuint_t addrEnd = env->catchStack->top().addrEnd;
	env->catchStack->pop();
	env->ip = addrEnd;
}

void KEnvironment::do_throw(KEnvironment *env)
{
	env->throwException();
}

	
void KEnvironment::do_calli(KEnvironment *env)
{
	ktoken16_t tok;
	BCREAD(tok, ktoken16_t);

	const KObject &obj = env->stackPeek();
	const MethodDef *met = obj.type->cls->iMethodList[tok];
	env->invokeLastThis(met);
}

void KEnvironment::do_calls(KEnvironment *env)
{
	ktoken16_t clstok, mettok;
	BCREAD(clstok, ktoken16_t);
	BCREAD(mettok, ktoken16_t);

	const MethodDef *met = env->module->classList[clstok]->sMethodList[mettok];
	env->invoke(met);
}

void KEnvironment::do_callo(KEnvironment *env)
{
	const KObject &obj = env->stackPop();

	const MethodDef *met = (MethodDef *)(obj.getField(1).getRaw());
	if (met->attrs & KMA_STATIC)
	{
		env->invoke(met);
	}
	else
	{
		env->stackPush(obj.getField(0));
		env->invokeLastThis(met);
	}
}

void KEnvironment::do_ret(KEnvironment *env)
{
	(void)0;
}

	
void KEnvironment::do_add(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	ARITH_OP(+);
}

void KEnvironment::do_sub(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	ARITH_OP(-);
}

void KEnvironment::do_mul(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	ARITH_OP(*);
}

void KEnvironment::do_div(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	ARITH_OP(/);
}

void KEnvironment::do_rem(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	switch (a.type->tag & KT_SCALAR_MASK)
	{
	case KT_CHAR:
		env->stackPushChar(a.vChar % b.vChar);
		break;
	case KT_BYTE:
		env->stackPushByte(a.vByte % b.vByte);
		break;
	case KT_SBYTE:
		env->stackPushSByte(a.vSByte % b.vSByte);
		break;
	case KT_SHORT:
		env->stackPushShort(a.vShort % b.vShort);
		break;
	case KT_USHORT:
		env->stackPushUShort(a.vUShort % b.vUShort);
		break;
	case KT_INT:
		env->stackPushInt(a.vInt % b.vInt);
		break;
	case KT_UINT:
		env->stackPushUInt(a.vUInt % b.vUInt);
		break;
	case KT_ULONG:
		env->stackPushULong(a.vULong % b.vULong);
		break;
	case KT_LONG:
		env->stackPushLong(a.vLong % b.vLong);
		break;
	}
}

void KEnvironment::do_neg(KEnvironment *env)
{
	const KObject &a = env->stackPop();

	switch (a.type->tag & KT_SCALAR_MASK)
	{
	case KT_CHAR:
		env->stackPushInt(-a.vChar);
		break;
	case KT_BYTE:
		env->stackPushInt(-a.vByte);
		break;
	case KT_SBYTE:
		env->stackPushInt(-a.vSByte);
		break;
	case KT_SHORT:
		env->stackPushInt(-a.vShort);
		break;
	case KT_USHORT:
		env->stackPushInt(-a.vUShort);
		break;
	case KT_INT:
		env->stackPushInt(-a.vInt);
		break;
	case KT_UINT:
		env->stackPushLong(-(klong_t)a.vUInt);
		break;
	case KT_LONG:
		env->stackPushLong(-a.vLong);
		break;
	case KT_ULONG:
		env->stackPush(a);
		break;
	case KT_FLOAT:
		env->stackPushFloat(-a.vFloat);
		break;
	case KT_DOUBLE:
		env->stackPushDouble(-a.vDouble);
		break;
	}
}

void KEnvironment::do_equ(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	env->stackPushBool(a.vLong == b.vLong);
}

void KEnvironment::do_neq(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	env->stackPushBool(a.vLong != b.vLong);
}

void KEnvironment::do_les(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	COMP_OP(<);
}

void KEnvironment::do_leq(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	COMP_OP(<=);
}

void KEnvironment::do_grt(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	COMP_OP(>);
}

void KEnvironment::do_geq(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	COMP_OP(>=);
}

void KEnvironment::do_cat(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	kstring_t s = krt_strcat(a.vString, a.length, b.vString, b.length);
	env->stackPushStringMoved(s, a.length + b.length);
}

void KEnvironment::do_and(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	LOGIC_OP(&&, &);
}

void KEnvironment::do_or(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	LOGIC_OP(||, |);
}

void KEnvironment::do_xor(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	LOGIC_OP(!=, ^);
}

void KEnvironment::do_not(KEnvironment *env)
{
	const KObject &a = env->stackPop();

	switch (a.type->tag & KT_SCALAR_MASK)
	{
	case KT_BOOL:
		env->stackPushBool(!a.vBool);
		break;
	case KT_CHAR:
		env->stackPushChar(~a.vChar);
		break;
	case KT_BYTE:
		env->stackPushByte(~a.vByte);
		break;
	case KT_SBYTE:
		env->stackPushSByte(~a.vSByte);
		break;
	case KT_SHORT:
		env->stackPushShort(~a.vShort);
		break;
	case KT_USHORT:
		env->stackPushUShort(~a.vUShort);
		break;
	case KT_INT:
		env->stackPushInt(~a.vInt);
		break;
	case KT_UINT:
		env->stackPushUInt(~a.vUInt);
		break;
	case KT_LONG:
		env->stackPushLong(~a.vLong);
		break;
	case KT_ULONG:
		env->stackPushULong(~a.vULong);
		break;
	}
}

void KEnvironment::do_shl(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	kuint_t n = b.vUInt;

	switch (a.type->tag & KT_SCALAR_MASK)
	{
	case KT_CHAR:
		env->stackPushChar(a.vChar << n);
		break;
	case KT_BYTE:
		env->stackPushByte(a.vByte << n);
		break;
	case KT_SBYTE:
		env->stackPushSByte(a.vSByte << n);
		break;
	case KT_SHORT:
		env->stackPushShort(a.vShort << n);
		break;
	case KT_USHORT:
		env->stackPushUShort(a.vUShort << n);
		break;
	case KT_INT:
		env->stackPushInt(a.vInt << n);
		break;
	case KT_UINT:
		env->stackPushUInt(a.vUInt << n);
		break;
	case KT_LONG:
		env->stackPushLong(a.vLong << n);
		break;
	case KT_ULONG:
		env->stackPushULong(a.vULong << n);
		break;
	}
}

void KEnvironment::do_shr(KEnvironment *env)
{
	const KObject &b = env->stackPop();
	const KObject &a = env->stackPop();

	kuint_t n = b.vUInt;

	switch (a.type->tag & KT_SCALAR_MASK)
	{
	case KT_CHAR:
		env->stackPushChar(a.vChar >> n);
		break;
	case KT_BYTE:
		env->stackPushByte(a.vByte >> n);
		break;
	case KT_SBYTE:
		env->stackPushSByte(a.vSByte >> n);
		break;
	case KT_SHORT:
		env->stackPushShort(a.vShort >> n);
		break;
	case KT_USHORT:
		env->stackPushUShort(a.vUShort >> n);
		break;
	case KT_INT:
		env->stackPushInt(a.vInt >> n);
		break;
	case KT_UINT:
		env->stackPushUInt(a.vUInt >> n);
		break;
	case KT_LONG:
		env->stackPushLong(a.vLong >> n);
		break;
	case KT_ULONG:
		env->stackPushULong(a.vULong >> n);
		break;
	}
}


void KEnvironment::do_istrue(KEnvironment *env)
{
	const KObject &obj = env->stackPop();
	env->stackPushBool((kbool_t)obj.vULong);
}

void KEnvironment::do_isnull(KEnvironment *env)
{
	const KObject &obj = env->stackPop();
	env->stackPushBool(obj.vRaw == NULL);
}

	
void KEnvironment::do_newobj(KEnvironment *env)
{
	ktoken16_t tok;
	BCREAD(tok, ktoken16_t);

	const ClassDef *cls = env->module->classList[tok];

	KObject obj;
	env->allocClassInstance(cls, obj);
	env->stackPush(obj);
	env->invokeLastThis(cls->ctor);

	env->stackPush(obj);
}

void KEnvironment::do_newdel(KEnvironment *env)
{
	ktoken16_t tok;
	BCREAD(tok, ktoken16_t);

	const DelegateDef *del = env->module->delegateList[tok];

	const KObject &objMet = env->stackPop();
	const MethodDef *met = (MethodDef *)objMet.getRaw();

	KObject obj;
	if (met->addr & KMA_STATIC)
		env->allocDelegateInstance(del, NULL, met, obj);
	else
		env->allocDelegateInstance(del, &env->stackPop(), met, obj);

	env->stackPush(obj);
}

void KEnvironment::do_newarr(KEnvironment *env)
{
	ktoken32_t tok;
	BCREAD(tok, ktoken32_t);

	const TypeDef *type = env->module->typeList[tok];

	const KObject &objLength = env->stackPop();
	kuint_t length = objLength.vUInt;

	KObject obj;
	env->allocArray(type, length, obj);
	env->stackPush(obj);
}

void KEnvironment::do_makearr(KEnvironment *env)
{
	ktoken32_t tok;
	BCREAD(tok, ktoken32_t);

	const TypeDef *type = env->module->typeList[tok];

	const KObject &objLength = env->stackPop();
	kuint_t length = objLength.vUInt;

	KObject obj;
	env->allocArrayBaking(type, length, obj);
	env->stackPush(obj);
}

	
void KEnvironment::do_conv_ch(KEnvironment *env)
{
	const KObject &obj = env->stackPop();
	CONV_OP(kchar_t, Char);
}

void KEnvironment::do_conv_i1(KEnvironment *env)
{
	const KObject &obj = env->stackPop();
	CONV_OP(ksbyte_t, SByte);
}

void KEnvironment::do_conv_u1(KEnvironment *env)
{
	const KObject &obj = env->stackPop();
	CONV_OP(kbyte_t, Byte);
}

void KEnvironment::do_conv_i2(KEnvironment *env)
{
	const KObject &obj = env->stackPop();
	CONV_OP(kshort_t, Short);
}

void KEnvironment::do_conv_u2(KEnvironment *env)
{
	const KObject &obj = env->stackPop();
	CONV_OP(kushort_t, UShort);
}

void KEnvironment::do_conv_i4(KEnvironment *env)
{
	const KObject &obj = env->stackPop();
	CONV_OP(kint_t, Int);
}

void KEnvironment::do_conv_u4(KEnvironment *env)
{
	const KObject &obj = env->stackPop();
	CONV_OP(kuint_t, UInt);
}

void KEnvironment::do_conv_i8(KEnvironment *env)
{
	const KObject &obj = env->stackPop();
	CONV_OP(klong_t, Long);
}

void KEnvironment::do_conv_u8(KEnvironment *env)
{
	const KObject &obj = env->stackPop();
	CONV_OP(kulong_t, ULong);
}

void KEnvironment::do_conv_r4(KEnvironment *env)
{
	const KObject &obj = env->stackPop();
	CONV_OP(kfloat_t, Float);
}

void KEnvironment::do_conv_r8(KEnvironment *env)
{
	const KObject &obj = env->stackPop();
	CONV_OP(kdouble_t, Double);
}

void KEnvironment::do_conv_s(KEnvironment *env)
{
	const KObject &obj = env->stackPop();
	// TODO: conv.s
}

	
void KEnvironment::do_cast(KEnvironment *env)
{
	ktoken32_t tok;
	BCREAD(tok, ktoken32_t);


	const KObject &obj = env->stackPop();
	const TypeDef *srcType = obj.type;
	const TypeDef *destType = env->module->typeList[tok];

	if (srcType == destType)
	{
		// identical type
		env->stackPush(obj);
	}
	else if (srcType == KEnvironment::nullType)
	{
		// null object
		if (destType->dim
				|| destType->cls
				|| (destType->tag & KT_REF_MASK))
		{
			// null -> reference type: ok
			env->stackPush(obj);
		}
		else
		{
			switch (destType->tag & KT_SCALAR_MASK)
			{
			case KT_BOOL:
				env->stackPushBool(false);
				break;
			case KT_CHAR:
				env->stackPushChar((kchar_t)0);
				break;
			case KT_BYTE:
				env->stackPushByte((kbyte_t)0);
				break;
			case KT_SBYTE:
				env->stackPushSByte((ksbyte_t)0);
				break;
			case KT_SHORT:
				env->stackPushShort((kshort_t)0);
				break;
			case KT_USHORT:
				env->stackPushUShort((kushort_t)0);
				break;
			case KT_INT:
				env->stackPushInt((kint_t)0);
				break;
			case KT_UINT:
				env->stackPushUInt((kuint_t)0);
				break;
			case KT_LONG:
				env->stackPushLong((klong_t)0);
				break;
			case KT_ULONG:
				env->stackPushULong((kulong_t)0);
				break;
			case KT_STRING:
				env->stackPushString(NULL);
				break;
			}
		}
	}
	else
	{
		// non-null object

		if (destType->tag & KT_SCALAR_MASK)
		{
			// -> scalar

			switch (destType->tag & KT_SCALAR_MASK)
			{
			case KT_BOOL:
				env->stackPushBool(obj.vLong != 0);
				break;
			case KT_CHAR:
				CAST_OP(kchar_t, Char);
				break;
			case KT_BYTE:
				CAST_OP(kbyte_t, Byte);
				break;
			case KT_SBYTE:
				CAST_OP(ksbyte_t, SByte);
				break;
			case KT_SHORT:
				CAST_OP(kshort_t, Short);
				break;
			case KT_USHORT:
				CAST_OP(kushort_t, UShort);
				break;
			case KT_INT:
				CAST_OP(kint_t, Int);
				break;
			case KT_UINT:
				CAST_OP(kuint_t, UInt);
				break;
			case KT_LONG:
				CAST_OP(klong_t, Long);
				break;
			case KT_ULONG:
				CAST_OP(kulong_t, ULong);
				break;
			case KT_FLOAT:
				CAST_OP(kfloat_t, Float);
				break;
			case KT_DOUBLE:
				CAST_OP(kdouble_t, Double);
				break;
			case KT_STRING:
				// TODO: conv.s
				break;
			}
		}
		else
		{
			// -> reference type

			if (destType == KEnvironment::objectType
				|| destType == KEnvironment::arrayType
				|| destType == KEnvironment::rawType)
			{
				env->stackPush(obj);
			}
			else
			{
				//TODO: InvalidCast exception
				env->throwException();
			}
		}
	}
}

void KEnvironment::do_isinst(KEnvironment *env)
{
	ktoken32_t tok;
	BCREAD(tok, ktoken32_t);

	const TypeDef *type = env->module->typeList[tok];

	const KObject &obj = env->stackPop();
	env->stackPushBool(obj.type == type);
}


void KEnvironment::do_hlt(KEnvironment *env)
{
	env->running = false;
}

void KEnvironment::do_brk(KEnvironment *env)
{
	env->running = false;
}

void KEnvironment::do_trace(KEnvironment *env)
{
	kuint_t line;
	BCREAD(line, kuint_t);

	// TODO: trace line
}

