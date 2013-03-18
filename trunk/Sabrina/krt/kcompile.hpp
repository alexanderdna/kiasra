#pragma once

#include "kcs.h"
#include "kmeta.hpp"

#include <map>
#include <vector>

class TypeTree;
class CodeGen;

struct ModuleBuilder;
struct ClassBuilder;
struct DelegateBuilder;
struct FieldBuilder;
struct MethodBuilder;
struct LocalBuilder;

struct ModuleBuilder
{
public:
	ModuleDef moduleDef;

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

public:
	ModuleBuilder(KMODULEATTRIBUTES attrs, KMODULETYPES type);
	~ModuleBuilder(void);

	const TypeDef * createType(KTYPETAG tag, kushort_t dim, HKUSERTYPE classOrDelegate);

	ClassBuilder * defineClass(KCLASSATTRIBUTES attrs, kstring_t name);
	DelegateBuilder * defineDelegate(KCLASSATTRIBUTES attrs, kstring_t name,
		const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount);

	void addField(FieldBuilder *builder);
	void addMethod(MethodBuilder *builder);
	
	void addParam(ParamDef *param);
	void addDelegateParam(ParamDef *param);

	void addLocal(LocalBuilder *builder);

	bool bake(void);
};

struct ClassBuilder : public ClassDef
{
	friend struct ModuleBuilder;

protected:
	ModuleBuilder *moduleBuilder;

	std::vector<FieldBuilder *>  *fieldBuilderList;
	std::vector<MethodBuilder *> *methodBuilderList;

public:
	ClassBuilder(ModuleBuilder *moduleBuilder, KCLASSATTRIBUTES attrs, kstring_t name);
	~ClassBuilder(void);

	FieldBuilder * defineField(KFIELDATTRIBUTES attrs, kstring_t name, const TypeDef *declType);

	MethodBuilder * defineMethod(KMETHODATTRIBUTES attrs, kstring_t name,
		const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount);
};

struct DelegateBuilder : public DelegateDef
{
	friend struct ModuleBuilder;

protected:
	ModuleBuilder *moduleBuilder;
	
public:
	DelegateBuilder(ModuleBuilder *moduleBuilder, KCLASSATTRIBUTES attrs, kstring_t name,
		const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount);
};

struct FieldBuilder : public FieldDef
{
	friend struct ModuleBuilder;
	friend struct ClassBuilder;

protected:
	ClassBuilder *classBuilder;

	ktoken16_t globalIndex; // position in module

public:
	FieldBuilder(ClassBuilder *classBuilder, KFIELDATTRIBUTES attrs, kstring_t name, const TypeDef *declType);
};

struct MethodBuilder : public MethodDef
{
	friend struct ModuleBuilder;
	friend struct ClassBuilder;

protected:
	ClassBuilder *classBuilder;
	CodeGen *codeGen;

	ktoken16_t globalIndex; // position in module

	std::vector<LocalBuilder *> localBuilderList;

public:
	MethodBuilder(ClassBuilder *classBuilder, KMETHODATTRIBUTES attrs, kstring_t name,
		const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount);

	LocalBuilder * declareLocal(const TypeDef *declType);
};

struct LocalBuilder
{
	const TypeDef *declType;
	
	kushort_t  localIndex;  // position in method
	ktoken16_t globalIndex; // position in module
};
