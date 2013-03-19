#pragma once

#include "kcs.h"
#include "kmeta.hpp"

#include <map>
#include <vector>

class TypeTree;
class CodeGen;

class ModuleBuilder;
class StringPool;

struct ClassBuilder;
struct DelegateBuilder;
struct FieldBuilder;
struct MethodBuilder;
struct LocalBuilder;

class ModuleBuilder
{
	friend class CodeGen;

protected:
	ModuleDef *moduleDef;
	bool isBaked;

	KMODULEATTRIBUTES attrs;
	KMODULETYPES mdlType;

	StringPool *stringPool;

	std::map<ModuleDef *, kushort_t>   *importedModuleMap;   // map<module reference, index in module list>
	std::map<ClassDef *, kushort_t>    *importedClassMap;    // map<class reference, index in class list>
	std::map<DelegateDef *, kushort_t> *importedDelegateMap; // map<delegate reference, index in delegate list>

	std::vector<ModuleDef *>     *moduleList;   // imported module list
	std::vector<ClassDef *>      *classList;    // class list, including internal (ClassBuilder) and external (ClassDef)
	std::vector<DelegateDef *>   *delegateList; // delegate list, including internal (DelegateBuilder) and external (DelegateDef)

	std::vector<FieldBuilder *>  *fieldList;    // field list, internal only
	std::vector<MethodBuilder *> *methodList;   // method list, internal only

	std::vector<ParamDef *>      *paramList;    // method parameter list
	std::vector<ParamDef *>      *dparamList;   // delegate parameter list

	std::vector<LocalBuilder *>  *localList;    // local variable list

	TypeTree                     *typeTree;     // unique TypeDef entries
	std::vector<const TypeDef *> *typeList;

	const MethodBuilder *entryPoint;

public:
	ModuleBuilder(KMODULEATTRIBUTES attrs, KMODULETYPES type);
	~ModuleBuilder(void);

	const TypeDef * createType(KTYPETAG tag, kushort_t dim, HKUSERTYPE classOrDelegate);

	void referenceClass(ClassDef *classDef);
	void referenceDelegate(DelegateDef *delegateDef);
	void referenceModule(ModuleDef *moduleDef);

	ClassBuilder * defineClass(KCLASSATTRIBUTES attrs, kstring_t name);
	DelegateBuilder * defineDelegate(KCLASSATTRIBUTES attrs, kstring_t name,
		const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount);

	void addField(FieldBuilder *builder);
	void addMethod(MethodBuilder *builder);
	
	void addParam(ParamDef *param);
	void addDelegateParam(ParamDef *param);

	void addLocal(LocalBuilder *builder);

	ktoken32_t addString(kstring_t s, kuint_t length);

	void setEntryPoint(MethodBuilder *method);
	ModuleDef * getModule(void);

	bool bake(void);
};

struct ClassBuilder : public ClassDef
{
	ModuleBuilder *moduleBuilder;

	std::vector<FieldBuilder *>  *fieldBuilderList;
	std::vector<MethodBuilder *> *methodBuilderList;

	ClassBuilder(ModuleBuilder *moduleBuilder, KCLASSATTRIBUTES attrs, kstring_t name);
	~ClassBuilder(void);

	FieldBuilder * defineField(KFIELDATTRIBUTES attrs, kstring_t name, const TypeDef *declType);

	MethodBuilder * defineMethod(KMETHODATTRIBUTES attrs, kstring_t name,
		const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount);
};

struct DelegateBuilder : public DelegateDef
{
	ModuleBuilder *moduleBuilder;
	
	DelegateBuilder(ModuleBuilder *moduleBuilder, KCLASSATTRIBUTES attrs, kstring_t name,
		const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount);
};

struct FieldBuilder : public FieldDef
{
	ClassBuilder *classBuilder;

	ktoken16_t globalIndex; // position in module

	FieldBuilder(ClassBuilder *classBuilder, KFIELDATTRIBUTES attrs, kstring_t name, const TypeDef *declType);
};

struct MethodBuilder : public MethodDef
{
	ClassBuilder *classBuilder;
	CodeGen *codeGen;

	ktoken16_t globalIndex; // position in module

	std::vector<LocalBuilder *> localBuilderList;

	kuint_t addr;

	MethodBuilder(ClassBuilder *classBuilder, KMETHODATTRIBUTES attrs, kstring_t name,
		const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount);

	LocalBuilder * declareLocal(const TypeDef *declType);

	CodeGen * getCodeGenerator(void);
};

struct LocalBuilder
{
	const TypeDef *declType;
	
	kushort_t  localIndex;  // position in method
	ktoken16_t globalIndex; // position in module
};
