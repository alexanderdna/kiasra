#include "kcompile.hpp"

#include "kcs.h"
#include "kcodegen.hpp"
#include "kstringpool.hpp"
#include "kmeta.hpp"
#include "kenv.hpp"
#include "ktypetree.hpp"
#include "kstringutils.hpp"

#include <map>
#include <vector>

//===================================================

bool ModuleBuilder::bake(void)
{
	return true;
}

void ModuleBuilder::setEntryPoint(MethodBuilder *method)
{
	this->entryPoint = method;
}

ModuleDef * ModuleBuilder::getModule(void)
{
	return this->moduleDef;
}

//===================================================

ModuleBuilder::ModuleBuilder(KMODULEATTRIBUTES attrs, KMODULETYPES type)
	: moduleDef(NULL), isBaked(false), attrs(attrs), mdlType(type), entryPoint(NULL)
{
	this->importedModuleMap = new std::map<ModuleDef *, kushort_t>;
	this->importedClassMap = new std::map<ClassDef *, kushort_t>;
	this->importedDelegateMap = new std::map<DelegateDef *, kushort_t>;

	this->moduleList = new std::vector<ModuleDef *>;
	this->classList = new std::vector<ClassDef *>;
	this->delegateList = new std::vector<DelegateDef *>;

	this->fieldList = new std::vector<FieldBuilder *>;
	this->methodList = new std::vector<MethodBuilder *>;

	this->dparamList = new std::vector<ParamDef *>;
	this->paramList = new std::vector<ParamDef *>;

	this->localList = new std::vector<LocalBuilder *>;

	this->typeTree = new TypeTree;
	this->typeList = new std::vector<const TypeDef *>;
}

ModuleBuilder::~ModuleBuilder(void)
{
}

const TypeDef * ModuleBuilder::createType(KTYPETAG tag, kushort_t dim, HKUSERTYPE classOrDelegate)
{
	TypeDef * typeDef = this->typeTree->add(tag, dim, classOrDelegate);
	
	typeDef->localIndex = (ktoken32_t)(this->typeList->size());
	this->typeList->push_back(typeDef);

	if ((tag & KT_SCALAR_MASK) == KT_CLASS)
	{
		ClassDef *cls = (ClassDef *)classOrDelegate;
		if (cls->size == sizeof(ClassDef))
			this->referenceClass(cls);
	}
	else if ((tag & KT_SCALAR_MASK) == KT_DELEGATE)
	{
		DelegateDef *del = (DelegateDef *)classOrDelegate;
		if (del->size == sizeof(DelegateDef))
			this->referenceDelegate(del);
	}

	return typeDef;
}

void ModuleBuilder::referenceClass(ClassDef *classDef)
{
	std::map<ClassDef *, kushort_t>::const_iterator it = this->importedClassMap->find(classDef);
	if (it == this->importedClassMap->end())
	{
		this->importedClassMap->operator[](classDef) = (kushort_t)this->classList->size();
		this->classList->push_back(classDef);

		this->referenceModule(classDef->module);
	}
}

void ModuleBuilder::referenceDelegate(DelegateDef *delegateDef)
{
	std::map<DelegateDef *, kushort_t>::const_iterator it = this->importedDelegateMap->find(delegateDef);
	if (it == this->importedDelegateMap->end())
	{
		this->importedDelegateMap->operator[](delegateDef) = (kushort_t)this->delegateList->size();
		this->delegateList->push_back(delegateDef);

		this->referenceModule(delegateDef->module);
	}
}

void ModuleBuilder::referenceModule(ModuleDef *moduleDef)
{
	std::map<ModuleDef *, kushort_t>::const_iterator it = this->importedModuleMap->find(moduleDef);
	if (it == this->importedModuleMap->end())
	{
		this->moduleList->push_back(moduleDef);
		this->importedModuleMap->operator[](moduleDef) = (kushort_t)this->moduleList->size();
	}
}

ClassBuilder * ModuleBuilder::defineClass(KCLASSATTRIBUTES attrs, kstring_t name)
{
	ClassBuilder *builder = new ClassBuilder(this, attrs, name);

	this->classList->push_back(builder);
	builder->localIndex = (ktoken16_t)this->classList->size();

	return builder;
}

DelegateBuilder * ModuleBuilder::defineDelegate(KCLASSATTRIBUTES attrs, kstring_t name,
                                                const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount)
{
	DelegateBuilder *builder = new DelegateBuilder(this, attrs, name, returnType, params, paramCount);

	this->delegateList->push_back(builder);
	builder->localIndex = (ktoken16_t)this->delegateList->size();

	return builder;
}

void ModuleBuilder::addField(FieldBuilder *builder)
{
	this->fieldList->push_back(builder);
	builder->globalIndex = (ktoken16_t)this->fieldList->size();
}

void ModuleBuilder::addMethod(MethodBuilder *builder)
{
	this->methodList->push_back(builder);
	builder->globalIndex = (ktoken16_t)this->methodList->size();
}

void ModuleBuilder::addParam(ParamDef *param)
{
	this->paramList->push_back(param);
	param->globalIndex = (ktoken16_t)this->paramList->size();
}

void ModuleBuilder::addDelegateParam(ParamDef *param)
{
	this->dparamList->push_back(param);
	param->globalIndex = (ktoken16_t)this->dparamList->size();
}

void ModuleBuilder::addLocal(LocalBuilder *builder)
{
	this->localList->push_back(builder);
	builder->globalIndex = (ktoken16_t)this->localList->size();
}

ktoken32_t ModuleBuilder::addString(kstring_t s, kuint_t length)
{
	return this->stringPool->addString(s, length);
}

//===================================================

ClassBuilder::ClassBuilder(ModuleBuilder *moduleBuilder, KCLASSATTRIBUTES attrs, kstring_t name)
{
	this->moduleBuilder = moduleBuilder;
	this->attrs = attrs;
	this->name = name;
	this->fieldCount = 0;
	this->iFieldCount = 0;
	this->sFieldCount = 0;
	this->methodCount = 0;
	this->fieldList = NULL;
	this->iFieldList = NULL;
	this->sFieldList = NULL;
	this->methodList = NULL;

	this->fieldBuilderList = new std::vector<FieldBuilder *>;
	this->methodBuilderList = new std::vector<MethodBuilder *>;
}

ClassBuilder::~ClassBuilder(void)
{
	if (this->fieldBuilderList)
	{
		delete []this->fieldBuilderList;
		this->fieldBuilderList = NULL;
	}

	if (this->methodBuilderList)
	{
		delete []this->methodBuilderList;
		this->methodBuilderList = NULL;
	}
}

FieldBuilder * ClassBuilder::defineField(KFIELDATTRIBUTES attrs, kstring_t name, const TypeDef *declType)
{
	FieldBuilder *builder = new FieldBuilder(this, attrs, name, declType);

	++this->fieldCount;
	if (builder->attrs & KMA_STATIC)
		builder->localIndex = this->sFieldCount++;
	else
		builder->localIndex = this->iFieldCount++;
	
	this->fieldBuilderList->push_back(builder);

	this->moduleBuilder->addField(builder);
	return builder;
}

MethodBuilder * ClassBuilder::defineMethod(KMETHODATTRIBUTES attrs, kstring_t name,
	const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount)
{
	MethodBuilder *builder = new MethodBuilder(this, attrs, name, returnType, params, paramCount);

	builder->localIndex = this->methodCount++;

	this->methodBuilderList->push_back(builder);

	this->moduleBuilder->addMethod(builder);
	return builder;
}

//===================================================

DelegateBuilder::DelegateBuilder(ModuleBuilder *moduleBuilder, KCLASSATTRIBUTES attrs, kstring_t name,
	const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount)
{
	this->moduleBuilder = moduleBuilder;
	this->attrs = attrs;
	this->name = name;
	this->returnType = returnType;
	this->paramCount = paramCount;

	ParamDef **paramList = new ParamDef*[paramCount];
	for (kushort_t i = 0; i < paramCount; ++i)
	{
		const KPARAMINFO &paramInfo = params[i];
		ParamDef *param = new ParamDef;
		param->name = paramInfo.szName;
		param->declType = (TypeDef *)paramInfo.declType;
		param->byRef = paramInfo.byRef != 0;
		this->moduleBuilder->addDelegateParam(param);
		paramList[i] = param;
	}
	this->paramList = paramList;
}

//===================================================

FieldBuilder::FieldBuilder(ClassBuilder *classBuilder, KFIELDATTRIBUTES attrs, kstring_t name, const TypeDef *declType)
{
	this->classBuilder = classBuilder;
	this->attrs = attrs;
	this->name = name;
	this->declType = declType;
}

//===================================================

MethodBuilder::MethodBuilder(ClassBuilder *classBuilder, KMETHODATTRIBUTES attrs, kstring_t name,
	const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount)
	: addr(NULL)
{
	this->classBuilder = classBuilder;
	this->attrs = attrs;
	this->name = name;
	this->returnType = returnType;
	this->paramCount = paramCount;
	this->localCount = 0;
	this->localList = NULL;

	ParamDef **paramList = new ParamDef*[paramCount];
	for (kushort_t i = 0; i < paramCount; ++i)
	{
		const KPARAMINFO &paramInfo = params[i];
		ParamDef *param = new ParamDef;
		param->name = paramInfo.szName;
		param->declType = (TypeDef *)paramInfo.declType;
		param->byRef = paramInfo.byRef != 0;
		this->classBuilder->moduleBuilder->addParam(param);
		paramList[i] = param;
	}
	this->paramList = paramList;
}

LocalBuilder * MethodBuilder::declareLocal(const TypeDef *declType)
{
	LocalBuilder *builder = new LocalBuilder;
	builder->declType = declType;
	builder->localIndex = this->localCount++;
	this->classBuilder->moduleBuilder->addLocal(builder);

	return builder;
}

CodeGen * MethodBuilder::getCodeGenerator(void)
{
	if (this->codeGen)
	{
		return this->codeGen;
	}
	else
	{
		CodeGen *codeGen = new CodeGen(this);
		this->codeGen = codeGen;
		return codeGen;
	}
}
