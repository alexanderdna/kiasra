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
	kstring_t name;
	HKTYPE declType;
	kbool_t byRef;
} KPARAMINFO;

/*===================================================*/

/* Gets the code of the last error */
KCSERRORS KCS_API KcsGetLastError(void);

/* Creates an environment for compiler services */
KRESULT KCS_API KcsCreateEnvironment(void);

/* Destroys the current environment and releases all resources */
void KCS_API KcsDestroyEnvironment(void);

/* Loads an existing module */
KRESULT KCS_API KcsLoadModule(KMODULEATTRIBUTES attrs, kstring_t szFullPath, HKMODULE *pHKModule);

/* Gets a type having the given properties */
HKTYPE KCS_API KcsGetType(HKMODULEBUILDER hKModuleBuilder, ktypetag_t tag, kushort_t dim, HKUSERTYPE hClassOrDelegate);

/* Creates a builder service for making a new module */
HKMODULEBUILDER KCS_API KcsCreateModuleBuilder(KMODULETYPES type);

/* Defines a new class in the given module */
HKCLASSBUILDER KCS_API KcsDefineClass(HKMODULEBUILDER hKModuleBuilder, kstring_t name, KCLASSATTRIBUTES attrs);

/* Defines a new delegate in the given module */
HKDELEGATEBUILDER KCS_API KcsDefineDelegate(HKMODULEBUILDER hKModuleBuilder, kstring_t name, KCLASSATTRIBUTES attrs,
											HKTYPE hReturnType, const KPARAMINFO *pParams);
/* Defines a new field in the given class */
HKFIELDBUILDER KCS_API KcsDefineField(HKCLASSBUILDER hKClassBuilder, kstring_t name, KFIELDATTRIBUTES attrs, HKTYPE hReturnType);

/* Defines a new method in the given class */
HKMETHODBUILDER KCS_API KcsDefineMethod(HKCLASSBUILDER hKClassBuilder, kstring_t name, KMETHODATTRIBUTES attrs,
										HKTYPE hReturnType, const KPARAMINFO *pParams);

/* Declares a new local variable in the given method */
HKLOCALBUILDER KCS_API KcsDeclareLocal(HKMETHODBUILDER hKMethodBuilder, HKTYPE hDeclType);

/* Gets the code generator of the given method */
HKCODEGEN KCS_API KcsGetCodeGen(HKMETHODBUILDER hKMethodBuilder);

HKCODELABEL KCS_API KcsDefineCodeLabel(HKCODEGEN hKCodeGen);
void KCS_API KcsMarkCodeLabel(HKCODEGEN hKCodeGen, HKCODELABEL hKCodeLabel);

KRESULT KCS_API KcsCodeEmit(HKCODEGEN hKCodeGen, OpCodes opcode);
KRESULT KCS_API KcsCodeEmitChar(HKCODEGEN hKCodeGen, OpCodes opcode, kchar_t ch);
KRESULT KCS_API KcsCodeEmitI1(HKCODEGEN hKCodeGen, OpCodes opcode, int8_t val);
KRESULT KCS_API KcsCodeEmitU1(HKCODEGEN hKCodeGen, OpCodes opcode, uint8_t val);
KRESULT KCS_API KcsCodeEmitI2(HKCODEGEN hKCodeGen, OpCodes opcode, int8_t val);
KRESULT KCS_API KcsCodeEmitU2(HKCODEGEN hKCodeGen, OpCodes opcode, uint8_t val);
KRESULT KCS_API KcsCodeEmitI4(HKCODEGEN hKCodeGen, OpCodes opcode, int32_t val);
KRESULT KCS_API KcsCodeEmitU4(HKCODEGEN hKCodeGen, OpCodes opcode, uint32_t val);
KRESULT KCS_API KcsCodeEmitI8(HKCODEGEN hKCodeGen, OpCodes opcode, int64_t val);
KRESULT KCS_API KcsCodeEmitU8(HKCODEGEN hKCodeGen, OpCodes opcode, uint64_t val);
KRESULT KCS_API KcsCodeEmitR4(HKCODEGEN hKCodeGen, OpCodes opcode, float val);
KRESULT KCS_API KcsCodeEmitR8(HKCODEGEN hKCodeGen, OpCodes opcode, double val);
KRESULT KCS_API KcsCodeEmitString(HKCODEGEN hKCodeGen, OpCodes opcode, kstring_t val, kuint_t length);
KRESULT KCS_API KcsCodeEmitLocal(HKCODEGEN hKCodeGen, OpCodes opcode, HKLOCALBUILDER hKLocalBuilder);
KRESULT KCS_API KcsCodeEmitJump(HKCODEGEN hKCodeGen, OpCodes opcode, HKCODELABEL hKCodeLabel);
KRESULT KCS_API KcsCodeEmitClass(HKCODEGEN hKCodeGen, OpCodes opcode, HKCLASS hKClass);
KRESULT KCS_API KcsCodeEmitDelegate(HKCODEGEN hKCodeGen, OpCodes opcode, HKDELEGATE hKDelegate);
KRESULT KCS_API KcsCodeEmitField(HKCODEGEN hKCodeGen, OpCodes opcode, HKFIELD hKField);
KRESULT KCS_API KcsCodeEmitMethod(HKCODEGEN hKCodeGen, OpCodes opcode, HKMETHOD hKMethod);
KRESULT KCS_API KcsCodeEmitType(HKCODEGEN hKCodeGen, OpCodes opcode, HKTYPE hKType);
KRESULT KCS_API KcsCodeEmitEnter(HKCODEGEN hKCodeGen, HKTYPE hExcType, HKCODELABEL hHandlerLabel, HKCODELABEL hLeaveLabel);

/* Bakes a module to prepare for saving */
KRESULT KCS_API KcsBakeModule(HKMODULEBUILDER hKModuleBuilder);

/* Saves the baked module to a file */
KRESULT KCS_API KcsSaveModule(HKMODULEBUILDER hKModuleBuilder, const char *szPath);

/*===================================================*/

#ifdef __cplusplus
}
#endif

#endif/*_KCS_H*/
