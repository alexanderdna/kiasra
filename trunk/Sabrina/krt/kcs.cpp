#include "kcs.h"
#include "kcompile.hpp"

#include "kconfig.hpp"
#include "kenv.hpp"
#include "kmodule.hpp"

#include "kstringutils.hpp"

//===================================================

static KCSERRORS lastError;

KCSERRORS KCS_API KcsGetLastError(void)
{
	return lastError;
}

KRESULT KCS_API KcsCreateEnvironment()
{
	KEnvironment::initialize();
	lastError = KCSE_NO_ERROR;
	return KRESULT_OK;
}

KRESULT KCS_API KcsLoadModule(KMODULEATTRIBUTES attrs, kstring_t szFullPath, HKMODULE *pHKModule)
{
	ModuleLoader *loader;

#ifdef ISWIN
	const kchar_t *lastSlash = wcsrchr(szFullPath, '\\');
#else
	const kchar_t *lastSlash = wcsrchr(szFullPath, '/');
#endif

	if (attrs & KMODA_SYSTEM)
	{
		loader = KEnvironment::createModuleLoader(attrs, KEnvironment::getSystemLibPath(), lastSlash);
	}
	else
	{
		kstring_t dirPath = krt_getdirpath(szFullPath);
		loader = KEnvironment::createModuleLoader(attrs, dirPath, lastSlash);
		delete []dirPath;
	}

	if (!loader->load())
	{
		*pHKModule = NULL;
		lastError = KCSE_CANNOT_LOAD_MODULE;
		return KRESULT_ERR;
	}

	*pHKModule = loader->getModule();
	lastError = KCSE_NO_ERROR;

	return KRESULT_OK;
}

HKTYPE KCS_API KcsGetType(HKMODULEBUILDER hKModuleBuilder, ktypetag_t tag, kushort_t dim, HKUSERTYPE hClassOrDelegate)
{
	return NULL;
}

HKMODULEBUILDER KCS_API KcsCreateModuleBuilder(KMODULETYPES type)
{
	return NULL;
}

HKCLASSBUILDER KCS_API KcsDefineClass(HKMODULEBUILDER hKModuleBuilder, kstring_t name, KCLASSATTRIBUTES attrs)
{
	return NULL;
}

HKDELEGATEBUILDER KCS_API KcsDefineDelegate(HKMODULEBUILDER hKModuleBuilder, kstring_t name, KCLASSATTRIBUTES attrs,
											HKTYPE hReturnType, const KPARAMINFO *pParams)
{
	return NULL;
}

HKFIELDBUILDER KCS_API KcsDefineField(HKCLASSBUILDER hKClassBuilder, kstring_t name, KFIELDATTRIBUTES attrs, HKTYPE hReturnType)
{
	return NULL;
}

HKMETHODBUILDER KCS_API KcsDefineMethod(HKCLASSBUILDER hKClassBuilder, kstring_t name, KMETHODATTRIBUTES attrs,
										HKTYPE hReturnType, const KPARAMINFO *pParams)
{
	return NULL;
}

HKLOCALBUILDER KCS_API KcsDeclareLocal(HKMETHODBUILDER hKMethodBuilder, HKTYPE hDeclType)
{
	return NULL;
}

HKCODEGEN KCS_API KcsGetCodeGen(HKMETHODBUILDER hKMethodBuilder)
{
	return NULL;
}

HKCODELABEL KCS_API KcsDefineCodeLabel(HKCODEGEN hKCodeGen)
{
	return NULL;
}

void KCS_API KcsMarkCodeLabel(HKCODEGEN hKCodeGen, HKCODELABEL hKCodeLabel)
{
}

KRESULT KCS_API KcsCodeEmit(HKCODEGEN hKCodeGen, OpCodes opcode)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitChar(HKCODEGEN hKCodeGen, OpCodes opcode, kchar_t ch)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitI1(HKCODEGEN hKCodeGen, OpCodes opcode, int8_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitU1(HKCODEGEN hKCodeGen, OpCodes opcode, uint8_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitI2(HKCODEGEN hKCodeGen, OpCodes opcode, int8_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitU2(HKCODEGEN hKCodeGen, OpCodes opcode, uint8_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitI4(HKCODEGEN hKCodeGen, OpCodes opcode, int32_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitU4(HKCODEGEN hKCodeGen, OpCodes opcode, uint32_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitI8(HKCODEGEN hKCodeGen, OpCodes opcode, int64_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitU8(HKCODEGEN hKCodeGen, OpCodes opcode, uint64_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitR4(HKCODEGEN hKCodeGen, OpCodes opcode, float val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitR8(HKCODEGEN hKCodeGen, OpCodes opcode, double val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitString(HKCODEGEN hKCodeGen, OpCodes opcode, kstring_t val, kuint_t length)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitLocal(HKCODEGEN hKCodeGen, OpCodes opcode, HKLOCALBUILDER hKLocalBuilder)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitJump(HKCODEGEN hKCodeGen, OpCodes opcode, HKCODELABEL hKCodeLabel)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitClass(HKCODEGEN hKCodeGen, OpCodes opcode, HKCLASS hKClass)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitDelegate(HKCODEGEN hKCodeGen, OpCodes opcode, HKDELEGATE hKDelegate)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitField(HKCODEGEN hKCodeGen, OpCodes opcode, HKFIELD hKField)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitMethod(HKCODEGEN hKCodeGen, OpCodes opcode, HKMETHOD hKMethod)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitType(HKCODEGEN hKCodeGen, OpCodes opcode, HKTYPE hKType)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitEnter(HKCODEGEN hKCodeGen, HKTYPE hExcType, HKCODELABEL hHandlerLabel, HKCODELABEL hLeaveLabel)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsBakeModule(HKMODULEBUILDER hKModuleBuilder)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsSaveModule(HKMODULEBUILDER hKModuleBuilder, const char *szPath)
{
	return KRESULT_OK;
}

//===================================================
