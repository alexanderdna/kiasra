#include "kenv.hpp"

#include "kconfig.hpp"
#include "krt.h"
#include "kni.h"
#include "kmeta.hpp"
#include "kgc.hpp"
#include "kframe.hpp"
#include "kobject.hpp"
#include "ktypetree.hpp"
#include "kexchandler.hpp"
#include "kstringutils.hpp"

#include <set>
#include <cstdlib>
#include <cstdio>
#include <cwchar>

//===================================================
// Defines

#define GC_ALLOC(len)		(KEnvironment::gc->alloc(len))
#define GC_REGISTER(p)		KEnvironment::gc->addRoot(p)
#define GC_UNREGISTER(p)	KEnvironment::gc->removeRoot(p)

#define BCREAD(v, t)		v = *(t *)(KEnvironment::code + KEnvironment::ip); KEnvironment::ip += sizeof(t)

#define COND_JUMP(OP) \
	{ \
		if (a.type == GET_PRIMITIVE_TYPE(KT_DOUBLE)) \
		{ \
			if (a.vDouble OP b.vDouble) KEnvironment::ip += offset; \
		} \
		else if (a.type == GET_PRIMITIVE_TYPE(KT_FLOAT)) \
		{ \
			if (a.vFloat OP b.vFloat) KEnvironment::ip += offset; \
		} \
		else \
		{ \
			switch (a.type->tag & KT_SCALAR_MASK) \
			{ \
			case KT_CHAR: case KT_BYTE: case KT_USHORT: case KT_UINT: \
				if (a.vUInt OP b.vUInt) KEnvironment::ip += offset; \
				break; \
			case KT_ULONG: \
				if (a.vULong OP b.vULong) KEnvironment::ip += offset; \
				break; \
			case KT_SBYTE: \
				if (a.vSByte OP b.vSByte) KEnvironment::ip += offset; \
				break; \
			case KT_SHORT: \
				if (a.vShort OP b.vShort) KEnvironment::ip += offset; \
				break; \
			case KT_INT: \
				if (a.vInt OP b.vInt) KEnvironment::ip += offset; \
				break; \
			case KT_LONG: \
				if (a.vLong OP b.vLong) KEnvironment::ip += offset; \
				break; \
			} \
		} \
	}

#define ARITH_OP(OP) \
	{ \
		switch (a.type->tag & KT_SCALAR_MASK) \
		{ \
		case KT_CHAR: \
			KEnvironment::stackPushChar(a.vChar OP b.vChar); \
			break; \
		case KT_BYTE: \
			KEnvironment::stackPushByte(a.vByte OP b.vByte); \
			break; \
		case KT_SBYTE: \
			KEnvironment::stackPushSByte(a.vSByte OP b.vSByte); \
			break; \
		case KT_SHORT: \
			KEnvironment::stackPushShort(a.vShort OP b.vShort); \
			break; \
		case KT_USHORT: \
			KEnvironment::stackPushUShort(a.vUShort OP b.vUShort); \
			break; \
		case KT_INT: \
			KEnvironment::stackPushInt(a.vInt OP b.vInt); \
			break; \
		case KT_UINT: \
			KEnvironment::stackPushUInt(a.vUInt OP b.vUInt); \
			break; \
		case KT_LONG: \
			KEnvironment::stackPushLong(a.vLong OP b.vLong); \
			break; \
		case KT_ULONG: \
			KEnvironment::stackPushULong(a.vULong OP b.vULong); \
			break; \
		case KT_FLOAT: \
			KEnvironment::stackPushFloat(a.vFloat OP b.vFloat); \
			break; \
		case KT_DOUBLE: \
			KEnvironment::stackPushDouble(a.vDouble OP + b.vDouble); \
			break; \
		} \
	}

#define COMP_OP(OP) \
	{ \
		switch (a.type->tag & KT_SCALAR_MASK) \
		{ \
		case KT_CHAR: \
			KEnvironment::stackPushBool(a.vChar OP b.vChar); \
			break; \
		case KT_BYTE: \
			KEnvironment::stackPushBool(a.vByte OP b.vByte); \
			break; \
		case KT_SBYTE: \
			KEnvironment::stackPushBool(a.vSByte OP b.vSByte); \
			break; \
		case KT_SHORT: \
			KEnvironment::stackPushBool(a.vShort OP b.vShort); \
			break; \
		case KT_USHORT: \
			KEnvironment::stackPushBool(a.vUShort OP b.vUShort); \
			break; \
		case KT_INT: \
			KEnvironment::stackPushBool(a.vInt OP b.vInt); \
			break; \
		case KT_UINT: \
			KEnvironment::stackPushBool(a.vUInt OP b.vUInt); \
			break; \
		case KT_LONG: \
			KEnvironment::stackPushBool(a.vLong OP b.vLong); \
			break; \
		case KT_ULONG: \
			KEnvironment::stackPushBool(a.vULong OP b.vULong); \
			break; \
		case KT_FLOAT: \
			KEnvironment::stackPushBool(a.vFloat OP b.vFloat); \
			break; \
		case KT_DOUBLE: \
			KEnvironment::stackPushBool(a.vDouble OP + b.vDouble); \
			break; \
		} \
	}

#define LOGIC_OP(LOP, BOP) \
	{ \
		switch (a.type->tag & KT_SCALAR_MASK) \
		{ \
		case KT_BOOL: \
			KEnvironment::stackPushChar(a.vBool LOP b.vBool); \
			break; \
		case KT_CHAR: \
			KEnvironment::stackPushChar(a.vChar BOP b.vChar); \
			break; \
		case KT_BYTE: \
			KEnvironment::stackPushByte(a.vByte BOP b.vByte); \
			break; \
		case KT_SBYTE: \
			KEnvironment::stackPushSByte(a.vSByte BOP b.vSByte); \
			break; \
		case KT_SHORT: \
			KEnvironment::stackPushShort(a.vShort BOP b.vShort); \
			break; \
		case KT_USHORT: \
			KEnvironment::stackPushUShort(a.vUShort BOP b.vUShort); \
			break; \
		case KT_INT: \
			KEnvironment::stackPushInt(a.vInt BOP b.vInt); \
			break; \
		case KT_UINT: \
			KEnvironment::stackPushUInt(a.vUInt BOP b.vUInt); \
			break; \
		case KT_LONG: \
			KEnvironment::stackPushLong(a.vLong BOP b.vLong); \
			break; \
		case KT_ULONG: \
			KEnvironment::stackPushULong(a.vULong BOP b.vULong); \
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
				KEnvironment::stackPush##T(obj.vBool ? (t)1 : (t)0); break; \
			case KT_CHAR: \
				KEnvironment::stackPush##T((t)obj.vChar); break; \
			case KT_BYTE: \
				KEnvironment::stackPush##T((t)obj.vByte); break; \
			case KT_SBYTE: \
				KEnvironment::stackPush##T((t)obj.vSByte); break; \
			case KT_SHORT: \
				KEnvironment::stackPush##T((t)obj.vShort); break; \
			case KT_USHORT: \
				KEnvironment::stackPush##T((t)obj.vUShort); break; \
			case KT_INT: \
				KEnvironment::stackPush##T((t)obj.vInt); break; \
			case KT_UINT: \
				KEnvironment::stackPush##T((t)obj.vUInt); break; \
			case KT_LONG: \
				KEnvironment::stackPush##T((t)obj.vLong); break; \
			case KT_ULONG: \
				KEnvironment::stackPush##T((t)obj.vULong); break; \
			case KT_FLOAT: \
				KEnvironment::stackPush##T((t)obj.vFloat); break; \
			case KT_DOUBLE: \
				KEnvironment::stackPush##T((t)obj.vDouble); break; \
			case KT_STRING: \
				KniThrowException(KEnvironment::exceptions.invalidCast); break; \
			default: \
				if (obj.type == KObject::nullType) \
				{ KEnvironment::stackPush##T((t)0); } \
				else \
				{ KniThrowException(KEnvironment::exceptions.invalidCast); } \
				break; \
			} \
		} \
		else \
		{ \
			KniThrowException(KEnvironment::exceptions.invalidCast); \
		} \
	}

#define CAST_OP(t, T) \
	{ \
		switch (srcType->tag & KT_SCALAR_MASK) \
		{ \
		case KT_BOOL: \
			KEnvironment::stackPush##T(obj.vBool ? (t)1 : (t)0); break; \
		case KT_CHAR: \
			KEnvironment::stackPush##T((t)obj.vChar); break; \
		case KT_BYTE: \
			KEnvironment::stackPush##T((t)obj.vByte); break; \
		case KT_SBYTE: \
			KEnvironment::stackPush##T((t)obj.vSByte); break; \
		case KT_SHORT: \
			KEnvironment::stackPush##T((t)obj.vShort); break; \
		case KT_USHORT: \
			KEnvironment::stackPush##T((t)obj.vUShort); break; \
		case KT_INT: \
			KEnvironment::stackPush##T((t)obj.vInt); break; \
		case KT_UINT: \
			KEnvironment::stackPush##T((t)obj.vUInt); break; \
		case KT_LONG: \
			KEnvironment::stackPush##T((t)obj.vLong); break; \
		case KT_ULONG: \
			KEnvironment::stackPush##T((t)obj.vULong); break; \
		case KT_FLOAT: \
			KEnvironment::stackPush##T((t)obj.vFloat); break; \
		case KT_DOUBLE: \
			KEnvironment::stackPush##T((t)obj.vDouble); break; \
		case KT_STRING: \
			KniThrowException(KEnvironment::exceptions.invalidCast); break; \
		default: \
			if (obj.type == KEnvironment::nullType) \
			{ KEnvironment::stackPush##T((t)0); } \
			else \
			{ KniThrowException(KEnvironment::exceptions.invalidCast); } \
			break; \
		} \
	}

#define CONV_S_BUFFER_SIZE	512

static kchar_t conv_s_buffer[CONV_S_BUFFER_SIZE];

#define CONV_S_ACTION(fmt, val) len = swprintf(conv_s_buffer, CONV_S_BUFFER_SIZE, fmt, val)

#define	CONV_S	\
	{ \
		int len; \
		if (srcType->dim == 0) \
		{ \
			switch (srcType->tag & KT_SCALAR_MASK) \
			{ \
			case KT_BOOL: \
				len = swprintf(conv_s_buffer, CONV_S_BUFFER_SIZE, obj.vBool ? L"true" : L"false"); \
				break; \
			case KT_CHAR: \
				CONV_S_ACTION(L"%c", obj.vChar); \
				break; \
			case KT_BYTE: \
				CONV_S_ACTION(L"%u", obj.vByte); \
				break; \
			case KT_SBYTE: \
				CONV_S_ACTION(L"%d", obj.vSByte); \
				break; \
			case KT_SHORT: \
				CONV_S_ACTION(L"%u", obj.vShort); \
				break; \
			case KT_USHORT: \
				CONV_S_ACTION(L"%u", obj.vUShort); \
				break; \
			case KT_INT: \
				CONV_S_ACTION(L"%ld", obj.vInt); \
				break; \
			case KT_UINT: \
				CONV_S_ACTION(L"%lu", obj.vUInt); \
				break; \
			case KT_LONG: \
				CONV_S_ACTION(L"%lld", obj.vLong); \
				break; \
			case KT_ULONG: \
				CONV_S_ACTION(L"%llu", obj.vULong); \
				break; \
			case KT_FLOAT: \
				CONV_S_ACTION(L"%f", obj.vFloat); \
				break; \
			case KT_DOUBLE: \
				CONV_S_ACTION(L"%f", obj.vDouble); \
				break; \
			case KT_STRING: \
				break; \
			default: \
				KniThrowException(KEnvironment::exceptions.invalidCast); \
				return; \
			} \
			kstring_t s = krt_strdup(conv_s_buffer, len); \
			KEnvironment::stackPushStringMoved(s, len); \
			return; \
		} \
		else \
		{ \
			KniThrowException(KEnvironment::exceptions.invalidCast); \
			return; \
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
	KEnvironment::do_ldtype,
	KEnvironment::do_ldmethod,

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

void KEnvironment::do_InvalidOpcode(void)
{
	KniThrowException(KEnvironment::exceptions.invalidOpCode);
}

void KEnvironment::do_nop(void)
{
	(void)0;
}

void KEnvironment::do_ldtrue(void)
{
	KEnvironment::stackPushBool(true);
}

void KEnvironment::do_ldfalse(void)
{
	KEnvironment::stackPushBool(false);
}

void KEnvironment::do_ldc_ch(void)
{
	kchar_t v;
	BCREAD(v, kchar_t);
	KEnvironment::stackPushChar(v);
}

void KEnvironment::do_ldc_i1(void)
{
	ksbyte_t v;
	BCREAD(v, ksbyte_t);
	KEnvironment::stackPushSByte(v);
}

void KEnvironment::do_ldc_u1(void)
{
	kbyte_t v;
	BCREAD(v, kbyte_t);
	KEnvironment::stackPushByte(v);
}

void KEnvironment::do_ldc_i2(void)
{
	kshort_t v;
	BCREAD(v, kshort_t);
	KEnvironment::stackPushShort(v);
}

void KEnvironment::do_ldc_u2(void)
{
	kushort_t v;
	BCREAD(v, kushort_t);
	KEnvironment::stackPushUShort(v);
}

void KEnvironment::do_ldc_i4(void)
{
	kint_t v;
	BCREAD(v, kint_t);
	KEnvironment::stackPushInt(v);
}

void KEnvironment::do_ldc_u4(void)
{
	kuint_t v;
	BCREAD(v, kuint_t);
	KEnvironment::stackPushUInt(v);
}

void KEnvironment::do_ldc_i8(void)
{
	klong_t v;
	BCREAD(v, klong_t);
	KEnvironment::stackPushLong(v);
}

void KEnvironment::do_ldc_u8(void)
{
	kulong_t v;
	BCREAD(v, kulong_t);
	KEnvironment::stackPushULong(v);
}

void KEnvironment::do_ldc_r4(void)
{
	kfloat_t v;
	BCREAD(v, kfloat_t);
	KEnvironment::stackPushFloat(v);
}

void KEnvironment::do_ldc_r8(void)
{
	kdouble_t v;
	BCREAD(v, kdouble_t);
	KEnvironment::stackPushDouble(v);
}

void KEnvironment::do_ldstr(void)
{
	ktoken32_t tok;
	BCREAD(tok, ktoken32_t);
	KEnvironment::stackPushString(KEnvironment::strings[tok], KEnvironment::stringLengths[tok]);
}


void KEnvironment::do_ldthis(void)
{
	KEnvironment::stackPush(KEnvironment::args[0]);
}

void KEnvironment::do_ldnull(void)
{
	KEnvironment::stackPushNull();
}

void KEnvironment::do_ldtype(void)
{
	ktoken32_t tok;
	BCREAD(tok, ktoken32_t);

	KEnvironment::stackPushRaw((void *) KEnvironment::module->typeList[tok]);
}

void KEnvironment::do_ldmethod(void)
{
	ktoken16_t clstok, mettok;
	BCREAD(clstok, ktoken16_t);
	BCREAD(mettok, ktoken16_t);

	KEnvironment::stackPushRaw((void *) KEnvironment::module->classList[clstok]->methodList[mettok]);
}

void KEnvironment::do_ldloc(void)
{
	kushort_t idx;
	BCREAD(idx, kushort_t);
	KEnvironment::stackPush(KEnvironment::locals[idx]);
}

void KEnvironment::do_ldloca(void)
{
	kushort_t idx;
	BCREAD(idx, kushort_t);
	KEnvironment::stackPushAddress(&KEnvironment::locals[idx]);
}

void KEnvironment::do_ldarg(void)
{
	kushort_t idx;
	BCREAD(idx, kushort_t);
	KEnvironment::stackPush(KEnvironment::args[idx]);
}

void KEnvironment::do_ldarga(void)
{
	kushort_t idx;
	BCREAD(idx, kushort_t);
	KEnvironment::stackPushAddress(&KEnvironment::args[idx]);
}

void KEnvironment::do_ldfld(void)
{
	ktoken16_t tok;
	BCREAD(tok, ktoken16_t);

	const KObject &obj = KEnvironment::stackPop();
	if (obj.type == KObject::nullType)
	{
		KniThrowException(KEnvironment::exceptions.nullReference);
		return;
	}

	KEnvironment::stackPush(obj.getField(tok));
}

void KEnvironment::do_ldflda(void)
{
	ktoken16_t tok;
	BCREAD(tok, ktoken16_t);

	const KObject &obj = KEnvironment::stackPop();
	if (obj.type == KObject::nullType)
	{
		KniThrowException(KEnvironment::exceptions.nullReference);
		return;
	}

	KEnvironment::stackPushAddress(&obj.getField(tok));
}

void KEnvironment::do_ldsfld(void)
{
	ktoken16_t clstok, fldtok;
	BCREAD(clstok, ktoken16_t);
	BCREAD(fldtok, ktoken16_t);

	const ClassDef *cls = KEnvironment::module->classList[clstok];
	
	const KObject &obj = cls->module->staticData[clstok];
	KEnvironment::stackPush(obj.getField(fldtok));
}

void KEnvironment::do_ldsflda(void)
{
	ktoken16_t clstok, fldtok;
	BCREAD(clstok, ktoken16_t);
	BCREAD(fldtok, ktoken16_t);

	const ClassDef *cls = KEnvironment::module->classList[clstok];

	const KObject &obj = cls->module->staticData[clstok];
	KEnvironment::stackPushAddress(&obj.getField(fldtok));
}

void KEnvironment::do_ldelem(void)
{
	const KObject &objIdx = KEnvironment::stackPop();
	const KObject &objArr = KEnvironment::stackPop();

	knint_t idx = objIdx.getNInt();
	knuint_t len = objArr.length;

	if (objArr.type == KObject::nullType)
	{
		KniThrowException(KEnvironment::exceptions.nullReference);
		return;
	}

	if (idx < 0 || idx + 1 > (knint_t)len)
	{
		KniThrowException(KEnvironment::exceptions.indexOutOfRange);
		return;
	}

	KEnvironment::stackPush(objArr.getElement(idx));
}

void KEnvironment::do_ldelema(void)
{
	const KObject &objIdx = KEnvironment::stackPop();
	const KObject &objArr = KEnvironment::stackPop();

	knint_t idx = objIdx.getNInt();
	knuint_t len = objArr.length;
	
	if (objArr.type == KObject::nullType)
	{
		KniThrowException(KEnvironment::exceptions.nullReference);
		return;
	}

	if (idx < 0 || idx + 1 > (knint_t)len)
	{
		KniThrowException(KEnvironment::exceptions.indexOutOfRange);
		return;
	}

	KEnvironment::stackPushAddress(&objArr.getElement(idx));
}

void KEnvironment::do_ldind(void)
{
	const KObject &objAddr = KEnvironment::stackPop();
	KEnvironment::stackPush(*objAddr.getRef());
}


void KEnvironment::do_stloc(void)
{
	kushort_t idx;
	BCREAD(idx, kushort_t);

	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::locals[idx] = obj;
}

void KEnvironment::do_starg(void)
{
	kushort_t idx;
	BCREAD(idx, kushort_t);

	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::args[idx] = obj;
}

void KEnvironment::do_stfld(void)
{
	ktoken16_t tok;
	BCREAD(tok, ktoken16_t);

	const KObject &val = KEnvironment::stackPop();
	KObject &obj = KEnvironment::stackPop();
	if (obj.type == KObject::nullType)
	{
		KniThrowException(KEnvironment::exceptions.nullReference);
		return;
	}

	obj.setField(tok, val);
}

void KEnvironment::do_stsfld(void)
{
	ktoken16_t clstok, fldtok;
	BCREAD(clstok, ktoken16_t);
	BCREAD(fldtok, ktoken16_t);

	const ClassDef *cls = KEnvironment::module->classList[clstok];

	const KObject &val = KEnvironment::stackPop();
	KObject &obj = cls->module->staticData[clstok];
	obj.setField(fldtok, val);
}

void KEnvironment::do_stelem(void)
{
	const KObject &objVal = KEnvironment::stackPop();
	const KObject &objIdx = KEnvironment::stackPop();
	KObject &objArr = KEnvironment::stackPop();

	knint_t idx = objIdx.getNInt();
	knuint_t len = objArr.length;
	
	if (objArr.type == KObject::nullType)
	{
		KniThrowException(KEnvironment::exceptions.nullReference);
		return;
	}

	if (idx < 0 || idx + 1 > (knint_t)len)
	{
		KniThrowException(KEnvironment::exceptions.indexOutOfRange);
		return;
	}

	objArr.setElement(idx, objVal);
}

void KEnvironment::do_stind(void)
{
	const KObject &val = KEnvironment::stackPop();
	const KObject &obj = KEnvironment::stackPop();
	obj.getRef()->operator=(val);
}

	
void KEnvironment::do_pop(void)
{
	KEnvironment::stackPop();
}

void KEnvironment::do_dup(void)
{
	KEnvironment::stackPush(KEnvironment::stackPeek());
}

	
void KEnvironment::do_jmp(void)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	KEnvironment::ip += offset;
}

void KEnvironment::do_je(void)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	if (a.vRaw == b.vRaw)
		KEnvironment::ip += offset;
}

void KEnvironment::do_jne(void)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	if (a.vRaw != b.vRaw)
		KEnvironment::ip += offset;
}

void KEnvironment::do_jl(void)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	COND_JUMP(<);
}

void KEnvironment::do_jle(void)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	COND_JUMP(<=);
}

void KEnvironment::do_jg(void)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	COND_JUMP(>);
}

void KEnvironment::do_jge(void)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	COND_JUMP(>=);
}

void KEnvironment::do_jtrue(void)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &obj = KEnvironment::stackPop();
	if (obj.vBool)
		KEnvironment::ip += offset;
}

void KEnvironment::do_jfalse(void)
{
	kshort_t offset;
	BCREAD(offset, kshort_t);

	const KObject &obj = KEnvironment::stackPop();
	if (!obj.vBool)
		KEnvironment::ip += offset;
}

	
void KEnvironment::do_enter(void)
{
	ktoken32_t tok;
	kshort_t offset, offsetEnd;
	BCREAD(tok, ktoken32_t);
	BCREAD(offset, kshort_t);
	BCREAD(offsetEnd, kshort_t);

	const TypeDef *excType = KEnvironment::module->typeList[tok];

	KExceptionHandler handler = { };
	handler.excType = excType;
	handler.frame = KEnvironment::frame;
	handler.addr = KEnvironment::ip + offset;
	handler.addrEnd = KEnvironment::ip + offsetEnd;

	KEnvironment::catchStack->push(handler);
}

void KEnvironment::do_leave(void)
{
	knuint_t addrEnd = KEnvironment::catchStack->top().addrEnd;
	KEnvironment::catchStack->pop();
	KEnvironment::ip = addrEnd;
}

void KEnvironment::do_throw(void)
{
	KEnvironment::throwException();
}

	
void KEnvironment::do_calli(void)
{
	ktoken16_t tok;
	BCREAD(tok, ktoken16_t);

	const KObject &obj = KEnvironment::stackPeek();
	const MethodDef *met = obj.type->cls->iMethodList[tok];
	KEnvironment::invokeLastThis(met);
}

void KEnvironment::do_calls(void)
{
	ktoken16_t clstok, mettok;
	BCREAD(clstok, ktoken16_t);
	BCREAD(mettok, ktoken16_t);

	const MethodDef *met = KEnvironment::module->classList[clstok]->sMethodList[mettok];
	KEnvironment::invoke(met);
}

void KEnvironment::do_callo(void)
{
	const KObject &obj = KEnvironment::stackPop();

	const MethodDef *met = (MethodDef *)(obj.getField(1).getRaw());
	if (met->attrs & KMA_STATIC)
	{
		KEnvironment::invoke(met);
	}
	else
	{
		KEnvironment::stackPush(obj.getField(0));
		KEnvironment::invokeLastThis(met);
	}
}

void KEnvironment::do_ret(void)
{
	(void)0;
}

	
void KEnvironment::do_add(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	ARITH_OP(+);
}

void KEnvironment::do_sub(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	ARITH_OP(-);
}

void KEnvironment::do_mul(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	ARITH_OP(*);
}

void KEnvironment::do_div(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	ARITH_OP(/);
}

void KEnvironment::do_rem(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	switch (a.type->tag & KT_SCALAR_MASK)
	{
	case KT_CHAR:
		KEnvironment::stackPushChar(a.vChar % b.vChar);
		break;
	case KT_BYTE:
		KEnvironment::stackPushByte(a.vByte % b.vByte);
		break;
	case KT_SBYTE:
		KEnvironment::stackPushSByte(a.vSByte % b.vSByte);
		break;
	case KT_SHORT:
		KEnvironment::stackPushShort(a.vShort % b.vShort);
		break;
	case KT_USHORT:
		KEnvironment::stackPushUShort(a.vUShort % b.vUShort);
		break;
	case KT_INT:
		KEnvironment::stackPushInt(a.vInt % b.vInt);
		break;
	case KT_UINT:
		KEnvironment::stackPushUInt(a.vUInt % b.vUInt);
		break;
	case KT_ULONG:
		KEnvironment::stackPushULong(a.vULong % b.vULong);
		break;
	case KT_LONG:
		KEnvironment::stackPushLong(a.vLong % b.vLong);
		break;
	}
}

void KEnvironment::do_neg(void)
{
	const KObject &a = KEnvironment::stackPop();

	switch (a.type->tag & KT_SCALAR_MASK)
	{
	case KT_CHAR:
		KEnvironment::stackPushInt(-a.vChar);
		break;
	case KT_BYTE:
		KEnvironment::stackPushInt(-a.vByte);
		break;
	case KT_SBYTE:
		KEnvironment::stackPushInt(-a.vSByte);
		break;
	case KT_SHORT:
		KEnvironment::stackPushInt(-a.vShort);
		break;
	case KT_USHORT:
		KEnvironment::stackPushInt(-a.vUShort);
		break;
	case KT_INT:
		KEnvironment::stackPushInt(-a.vInt);
		break;
	case KT_UINT:
		KEnvironment::stackPushLong(-(klong_t)a.vUInt);
		break;
	case KT_LONG:
		KEnvironment::stackPushLong(-a.vLong);
		break;
	case KT_ULONG:
		KEnvironment::stackPush(a);
		break;
	case KT_FLOAT:
		KEnvironment::stackPushFloat(-a.vFloat);
		break;
	case KT_DOUBLE:
		KEnvironment::stackPushDouble(-a.vDouble);
		break;
	}
}

void KEnvironment::do_equ(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	KEnvironment::stackPushBool(a.vLong == b.vLong);
}

void KEnvironment::do_neq(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	KEnvironment::stackPushBool(a.vLong != b.vLong);
}

void KEnvironment::do_les(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	COMP_OP(<);
}

void KEnvironment::do_leq(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	COMP_OP(<=);
}

void KEnvironment::do_grt(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	COMP_OP(>);
}

void KEnvironment::do_geq(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	COMP_OP(>=);
}

void KEnvironment::do_cat(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	kstring_t s = krt_strcat(a.vString, a.length, b.vString, b.length);
	KEnvironment::stackPushStringMoved(s, a.length + b.length);
}

void KEnvironment::do_and(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	LOGIC_OP(&&, &);
}

void KEnvironment::do_or(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	LOGIC_OP(||, |);
}

void KEnvironment::do_xor(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	LOGIC_OP(!=, ^);
}

void KEnvironment::do_not(void)
{
	const KObject &a = KEnvironment::stackPop();

	switch (a.type->tag & KT_SCALAR_MASK)
	{
	case KT_BOOL:
		KEnvironment::stackPushBool(!a.vBool);
		break;
	case KT_CHAR:
		KEnvironment::stackPushChar(~a.vChar);
		break;
	case KT_BYTE:
		KEnvironment::stackPushByte(~a.vByte);
		break;
	case KT_SBYTE:
		KEnvironment::stackPushSByte(~a.vSByte);
		break;
	case KT_SHORT:
		KEnvironment::stackPushShort(~a.vShort);
		break;
	case KT_USHORT:
		KEnvironment::stackPushUShort(~a.vUShort);
		break;
	case KT_INT:
		KEnvironment::stackPushInt(~a.vInt);
		break;
	case KT_UINT:
		KEnvironment::stackPushUInt(~a.vUInt);
		break;
	case KT_LONG:
		KEnvironment::stackPushLong(~a.vLong);
		break;
	case KT_ULONG:
		KEnvironment::stackPushULong(~a.vULong);
		break;
	}
}

void KEnvironment::do_shl(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	kuint_t n = b.vUInt;

	switch (a.type->tag & KT_SCALAR_MASK)
	{
	case KT_CHAR:
		KEnvironment::stackPushChar(a.vChar << n);
		break;
	case KT_BYTE:
		KEnvironment::stackPushByte(a.vByte << n);
		break;
	case KT_SBYTE:
		KEnvironment::stackPushSByte(a.vSByte << n);
		break;
	case KT_SHORT:
		KEnvironment::stackPushShort(a.vShort << n);
		break;
	case KT_USHORT:
		KEnvironment::stackPushUShort(a.vUShort << n);
		break;
	case KT_INT:
		KEnvironment::stackPushInt(a.vInt << n);
		break;
	case KT_UINT:
		KEnvironment::stackPushUInt(a.vUInt << n);
		break;
	case KT_LONG:
		KEnvironment::stackPushLong(a.vLong << n);
		break;
	case KT_ULONG:
		KEnvironment::stackPushULong(a.vULong << n);
		break;
	}
}

void KEnvironment::do_shr(void)
{
	const KObject &b = KEnvironment::stackPop();
	const KObject &a = KEnvironment::stackPop();

	kuint_t n = b.vUInt;

	switch (a.type->tag & KT_SCALAR_MASK)
	{
	case KT_CHAR:
		KEnvironment::stackPushChar(a.vChar >> n);
		break;
	case KT_BYTE:
		KEnvironment::stackPushByte(a.vByte >> n);
		break;
	case KT_SBYTE:
		KEnvironment::stackPushSByte(a.vSByte >> n);
		break;
	case KT_SHORT:
		KEnvironment::stackPushShort(a.vShort >> n);
		break;
	case KT_USHORT:
		KEnvironment::stackPushUShort(a.vUShort >> n);
		break;
	case KT_INT:
		KEnvironment::stackPushInt(a.vInt >> n);
		break;
	case KT_UINT:
		KEnvironment::stackPushUInt(a.vUInt >> n);
		break;
	case KT_LONG:
		KEnvironment::stackPushLong(a.vLong >> n);
		break;
	case KT_ULONG:
		KEnvironment::stackPushULong(a.vULong >> n);
		break;
	}
}


void KEnvironment::do_istrue(void)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPushBool((kbool_t)obj.vULong);
}

void KEnvironment::do_isnull(void)
{
	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPushBool(obj.vRaw == NULL);
}

	
void KEnvironment::do_newobj(void)
{
	ktoken16_t tok;
	BCREAD(tok, ktoken16_t);

	const ClassDef *cls = KEnvironment::module->classList[tok];

	KObject obj;
	KEnvironment::allocClassInstance(cls, obj);
	KEnvironment::stackPush(obj);
	KEnvironment::invokeLastThis(cls->ctor);

	KEnvironment::stackPush(obj);
}

void KEnvironment::do_newdel(void)
{
	ktoken16_t tok;
	BCREAD(tok, ktoken16_t);

	const DelegateDef *del = KEnvironment::module->delegateList[tok];

	const KObject &objMet = KEnvironment::stackPop();
	const MethodDef *met = (MethodDef *)objMet.getRaw();

	KObject obj;
	if (met->addr & KMA_STATIC)
		KEnvironment::allocDelegateInstance(del, NULL, met, obj);
	else
		KEnvironment::allocDelegateInstance(del, &KEnvironment::stackPop(), met, obj);

	KEnvironment::stackPush(obj);
}

void KEnvironment::do_newarr(void)
{
	ktoken32_t tok;
	BCREAD(tok, ktoken32_t);

	const TypeDef *type = KEnvironment::module->typeList[tok];

	const KObject &objLength = KEnvironment::stackPop();
	kuint_t length = objLength.vUInt;

	KObject obj;
	KEnvironment::allocArray(type, length, obj);
	KEnvironment::stackPush(obj);
}

void KEnvironment::do_makearr(void)
{
	ktoken32_t tok;
	BCREAD(tok, ktoken32_t);

	const TypeDef *type = KEnvironment::module->typeList[tok];

	const KObject &objLength = KEnvironment::stackPop();
	kuint_t length = objLength.vUInt;

	KObject obj;
	KEnvironment::allocArrayBaking(type, length, obj);
	KEnvironment::stackPush(obj);
}

	
void KEnvironment::do_conv_ch(void)
{
	const KObject &obj = KEnvironment::stackPop();
	CONV_OP(kchar_t, Char);
}

void KEnvironment::do_conv_i1(void)
{
	const KObject &obj = KEnvironment::stackPop();
	CONV_OP(ksbyte_t, SByte);
}

void KEnvironment::do_conv_u1(void)
{
	const KObject &obj = KEnvironment::stackPop();
	CONV_OP(kbyte_t, Byte);
}

void KEnvironment::do_conv_i2(void)
{
	const KObject &obj = KEnvironment::stackPop();
	CONV_OP(kshort_t, Short);
}

void KEnvironment::do_conv_u2(void)
{
	const KObject &obj = KEnvironment::stackPop();
	CONV_OP(kushort_t, UShort);
}

void KEnvironment::do_conv_i4(void)
{
	const KObject &obj = KEnvironment::stackPop();
	CONV_OP(kint_t, Int);
}

void KEnvironment::do_conv_u4(void)
{
	const KObject &obj = KEnvironment::stackPop();
	CONV_OP(kuint_t, UInt);
}

void KEnvironment::do_conv_i8(void)
{
	const KObject &obj = KEnvironment::stackPop();
	CONV_OP(klong_t, Long);
}

void KEnvironment::do_conv_u8(void)
{
	const KObject &obj = KEnvironment::stackPop();
	CONV_OP(kulong_t, ULong);
}

void KEnvironment::do_conv_r4(void)
{
	const KObject &obj = KEnvironment::stackPop();
	CONV_OP(kfloat_t, Float);
}

void KEnvironment::do_conv_r8(void)
{
	const KObject &obj = KEnvironment::stackPop();
	CONV_OP(kdouble_t, Double);
}

void KEnvironment::do_conv_s(void)
{
	const KObject &obj = KEnvironment::stackPop();

	if (obj.type == GET_PRIMITIVE_TYPE(KT_STRING)
		|| obj.type == KObject::nullType)
	{
		KEnvironment::stackPush(obj);
		return;
	}

	const TypeDef *srcType = obj.type;
	CONV_S;
}

	
void KEnvironment::do_cast(void)
{
	ktoken32_t tok;
	BCREAD(tok, ktoken32_t);


	const KObject &obj = KEnvironment::stackPop();
	const TypeDef *srcType = obj.type;
	const TypeDef *destType = KEnvironment::module->typeList[tok];

	if (srcType == destType)
	{
		// identical type
		KEnvironment::stackPush(obj);
	}
	else if (srcType == KEnvironment::nullType)
	{
		// null object

		if (destType->dim
				|| destType->cls
				|| (destType->tag & KT_REF_MASK))
		{
			// null -> reference type: ok
			KEnvironment::stackPush(obj);
		}
		else
		{
			switch (destType->tag & KT_SCALAR_MASK)
			{
			case KT_BOOL:
				KEnvironment::stackPushBool(false);
				break;
			case KT_CHAR:
				KEnvironment::stackPushChar((kchar_t)0);
				break;
			case KT_BYTE:
				KEnvironment::stackPushByte((kbyte_t)0);
				break;
			case KT_SBYTE:
				KEnvironment::stackPushSByte((ksbyte_t)0);
				break;
			case KT_SHORT:
				KEnvironment::stackPushShort((kshort_t)0);
				break;
			case KT_USHORT:
				KEnvironment::stackPushUShort((kushort_t)0);
				break;
			case KT_INT:
				KEnvironment::stackPushInt((kint_t)0);
				break;
			case KT_UINT:
				KEnvironment::stackPushUInt((kuint_t)0);
				break;
			case KT_LONG:
				KEnvironment::stackPushLong((klong_t)0);
				break;
			case KT_ULONG:
				KEnvironment::stackPushULong((kulong_t)0);
				break;
			case KT_STRING:
				KEnvironment::stackPushString(NULL, 0);
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
				KEnvironment::stackPushBool(obj.vLong != 0);
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
				CONV_S;
				break;
			}
		}
		else
		{
			// -> reference type

			if (destType == KEnvironment::objectType
				|| destType == KEnvironment::rawType)
			{
				// -> object || raw
				KEnvironment::stackPush(obj);
			}
			else if (destType == KEnvironment::arrayType)
			{
				// -> array
				if (srcType->dim || (srcType->tag & KT_ARRAY))
					// src is array
					KEnvironment::stackPush(obj);
				else
					KniThrowException(KEnvironment::exceptions.invalidCast);
			}
			else
			{
				KniThrowException(KEnvironment::exceptions.invalidCast);
			}
		}
	}
}

void KEnvironment::do_isinst(void)
{
	ktoken32_t tok;
	BCREAD(tok, ktoken32_t);

	const TypeDef *type = KEnvironment::module->typeList[tok];

	const KObject &obj = KEnvironment::stackPop();
	KEnvironment::stackPushBool(obj.type == type || obj.type == KObject::nullType);
}


void KEnvironment::do_hlt(void)
{
	KEnvironment::running = false;
}

void KEnvironment::do_brk(void)
{
	KEnvironment::running = false;
}

void KEnvironment::do_trace(void)
{
	kuint_t line;
	BCREAD(line, kuint_t);

	// TODO: trace line
}

