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
KCS_API KCSERRORS KcsGetLastError(void);

/* Creates an environment for compiler services. */
KCS_API KRESULT KcsCreateEnvironment(void);

/* Destroys the current environment and releases all resources. */
KCS_API void KcsDestroyEnvironment(void);

/* Loads an existing module. */
KCS_API KRESULT KcsLoadModule(KMODULEATTRIBUTES attrs, kstring_t szFullPath, HKMODULE *pHKModule);

/*===================================================*/

/* Finds a class in the given module. */
KCS_API HKCLASS KcsFindClass(HKMODULE hKModule, kstring_t szName);

/* Finds a delegate in the given module. */
KCS_API HKDELEGATE KcsFindDelegate(HKMODULE hKModule, kstring_t szName);

/* Finds a member field in the given class. */
KCS_API HKFIELD KcsFindField(HKCLASS hKClass, kstring_t szName);

/* Finds a member method in the given class. */
KCS_API HKMETHOD KcsFindMethod(HKCLASS hKClass, kstring_t szName);

/* Gets the attributes of the given class. */
KCS_API KCLASSATTRIBUTES KcsGetClassAttributes(HKCLASS hKClass);

/* Gets the attributes of the given delegate. */
KCS_API KCLASSATTRIBUTES KcsGetDelegateAttributes(HKDELEGATE hKDelegate);

/* Gets the attributes of the given field. */
KCS_API KFIELDATTRIBUTES KcsGetFieldAttributes(HKFIELD hKField);

/* Gets the type of the given field. */
KCS_API HKTYPE KcsGetFieldType(HKFIELD hKField);

/* Gets the attributes of the given method. */
KCS_API KMETHODATTRIBUTES KcsGetMethodAttributes(HKMETHOD hKMethod);

/* Gets the return type of the given method. */
KCS_API HKTYPE KcsGetMethodReturnType(HKMETHOD hKMethod);

/* Gets the parameters of the given method. */
KCS_API KPARAMINFO * KcsGetMethodParameters(HKMETHOD hKMethod);

/* Gets the return type of the given delegate. */
KCS_API HKTYPE KcsGetDelegateReturnType(HKDELEGATE hKDelegate);

/* Gets the parameters of the given delegate. */
KCS_API KPARAMINFO * KcsGetDelegateParameters(HKDELEGATE hKDelegate);

/* Deallocates the given array of KPARAMINFO structs and sets the pointer to NULL. */
KCS_API void KcsFreeParameters(KPARAMINFO **ppKParamInfo);

/*===================================================*/

/* Creates a builder service for making a new module. */
KCS_API HKMODULEBUILDER KcsCreateModuleBuilder(KMODULETYPES type);

/* Bakes a module to prepare for saving. */
KCS_API KRESULT KcsBakeModule(HKMODULEBUILDER hKModuleBuilder);

/* Saves the baked module to a file. */
KCS_API KRESULT KcsSaveModule(HKMODULEBUILDER hKModuleBuilder, const char *szPath);

/*===================================================*/

/* Creates a type having the given properties. */
KCS_API HKTYPE KcsCreateType(HKMODULEBUILDER hKModuleBuilder, KTYPETAG tag, kushort_t dim, HKUSERTYPE hClassOrDelegate);

/* Defines a new class in the given module. */
KCS_API HKCLASSBUILDER KcsDefineClass(HKMODULEBUILDER hKModuleBuilder, kstring_t szName, KCLASSATTRIBUTES attrs);

/* Defines a new delegate in the given module. */
KCS_API HKDELEGATEBUILDER KcsDefineDelegate(HKMODULEBUILDER hKModuleBuilder, kstring_t szName, KCLASSATTRIBUTES attrs,
                                            HKTYPE hReturnType, const KPARAMINFO *pParams);
/* Defines a new field in the given class. */
KCS_API HKFIELDBUILDER KcsDefineField(HKCLASSBUILDER hKClassBuilder, kstring_t szName, KFIELDATTRIBUTES attrs, HKTYPE hReturnType);

/* Defines a new method in the given class. */
KCS_API HKMETHODBUILDER KcsDefineMethod(HKCLASSBUILDER hKClassBuilder, kstring_t szName, KMETHODATTRIBUTES attrs,
                                        HKTYPE hReturnType, const KPARAMINFO *pParams);

/* Declares a new local variable in the given method. */
KCS_API HKLOCALBUILDER KcsDeclareLocal(HKMETHODBUILDER hKMethodBuilder, HKTYPE hDeclType);

/*===================================================*/

/* Gets the code generator of the given method. */
KCS_API HKCODEGEN KcsGetCodeGen(HKMETHODBUILDER hKMethodBuilder);

/* Marks the end of a code generation for the given method. No further emitting is allowed. */
KCS_API KRESULT KcsFinishCodeGen(HKMETHODBUILDER hKMethodBuilder);

/* Defines a label for the given code generator. */
KCS_API HKCODELABEL KcsDefineCodeLabel(HKCODEGEN hKCodeGen);

/* Marks the position of the given label. */
KCS_API void KcsMarkCodeLabel(HKCODEGEN hKCodeGen, HKCODELABEL hKCodeLabel);

/* Emits an opcode. */
KCS_API KRESULT KcsCodeEmit(HKCODEGEN hKCodeGen, KOPCODES opcode);
/* Emits an opcode with a `char` operand. */
KCS_API KRESULT KcsCodeEmitChar(HKCODEGEN hKCodeGen, KOPCODES opcode, kchar_t ch);
/* Emits an opcode with an `i1` operand. */
KCS_API KRESULT KcsCodeEmitI1(HKCODEGEN hKCodeGen, KOPCODES opcode, int8_t val);
/* Emits an opcode with an `u1` operand. */
KCS_API KRESULT KcsCodeEmitU1(HKCODEGEN hKCodeGen, KOPCODES opcode, uint8_t val);
/* Emits an opcode with an `i2` operand. */
KCS_API KRESULT KcsCodeEmitI2(HKCODEGEN hKCodeGen, KOPCODES opcode, int16_t val);
/* Emits an opcode with an `u2` operand. */
KCS_API KRESULT KcsCodeEmitU2(HKCODEGEN hKCodeGen, KOPCODES opcode, uint16_t val);
/* Emits an opcode with an `i4` operand. */
KCS_API KRESULT KcsCodeEmitI4(HKCODEGEN hKCodeGen, KOPCODES opcode, int32_t val);
/* Emits an opcode with an `u4` operand. */
KCS_API KRESULT KcsCodeEmitU4(HKCODEGEN hKCodeGen, KOPCODES opcode, uint32_t val);
/* Emits an opcode with an `i8` operand. */
KCS_API KRESULT KcsCodeEmitI8(HKCODEGEN hKCodeGen, KOPCODES opcode, int64_t val);
/* Emits an opcode with an `u8` operand. */
KCS_API KRESULT KcsCodeEmitU8(HKCODEGEN hKCodeGen, KOPCODES opcode, uint64_t val);
/* Emits an opcode with an `r4` operand. */
KCS_API KRESULT KcsCodeEmitR4(HKCODEGEN hKCodeGen, KOPCODES opcode, float val);
/* Emits an opcode with an `r8` operand. */
KCS_API KRESULT KcsCodeEmitR8(HKCODEGEN hKCodeGen, KOPCODES opcode, double val);
/* Emits an opcode with a `string` operand. */
KCS_API KRESULT KcsCodeEmitString(HKCODEGEN hKCodeGen, KOPCODES opcode, kstring_t val, kuint_t length);
/* Emits an opcode with a local variable operand. */
KCS_API KRESULT KcsCodeEmitLocal(HKCODEGEN hKCodeGen, KOPCODES opcode, HKLOCALBUILDER hKLocalBuilder);
/* Emits a branching opcode with a label operand. */
KCS_API KRESULT KcsCodeEmitJump(HKCODEGEN hKCodeGen, KOPCODES opcode, HKCODELABEL hKCodeLabel);
/* Emits an opcode with a class operand. */
KCS_API KRESULT KcsCodeEmitClass(HKCODEGEN hKCodeGen, KOPCODES opcode, HKCLASS hKClass);
/* Emits an opcode with a delegate. */
KCS_API KRESULT KcsCodeEmitDelegate(HKCODEGEN hKCodeGen, KOPCODES opcode, HKDELEGATE hKDelegate);
/* Emits an opcode with a class field operand. */
KCS_API KRESULT KcsCodeEmitField(HKCODEGEN hKCodeGen, KOPCODES opcode, HKFIELD hKField);
/* Emits an opcode with a class method operand. */
KCS_API KRESULT KcsCodeEmitMethod(HKCODEGEN hKCodeGen, KOPCODES opcode, HKMETHOD hKMethod);
/* Emits an opcode with a type operand. */
KCS_API KRESULT KcsCodeEmitType(HKCODEGEN hKCodeGen, KOPCODES opcode, HKTYPE hKType);
/* Emits an `enter` opcode with the given exception type, handling position and leaving position. */
KCS_API KRESULT KcsCodeEmitEnter(HKCODEGEN hKCodeGen, HKTYPE hExcType, HKCODELABEL hHandlerLabel, HKCODELABEL hLeaveLabel);

/*===================================================*/

#ifdef __cplusplus
}
#endif

#endif/*_KCS_H*/
