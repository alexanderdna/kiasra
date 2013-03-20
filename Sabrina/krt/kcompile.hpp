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
	KMODULEATTRIBUTES attrs;

	struct
	{
		uint32_t   rowCount;
		kuint_t   *lengths;
		kstring_t *rows;
	} stringTable;

	struct
	{
		uint32_t rowCount;
		MetaTypeDef *rows;
	} typeTable;

	struct
	{
		uint16_t rowCount;
		MetaModuleDef *rows;
	} moduleTable;

	struct
	{
		uint16_t rowCount;
		MetaClassDef *rows;
	} classTable;

	struct
	{
		uint16_t rowCount;
		MetaDelegateDef *rows;
	} delegateTable;

	struct
	{
		uint32_t rowCount;
		MetaFieldDef *rows;
	} fieldTable;

	struct
	{
		uint32_t rowCount;
		MetaMethodDef *rows;
	} methodTable;

	struct
	{
		uint32_t rowCount;
		MetaParamDef *rows;
	} paramTable;

	struct
	{
		uint32_t rowCount;
		MetaParamDef *rows;
	} dparamTable;

	struct
	{
		uint32_t rowCount;
		ktoken32_t *rows;
	} localTable;

	ktoken16_t entryClassToken;
	ktoken16_t entryMethodToken;

	unsigned char *codeStream;
	kuint_t codeSize;

	kuint_t binarySize; // size of binary file

protected:
	bool baked;
	bool native;
	KMODULETYPES type;

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

protected:
	bool validateBaking(void);
	void bakeBinary(void);

public:
	ModuleBuilder(bool isNative, KMODULETYPES type);
	~ModuleBuilder(void);

	const unsigned char * getCodeStream(kuint_t &codeSize);

	bool isBaked(void);
	bool isNative(void);

	bool bake(void);

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

	bool setEntryPoint(MethodBuilder *method);
};

struct ClassBuilder : public ClassDef
{
	ModuleBuilder *moduleBuilder;

	kuint_t relativeAddr;
	kuint_t codeSize;
	bool baked;

	std::vector<FieldBuilder *>  *fieldBuilderList;
	std::vector<MethodBuilder *> *methodBuilderList;

	ClassBuilder(ModuleBuilder *moduleBuilder, KCLASSATTRIBUTES attrs, kstring_t name);
	~ClassBuilder(void);

	FieldBuilder * defineField(KFIELDATTRIBUTES attrs, kstring_t name, const TypeDef *declType);

	MethodBuilder * defineMethod(KMETHODATTRIBUTES attrs, kstring_t name,
		const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount);

	bool bake(void);
};

struct DelegateBuilder : public DelegateDef
{
	ModuleBuilder *moduleBuilder;
	
	DelegateBuilder(ModuleBuilder *moduleBuilder, KCLASSATTRIBUTES attrs, kstring_t name,
		const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount);

	~DelegateBuilder(void);
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
	
	~MethodBuilder(void);

	LocalBuilder * declareLocal(const TypeDef *declType);

	CodeGen * getCodeGenerator(void);
};

struct LocalBuilder
{
	const TypeDef *declType;
	
	kushort_t  localIndex;  // position in method
	ktoken16_t globalIndex; // position in module
};
