#include "kcompile.hpp"

#include "kcs.h"
#include "kmeta.hpp"
#include "kenv.hpp"
#include "ktypetree.hpp"
#include "kstringutils.hpp"

#include <map>
#include <vector>

//===================================================

const TypeDef * ModuleBuilder::createType(KTYPETAG tag, kushort_t dim, HKUSERTYPE classOrDelegate)
{
	TypeDef * typeDef = this->typeTree->add(tag, dim, classOrDelegate);
	
	typeDef->localIndex = (ktoken32_t)(this->typeList->size());
	this->typeList->push_back(typeDef);

	if ((tag & KT_SCALAR_MASK) == KT_CLASS)
	{
		ClassDef *cls = (ClassDef *)classOrDelegate;
		if (cls->size == sizeof(ClassDef))
		{
			// external class

			std::map<ClassDef *, kushort_t>::const_iterator it = this->importedClassMap->find(cls);
			if (it == this->importedClassMap->end())
			{
				this->importedClassMap->operator[](cls) = (kushort_t)this->classList->size();
				this->classList->push_back(cls);
			}
		}
	}
	else if ((tag & KT_SCALAR_MASK) == KT_DELEGATE)
	{
		DelegateDef *del = (DelegateDef *)classOrDelegate;
		if (del->size == sizeof(DelegateDef))
		{
			// external delegate

			std::map<DelegateDef *, kushort_t>::const_iterator it = this->importedDelegateMap->find(del);
			if (it == this->importedDelegateMap->end())
			{
				this->importedDelegateMap->operator[](del) = (kushort_t)this->delegateList->size();
				this->delegateList->push_back(del);
			}
		}
	}

	return typeDef;
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

bool ModuleBuilder::bake(void)
{
	return true;
}

//===================================================

ClassBuilder::ClassBuilder(ModuleBuilder *moduleBuilder, KCLASSATTRIBUTES attrs, kstring_t name)
{
	this->moduleBuilder = moduleBuilder;
	this->attrs = attrs;
	this->name = name;

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
	
	this->fieldBuilderList->push_back(builder);

	this->moduleBuilder->addField(builder);
	return builder;
}

MethodBuilder * ClassBuilder::defineMethod(KMETHODATTRIBUTES attrs, kstring_t name,
	const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount)
{
	MethodBuilder *builder = new MethodBuilder(this, attrs, name, returnType, params, paramCount);

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
	//this->paramList
	this->paramCount = paramCount;
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
{
	this->classBuilder = classBuilder;
	this->attrs = attrs;
	this->name = name;
	this->returnType = returnType;
	//this->paramList
	this->paramCount = paramCount;
}
