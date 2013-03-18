#include "kcs.h"
#include "kcompile.hpp"

#include "kconfig.hpp"
#include "kenv.hpp"
#include "kmodule.hpp"
#include "ktypetree.hpp"

#include "kstringutils.hpp"

//===================================================

static KCSERRORS lastError;

KCS_API KCSERRORS KcsGetLastError(void)
{
	return lastError;
}

KCS_API KRESULT KcsCreateEnvironment()
{
	KEnvironment::initialize(false);
	lastError = KCSE_NO_ERROR;
	return KRESULT_OK;
}

KCS_API void KcsDestroyEnvironment(void)
{
	KEnvironment::finalize();
	lastError = KCSE_NO_ERROR;
}

KCS_API KRESULT KcsLoadModule(KMODULEATTRIBUTES attrs, kstring_t szFullPath, HKMODULE *pHKModule)
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

//===================================================

KCS_API HKCLASS KcsFindClass(HKMODULE hKModule, kstring_t szName)
{
	return KEnvironment::findClass(szName, (ModuleDef *)hKModule);
}

KCS_API HKDELEGATE KcsFindDelegate(HKMODULE hKModule, kstring_t szName)
{
	return KEnvironment::findDelegate(szName, (ModuleDef *)hKModule);
}

KCS_API HKFIELD KcsFindField(HKCLASS hKClass, kstring_t szName)
{
	return KEnvironment::findField((ClassDef *)hKClass, szName);
}

KCS_API HKMETHOD KcsFindMethod(HKCLASS hKClass, kstring_t szName)
{
	return KEnvironment::findMethod((ClassDef *)hKClass, szName);
}

KCS_API KCLASSATTRIBUTES KcsGetClassAttributes(HKCLASS hKClass)
{
	return (KCLASSATTRIBUTES)((ClassDef *)hKClass)->attrs;
}

KCS_API KCLASSATTRIBUTES KcsGetDelegateAttributes(HKDELEGATE hKDelegate)
{
	return (KCLASSATTRIBUTES)((DelegateDef *)hKDelegate)->attrs;
}

KCS_API KFIELDATTRIBUTES KcsGetFieldAttributes(HKFIELD hKField)
{
	return (KFIELDATTRIBUTES)((FieldDef *)hKField)->attrs;
}

KCS_API HKTYPE KcsGetFieldType(HKFIELD hKField)
{
	return ((FieldDef *)hKField)->declType;
}

KCS_API KMETHODATTRIBUTES KcsGetMethodAttributes(HKMETHOD hKMethod)
{
	return (KMETHODATTRIBUTES)((MethodDef *)hKMethod)->attrs;
}

KCS_API HKTYPE KcsGetMethodReturnType(HKMETHOD hKMethod)
{
	return ((MethodDef *)hKMethod)->returnType;
}

KCS_API KPARAMINFO * KcsGetMethodParameters(HKMETHOD hKMethod, kushort_t *puwCount)
{
	const MethodDef *met = (MethodDef *)hKMethod;
	kushort_t paramCount = met->paramCount;
	ParamDef **paramList = met->paramList;
	KPARAMINFO *params = new KPARAMINFO[paramCount];
	for (kuint_t i = 0; i < paramCount; ++i)
	{
		params[i].szName = paramList[i]->name;
		params[i].declType = paramList[i]->declType;
		params[i].byRef = (kbool_t)paramList[i]->byRef;
	}

	*puwCount = paramCount;
	return params;
}

KCS_API HKTYPE KcsGetDelegateReturnType(HKDELEGATE hKDelegate)
{
	return ((DelegateDef *)hKDelegate)->returnType;
}

KCS_API KPARAMINFO * KcsGetDelegateParameters(HKDELEGATE hKDelegate, kushort_t *puwCount)
{
	const DelegateDef *del = (DelegateDef *)hKDelegate;
	kushort_t paramCount = del->paramCount;
	ParamDef **paramList = del->paramList;
	KPARAMINFO *params = new KPARAMINFO[paramCount];
	for (kuint_t i = 0; i < paramCount; ++i)
	{
		params[i].szName = paramList[i]->name;
		params[i].declType = paramList[i]->declType;
		params[i].byRef = (kbool_t)paramList[i]->byRef;
	}

	*puwCount = paramCount;
	return params;
}

KCS_API void KcsFreeParameters(KPARAMINFO **ppKParamInfo)
{
	delete [](*ppKParamInfo);
	*ppKParamInfo = NULL;
}

//===================================================

KCS_API HKMODULEBUILDER KcsCreateModuleBuilder(KMODULETYPES type)
{
	return NULL;
}

KCS_API KRESULT KcsBakeModule(HKMODULEBUILDER hKModuleBuilder)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsSaveModule(HKMODULEBUILDER hKModuleBuilder, const char *szPath)
{
	return KRESULT_OK;
}

//===================================================

KCS_API HKTYPE KcsCreateType(HKMODULEBUILDER hKModuleBuilder, KTYPETAG tag, kushort_t dim, HKUSERTYPE hClassOrDelegate)
{
	return ((ModuleBuilder *)hKModuleBuilder)->createType(tag, dim, hClassOrDelegate);
}

KCS_API HKCLASSBUILDER KcsDefineClass(HKMODULEBUILDER hKModuleBuilder, kstring_t szName, KCLASSATTRIBUTES attrs)
{
	return ((ModuleBuilder *)hKModuleBuilder)->defineClass(attrs, szName);
}

KCS_API HKDELEGATEBUILDER KcsDefineDelegate(HKMODULEBUILDER hKModuleBuilder, kstring_t szName, KCLASSATTRIBUTES attrs,
                                            HKTYPE hReturnType, const KPARAMINFO *pParams, kushort_t uwCount)
{
	return ((ModuleBuilder *)hKModuleBuilder)->defineDelegate(attrs, szName, (TypeDef *)hReturnType, pParams, uwCount);
}

KCS_API HKFIELDBUILDER KcsDefineField(HKCLASSBUILDER hKClassBuilder, kstring_t szName, KFIELDATTRIBUTES attrs, HKTYPE hDeclType)
{
	return ((ClassBuilder *)hKClassBuilder)->defineField(attrs, szName, (TypeDef *)hDeclType);
}

KCS_API HKMETHODBUILDER KcsDefineMethod(HKCLASSBUILDER hKClassBuilder, kstring_t szName, KMETHODATTRIBUTES attrs,
                                        HKTYPE hReturnType, const KPARAMINFO *pParams, kushort_t uwCount)
{
	return ((ClassBuilder *)hKClassBuilder)->defineMethod(attrs, szName, (TypeDef *)hReturnType, pParams, uwCount);
}

KCS_API HKLOCALBUILDER KcsDeclareLocal(HKMETHODBUILDER hKMethodBuilder, HKTYPE hDeclType)
{
	return NULL;
}

//===================================================

KCS_API HKCODEGEN KcsGetCodeGen(HKMETHODBUILDER hKMethodBuilder)
{
	return NULL;
}

KCS_API KRESULT KcsFinishCodeGen(HKMETHODBUILDER hKMethodBuilder)
{
	return KRESULT_OK;
}

KCS_API HKCODELABEL KcsDefineCodeLabel(HKCODEGEN hKCodeGen)
{
	return NULL;
}

KCS_API void KcsMarkCodeLabel(HKCODEGEN hKCodeGen, HKCODELABEL hKCodeLabel)
{
}

KCS_API KRESULT KcsCodeEmit(HKCODEGEN hKCodeGen, KOPCODES opcode)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitChar(HKCODEGEN hKCodeGen, KOPCODES opcode, kchar_t ch)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitI1(HKCODEGEN hKCodeGen, KOPCODES opcode, int8_t val)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitU1(HKCODEGEN hKCodeGen, KOPCODES opcode, uint8_t val)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitI2(HKCODEGEN hKCodeGen, KOPCODES opcode, int16_t val)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitU2(HKCODEGEN hKCodeGen, KOPCODES opcode, uint16_t val)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitI4(HKCODEGEN hKCodeGen, KOPCODES opcode, int32_t val)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitU4(HKCODEGEN hKCodeGen, KOPCODES opcode, uint32_t val)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitI8(HKCODEGEN hKCodeGen, KOPCODES opcode, int64_t val)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitU8(HKCODEGEN hKCodeGen, KOPCODES opcode, uint64_t val)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitR4(HKCODEGEN hKCodeGen, KOPCODES opcode, float val)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitR8(HKCODEGEN hKCodeGen, KOPCODES opcode, double val)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitString(HKCODEGEN hKCodeGen, KOPCODES opcode, kstring_t val, kuint_t length)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitLocal(HKCODEGEN hKCodeGen, KOPCODES opcode, HKLOCALBUILDER hKLocalBuilder)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitJump(HKCODEGEN hKCodeGen, KOPCODES opcode, HKCODELABEL hKCodeLabel)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitClass(HKCODEGEN hKCodeGen, KOPCODES opcode, HKCLASS hKClass)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitDelegate(HKCODEGEN hKCodeGen, KOPCODES opcode, HKDELEGATE hKDelegate)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitField(HKCODEGEN hKCodeGen, KOPCODES opcode, HKFIELD hKField)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitMethod(HKCODEGEN hKCodeGen, KOPCODES opcode, HKMETHOD hKMethod)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitType(HKCODEGEN hKCodeGen, KOPCODES opcode, HKTYPE hKType)
{
	return KRESULT_OK;
}

KCS_API KRESULT KcsCodeEmitEnter(HKCODEGEN hKCodeGen, HKTYPE hExcType, HKCODELABEL hHandlerLabel, HKCODELABEL hLeaveLabel)
{
	return KRESULT_OK;
}
