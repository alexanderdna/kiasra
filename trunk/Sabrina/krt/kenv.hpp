#pragma once

#include "kopcodes.hpp"
#include "kmeta.hpp"
#include "kframe.hpp"
#include "kexchandler.hpp"

#include <set>
#include <deque>

#define GET_PRIMITIVE_TYPE(tag) (KEnvironment::primitiveTypes[(unsigned)tag])

class KGC;
class KObject;
struct ModuleDef;
class KTypeTree;

class KModuleLoader;

class KEnvironment
{
	friend class KGC;

public:
	struct KExceptions
	{
		const ClassDef *klass;

		const MethodDef *ctor;
		const MethodDef *fromCode;
		const MethodDef *fromMessage;

		const FieldDef *custom;
		const FieldDef *invalidCast;
		const FieldDef *invalidOperation;
		const FieldDef *invalidArgument;
		const FieldDef *nullArgument;
		const FieldDef *indexOutOfRange;
		const FieldDef *divisionByZero;
	};

public:
	static KEnvironment *instance;

protected:
	static kstring_t systemLibPath;

protected:
	typedef void (EXECUTOR)(KEnvironment *env);
	typedef std::deque<const MethodDef *> ktracedeque_t;

protected:
	static EXECUTOR *executors[256];
	static EXECUTOR *defaultExecutors[];

	KTypeTree *typeTree;

	static const TypeDef **primitiveTypes;
	static const TypeDef  *voidType;
	static const TypeDef  *arrayType;
	static const TypeDef  *objectType;
	static const TypeDef  *rawType;
	static const TypeDef  *excType;
	static const TypeDef  *nullType;	// type for null value (Singleton)

	KGC *gc;

	KModuleLoader             *rootModule;
	std::set<KModuleLoader *> *loadedModules;
	KModuleLoader             *corlibModule;

	KObject *stack;
	knuint_t stackSize;
	knuint_t stackPointer;

	kcallstack_t    *callStack;
	KFrame          *frame;		// current call frame
	ModuleDef       *module;	// current method's module
	const MethodDef *method;	// current method
	KObject         *locals;	// current local variables
	KObject         *args;		// current arguments
	kstring_t       *strings;	// current string table
	kuint_t         *stringLengths;

	const unsigned char *code;	// code to execute
	knuint_t             ip;	// pointer to next instruction

	bool running;
	bool hasException;

	KObject       *exc;			// latest unhandled exception
	kcatchstack_t *catchStack;	// stack of exception handlers
	ktracedeque_t *traceDeque;	// deque of methods traced from exception

	KExceptions    exceptions;	// bundle of common exceptions

public:
	KEnvironment(void);
	~KEnvironment(void);

	KModuleLoader * createModuleLoader(kstring_t importerPath, kstring_t path, uint32_t hash);
	kstring_t getSystemLibPath(void);

	// Meta

	static const ClassDef * findClass(kstring_t name, const ModuleDef *module);
	static const DelegateDef * findDelegate(kstring_t name, const ModuleDef *module);

	static const FieldDef * findField(const ClassDef *cls, kstring_t name);
	static const MethodDef * findMethod(const ClassDef *cls, kstring_t name);
	
	const TypeDef * createType(ktypetag_t tag, kushort_t dim);
	const TypeDef * createType(ktypetag_t tag, kushort_t dim, ClassDef *cls);
	const TypeDef * createType(ktypetag_t tag, kushort_t dim, DelegateDef *del);

	const TypeDef * makeByRefType(const TypeDef *typeDef);
	const TypeDef * makeArrayType(const TypeDef *typeDef);
	const TypeDef * makeElementType(const TypeDef *typeDef);

protected:
	static void initSystemLibPath(void);

	void initCorLib(void);
	void initExceptions(void);

	KRESULT execute(void);

	// Objects

	void allocClassInstance(const ClassDef *classDef, KObject &outObj);
	void allocDelegateInstance(const DelegateDef *delegateDef, KObject *objThis, const MethodDef *method, KObject &outObj);
	void allocArray(const TypeDef *arrayType, knuint_t length, KObject &outObj);
	void allocArrayBaking(const TypeDef *arrayType, knuint_t length, KObject &outObj);

	void initLocals(const TypeDef **types, knuint_t count);
	
	// Calls

	KRESULT invoke(const MethodDef *methodDef);
	KRESULT invokeLastThis(const MethodDef *methodDef);

	void enterMethod(const MethodDef *method, KObject *args);
	void leaveMethod(void);

	void throwException(void);
	void printException(void);

	// Stack manipulation

	void stackPush(const KObject &obj);
	void stackPushNull(void);
	void stackPushAddress(KObject *p);
	void stackPushBool(kbool_t val);
	void stackPushChar(kchar_t val);
	void stackPushByte(kbyte_t val);
	void stackPushSByte(ksbyte_t val);
	void stackPushShort(kshort_t val);
	void stackPushUShort(kushort_t val);
	void stackPushInt(kint_t val);
	void stackPushUInt(kuint_t val);
	void stackPushLong(klong_t val);
	void stackPushULong(kulong_t val);
	void stackPushFloat(kfloat_t val);
	void stackPushDouble(kdouble_t val);
	void stackPushString(kstring_t val, knuint_t length);
	void stackPushStringMoved(kstring_t val, knuint_t len);
	void stackPushRaw(void *p);
	KObject & stackPeek(void);
	KObject & stackPeek(knuint_t offset);
	KObject & stackPop(void);
	
	// definition in kenv_stack.cpp and
	// used by only the functions there
	inline void stackExpand(void);

	// definition put right here for uses in many other places
	inline void stackClear(knuint_t count)
	{
		this->stackPointer -= count;
	}


	//===================================================
	// Opcode executors

	static void do_InvalidOpcode(KEnvironment *env);

	static void do_nop(KEnvironment *env);

	static void do_ldtrue(KEnvironment *env);
	static void do_ldfalse(KEnvironment *env);
	static void do_ldc_ch(KEnvironment *env);
	static void do_ldc_i1(KEnvironment *env);
	static void do_ldc_u1(KEnvironment *env);
	static void do_ldc_i2(KEnvironment *env);
	static void do_ldc_u2(KEnvironment *env);
	static void do_ldc_i4(KEnvironment *env);
	static void do_ldc_u4(KEnvironment *env);
	static void do_ldc_i8(KEnvironment *env);
	static void do_ldc_u8(KEnvironment *env);
	static void do_ldc_r4(KEnvironment *env);
	static void do_ldc_r8(KEnvironment *env);
	static void do_ldstr(KEnvironment *env);

	static void do_ldthis(KEnvironment *env);
	static void do_ldnull(KEnvironment *env);
	static void do_ldtype(KEnvironment *env);
	static void do_ldmethod(KEnvironment *env);

	static void do_ldloc(KEnvironment *env);
	static void do_ldloca(KEnvironment *env);
	static void do_ldarg(KEnvironment *env);
	static void do_ldarga(KEnvironment *env);
	static void do_ldfld(KEnvironment *env);
	static void do_ldflda(KEnvironment *env);
	static void do_ldsfld(KEnvironment *env);
	static void do_ldsflda(KEnvironment *env);
	static void do_ldelem(KEnvironment *env);
	static void do_ldelema(KEnvironment *env);
	static void do_ldind(KEnvironment *env);

	static void do_stloc(KEnvironment *env);
	static void do_starg(KEnvironment *env);
	static void do_stfld(KEnvironment *env);
	static void do_stsfld(KEnvironment *env);
	static void do_stelem(KEnvironment *env);
	static void do_stind(KEnvironment *env);
	
	static void do_pop(KEnvironment *env);
	static void do_dup(KEnvironment *env);
	
	static void do_jmp(KEnvironment *env);
	static void do_je(KEnvironment *env);
	static void do_jne(KEnvironment *env);
	static void do_jl(KEnvironment *env);
	static void do_jle(KEnvironment *env);
	static void do_jg(KEnvironment *env);
	static void do_jge(KEnvironment *env);
	static void do_jtrue(KEnvironment *env);
	static void do_jfalse(KEnvironment *env);
	
	static void do_enter(KEnvironment *env);
	static void do_leave(KEnvironment *env);
	static void do_throw(KEnvironment *env);
	
	static void do_calli(KEnvironment *env);
	static void do_calls(KEnvironment *env);
	static void do_callo(KEnvironment *env);
	static void do_ret(KEnvironment *env);
	
	static void do_add(KEnvironment *env);
	static void do_sub(KEnvironment *env);
	static void do_mul(KEnvironment *env);
	static void do_div(KEnvironment *env);
	static void do_rem(KEnvironment *env);
	static void do_neg(KEnvironment *env);
	static void do_equ(KEnvironment *env);
	static void do_neq(KEnvironment *env);
	static void do_les(KEnvironment *env);
	static void do_leq(KEnvironment *env);
	static void do_grt(KEnvironment *env);
	static void do_geq(KEnvironment *env);
	static void do_cat(KEnvironment *env);
	static void do_and(KEnvironment *env);
	static void do_or(KEnvironment *env);
	static void do_xor(KEnvironment *env);
	static void do_not(KEnvironment *env);
	static void do_shl(KEnvironment *env);
	static void do_shr(KEnvironment *env);

	static void do_istrue(KEnvironment *env);
	static void do_isnull(KEnvironment *env);
	
	static void do_newobj(KEnvironment *env);
	static void do_newdel(KEnvironment *env);
	static void do_newarr(KEnvironment *env);
	static void do_makearr(KEnvironment *env);
	
	static void do_conv_ch(KEnvironment *env);
	static void do_conv_i1(KEnvironment *env);
	static void do_conv_u1(KEnvironment *env);
	static void do_conv_i2(KEnvironment *env);
	static void do_conv_u2(KEnvironment *env);
	static void do_conv_i4(KEnvironment *env);
	static void do_conv_u4(KEnvironment *env);
	static void do_conv_i8(KEnvironment *env);
	static void do_conv_u8(KEnvironment *env);
	static void do_conv_r4(KEnvironment *env);
	static void do_conv_r8(KEnvironment *env);
	static void do_conv_s(KEnvironment *env);
	
	static void do_cast(KEnvironment *env);
	static void do_isinst(KEnvironment *env);

	static void do_hlt(KEnvironment *env);
	static void do_brk(KEnvironment *env);
	static void do_trace(KEnvironment *env);

	//===================================================
	// KNI API functions

	friend kbool_t KniGetLocalBool(HKENV hKEnv, kushort_t index);
	friend kchar_t KniGetLocalChar(HKENV hKEnv, kushort_t index);
	friend kbyte_t KniGetLocalByte(HKENV hKEnv, kushort_t index);
	friend ksbyte_t KniGetLocalSByte(HKENV hKEnv, kushort_t index);
	friend kshort_t KniGetLocalShort(HKENV hKEnv, kushort_t index);
	friend kushort_t KniGetLocalUShort(HKENV hKEnv, kushort_t index);
	friend kint_t KniGetLocalInt(HKENV hKEnv, kushort_t index);
	friend kuint_t KniGetLocalUInt(HKENV hKEnv, kushort_t index);
	friend klong_t KniGetLocalLong(HKENV hKEnv, kushort_t index);
	friend kulong_t KniGetLocalULong(HKENV hKEnv, kushort_t index);
	friend kfloat_t KniGetLocalFloat(HKENV hKEnv, kushort_t index);
	friend kdouble_t KniGetLocalDouble(HKENV hKEnv, kushort_t index);
	friend kstring_t KniGetLocalString(HKENV hKEnv, kushort_t index);
	friend knuint_t KniGetLocalLength(HKENV hKEnv, kushort_t index);

	friend kbool_t KniGetArgBool(HKENV hKEnv, kushort_t index);
	friend kchar_t KniGetArgChar(HKENV hKEnv, kushort_t index);
	friend kbyte_t KniGetArgByte(HKENV hKEnv, kushort_t index);
	friend ksbyte_t KniGetArgSByte(HKENV hKEnv, kushort_t index);
	friend kshort_t KniGetArgShort(HKENV hKEnv, kushort_t index);
	friend kushort_t KniGetArgUShort(HKENV hKEnv, kushort_t index);
	friend kint_t KniGetArgInt(HKENV hKEnv, kushort_t index);
	friend kuint_t KniGetArgUInt(HKENV hKEnv, kushort_t index);
	friend klong_t KniGetArgLong(HKENV hKEnv, kushort_t index);
	friend kulong_t KniGetArgULong(HKENV hKEnv, kushort_t index);
	friend kfloat_t KniGetArgFloat(HKENV hKEnv, kushort_t index);
	friend kdouble_t KniGetArgDouble(HKENV hKEnv, kushort_t index);
	friend kstring_t KniGetArgString(HKENV hKEnv, kushort_t index);
	friend knuint_t KniGetArgLength(HKENV hKEnv, kushort_t index);

	friend kbool_t KniGetFieldBool(HKENV hKEnv, HKFIELD hKField);
	friend kchar_t KniGetFieldChar(HKENV hKEnv, HKFIELD hKField);
	friend kbyte_t KniGetFieldByte(HKENV hKEnv, HKFIELD hKField);
	friend ksbyte_t KniGetFieldSByte(HKENV hKEnv, HKFIELD hKField);
	friend kshort_t KniGetFieldShort(HKENV hKEnv, HKFIELD hKField);
	friend kushort_t KniGetFieldUShort(HKENV hKEnv, HKFIELD hKField);
	friend kint_t KniGetFieldInt(HKENV hKEnv, HKFIELD hKField);
	friend kuint_t KniGetFieldUInt(HKENV hKEnv, HKFIELD hKField);
	friend klong_t KniGetFieldLong(HKENV hKEnv, HKFIELD hKField);
	friend kulong_t KniGetFieldULong(HKENV hKEnv, HKFIELD hKField);
	friend kfloat_t KniGetFieldFloat(HKENV hKEnv, HKFIELD hKField);
	friend kdouble_t KniGetFieldDouble(HKENV hKEnv, HKFIELD hKField);
	friend kstring_t KniGetFieldString(HKENV hKEnv, HKFIELD hKField);
	friend knuint_t KniGetFieldLength(HKENV hKEnv, HKFIELD hKField);

	friend kbool_t KniGetStaticFieldBool(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend kchar_t KniGetStaticFieldChar(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend kbyte_t KniGetStaticFieldByte(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend ksbyte_t KniGetStaticFieldSByte(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend kshort_t KniGetStaticFieldShort(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend kushort_t KniGetStaticFieldUShort(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend kint_t KniGetStaticFieldInt(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend kuint_t KniGetStaticFieldUInt(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend klong_t KniGetStaticFieldLong(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend kulong_t KniGetStaticFieldULong(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend kfloat_t KniGetStaticFieldFloat(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend kdouble_t KniGetStaticFieldDouble(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend kstring_t KniGetStaticFieldString(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend knuint_t KniGetStaticFieldLength(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);

	friend kbool_t KniGetElementBool(HKENV hKEnv, knuint_t index);
	friend kchar_t KniGetElementChar(HKENV hKEnv, knuint_t index);
	friend kbyte_t KniGetElementByte(HKENV hKEnv, knuint_t index);
	friend ksbyte_t KniGetElementSByte(HKENV hKEnv, knuint_t index);
	friend kshort_t KniGetElementShort(HKENV hKEnv, knuint_t index);
	friend kushort_t KniGetElementUShort(HKENV hKEnv, knuint_t index);
	friend kint_t KniGetElementInt(HKENV hKEnv, knuint_t index);
	friend kuint_t KniGetElementUInt(HKENV hKEnv, knuint_t index);
	friend klong_t KniGetElementLong(HKENV hKEnv, knuint_t index);
	friend kulong_t KniGetElementULong(HKENV hKEnv, knuint_t index);
	friend kfloat_t KniGetElementFloat(HKENV hKEnv, knuint_t index);
	friend kdouble_t KniGetElementDouble(HKENV hKEnv, knuint_t index);
	friend kstring_t KniGetElementString(HKENV hKEnv, knuint_t index);
	friend knuint_t KniGetElementLength(HKENV hKEnv, knuint_t index);

	friend kbool_t KniGetIndirectBool(HKENV hKEnv);
	friend kchar_t KniGetIndirectChar(HKENV hKEnv);
	friend kbyte_t KniGetIndirectByte(HKENV hKEnv);
	friend ksbyte_t KniGetIndirectSByte(HKENV hKEnv);
	friend kshort_t KniGetIndirectShort(HKENV hKEnv);
	friend kushort_t KniGetIndirectUShort(HKENV hKEnv);
	friend kint_t KniGetIndirectInt(HKENV hKEnv);
	friend kuint_t KniGetIndirectUInt(HKENV hKEnv);
	friend klong_t KniGetIndirectLong(HKENV hKEnv);
	friend kulong_t KniGetIndirectULong(HKENV hKEnv);
	friend kfloat_t KniGetIndirectFloat(HKENV hKEnv);
	friend kdouble_t KniGetIndirectDouble(HKENV hKEnv);
	friend kstring_t KniGetIndirectString(HKENV hKEnv);
	friend knuint_t KniGetIndirectLength(HKENV hKEnv);

	friend void KniSetLocalBool(HKENV hKEnv, kushort_t index, kbool_t val);
	friend void KniSetLocalChar(HKENV hKEnv, kushort_t index, kchar_t val);
	friend void KniSetLocalByte(HKENV hKEnv, kushort_t index, kbyte_t val);
	friend void KniSetLocalSByte(HKENV hKEnv, kushort_t index, ksbyte_t val);
	friend void KniSetLocalShort(HKENV hKEnv, kushort_t index, kshort_t val);
	friend void KniSetLocalUShort(HKENV hKEnv, kushort_t index, kushort_t val);
	friend void KniSetLocalInt(HKENV hKEnv, kushort_t index, kint_t val);
	friend void KniSetLocalUInt(HKENV hKEnv, kushort_t index, kuint_t val);
	friend void KniSetLocalLong(HKENV hKEnv, kushort_t index, klong_t val);
	friend void KniSetLocalULong(HKENV hKEnv, kushort_t index, kulong_t val);
	friend void KniSetLocalFloat(HKENV hKEnv, kushort_t index, kfloat_t val);
	friend void KniSetLocalDouble(HKENV hKEnv, kushort_t index, kdouble_t val);
	friend void KniSetLocalString(HKENV hKEnv, kushort_t index, kstring_t val, knuint_t length);

	friend void KniSetArgBool(HKENV hKEnv, kushort_t index, kbool_t val);
	friend void KniSetArgChar(HKENV hKEnv, kushort_t index, kchar_t val);
	friend void KniSetArgByte(HKENV hKEnv, kushort_t index, kbyte_t val);
	friend void KniSetArgSByte(HKENV hKEnv, kushort_t index, ksbyte_t val);
	friend void KniSetArgShort(HKENV hKEnv, kushort_t index, kshort_t val);
	friend void KniSetArgUShort(HKENV hKEnv, kushort_t index, kushort_t val);
	friend void KniSetArgInt(HKENV hKEnv, kushort_t index, kint_t val);
	friend void KniSetArgUInt(HKENV hKEnv, kushort_t index, kuint_t val);
	friend void KniSetArgLong(HKENV hKEnv, kushort_t index, klong_t val);
	friend void KniSetArgULong(HKENV hKEnv, kushort_t index, kulong_t val);
	friend void KniSetArgFloat(HKENV hKEnv, kushort_t index, kfloat_t val);
	friend void KniSetArgDouble(HKENV hKEnv, kushort_t index, kdouble_t val);
	friend void KniSetArgString(HKENV hKEnv, kushort_t index, kstring_t val, knuint_t length);

	friend void KniSetFieldBool(HKENV hKEnv, HKFIELD hKField, kbool_t val);
	friend void KniSetFieldChar(HKENV hKEnv, HKFIELD hKField, kchar_t val);
	friend void KniSetFieldByte(HKENV hKEnv, HKFIELD hKField, kbyte_t val);
	friend void KniSetFieldSByte(HKENV hKEnv, HKFIELD hKField, ksbyte_t val);
	friend void KniSetFieldShort(HKENV hKEnv, HKFIELD hKField, kshort_t val);
	friend void KniSetFieldUShort(HKENV hKEnv, HKFIELD hKField, kushort_t val);
	friend void KniSetFieldInt(HKENV hKEnv, HKFIELD hKField, kint_t val);
	friend void KniSetFieldUInt(HKENV hKEnv, HKFIELD hKField, kuint_t val);
	friend void KniSetFieldLong(HKENV hKEnv, HKFIELD hKField, klong_t val);
	friend void KniSetFieldULong(HKENV hKEnv, HKFIELD hKField, kulong_t val);
	friend void KniSetFieldFloat(HKENV hKEnv, HKFIELD hKField, kfloat_t val);
	friend void KniSetFieldDouble(HKENV hKEnv, HKFIELD hKField, kdouble_t val);
	friend void KniSetFieldString(HKENV hKEnv, HKFIELD hKField, kstring_t val, knuint_t length);

	friend void KniSetStaticFieldBool(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kbool_t val);
	friend void KniSetStaticFieldChar(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kchar_t val);
	friend void KniSetStaticFieldByte(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kbyte_t val);
	friend void KniSetStaticFieldSByte(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, ksbyte_t val);
	friend void KniSetStaticFieldShort(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kshort_t val);
	friend void KniSetStaticFieldUShort(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kushort_t val);
	friend void KniSetStaticFieldInt(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kint_t val);
	friend void KniSetStaticFieldUInt(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kuint_t val);
	friend void KniSetStaticFieldLong(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, klong_t val);
	friend void KniSetStaticFieldULong(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kulong_t val);
	friend void KniSetStaticFieldFloat(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kfloat_t val);
	friend void KniSetStaticFieldDouble(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kdouble_t val);
	friend void KniSetStaticFieldString(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, kstring_t val, knuint_t length);

	friend void KniSetElementBool(HKENV hKEnv, knuint_t index, kbool_t val);
	friend void KniSetElementChar(HKENV hKEnv, knuint_t index, kchar_t val);
	friend void KniSetElementByte(HKENV hKEnv, knuint_t index, kbyte_t val);
	friend void KniSetElementSByte(HKENV hKEnv, knuint_t index, ksbyte_t val);
	friend void KniSetElementShort(HKENV hKEnv, knuint_t index, kshort_t val);
	friend void KniSetElementUShort(HKENV hKEnv, knuint_t index, kushort_t val);
	friend void KniSetElementInt(HKENV hKEnv, knuint_t index, kint_t val);
	friend void KniSetElementUInt(HKENV hKEnv, knuint_t index, kuint_t val);
	friend void KniSetElementLong(HKENV hKEnv, knuint_t index, klong_t val);
	friend void KniSetElementULong(HKENV hKEnv, knuint_t index, kulong_t val);
	friend void KniSetElementFloat(HKENV hKEnv, knuint_t index, kfloat_t val);
	friend void KniSetElementDouble(HKENV hKEnv, knuint_t index, kdouble_t val);
	friend void KniSetElementString(HKENV hKEnv, knuint_t index, kstring_t val, knuint_t length);

	friend void KniSetIndirectBool(HKENV hKEnv, kbool_t val);
	friend void KniSetIndirectChar(HKENV hKEnv, kchar_t val);
	friend void KniSetIndirectByte(HKENV hKEnv, kbyte_t val);
	friend void KniSetIndirectSByte(HKENV hKEnv, ksbyte_t val);
	friend void KniSetIndirectShort(HKENV hKEnv, kshort_t val);
	friend void KniSetIndirectUShort(HKENV hKEnv, kushort_t val);
	friend void KniSetIndirectInt(HKENV hKEnv, kint_t val);
	friend void KniSetIndirectUInt(HKENV hKEnv, kuint_t val);
	friend void KniSetIndirectLong(HKENV hKEnv, klong_t val);
	friend void KniSetIndirectULong(HKENV hKEnv, kulong_t val);
	friend void KniSetIndirectFloat(HKENV hKEnv, kfloat_t val);
	friend void KniSetIndirectDouble(HKENV hKEnv, kdouble_t val);
	friend void KniSetIndirectString(HKENV hKEnv, kstring_t val, knuint_t length);

	friend void KniLoadBool(HKENV hKEnv, kbool_t val);
	friend void KniLoadChar(HKENV hKEnv, kchar_t val);
	friend void KniLoadByte(HKENV hKEnv, kbyte_t val);
	friend void KniLoadSByte(HKENV hKEnv, ksbyte_t val);
	friend void KniLoadShort(HKENV hKEnv, kshort_t val);
	friend void KniLoadUShort(HKENV hKEnv, kushort_t val);
	friend void KniLoadInt(HKENV hKEnv, kint_t val);
	friend void KniLoadUInt(HKENV hKEnv, kuint_t val);
	friend void KniLoadLong(HKENV hKEnv, klong_t val);
	friend void KniLoadULong(HKENV hKEnv, kulong_t val);
	friend void KniLoadFloat(HKENV hKEnv, kfloat_t val);
	friend void KniLoadDouble(HKENV hKEnv, kdouble_t val);
	friend void KniLoadString(HKENV hKEnv, kstring_t val, knuint_t length);

	friend void KniLoadLocal(HKENV hKEnv, kushort_t index);
	friend void KniLoadArg(HKENV hKEnv, kushort_t index);
	friend void KniLoadArgByRef(HKENV hKEnv, kushort_t index);
	friend void KniLoadField(HKENV hKEnv, HKFIELD hKField);
	friend void KniLoadStaticField(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend void KniLoadElement(HKENV hKEnv, knuint_t index);
	friend void KniLoadIndirect(HKENV hKEnv);

	friend void KniLoadLocalLength(HKENV hKEnv, kushort_t index);
	friend void KniLoadArgLength(HKENV hKEnv, kushort_t index);
	friend void KniLoadArgLengthByRef(HKENV hKEnv, kushort_t index);
	friend void KniLoadFieldLength(HKENV hKEnv, HKFIELD hKField);
	friend void KniLoadStaticFieldLength(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend void KniLoadElementLength(HKENV hKEnv, knuint_t index);
	friend void KniLoadIndirectLength(HKENV hKEnv);

	friend void KniLoadLocalElement(HKENV hKEnv, kushort_t index, knuint_t elemIndex);
	friend void KniLoadArgElement(HKENV hKEnv, kushort_t index, knuint_t elemIndex);
	friend void KniLoadArgElementByRef(HKENV hKEnv, kushort_t index, knuint_t elemIndex);
	friend void KniLoadFieldElement(HKENV hKEnv, HKFIELD hKField, knuint_t elemIndex);
	friend void KniLoadStaticFieldElement(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField, knuint_t elemIndex);
	friend void KniLoadElementElement(HKENV hKEnv, knuint_t index, knuint_t elemIndex);
	friend void KniLoadIndirectElement(HKENV hKEnv, knuint_t elemIndex);

	friend void KniLoadLocalAddress(HKENV hKEnv, kushort_t index);
	friend void KniLoadArgAddress(HKENV hKEnv, kushort_t index);
	friend void KniLoadFieldAddress(HKENV hKEnv, HKFIELD hKField);
	friend void KniLoadStaticFieldAddress(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend void KniLoadElementAddress(HKENV hKEnv, knuint_t index);

	friend void KniLoadNull(HKENV hKEnv);

	friend void KniLoadNewObject(HKENV hKEnv, HKCLASS hKClass);
	friend void KniLoadNewDelegate(HKENV HKEnv, HKDELEGATE hKDelegate, HKMETHOD hKMethod);
	friend void KniLoadNewDelegateNative(HKENV HKEnv, HKDELEGATE hKDelegate, NFUNC pNFunc, kbool_t isStatic);
	friend void KniLoadNewArray(HKENV hKEnv, HKTYPE hKType, knuint_t length);
	friend void KniLoadNewArrayBaking(HKENV hKEnv, HKTYPE hKType, knuint_t length);

	friend void KniStoreLocal(HKENV hKEnv, kushort_t index);
	friend void KniStoreArg(HKENV hKEnv, kushort_t index);
	friend void KniStoreArgByRef(HKENV hKEnv, kushort_t index);
	friend void KniStoreField(HKENV hKEnv, HKFIELD hKField);
	friend void KniStoreStaticField(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend void KniStoreElement(HKENV hKEnv, knuint_t index);
	friend void KniStoreIndirect(HKENV HKENV);

	friend kbool_t KniIsNull(HKENV hKEnv);
	friend kbool_t KniIsNullLocal(HKENV hKEnv, kushort_t index);
	friend kbool_t KniIsNullArg(HKENV hKEnv, kushort_t index);
	friend kbool_t KniIsNullArgByRef(HKENV hKEnv, kushort_t index);
	friend kbool_t KniIsNullField(HKENV hKEnv, HKFIELD hKField);
	friend kbool_t KniIsNullStaticField(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend kbool_t KniIsNullElement(HKENV hKEnv, knuint_t index);
	friend kbool_t KniIsNullIndirect(HKENV hKEnv);

	friend kbool_t KniIsReferenceEqual(HKENV hKEnv);

	friend kbool_t KniIsClass(HKENV hKEnv);
	friend kbool_t KniIsDelegate(HKENV hKEnv);
	friend kbool_t KniIsArray(HKENV hKEnv);

	friend kbool_t KniIsInstanceOfClass(HKENV hKEnv, HKCLASS hKClass);
	friend kbool_t KniIsInstanceOfDelegate(HKENV hKEnv, HKDELEGATE hKDelegate);

	friend KRESULT KniInvoke(HKENV hKEnv, HKMETHOD hKMethod);
	friend KRESULT KniInvokeStatic(HKENV hKEnv, HKCLASS hKClass, HKMETHOD hKMethod);
	friend KRESULT KniInvokeObject(HKENV hKEnv);

	friend kbool_t KniHasException(HKENV hKEnv);
	friend void KniPrintExceptionDescription(HKENV hKEnv);
	friend void KniClearException(HKENV hKEnv);
	friend void KniThrowException(HKENV hKEnv, HKFIELD hKFCode);
	friend void KniThrowExceptionEx(HKENV hKEnv, kstring_t message, knuint_t length);

	friend void KniInitLocals(HKENV hKEnv, HKTYPE *pHKTypes, kushort_t count);

	friend HKTYPE KniGetType(HKENV hKEnv);

	friend HKTYPE KniGetTypeLocal(HKENV hKEnv, kushort_t index);
	friend HKTYPE KniGetTypeArg(HKENV hKEnv, kushort_t index);
	friend HKTYPE KniGetTypeArgByRef(HKENV hKEnv, kushort_t index);
	friend HKTYPE KniGetTypeField(HKENV hKEnv, HKFIELD hKField);
	friend HKTYPE KniGetTypeStaticField(HKENV hKEnv, HKCLASS hKClass, HKFIELD hKField);
	friend HKTYPE KniGetTypeElement(HKENV hKEnv, knuint_t index);
	friend HKTYPE KniGetTypeIndirect(HKENV hKEnv);

	friend HKCLASS KniGetClass(HKENV hKEnv, kstring_t ksName);
	friend HKDELEGATE KniGetDelegate(HKENV hKEnv, kstring_t ksName);
	friend HKFIELD KniGetField(HKENV HKEnv, HKCLASS hKClass, kstring_t ksName);
	friend HKMETHOD KniGetMethod(HKENV hKEnv, HKCLASS hKClass, kstring_t ksName);

	friend HKTYPE KniGetPrimitiveType(HKENV hKEnv, ktypetag_t tag);

	friend HKTYPE KniCreateType(HKENV hKEnv, ktypetag_t tag, kushort_t dim, HKUSERTYPE hKClassOrDelegate);

	friend kbool_t KniIsClassType(HKENV hKEnv, HKTYPE hKType);
	friend kbool_t KniIsDelegateType(HKENV hKEnv, HKTYPE hKType);
	friend kbool_t KniIsArrayType(HKENV hKEnv, HKTYPE hKType);
};
