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
	KEnvironment::initialize(false);
	lastError = KCSE_NO_ERROR;
	return KRESULT_OK;
}

void KCS_API KcsDestroyEnvironment(void)
{
	KEnvironment::finalize();
	lastError = KCSE_NO_ERROR;
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

//===================================================

HKCLASS KCS_API KcsFindClass(HKMODULE hKModule, kstring_t szName)
{
	return KEnvironment::findClass(szName, (ModuleDef *)hKModule);
}

HKDELEGATE KCS_API KcsFindDelegate(HKMODULE hKModule, kstring_t szName)
{
	return KEnvironment::findDelegate(szName, (ModuleDef *)hKModule);
}

HKFIELD KCS_API KcsFindField(HKCLASS hKClass, kstring_t szName)
{
	return KEnvironment::findField((ClassDef *)hKClass, szName);
}

HKMETHOD KCS_API KcsFindMethod(HKCLASS hKClass, kstring_t szName)
{
	return KEnvironment::findMethod((ClassDef *)hKClass, szName);
}

KCLASSATTRIBUTES KCS_API KcsGetClassAttributes(HKCLASS hKClass)
{
	return ((ClassDef *)hKClass)->attrs;
}

KCLASSATTRIBUTES KCS_API KcsGetDelegateAttributes(HKDELEGATE hKDelegate)
{
	return ((DelegateDef *)hKDelegate)->attrs;
}

KFIELDATTRIBUTES KCS_API KcsGetFieldAttributes(HKFIELD hKField)
{
	return (KFIELDATTRIBUTES)((FieldDef *)hKField)->attrs;
}

HKTYPE KCS_API KcsGetFieldType(HKFIELD hKField)
{
	return ((FieldDef *)hKField)->declType;
}

KMETHODATTRIBUTES KCS_API KcsGetMethodAttributes(HKMETHOD hKMethod)
{
	return (KMETHODATTRIBUTES)((MethodDef *)hKMethod)->attrs;
}

HKTYPE KCS_API KcsGetMethodReturnType(HKMETHOD hKMethod)
{
	return ((MethodDef *)hKMethod)->returnType;
}

KPARAMINFO * KCS_API KcsGetMethodParameters(HKMETHOD hKMethod)
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

	return params;
}

HKTYPE KCS_API KcsGetDelegateReturnType(HKDELEGATE hKDelegate)
{
	return ((DelegateDef *)hKDelegate)->returnType;
}

KPARAMINFO * KCS_API KcsGetDelegateParameters(HKDELEGATE hKDelegate)
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

	return params;
}

void KCS_API KcsFreeParameters(KPARAMINFO **ppKParamInfo)
{
	delete [](*ppKParamInfo);
	*ppKParamInfo = NULL;
}

//===================================================

HKMODULEBUILDER KCS_API KcsCreateModuleBuilder(KMODULETYPES type)
{
	return NULL;
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

HKTYPE KCS_API KcsCreateType(HKMODULEBUILDER hKModuleBuilder, KTYPETAG tag, kushort_t dim, HKUSERTYPE hClassOrDelegate)
{
	if ((tag & KT_SCALAR_MASK) == KT_CLASS)
		return KEnvironment::createType(tag, dim, (ClassDef *)hClassOrDelegate);
	else if ((tag & KT_SCALAR_MASK) == KT_DELEGATE)
		return KEnvironment::createType(tag, dim, (DelegateDef *)hClassOrDelegate);
	else
		return KEnvironment::createType(tag, dim);
}

HKCLASSBUILDER KCS_API KcsDefineClass(HKMODULEBUILDER hKModuleBuilder, kstring_t szName, KCLASSATTRIBUTES attrs)
{
	return NULL;
}

HKDELEGATEBUILDER KCS_API KcsDefineDelegate(HKMODULEBUILDER hKModuleBuilder, kstring_t szName, KCLASSATTRIBUTES attrs,
                                            HKTYPE hReturnType, const KPARAMINFO *pParams)
{
	return NULL;
}

HKFIELDBUILDER KCS_API KcsDefineField(HKCLASSBUILDER hKClassBuilder, kstring_t szName, KFIELDATTRIBUTES attrs, HKTYPE hReturnType)
{
	return NULL;
}

HKMETHODBUILDER KCS_API KcsDefineMethod(HKCLASSBUILDER hKClassBuilder, kstring_t szName, KMETHODATTRIBUTES attrs,
                                        HKTYPE hReturnType, const KPARAMINFO *pParams)
{
	return NULL;
}

HKLOCALBUILDER KCS_API KcsDeclareLocal(HKMETHODBUILDER hKMethodBuilder, HKTYPE hDeclType)
{
	return NULL;
}

//===================================================

HKCODEGEN KCS_API KcsGetCodeGen(HKMETHODBUILDER hKMethodBuilder)
{
	return NULL;
}

KRESULT KCS_API KcsFinishCodeGen(HKMETHODBUILDER hKMethodBuilder)
{
	return KRESULT_OK;
}

HKCODELABEL KCS_API KcsDefineCodeLabel(HKCODEGEN hKCodeGen)
{
	return NULL;
}

void KCS_API KcsMarkCodeLabel(HKCODEGEN hKCodeGen, HKCODELABEL hKCodeLabel)
{
}

KRESULT KCS_API KcsCodeEmit(HKCODEGEN hKCodeGen, KOPCODES opcode)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitChar(HKCODEGEN hKCodeGen, KOPCODES opcode, kchar_t ch)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitI1(HKCODEGEN hKCodeGen, KOPCODES opcode, int8_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitU1(HKCODEGEN hKCodeGen, KOPCODES opcode, uint8_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitI2(HKCODEGEN hKCodeGen, KOPCODES opcode, int16_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitU2(HKCODEGEN hKCodeGen, KOPCODES opcode, uint16_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitI4(HKCODEGEN hKCodeGen, KOPCODES opcode, int32_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitU4(HKCODEGEN hKCodeGen, KOPCODES opcode, uint32_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitI8(HKCODEGEN hKCodeGen, KOPCODES opcode, int64_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitU8(HKCODEGEN hKCodeGen, KOPCODES opcode, uint64_t val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitR4(HKCODEGEN hKCodeGen, KOPCODES opcode, float val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitR8(HKCODEGEN hKCodeGen, KOPCODES opcode, double val)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitString(HKCODEGEN hKCodeGen, KOPCODES opcode, kstring_t val, kuint_t length)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitLocal(HKCODEGEN hKCodeGen, KOPCODES opcode, HKLOCALBUILDER hKLocalBuilder)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitJump(HKCODEGEN hKCodeGen, KOPCODES opcode, HKCODELABEL hKCodeLabel)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitClass(HKCODEGEN hKCodeGen, KOPCODES opcode, HKCLASS hKClass)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitDelegate(HKCODEGEN hKCodeGen, KOPCODES opcode, HKDELEGATE hKDelegate)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitField(HKCODEGEN hKCodeGen, KOPCODES opcode, HKFIELD hKField)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitMethod(HKCODEGEN hKCodeGen, KOPCODES opcode, HKMETHOD hKMethod)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitType(HKCODEGEN hKCodeGen, KOPCODES opcode, HKTYPE hKType)
{
	return KRESULT_OK;
}

KRESULT KCS_API KcsCodeEmitEnter(HKCODEGEN hKCodeGen, HKTYPE hExcType, HKCODELABEL hHandlerLabel, HKCODELABEL hLeaveLabel)
{
	return KRESULT_OK;
}
