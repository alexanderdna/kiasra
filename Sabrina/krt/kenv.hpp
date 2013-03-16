#pragma once

#include "kopcodes.h"
#include "kmeta.hpp"
#include "kframe.hpp"
#include "kexchandler.hpp"

#include <set>
#include <deque>

#define GET_PRIMITIVE_TYPE(tag) (KEnvironment::primitiveTypes[(unsigned)tag])

class KGC;
class KObject;
struct ModuleDef;
class TypeTree;

class ModuleLoader;

class KEnvironment
{
	friend class KGC;

public:
	struct Exceptions
	{
		const ClassDef *general;

		const ClassDef *invalidCast;
		const ClassDef *invalidOperation;
		const ClassDef *invalidArgument;
		const ClassDef *nullArgument;
		const ClassDef *nullReference;
		const ClassDef *indexOutOfRange;
		const ClassDef *divisionByZero;

		const ClassDef *invalidOpCode;
	};

protected:
	typedef void (EXECUTOR)(void);
	typedef std::deque<const MethodDef *> tracedeque_t;

protected:
	static bool isInitialized;
	static bool isForExecution;

	static kstring_t systemLibPath;

	static EXECUTOR *executors[256];
	static EXECUTOR *defaultExecutors[];

	static TypeTree *typeTree;

	static const TypeDef **primitiveTypes;
	static const TypeDef  *voidType;
	static const TypeDef  *arrayType;
	static const TypeDef  *objectType;
	static const TypeDef  *rawType;
	static const TypeDef  *excType;
	static const TypeDef  *nullType;	// type for null value (Singleton)

	static KGC *gc;

	static ModuleLoader             *rootModule;
	static std::set<ModuleLoader *> *loadedModules;
	static ModuleLoader             *corlibModule;

	static KObject *stack;
	static knuint_t stackSize;
	static knuint_t stackPointer;

	static callstack_t     *callStack;
	static CallFrame       *frame;		// current call frame
	static ModuleDef       *module;	// current method's module
	static const MethodDef *method;	// current method
	static KObject         *locals;	// current local variables
	static KObject         *args;		// current arguments
	static kstring_t       *strings;	// current string table
	static kuint_t         *stringLengths;

	static const unsigned char *code;	// code to execute
	static knuint_t             ip;	// pointer to next instruction

	static bool running;
	static bool hasException;

	static KObject      *exc;			// latest unhandled exception
	static catchstack_t *catchStack;	// stack of exception handlers
	static tracedeque_t *traceDeque;	// deque of methods traced from exception

	static Exceptions    exceptions;	// bundle of common exceptions

private:
	KEnvironment(void) { }
	~KEnvironment(void) { }

public:
	static void initialize(bool isForExecution);
	static void finalize(void);

	static ModuleLoader * createModuleLoader(KMODULEATTRIBUTES attrs, kstring_t importerPath, kstring_t filename);

	static kstring_t getSystemLibPath(void);

	// Meta

	static const ClassDef * findClass(kstring_t name, const ModuleDef *module);
	static const DelegateDef * findDelegate(kstring_t name, const ModuleDef *module);

	static const FieldDef * findField(const ClassDef *cls, kstring_t name);
	static const MethodDef * findMethod(const ClassDef *cls, kstring_t name);
	
	static const TypeDef * createType(KTYPETAG tag, kushort_t dim);
	static const TypeDef * createType(KTYPETAG tag, kushort_t dim, const ClassDef *cls);
	static const TypeDef * createType(KTYPETAG tag, kushort_t dim, const DelegateDef *del);

	static const TypeDef * makeByRefType(const TypeDef *typeDef);
	static const TypeDef * makeByValType(const TypeDef *typeDef);
	static const TypeDef * makeArrayType(const TypeDef *typeDef);
	static const TypeDef * makeElementType(const TypeDef *typeDef);

protected:
	static void initSystemLibPath(void);

	static void initCorLib(void);
	static void initExceptions(void);

	static KRESULT execute(void);

	// Objects

	static void allocClassInstance(const ClassDef *classDef, KObject &outObj);
	static void allocDelegateInstance(const DelegateDef *delegateDef, KObject *objThis, const MethodDef *method, KObject &outObj);
	static void allocArray(const TypeDef *arrayType, kuint_t length, KObject &outObj);
	static void allocArrayBaking(const TypeDef *arrayType, kuint_t length, KObject &outObj);

	static void initLocals(const TypeDef **types, kuint_t count);
	
	// Calls

	static KRESULT invoke(const MethodDef *methodDef);
	static KRESULT invokeLastThis(const MethodDef *methodDef);

	static void enterMethod(const MethodDef *method, KObject *args);
	static void leaveMethod(void);

	static void throwException(void);
	static void printException(void);

	// Stack manipulation

	static void stackPush(const KObject &obj);
	static void stackPushNull(void);
	static void stackPushAddress(KObject *p);
	static void stackPushBool(kbool_t val);
	static void stackPushChar(kchar_t val);
	static void stackPushByte(kbyte_t val);
	static void stackPushSByte(ksbyte_t val);
	static void stackPushShort(kshort_t val);
	static void stackPushUShort(kushort_t val);
	static void stackPushInt(kint_t val);
	static void stackPushUInt(kuint_t val);
	static void stackPushLong(klong_t val);
	static void stackPushULong(kulong_t val);
	static void stackPushFloat(kfloat_t val);
	static void stackPushDouble(kdouble_t val);
	static void stackPushString(kstring_t val, kuint_t length);
	static void stackPushStringMoved(kstring_t val, kuint_t len);
	static void stackPushRaw(kref_t val);
	static KObject & stackPeek(void);
	static KObject & stackPeek(knuint_t offset);
	static KObject & stackPop(void);
	
	// definition in kenv_stack.cpp and
	// used by only the functions there
	inline static void stackExpand(void);

	// definition put right here for uses in many other places
	inline static void stackClear(knuint_t count)
	{
		KEnvironment::stackPointer -= count;
	}


	//===================================================
	// Opcode executors

	static void do_InvalidOpcode(void);

	static void do_nop(void);

	static void do_ldtrue(void);
	static void do_ldfalse(void);
	static void do_ldc_ch(void);
	static void do_ldc_i1(void);
	static void do_ldc_u1(void);
	static void do_ldc_i2(void);
	static void do_ldc_u2(void);
	static void do_ldc_i4(void);
	static void do_ldc_u4(void);
	static void do_ldc_i8(void);
	static void do_ldc_u8(void);
	static void do_ldc_r4(void);
	static void do_ldc_r8(void);
	static void do_ldstr(void);
	static void do_ldlen(void);

	static void do_ldthis(void);
	static void do_ldnull(void);
	static void do_ldtype(void);
	static void do_ldmethod(void);

	static void do_ldloc(void);
	static void do_ldloca(void);
	static void do_ldarg(void);
	static void do_ldarga(void);
	static void do_ldfld(void);
	static void do_ldflda(void);
	static void do_ldsfld(void);
	static void do_ldsflda(void);
	static void do_ldelem(void);
	static void do_ldelema(void);
	static void do_ldind(void);

	static void do_stloc(void);
	static void do_starg(void);
	static void do_stfld(void);
	static void do_stsfld(void);
	static void do_stelem(void);
	static void do_stind(void);
	
	static void do_pop(void);
	static void do_dup(void);
	
	static void do_jmp(void);
	static void do_je(void);
	static void do_jne(void);
	static void do_jl(void);
	static void do_jle(void);
	static void do_jg(void);
	static void do_jge(void);
	static void do_jtrue(void);
	static void do_jfalse(void);
	
	static void do_enter(void);
	static void do_leave(void);
	static void do_throw(void);
	
	static void do_calli(void);
	static void do_calls(void);
	static void do_callo(void);
	static void do_ret(void);
	
	static void do_add(void);
	static void do_sub(void);
	static void do_mul(void);
	static void do_div(void);
	static void do_rem(void);
	static void do_neg(void);
	static void do_equ(void);
	static void do_neq(void);
	static void do_les(void);
	static void do_leq(void);
	static void do_grt(void);
	static void do_geq(void);
	static void do_cat(void);
	static void do_and(void);
	static void do_or(void);
	static void do_xor(void);
	static void do_not(void);
	static void do_shl(void);
	static void do_shr(void);

	static void do_istrue(void);
	static void do_isnull(void);
	
	static void do_newobj(void);
	static void do_newdel(void);
	static void do_newarr(void);
	static void do_makearr(void);
	
	static void do_conv_ch(void);
	static void do_conv_i1(void);
	static void do_conv_u1(void);
	static void do_conv_i2(void);
	static void do_conv_u2(void);
	static void do_conv_i4(void);
	static void do_conv_u4(void);
	static void do_conv_i8(void);
	static void do_conv_u8(void);
	static void do_conv_r4(void);
	static void do_conv_r8(void);
	static void do_conv_s(void);
	
	static void do_cast(void);
	static void do_isinst(void);

	static void do_hlt(void);
	static void do_brk(void);
	static void do_trace(void);

	//===================================================
	// KNI API functions
	
	friend kbool_t KniGetLocalBool(kushort_t index);
	friend kchar_t KniGetLocalChar(kushort_t index);
	friend kbyte_t KniGetLocalByte(kushort_t index);
	friend ksbyte_t KniGetLocalSByte(kushort_t index);
	friend kshort_t KniGetLocalShort(kushort_t index);
	friend kushort_t KniGetLocalUShort(kushort_t index);
	friend kint_t KniGetLocalInt(kushort_t index);
	friend kuint_t KniGetLocalUInt(kushort_t index);
	friend klong_t KniGetLocalLong(kushort_t index);
	friend kulong_t KniGetLocalULong(kushort_t index);
	friend kfloat_t KniGetLocalFloat(kushort_t index);
	friend kdouble_t KniGetLocalDouble(kushort_t index);
	friend kstring_t KniGetLocalString(kushort_t index);
	friend kref_t KniGetLocalRaw(kushort_t index);
	friend knuint_t KniGetLocalLength(kushort_t index);

	friend kbool_t KniGetArgBool(kushort_t index);
	friend kchar_t KniGetArgChar(kushort_t index);
	friend kbyte_t KniGetArgByte(kushort_t index);
	friend ksbyte_t KniGetArgSByte(kushort_t index);
	friend kshort_t KniGetArgShort(kushort_t index);
	friend kushort_t KniGetArgUShort(kushort_t index);
	friend kint_t KniGetArgInt(kushort_t index);
	friend kuint_t KniGetArgUInt(kushort_t index);
	friend klong_t KniGetArgLong(kushort_t index);
	friend kulong_t KniGetArgULong(kushort_t index);
	friend kfloat_t KniGetArgFloat(kushort_t index);
	friend kdouble_t KniGetArgDouble(kushort_t index);
	friend kstring_t KniGetArgString(kushort_t index);
	friend kref_t KniGetArgRaw(kushort_t index);
	friend knuint_t KniGetArgLength(kushort_t index);

	friend kbool_t KniGetFieldBool(HKFIELD hKField);
	friend kchar_t KniGetFieldChar(HKFIELD hKField);
	friend kbyte_t KniGetFieldByte(HKFIELD hKField);
	friend ksbyte_t KniGetFieldSByte(HKFIELD hKField);
	friend kshort_t KniGetFieldShort(HKFIELD hKField);
	friend kushort_t KniGetFieldUShort(HKFIELD hKField);
	friend kint_t KniGetFieldInt(HKFIELD hKField);
	friend kuint_t KniGetFieldUInt(HKFIELD hKField);
	friend klong_t KniGetFieldLong(HKFIELD hKField);
	friend kulong_t KniGetFieldULong(HKFIELD hKField);
	friend kfloat_t KniGetFieldFloat(HKFIELD hKField);
	friend kdouble_t KniGetFieldDouble(HKFIELD hKField);
	friend kstring_t KniGetFieldString(HKFIELD hKField);
	friend kref_t KniGetFieldRaw(HKFIELD hKField);
	friend knuint_t KniGetFieldLength(HKFIELD hKField);

	friend kbool_t KniGetStaticFieldBool(HKCLASS hKClass, HKFIELD hKField);
	friend kchar_t KniGetStaticFieldChar(HKCLASS hKClass, HKFIELD hKField);
	friend kbyte_t KniGetStaticFieldByte(HKCLASS hKClass, HKFIELD hKField);
	friend ksbyte_t KniGetStaticFieldSByte(HKCLASS hKClass, HKFIELD hKField);
	friend kshort_t KniGetStaticFieldShort(HKCLASS hKClass, HKFIELD hKField);
	friend kushort_t KniGetStaticFieldUShort(HKCLASS hKClass, HKFIELD hKField);
	friend kint_t KniGetStaticFieldInt(HKCLASS hKClass, HKFIELD hKField);
	friend kuint_t KniGetStaticFieldUInt(HKCLASS hKClass, HKFIELD hKField);
	friend klong_t KniGetStaticFieldLong(HKCLASS hKClass, HKFIELD hKField);
	friend kulong_t KniGetStaticFieldULong(HKCLASS hKClass, HKFIELD hKField);
	friend kfloat_t KniGetStaticFieldFloat(HKCLASS hKClass, HKFIELD hKField);
	friend kdouble_t KniGetStaticFieldDouble(HKCLASS hKClass, HKFIELD hKField);
	friend kstring_t KniGetStaticFieldString(HKCLASS hKClass, HKFIELD hKField);
	friend kref_t KniGetStaticFieldRaw(HKCLASS hKClass, HKFIELD hKField);
	friend knuint_t KniGetStaticFieldLength(HKCLASS hKClass, HKFIELD hKField);

	friend kbool_t KniGetElementBool(knuint_t index);
	friend kchar_t KniGetElementChar(knuint_t index);
	friend kbyte_t KniGetElementByte(knuint_t index);
	friend ksbyte_t KniGetElementSByte(knuint_t index);
	friend kshort_t KniGetElementShort(knuint_t index);
	friend kushort_t KniGetElementUShort(knuint_t index);
	friend kint_t KniGetElementInt(knuint_t index);
	friend kuint_t KniGetElementUInt(knuint_t index);
	friend klong_t KniGetElementLong(knuint_t index);
	friend kulong_t KniGetElementULong(knuint_t index);
	friend kfloat_t KniGetElementFloat(knuint_t index);
	friend kdouble_t KniGetElementDouble(knuint_t index);
	friend kstring_t KniGetElementString(knuint_t index);
	friend kref_t KniGetElementRaw(knuint_t index);
	friend knuint_t KniGetElementLength(knuint_t index);

	friend kbool_t KniGetIndirectBool();
	friend kchar_t KniGetIndirectChar();
	friend kbyte_t KniGetIndirectByte();
	friend ksbyte_t KniGetIndirectSByte();
	friend kshort_t KniGetIndirectShort();
	friend kushort_t KniGetIndirectUShort();
	friend kint_t KniGetIndirectInt();
	friend kuint_t KniGetIndirectUInt();
	friend klong_t KniGetIndirectLong();
	friend kulong_t KniGetIndirectULong();
	friend kfloat_t KniGetIndirectFloat();
	friend kdouble_t KniGetIndirectDouble();
	friend kstring_t KniGetIndirectString();
	friend kref_t KniGetIndirectRaw();
	friend knuint_t KniGetIndirectLength();

	friend void KniSetLocalBool(kushort_t index, kbool_t val);
	friend void KniSetLocalChar(kushort_t index, kchar_t val);
	friend void KniSetLocalByte(kushort_t index, kbyte_t val);
	friend void KniSetLocalSByte(kushort_t index, ksbyte_t val);
	friend void KniSetLocalShort(kushort_t index, kshort_t val);
	friend void KniSetLocalUShort(kushort_t index, kushort_t val);
	friend void KniSetLocalInt(kushort_t index, kint_t val);
	friend void KniSetLocalUInt(kushort_t index, kuint_t val);
	friend void KniSetLocalLong(kushort_t index, klong_t val);
	friend void KniSetLocalULong(kushort_t index, kulong_t val);
	friend void KniSetLocalFloat(kushort_t index, kfloat_t val);
	friend void KniSetLocalDouble(kushort_t index, kdouble_t val);
	friend void KniSetLocalString(kushort_t index, kstring_t val, kuint_t length);
	friend void KniSetLocalRaw(kushort_t index, kref_t val);
	friend void KniSetLocalNull(kushort_t index);

	friend void KniSetArgBool(kushort_t index, kbool_t val);
	friend void KniSetArgChar(kushort_t index, kchar_t val);
	friend void KniSetArgByte(kushort_t index, kbyte_t val);
	friend void KniSetArgSByte(kushort_t index, ksbyte_t val);
	friend void KniSetArgShort(kushort_t index, kshort_t val);
	friend void KniSetArgUShort(kushort_t index, kushort_t val);
	friend void KniSetArgInt(kushort_t index, kint_t val);
	friend void KniSetArgUInt(kushort_t index, kuint_t val);
	friend void KniSetArgLong(kushort_t index, klong_t val);
	friend void KniSetArgULong(kushort_t index, kulong_t val);
	friend void KniSetArgFloat(kushort_t index, kfloat_t val);
	friend void KniSetArgDouble(kushort_t index, kdouble_t val);
	friend void KniSetArgString(kushort_t index, kstring_t val, kuint_t length);
	friend void KniSetArgRaw(kushort_t index, kref_t val);
	friend void KniSetArgNull(kushort_t index);

	friend void KniSetFieldBool(HKFIELD hKField, kbool_t val);
	friend void KniSetFieldChar(HKFIELD hKField, kchar_t val);
	friend void KniSetFieldByte(HKFIELD hKField, kbyte_t val);
	friend void KniSetFieldSByte(HKFIELD hKField, ksbyte_t val);
	friend void KniSetFieldShort(HKFIELD hKField, kshort_t val);
	friend void KniSetFieldUShort(HKFIELD hKField, kushort_t val);
	friend void KniSetFieldInt(HKFIELD hKField, kint_t val);
	friend void KniSetFieldUInt(HKFIELD hKField, kuint_t val);
	friend void KniSetFieldLong(HKFIELD hKField, klong_t val);
	friend void KniSetFieldULong(HKFIELD hKField, kulong_t val);
	friend void KniSetFieldFloat(HKFIELD hKField, kfloat_t val);
	friend void KniSetFieldDouble(HKFIELD hKField, kdouble_t val);
	friend void KniSetFieldString(HKFIELD hKField, kstring_t val, kuint_t length);
	friend void KniSetFieldRaw(HKFIELD hKField, kref_t val);
	friend void KniSetFieldNull(HKFIELD hKField);

	friend void KniSetStaticFieldBool(HKCLASS hKClass, HKFIELD hKField, kbool_t val);
	friend void KniSetStaticFieldChar(HKCLASS hKClass, HKFIELD hKField, kchar_t val);
	friend void KniSetStaticFieldByte(HKCLASS hKClass, HKFIELD hKField, kbyte_t val);
	friend void KniSetStaticFieldSByte(HKCLASS hKClass, HKFIELD hKField, ksbyte_t val);
	friend void KniSetStaticFieldShort(HKCLASS hKClass, HKFIELD hKField, kshort_t val);
	friend void KniSetStaticFieldUShort(HKCLASS hKClass, HKFIELD hKField, kushort_t val);
	friend void KniSetStaticFieldInt(HKCLASS hKClass, HKFIELD hKField, kint_t val);
	friend void KniSetStaticFieldUInt(HKCLASS hKClass, HKFIELD hKField, kuint_t val);
	friend void KniSetStaticFieldLong(HKCLASS hKClass, HKFIELD hKField, klong_t val);
	friend void KniSetStaticFieldULong(HKCLASS hKClass, HKFIELD hKField, kulong_t val);
	friend void KniSetStaticFieldFloat(HKCLASS hKClass, HKFIELD hKField, kfloat_t val);
	friend void KniSetStaticFieldDouble(HKCLASS hKClass, HKFIELD hKField, kdouble_t val);
	friend void KniSetStaticFieldString(HKCLASS hKClass, HKFIELD hKField, kstring_t val, kuint_t length);
	friend void KniSetStaticFieldRaw(HKCLASS hKClass, HKFIELD hKField, kref_t val);
	friend void KniSetStaticFieldNull(HKCLASS hKClass, HKFIELD hKField);

	friend void KniSetElementBool(knuint_t index, kbool_t val);
	friend void KniSetElementChar(knuint_t index, kchar_t val);
	friend void KniSetElementByte(knuint_t index, kbyte_t val);
	friend void KniSetElementSByte(knuint_t index, ksbyte_t val);
	friend void KniSetElementShort(knuint_t index, kshort_t val);
	friend void KniSetElementUShort(knuint_t index, kushort_t val);
	friend void KniSetElementInt(knuint_t index, kint_t val);
	friend void KniSetElementUInt(knuint_t index, kuint_t val);
	friend void KniSetElementLong(knuint_t index, klong_t val);
	friend void KniSetElementULong(knuint_t index, kulong_t val);
	friend void KniSetElementFloat(knuint_t index, kfloat_t val);
	friend void KniSetElementDouble(knuint_t index, kdouble_t val);
	friend void KniSetElementString(knuint_t index, kstring_t val, kuint_t length);
	friend void KniSetElementRaw(knuint_t index, kref_t val);
	friend void KniSetElementNull(knuint_t index);

	friend void KniSetIndirectBool(kbool_t val);
	friend void KniSetIndirectChar(kchar_t val);
	friend void KniSetIndirectByte(kbyte_t val);
	friend void KniSetIndirectSByte(ksbyte_t val);
	friend void KniSetIndirectShort(kshort_t val);
	friend void KniSetIndirectUShort(kushort_t val);
	friend void KniSetIndirectInt(kint_t val);
	friend void KniSetIndirectUInt(kuint_t val);
	friend void KniSetIndirectLong(klong_t val);
	friend void KniSetIndirectULong(kulong_t val);
	friend void KniSetIndirectFloat(kfloat_t val);
	friend void KniSetIndirectDouble(kdouble_t val);
	friend void KniSetIndirectString(kstring_t val, kuint_t length);
	friend void KniSetIndirectRaw(kref_t val);
	friend void KniSetIndirectNull(void);

	friend void KniLoadBool(kbool_t val);
	friend void KniLoadChar(kchar_t val);
	friend void KniLoadByte(kbyte_t val);
	friend void KniLoadSByte(ksbyte_t val);
	friend void KniLoadShort(kshort_t val);
	friend void KniLoadUShort(kushort_t val);
	friend void KniLoadInt(kint_t val);
	friend void KniLoadUInt(kuint_t val);
	friend void KniLoadLong(klong_t val);
	friend void KniLoadULong(kulong_t val);
	friend void KniLoadFloat(kfloat_t val);
	friend void KniLoadDouble(kdouble_t val);
	friend void KniLoadString(kstring_t val, kuint_t length);
	friend void KniLoadRaw(kref_t val);

	friend void KniLoadLocal(kushort_t index);
	friend void KniLoadArg(kushort_t index);
	friend void KniLoadArgByRef(kushort_t index);
	friend void KniLoadField(HKFIELD hKField);
	friend void KniLoadStaticField(HKCLASS hKClass, HKFIELD hKField);
	friend void KniLoadElement(knuint_t index);
	friend void KniLoadIndirect();

	friend void KniLoadLocalLength(kushort_t index);
	friend void KniLoadArgLength(kushort_t index);
	friend void KniLoadArgLengthByRef(kushort_t index);
	friend void KniLoadFieldLength(HKFIELD hKField);
	friend void KniLoadStaticFieldLength(HKCLASS hKClass, HKFIELD hKField);
	friend void KniLoadElementLength(knuint_t index);
	friend void KniLoadIndirectLength();

	friend void KniLoadLocalElement(kushort_t index, knuint_t elemIndex);
	friend void KniLoadArgElement(kushort_t index, knuint_t elemIndex);
	friend void KniLoadArgElementByRef(kushort_t index, knuint_t elemIndex);
	friend void KniLoadFieldElement(HKFIELD hKField, knuint_t elemIndex);
	friend void KniLoadStaticFieldElement(HKCLASS hKClass, HKFIELD hKField, knuint_t elemIndex);
	friend void KniLoadElementElement(knuint_t index, knuint_t elemIndex);
	friend void KniLoadIndirectElement(knuint_t elemIndex);

	friend void KniLoadLocalAddress(kushort_t index);
	friend void KniLoadArgAddress(kushort_t index);
	friend void KniLoadFieldAddress(HKFIELD hKField);
	friend void KniLoadStaticFieldAddress(HKCLASS hKClass, HKFIELD hKField);
	friend void KniLoadElementAddress(knuint_t index);

	friend void KniLoadNull();

	friend void KniLoadNewObject(HKCLASS hKClass);
	friend void KniLoadNewDelegate(HKDELEGATE hKDelegate, HKMETHOD hKMethod);
	friend void KniLoadNewArray(HKTYPE hKType, kuint_t length);
	friend void KniLoadNewArrayBaking(HKTYPE hKType, kuint_t length);

	friend void KniStoreLocal(kushort_t index);
	friend void KniStoreArg(kushort_t index);
	friend void KniStoreArgByRef(kushort_t index);
	friend void KniStoreField(HKFIELD hKField);
	friend void KniStoreStaticField(HKCLASS hKClass, HKFIELD hKField);
	friend void KniStoreElement(knuint_t index);
	friend void KniStoreIndirect();

	friend kbool_t KniIsNull();
	friend kbool_t KniIsNullLocal(kushort_t index);
	friend kbool_t KniIsNullArg(kushort_t index);
	friend kbool_t KniIsNullArgByRef(kushort_t index);
	friend kbool_t KniIsNullField(HKFIELD hKField);
	friend kbool_t KniIsNullStaticField(HKCLASS hKClass, HKFIELD hKField);
	friend kbool_t KniIsNullElement(knuint_t index);
	friend kbool_t KniIsNullIndirect();

	friend kbool_t KniIsReferenceEqual();

	friend kbool_t KniIsClass();
	friend kbool_t KniIsDelegate();
	friend kbool_t KniIsArray();

	friend kbool_t KniIsInstanceOfClass(HKCLASS hKClass);
	friend kbool_t KniIsInstanceOfDelegate(HKDELEGATE hKDelegate);

	friend KRESULT KniInvoke(HKMETHOD hKMethod);
	friend KRESULT KniInvokeObject();

	friend kbool_t KniHasException();
	friend void KniPrintExceptionDescription();
	friend void KniClearException();
	friend void KniThrowException(HKCLASS excType);
	friend void KniThrowExceptionEx(HKCLASS excType);
	friend void KniThrowExceptionGeneral(kstring_t message, kuint_t length);

	friend void KniInitLocals(HKTYPE *pHKTypes, kushort_t count);

	friend HKTYPE KniGetType();

	friend HKTYPE KniGetTypeLocal(kushort_t index);
	friend HKTYPE KniGetTypeArg(kushort_t index);
	friend HKTYPE KniGetTypeArgByRef(kushort_t index);
	friend HKTYPE KniGetTypeField(HKFIELD hKField);
	friend HKTYPE KniGetTypeStaticField(HKCLASS hKClass, HKFIELD hKField);
	friend HKTYPE KniGetTypeElement(knuint_t index);
	friend HKTYPE KniGetTypeIndirect();

	friend HKCLASS KniGetClass(kstring_t ksName);
	friend HKDELEGATE KniGetDelegate(kstring_t ksName);
	friend HKFIELD KniGetField(HKCLASS hKClass, kstring_t ksName);
	friend HKMETHOD KniGetMethod(HKCLASS hKClass, kstring_t ksName);

	friend HKTYPE KniGetPrimitiveType(KTYPETAG tag);

	friend HKTYPE KniCreateType(KTYPETAG tag, kushort_t dim, HKUSERTYPE hKClassOrDelegate);

	friend kbool_t KniIsClassType(HKTYPE hKType);
	friend kbool_t KniIsDelegateType(HKTYPE hKType);
	friend kbool_t KniIsArrayType(HKTYPE hKType);
};
