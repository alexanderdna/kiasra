#ifndef _KCS_H
#define _KCS_H

#include "kni.h"
#include "krt.h"
#include "kopcodes.h"

#include <stdint.h>

/*===================================================*/

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
	#ifdef KRT_EXPORTS
		#define KCS_API __declspec(dllexport)
	#else
		#define KCS_API __declspec(dllimport)
	#endif
#else
	#define KCS_API
#endif

/*===================================================*/

enum KMODULETYPES
{
	KMOD_WINDOW  = 0,
	KMOD_CONSOLE = 1,
	KMOD_LIBRARY = 2,
};

enum KCSERRORS
{
	KCSE_NO_ERROR = 0,

	KCSE_NO_ENVIRONMENT,

	KCSE_OUT_OF_MEMORY,
	KCSE_CANNOT_CREATE_ENVIRONMENT,
	KCSE_CANNOT_LOAD_MODULE,
	
	KCSE_INVALID_NAME,
	KCSE_DUPLICATE_CLASS,
	KCSE_DUPLICATE_DELEGATE,
	KCSE_DUPLICATE_FIELD,
	KCSE_DUPLICATE_METHOD,
	
	KCSE_INCOMPLETE_MODULE,
	KCSE_NO_ENTRY,

	KCSE_INVALID_PATH,
	KCSE_IO_ERROR,
};

/*===================================================*/

typedef void *HKBUILDER;

typedef HKBUILDER	HKMODULEBUILDER;
typedef HKBUILDER	HKCLASSBUILDER;
typedef HKBUILDER	HKDELEGATEBUILDER;
typedef HKBUILDER	HKFIELDBUILDER;
typedef HKBUILDER	HKMETHODBUILDER;
typedef HKBUILDER	HKLOCALBUILDER;
typedef HKBUILDER	HKTYPEBUILDER;

typedef void *HKCODEGEN;
typedef void *HKCODELABEL;

typedef struct _KPARAMINFO
{
	kstring_t szName;
	HKTYPE declType;
	kbool_t byRef;
} KPARAMINFO;

/*===================================================*/

/* Gets the code of the last error. */
KCSERRORS KCS_API KcsGetLastError(void);

/* Creates an environment for compiler services. */
KRESULT KCS_API KcsCreateEnvironment(void);

/* Destroys the current environment and releases all resources. */
void KCS_API KcsDestroyEnvironment(void);

/* Loads an existing module. */
KRESULT KCS_API KcsLoadModule(KMODULEATTRIBUTES attrs, kstring_t szFullPath, HKMODULE *pHKModule);

/*===================================================*/

/* Finds a class in the given module. */
HKCLASS KCS_API KcsFindClass(HKMODULE hKModule, kstring_t szName);

/* Finds a delegate in the given module. */
HKDELEGATE KCS_API KcsFindDelegate(HKMODULE hKModule, kstring_t szName);

/* Finds a member field in the given class. */
HKFIELD KCS_API KcsFindField(HKCLASS hKClass, kstring_t szName);

/* Finds a member method in the given class. */
HKMETHOD KCS_API KcsFindMethod(HKCLASS hKClass, kstring_t szName);

/* Gets the attributes of the given class. */
KCLASSATTRIBUTES KCS_API KcsGetClassAttributes(HKCLASS hKClass);

/* Gets the attributes of the given delegate. */
KCLASSATTRIBUTES KCS_API KcsGetDelegateAttributes(HKDELEGATE hKDelegate);

/* Gets the attributes of the given field. */
KFIELDATTRIBUTES KCS_API KcsGetFieldAttributes(HKFIELD hKField);

/* Gets the type of the given field. */
HKTYPE KCS_API KcsGetFieldType(HKFIELD hKField);

/* Gets the attributes of the given method. */
KMETHODATTRIBUTES KCS_API KcsGetMethodAttributes(HKMETHOD hKMethod);

/* Gets the return type of the given method. */
HKTYPE KCS_API KcsGetMethodReturnType(HKMETHOD hKMethod);

/* Gets the parameters of the given method. */
KPARAMINFO * KCS_API KcsGetMethodParameters(HKMETHOD hKMethod);

/* Gets the return type of the given delegate. */
HKTYPE KCS_API KcsGetDelegateReturnType(HKDELEGATE hKDelegate);

/* Gets the parameters of the given delegate. */
KPARAMINFO * KCS_API KcsGetDelegateParameters(HKDELEGATE hKDelegate);

/* Deallocates the given array of KPARAMINFO structs and sets the pointer to NULL. */
void KCS_API KcsFreeParameters(KPARAMINFO **ppKParamInfo);

/*===================================================*/

/* Creates a builder service for making a new module. */
HKMODULEBUILDER KCS_API KcsCreateModuleBuilder(KMODULETYPES type);

/* Bakes a module to prepare for saving. */
KRESULT KCS_API KcsBakeModule(HKMODULEBUILDER hKModuleBuilder);

/* Saves the baked module to a file. */
KRESULT KCS_API KcsSaveModule(HKMODULEBUILDER hKModuleBuilder, const char *szPath);

/*===================================================*/

/* Creates a type having the given properties. */
HKTYPE KCS_API KcsCreateType(HKMODULEBUILDER hKModuleBuilder, KTYPETAG tag, kushort_t dim, HKUSERTYPE hClassOrDelegate);

/* Defines a new class in the given module. */
HKCLASSBUILDER KCS_API KcsDefineClass(HKMODULEBUILDER hKModuleBuilder, kstring_t szName, KCLASSATTRIBUTES attrs);

/* Defines a new delegate in the given module. */
HKDELEGATEBUILDER KCS_API KcsDefineDelegate(HKMODULEBUILDER hKModuleBuilder, kstring_t szName, KCLASSATTRIBUTES attrs,
                                            HKTYPE hReturnType, const KPARAMINFO *pParams);
/* Defines a new field in the given class. */
HKFIELDBUILDER KCS_API KcsDefineField(HKCLASSBUILDER hKClassBuilder, kstring_t szName, KFIELDATTRIBUTES attrs, HKTYPE hReturnType);

/* Defines a new method in the given class. */
HKMETHODBUILDER KCS_API KcsDefineMethod(HKCLASSBUILDER hKClassBuilder, kstring_t szName, KMETHODATTRIBUTES attrs,
                                        HKTYPE hReturnType, const KPARAMINFO *pParams);

/* Declares a new local variable in the given method. */
HKLOCALBUILDER KCS_API KcsDeclareLocal(HKMETHODBUILDER hKMethodBuilder, HKTYPE hDeclType);

/*===================================================*/

/* Gets the code generator of the given method. */
HKCODEGEN KCS_API KcsGetCodeGen(HKMETHODBUILDER hKMethodBuilder);

/* Marks the end of a code generation for the given method. No further emitting is allowed. */
KRESULT KCS_API KcsFinishCodeGen(HKMETHODBUILDER hKMethodBuilder);

/* Defines a label for the given code generator. */
HKCODELABEL KCS_API KcsDefineCodeLabel(HKCODEGEN hKCodeGen);

/* Marks the position of the given label. */
void KCS_API KcsMarkCodeLabel(HKCODEGEN hKCodeGen, HKCODELABEL hKCodeLabel);

/* Emits an opcode. */
KRESULT KCS_API KcsCodeEmit(HKCODEGEN hKCodeGen, KOPCODES opcode);
/* Emits an opcode with a `char` operand. */
KRESULT KCS_API KcsCodeEmitChar(HKCODEGEN hKCodeGen, KOPCODES opcode, kchar_t ch);
/* Emits an opcode with an `i1` operand. */
KRESULT KCS_API KcsCodeEmitI1(HKCODEGEN hKCodeGen, KOPCODES opcode, int8_t val);
/* Emits an opcode with an `u1` operand. */
KRESULT KCS_API KcsCodeEmitU1(HKCODEGEN hKCodeGen, KOPCODES opcode, uint8_t val);
/* Emits an opcode with an `i2` operand. */
KRESULT KCS_API KcsCodeEmitI2(HKCODEGEN hKCodeGen, KOPCODES opcode, int16_t val);
/* Emits an opcode with an `u2` operand. */
KRESULT KCS_API KcsCodeEmitU2(HKCODEGEN hKCodeGen, KOPCODES opcode, uint16_t val);
/* Emits an opcode with an `i4` operand. */
KRESULT KCS_API KcsCodeEmitI4(HKCODEGEN hKCodeGen, KOPCODES opcode, int32_t val);
/* Emits an opcode with an `u4` operand. */
KRESULT KCS_API KcsCodeEmitU4(HKCODEGEN hKCodeGen, KOPCODES opcode, uint32_t val);
/* Emits an opcode with an `i8` operand. */
KRESULT KCS_API KcsCodeEmitI8(HKCODEGEN hKCodeGen, KOPCODES opcode, int64_t val);
/* Emits an opcode with an `u8` operand. */
KRESULT KCS_API KcsCodeEmitU8(HKCODEGEN hKCodeGen, KOPCODES opcode, uint64_t val);
/* Emits an opcode with an `r4` operand. */
KRESULT KCS_API KcsCodeEmitR4(HKCODEGEN hKCodeGen, KOPCODES opcode, float val);
/* Emits an opcode with an `r8` operand. */
KRESULT KCS_API KcsCodeEmitR8(HKCODEGEN hKCodeGen, KOPCODES opcode, double val);
/* Emits an opcode with a `string` operand. */
KRESULT KCS_API KcsCodeEmitString(HKCODEGEN hKCodeGen, KOPCODES opcode, kstring_t val, kuint_t length);
/* Emits an opcode with a local variable operand. */
KRESULT KCS_API KcsCodeEmitLocal(HKCODEGEN hKCodeGen, KOPCODES opcode, HKLOCALBUILDER hKLocalBuilder);
/* Emits a branching opcode with a label operand. */
KRESULT KCS_API KcsCodeEmitJump(HKCODEGEN hKCodeGen, KOPCODES opcode, HKCODELABEL hKCodeLabel);
/* Emits an opcode with a class operand. */
KRESULT KCS_API KcsCodeEmitClass(HKCODEGEN hKCodeGen, KOPCODES opcode, HKCLASS hKClass);
/* Emits an opcode with a delegate. */
KRESULT KCS_API KcsCodeEmitDelegate(HKCODEGEN hKCodeGen, KOPCODES opcode, HKDELEGATE hKDelegate);
/* Emits an opcode with a class field operand. */
KRESULT KCS_API KcsCodeEmitField(HKCODEGEN hKCodeGen, KOPCODES opcode, HKFIELD hKField);
/* Emits an opcode with a class method operand. */
KRESULT KCS_API KcsCodeEmitMethod(HKCODEGEN hKCodeGen, KOPCODES opcode, HKMETHOD hKMethod);
/* Emits an opcode with a type operand. */
KRESULT KCS_API KcsCodeEmitType(HKCODEGEN hKCodeGen, KOPCODES opcode, HKTYPE hKType);
/* Emits an `enter` opcode with the given exception type, handling position and leaving position. */
KRESULT KCS_API KcsCodeEmitEnter(HKCODEGEN hKCodeGen, HKTYPE hExcType, HKCODELABEL hHandlerLabel, HKCODELABEL hLeaveLabel);

/*===================================================*/

#ifdef __cplusplus
}
#endif

#endif/*_KCS_H*/
