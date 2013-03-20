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
#include <cstring>

extern KCSERRORS lastCompileError;

//===================================================

bool ModuleBuilder::bake(void)
{
	if (!this->validateBaking())
		return false;

	kuint_t binarySize = 0;

	StringPool *stringPool = this->stringPool;

	// type table

	decltype(this->typeList) typeList = this->typeList;
	kuint_t typeCount = (kuint_t)typeList->size();
	MetaTypeDef *typeRows = new MetaTypeDef[typeCount];

	this->typeTable.rowCount = (uint32_t)typeCount;
	this->typeTable.rows = typeRows;

	binarySize += sizeof(kuint_t); //count

	for (kuint_t i = 0; i < typeCount; ++i)
	{
		MetaTypeDef &row = typeRows[i];
		const TypeDef *def = typeList->operator[](i);

		row.tag = def->tag;
		row.dim = def->dim;

		if (def->cls)
		{
			if ((def->tag & KT_SCALAR_MASK) == KT_CLASS)
			{
				ClassDef *cls = (ClassDef *)def->cls;
				if (cls->size == sizeof(ClassDef))
					row.tok = this->importedClassMap->operator[](cls);
				else
					row.tok = cls->localIndex;
			}
			else
			{
				DelegateDef *del = (DelegateDef *)def->del;
				if (del->size == sizeof(DelegateDef))
					row.tok = this->importedDelegateMap->operator[](del);
				else
					row.tok = del->localIndex;
			}

			binarySize += sizeof(kushort_t)+sizeof(kushort_t)+sizeof(ktoken16_t);
		}
		else
		{
			row.tok = 0;
			binarySize += sizeof(kushort_t)+sizeof(kushort_t);
		}
	}

	// module table

	decltype(this->moduleList) moduleList = this->moduleList;
	kuint_t moduleCount = (kuint_t)moduleList->size();
	MetaModuleDef *moduleRows = new MetaModuleDef[moduleCount];

	this->moduleTable.rowCount = (uint16_t)moduleCount;
	this->moduleTable.rows = moduleRows;

	//count + count * (attrs+path+hash)
	binarySize += sizeof(kushort_t) + moduleCount * (sizeof(uint16_t)+sizeof(ktoken32_t)+sizeof(uint32_t));

	for (kuint_t i = 0; i < moduleCount; ++i)
	{
		MetaModuleDef &meta = moduleRows[i];
		ModuleDef *def = moduleList->operator[](i);

		meta.attrs = def->attrs;
		meta.path = stringPool->addString(def->path, (kuint_t)wcslen(def->path));
		meta.hash = 0;
	}

	// class table

	decltype(this->classList) classList = this->classList;
	kuint_t classCount = (kuint_t)classList->size();
	MetaClassDef *classRows = new MetaClassDef[classCount];

	this->classTable.rowCount = (uint16_t)classCount;
	this->classTable.rows = classRows;

	binarySize += sizeof(kushort_t); //count

	for (kuint_t i = 0; i < classCount; ++i)
	{
		MetaClassDef &meta = classRows[i];
		ClassDef *def = classList->operator[](i);

		meta.attrs = def->attrs;
		meta.name = stringPool->addString(def->name, (kuint_t)wcslen(def->name));
		
		if (def->size == sizeof(ClassDef))
		{
			meta.moduleIndex = (ktoken16_t)this->importedModuleMap->operator[](def->module);
			meta.farIndex = def->localIndex;

			//attrs+name+moduleIndex+farIndex
			binarySize += sizeof(uint16_t)+sizeof(ktoken32_t)+sizeof(ktoken16_t)+sizeof(ktoken16_t);
		}
		else
		{
			ClassBuilder *builder = (ClassBuilder *)def;
			
			if (builder->fieldCount)
				meta.fieldList = (ktoken32_t)((*builder->fieldBuilderList->begin())->globalIndex);
			else
				meta.fieldList = 0;

			if (builder->methodCount)
				meta.methodList = (ktoken32_t)((*builder->methodBuilderList->begin())->globalIndex);
			else
				meta.methodList = 0;

			meta.moduleIndex = 0;

			//attrs+name+moduleIndex+fieldList+methodList
			binarySize += sizeof(uint16_t)+sizeof(ktoken32_t)+sizeof(ktoken32_t)+sizeof(ktoken32_t);
		}
	}

	// delegate table

	decltype(this->delegateList) delegateList = this->delegateList;
	kuint_t delegateCount = (kuint_t)delegateList->size();
	MetaDelegateDef *delegateRows = new MetaDelegateDef[delegateCount];

	this->delegateTable.rowCount = delegateCount;
	this->delegateTable.rows = delegateRows;

	binarySize += sizeof(kushort_t); //count

	for (kuint_t i = 0; i < delegateCount; ++i)
	{
		MetaDelegateDef &meta = delegateRows[i];
		DelegateDef *def = delegateList->operator[](i);

		meta.attrs = def->attrs;
		meta.name = stringPool->addString(def->name, (kuint_t)wcslen(def->name));
		
		if (def->size == sizeof(DelegateDef))
		{
			meta.moduleIndex = (ktoken16_t)this->importedModuleMap->operator[](def->module);
			meta.farIndex = def->localIndex;

			//attrs+name+moduleIndex+farIndex
			binarySize += sizeof(uint16_t)+sizeof(ktoken32_t)+sizeof(ktoken16_t)+sizeof(ktoken16_t);
		}
		else
		{
			DelegateBuilder *builder = (DelegateBuilder *)def;

			meta.returnType = builder->returnType->localIndex;
			if (builder->paramCount)
				meta.paramList = (ktoken32_t)(builder->paramList[0]->globalIndex);
			else
				meta.paramList = 0;

			meta.moduleIndex = 0;

			//attrs+name+moduleIndex+returnType+paramList
			binarySize += sizeof(uint16_t)+sizeof(ktoken32_t)+sizeof(ktoken32_t)+sizeof(ktoken32_t);
		}
	}

	// field table

	decltype(this->fieldList) fieldList = this->fieldList;
	kuint_t fieldCount = (kuint_t)fieldList->size();
	MetaFieldDef *fieldRows = new MetaFieldDef[fieldCount];

	this->fieldTable.rowCount = fieldCount;
	this->fieldTable.rows = fieldRows;

	//count + count * (attrs+name+declType)
	binarySize += sizeof(kuint_t) + fieldCount * (sizeof(uint16_t)+sizeof(ktoken32_t)+sizeof(ktoken32_t));

	for (kuint_t i = 0; i < fieldCount; ++i)
	{
		MetaFieldDef &meta = fieldRows[i];
		FieldBuilder *builder = fieldList->operator[](i);

		meta.attrs = builder->attrs;
		meta.name = stringPool->addString(builder->name, (kuint_t)wcslen(builder->name));
		meta.declType = builder->declType->localIndex;
	}

	// method table

	decltype(this->methodList) methodList = this->methodList;
	kuint_t methodCount = (kuint_t)methodList->size();
	MetaMethodDef *methodRows = new MetaMethodDef[methodCount];

	this->methodTable.rowCount = methodCount;
	this->methodTable.rows = methodRows;

	//count + count * (attrs+name+returnType+paramList+localList+addr)
	binarySize += sizeof(kuint_t) + methodCount *
		(sizeof(uint16_t)+sizeof(ktoken32_t)+sizeof(ktoken32_t)+sizeof(ktoken32_t)+sizeof(ktoken32_t)+sizeof(kuint_t));

	for (kuint_t i = 0; i < methodCount; ++i)
	{
		MetaMethodDef &meta = methodRows[i];
		MethodBuilder *builder = methodList->operator[](i);

		meta.attrs = builder->attrs;
		meta.name = stringPool->addString(builder->name, (kuint_t)wcslen(builder->name));
		meta.returnType = builder->returnType->localIndex;
		
		if (builder->paramCount)
			meta.paramList = (ktoken32_t)builder->paramList[0]->globalIndex;
		else
			meta.paramList = 0;

		if (builder->localCount)
			meta.localList = (ktoken32_t)builder->localBuilderList[0]->globalIndex;
		else
			meta.localList = 0;

		meta.addr = builder->classBuilder->relativeAddr + builder->addr;
	}

	// param table

	decltype(this->paramList) paramList = this->paramList;
	kuint_t paramCount = (kuint_t)paramList->size();
	MetaParamDef *paramRows = new MetaParamDef[paramCount];

	this->paramTable.rowCount = paramCount;
	this->paramTable.rows = paramRows;

	//count + count * (name+declType+byRef)
	binarySize += sizeof(kuint_t) + paramCount * (sizeof(ktoken32_t)+sizeof(ktoken32_t)+sizeof(uint8_t));

	for (kuint_t i = 0; i < paramCount; ++i)
	{
		MetaParamDef &meta = paramRows[i];
		ParamDef *def = paramList->operator[](i);

		meta.name = stringPool->addString(def->name, (kuint_t)wcslen(def->name));
		meta.declType = def->declType->localIndex;
		meta.byRef = def->byRef ? 1 : 0;
	}

	// delegate param table

	decltype(this->dparamList) dparamList = this->dparamList;
	kuint_t dparamCount = (kuint_t)dparamList->size();
	MetaParamDef *dparamRows = new MetaParamDef[dparamCount];

	this->dparamTable.rowCount = dparamCount;
	this->dparamTable.rows = dparamRows;

	//count + count * (name+declType+byRef)
	binarySize += sizeof(kuint_t) + dparamCount * (sizeof(ktoken32_t)+sizeof(ktoken32_t)+sizeof(uint8_t));

	for (kuint_t i = 0; i < dparamCount; ++i)
	{
		MetaParamDef &meta = dparamRows[i];
		ParamDef *def = dparamList->operator[](i);

		meta.name = stringPool->addString(def->name, (kuint_t)wcslen(def->name));
		meta.declType = def->declType->localIndex;
		meta.byRef = def->byRef ? 1 : 0;
	}

	// local table

	decltype(this->localList) localList = this->localList;
	kuint_t localCount = (kuint_t)localList->size();
	ktoken32_t *localRows = new ktoken32_t[localCount];

	this->localTable.rowCount = localCount;
	this->localTable.rows = localRows;

	binarySize += sizeof(kuint_t) + localCount * sizeof(ktoken32_t); //tok

	for (kuint_t i = 0; i < localCount; ++i)
		localRows[i] = localList->operator[](i)->declType->localIndex;

	// string table

	std::vector<StringPool::StringInfo> *stringList = stringPool->getStringList();
	kuint_t stringCount = (kuint_t)stringList->size();
	kstring_t *stringRows = new kstring_t[stringCount];
	kuint_t *lengthRows = new kuint_t[stringCount];

	this->stringTable.rowCount = stringCount;
	this->stringTable.rows = stringRows;
	this->stringTable.lengths = lengthRows;

	binarySize += sizeof(kuint_t); //count

	for (kuint_t i = 0; i < stringCount; ++i)
	{
		const StringPool::StringInfo &si = stringList->operator[](i);
		stringRows[i] = si.str;
		lengthRows[i] = si.len;

		binarySize += sizeof(kuint_t) + si.len * sizeof(kchar_t); //len + lengthof(str)
	}

	// now bake the binary representation

	this->binarySize = binarySize;
	this->bakeBinary();

	// DONE
	
	lastCompileError = KCSE_NO_ERROR;
	return true;
}

#define BINWRITE(val, t, offset) *(t *)(stream + (offset)) = (t)val

void ModuleBuilder::bakeBinary(void)
{
	// header
	unsigned char header[] =
	{
		// magic
		0xAA, 0xCE, 0xCA, 0xDE,
		// version
		0x01, 0x00,
		// type
		(unsigned char)((this->type == KMOD_WINDOW) ? 0x00 : (this->type == KMOD_CONSOLE) ? 0x01 : 0x02),
		// debug
		0x00,
		// hash
		0x00, 0x00, 0x00, 0x00,
		// entry
		(unsigned char)((this->entryClassToken & 0xFF00) >> 8),
		(unsigned char)(this->entryClassToken & 0x00FF),
		(unsigned char)((this->entryMethodToken & 0xFF00) >> 8),
		(unsigned char)(this->entryMethodToken & 0x00FF)
	};

	this->binarySize += sizeof(header) + this->codeSize;

	unsigned char *stream = new unsigned char[this->binarySize];
	kuint_t pos = 0;

	this->codeStream = stream;
	
	memcpy(stream, header, sizeof(header));
	pos += sizeof(header);

	// string table

	kuint_t stringCount = this->stringTable.rowCount;
	kstring_t *stringRows = this->stringTable.rows;
	kuint_t *lengthRows = this->stringTable.lengths;

	BINWRITE(stringCount, kuint_t, pos);
	pos += sizeof(kuint_t);

	for (kuint_t i = 0; i < stringCount; ++i)
	{
		BINWRITE(lengthRows[i], kuint_t, pos);
		pos += sizeof(kuint_t);

		memcpy(stream+pos, stringRows+i, lengthRows[i] * sizeof(kchar_t));
		pos += lengthRows[i] * sizeof(kchar_t);
	}

	// type table

	kuint_t typeCount = this->typeTable.rowCount;
	MetaTypeDef *typeRows = this->typeTable.rows;
	
	BINWRITE(typeCount, kuint_t, pos);
	pos += sizeof(kuint_t);

	for (kuint_t i = 0; i < typeCount; ++i)
	{
		const MetaTypeDef &meta = typeRows[i];

		if (meta.tok)
		{
			BINWRITE(meta.tag, kushort_t, pos);
			BINWRITE(meta.dim, kushort_t, pos + sizeof(kushort_t));
			BINWRITE(meta.tok, ktoken16_t, pos + sizeof(kuint_t) + sizeof(kushort_t));

			pos += sizeof(kushort_t) + sizeof(kushort_t) + sizeof(ktoken16_t);
		}
		else
		{
			BINWRITE(meta.tag, kushort_t, pos);
			BINWRITE(meta.dim, kushort_t, pos + sizeof(kushort_t));

			pos += sizeof(kushort_t) + sizeof(kushort_t);
		}
	}

	// module table

	kuint_t moduleCount = this->moduleTable.rowCount;
	MetaModuleDef *moduleRows = this->moduleTable.rows;

	BINWRITE(moduleCount, kushort_t, pos);
	pos += sizeof(kushort_t);

	for (kuint_t i = 0; i < moduleCount; ++i)
	{
		const MetaModuleDef &meta = moduleRows[i];
		BINWRITE(meta.attrs, uint16_t, pos);
		BINWRITE(meta.path, ktoken32_t, pos + sizeof(uint16_t));
		BINWRITE(meta.hash, uint32_t, pos + sizeof(uint16_t) + sizeof(ktoken32_t));

		pos += sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(uint32_t);
	}

	// class table

	kuint_t classCount = this->classTable.rowCount;
	MetaClassDef *classRows = this->classTable.rows;

	BINWRITE(classCount, kushort_t, pos);
	pos += sizeof(kushort_t);

	for (kuint_t i = 0; i < classCount; ++i)
	{
		const MetaClassDef &meta = classRows[i];
		BINWRITE(meta.attrs, uint16_t, pos);
		BINWRITE(meta.name, ktoken32_t, pos + sizeof(uint16_t));
		BINWRITE(meta.moduleIndex, ktoken16_t, pos + sizeof(uint16_t) + sizeof(ktoken32_t));

		if (meta.moduleIndex)
		{
			BINWRITE(meta.farIndex, ktoken16_t, pos + sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(ktoken16_t));
			pos += sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(ktoken16_t) + sizeof(ktoken16_t);
		}
		else
		{
			BINWRITE(meta.fieldList, ktoken32_t, pos + sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(ktoken16_t));
			BINWRITE(meta.methodList, ktoken32_t, pos + sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(ktoken16_t) + sizeof(ktoken32_t));
			pos += sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(ktoken16_t) + sizeof(ktoken32_t) + sizeof(ktoken32_t);
		}
	}

	// delegate table

	kuint_t delegateCount = this->delegateTable.rowCount;
	MetaDelegateDef *delegateRows = this->delegateTable.rows;

	BINWRITE(delegateCount, kushort_t, pos);
	pos += sizeof(kushort_t);

	for (kuint_t i = 0; i < delegateCount; ++i)
	{
		const MetaDelegateDef &meta = delegateRows[i];
		BINWRITE(meta.attrs, uint16_t, pos);
		BINWRITE(meta.name, ktoken32_t, pos + sizeof(uint16_t));
		BINWRITE(meta.moduleIndex, ktoken16_t, pos + sizeof(uint16_t) + sizeof(ktoken32_t));

		if (meta.moduleIndex)
		{
			BINWRITE(meta.farIndex, ktoken16_t, pos + sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(ktoken16_t));
			pos += sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(ktoken16_t) + sizeof(ktoken16_t);
		}
		else
		{
			BINWRITE(meta.returnType, ktoken32_t, pos + sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(ktoken16_t));
			BINWRITE(meta.paramList, ktoken32_t, pos + sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(ktoken16_t) + sizeof(ktoken32_t));
			pos += sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(ktoken16_t) + sizeof(ktoken32_t) + sizeof(ktoken32_t);
		}
	}

	// field table

	kuint_t fieldCount = this->fieldTable.rowCount;
	MetaFieldDef *fieldRows = this->fieldTable.rows;

	BINWRITE(fieldCount, kuint_t, pos);
	pos += sizeof(kuint_t);

	for (kuint_t i = 0; i < fieldCount; ++i)
	{
		const MetaFieldDef &meta = fieldRows[i];
		BINWRITE(meta.attrs, uint16_t, pos);
		BINWRITE(meta.name, ktoken32_t, pos + sizeof(uint16_t));
		BINWRITE(meta.declType, ktoken32_t, pos + sizeof(uint16_t) + sizeof(ktoken32_t));

		pos += sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(ktoken32_t);
	}

	// method table

	kuint_t methodCount = this->methodTable.rowCount;
	MetaMethodDef *methodRows = this->methodTable.rows;

	BINWRITE(methodCount, kuint_t, pos);
	pos += sizeof(kuint_t);

	for (kuint_t i = 0; i < methodCount; ++i)
	{
		const MetaMethodDef &meta = methodRows[i];
		BINWRITE(meta.attrs, uint16_t, pos);
		BINWRITE(meta.name, ktoken32_t, pos + sizeof(uint16_t));
		BINWRITE(meta.returnType, ktoken32_t, pos + sizeof(uint16_t) + sizeof(ktoken32_t));
		BINWRITE(meta.paramList, ktoken32_t, pos + sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(ktoken32_t));
		BINWRITE(meta.localList, ktoken32_t, pos + sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(ktoken32_t) + sizeof(ktoken32_t));

		pos += sizeof(uint16_t) + sizeof(ktoken32_t) + sizeof(ktoken32_t) + sizeof(ktoken32_t) + sizeof(ktoken32_t);
	}

	// param table

	kuint_t paramCount = this->paramTable.rowCount;
	MetaParamDef *paramRows = this->paramTable.rows;

	BINWRITE(paramCount, kuint_t, pos);
	pos += sizeof(kuint_t);

	for (kuint_t i = 0; i < paramCount; ++i)
	{
		const MetaParamDef &meta = paramRows[i];
		BINWRITE(meta.name, ktoken32_t, pos);
		BINWRITE(meta.declType, ktoken32_t, pos + sizeof(ktoken32_t));
		BINWRITE(meta.byRef, uint8_t, pos + sizeof(ktoken32_t) + sizeof(ktoken32_t));

		pos += sizeof(ktoken32_t) + sizeof(ktoken32_t) + sizeof(uint8_t);
	}

	// delegate param table

	kuint_t dparamCount = this->dparamTable.rowCount;
	MetaParamDef *dparamRows = this->dparamTable.rows;

	BINWRITE(dparamCount, kuint_t, pos);
	pos += sizeof(kuint_t);

	for (kuint_t i = 0; i < dparamCount; ++i)
	{
		const MetaParamDef &meta = dparamRows[i];
		BINWRITE(meta.name, ktoken32_t, pos);
		BINWRITE(meta.declType, ktoken32_t, pos + sizeof(ktoken32_t));
		BINWRITE(meta.byRef, uint8_t, pos + sizeof(ktoken32_t) + sizeof(ktoken32_t));

		pos += sizeof(ktoken32_t) + sizeof(ktoken32_t) + sizeof(uint8_t);
	}

	// local table

	kuint_t localCount = this->localTable.rowCount;
	ktoken32_t *localRows = this->localTable.rows;

	BINWRITE(localCount, kuint_t, pos);
	pos += sizeof(kuint_t);

	memcpy(stream+pos, localRows, localCount * sizeof(ktoken32_t));
	pos += localCount * sizeof(ktoken32_t);

	// code

	decltype(this->classList) classList = this->classList;
	for (kuint_t i = 0; i < classCount; ++i)
	{
		if (classList->operator[](i)->size == sizeof(ClassBuilder))
		{
			ClassBuilder *builder = (ClassBuilder *)classList->operator[](i);
			for (std::vector<MethodBuilder *>::const_iterator it = builder->methodBuilderList->begin();
				it != builder->methodBuilderList->end(); ++it)
			{
				MethodBuilder *mb = *it;
				if (!(mb->addr & KMA_NATIVE))
				{
					kuint_t codeSize = mb->getCodeGenerator()->getCodeSize();
					memcpy(stream+pos, mb->getCodeGenerator()->getCodeStream(), codeSize);
					pos += codeSize;
				}
			}
		}
	}
}

bool ModuleBuilder::validateBaking(void)
{
	// class completeness
	
	for (std::vector<ClassDef *>::const_iterator it = this->classList->begin();
		it != this->classList->end(); ++it)
	{
		ClassDef *cls = *it;
		if (cls->size == sizeof(ClassBuilder))
		{
			if (((ClassBuilder *)cls)->baked == false)
			{
				lastCompileError = KCSE_INCOMPLETE_MODULE;
				return false;
			}

			((ClassBuilder *)cls)->relativeAddr = this->codeSize;
			this->codeSize += ((ClassBuilder *)cls)->codeSize;
		}
	}

	// entry point

	if (this->type != KMOD_LIBRARY && this->entryPoint == NULL)
	{
		lastCompileError = KCSE_NO_ENTRY;
		return false;
	}

	lastCompileError = KCSE_NO_ERROR;
	return true;
}

bool ModuleBuilder::setEntryPoint(MethodBuilder *method)
{
	if (this->type == KMOD_LIBRARY)
	{
		lastCompileError = KCSE_ENTRY_NOT_ALLOWED;
		return false;
	}

	this->entryPoint = method;
	this->entryClassToken = method->klass->localIndex;
	this->entryMethodToken = method->localIndex;

	lastCompileError = KCSE_NO_ERROR;
	return true;
}

//===================================================

ModuleBuilder::ModuleBuilder(bool isNative, KMODULETYPES type)
	: baked(false), native(isNative), type(type), entryPoint(NULL), codeSize(0), entryClassToken(0), entryMethodToken(0)
{
	this->binarySize = 16; //header

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

	this->stringPool = new StringPool(this);

	this->codeStream = NULL;
	this->stringTable.rows = NULL;
	this->stringTable.lengths = NULL;
	this->typeTable.rows = NULL;
	this->moduleTable.rows = NULL;
	this->classTable.rows = NULL;
	this->delegateTable.rows = NULL;
	this->fieldTable.rows = NULL;
	this->methodTable.rows = NULL;
	this->paramTable.rows = NULL;
	this->dparamTable.rows = NULL;
	this->localTable.rows = NULL;
}

#define DELETE_IF_NOT_NULL(p) if (p) { delete p; p = NULL; }
#define ADELETE_IF_NOT_NULL(p) if (p) { delete p; p = NULL; }

ModuleBuilder::~ModuleBuilder(void)
{
	DELETE_IF_NOT_NULL(this->stringPool);
	DELETE_IF_NOT_NULL(this->importedModuleMap);
	DELETE_IF_NOT_NULL(this->importedClassMap);
	DELETE_IF_NOT_NULL(this->importedDelegateMap);

	if (this->moduleList)
	{
		for (std::vector<ModuleDef *>::const_iterator it = this->moduleList->begin();
			it != this->moduleList->end(); ++it)
			delete *it;

		delete this->moduleList;
		this->moduleList = NULL;
	}

	if (this->classList)
	{
		for (std::vector<ClassDef *>::const_iterator it = this->classList->begin();
			it != this->classList->end(); ++it)
		{
			if ((*it)->size == sizeof(ClassBuilder))
				delete ((ClassBuilder *)(*it));
		}

		delete this->classList;
		this->classList = NULL;
	}

	if (this->delegateList)
	{
		for (std::vector<DelegateDef *>::const_iterator it = this->delegateList->begin();
			it != this->delegateList->end(); ++it)
		{
			if ((*it)->size == sizeof(DelegateBuilder))
				delete ((DelegateBuilder *)(*it));
		}

		delete this->delegateList;
		this->delegateList = NULL;
	}

	if (this->fieldList)
	{
		for (std::vector<FieldBuilder *>::const_iterator it = this->fieldList->begin();
			it != this->fieldList->end(); ++it)
			delete *it;

		delete this->fieldList;
		this->fieldList = NULL;
	}

	if (this->methodList)
	{
		for (std::vector<MethodBuilder *>::const_iterator it = this->methodList->begin();
			it != this->methodList->end(); ++it)
			delete *it;

		delete this->methodList;
		this->methodList = NULL;
	}

	if (this->paramList)
	{
		for (std::vector<ParamDef *>::const_iterator it = this->paramList->begin();
			it != this->paramList->end(); ++it)
			delete *it;

		delete this->paramList;
		this->paramList = NULL;
	}

	if (this->dparamList)
	{
		for (std::vector<ParamDef *>::const_iterator it = this->dparamList->begin();
			it != this->dparamList->end(); ++it)
			delete *it;

		delete this->dparamList;
		this->dparamList = NULL;
	}

	DELETE_IF_NOT_NULL(this->localList);
	DELETE_IF_NOT_NULL(this->typeList);

	DELETE_IF_NOT_NULL(this->typeTree);

	ADELETE_IF_NOT_NULL(this->codeStream);
	ADELETE_IF_NOT_NULL(this->stringTable.rows);
	ADELETE_IF_NOT_NULL(this->stringTable.lengths);
	ADELETE_IF_NOT_NULL(this->typeTable.rows);
	ADELETE_IF_NOT_NULL(this->moduleTable.rows);
	ADELETE_IF_NOT_NULL(this->classTable.rows);
	ADELETE_IF_NOT_NULL(this->delegateTable.rows);
	ADELETE_IF_NOT_NULL(this->fieldTable.rows);
	ADELETE_IF_NOT_NULL(this->methodTable.rows);
	ADELETE_IF_NOT_NULL(this->paramTable.rows);
	ADELETE_IF_NOT_NULL(this->dparamTable.rows);
	ADELETE_IF_NOT_NULL(this->localTable.rows);
}

bool ModuleBuilder::isBaked(void)
{
	return this->baked;
}

bool ModuleBuilder::isNative(void)
{
	return this->native;
}

const unsigned char * ModuleBuilder::getCodeStream(kuint_t &codeSize)
{
	codeSize = this->binarySize;
	return this->codeStream;
}

const TypeDef * ModuleBuilder::createType(KTYPETAG tag, kushort_t dim, HKUSERTYPE classOrDelegate)
{
	if (this->baked)
	{
		lastCompileError = KCSE_MODULE_BAKED;
		return NULL;
	}

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
	
	lastCompileError = KCSE_NO_ERROR;
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
	if (this->baked)
	{
		lastCompileError = KCSE_MODULE_BAKED;
		return NULL;
	}

	for (std::vector<ClassDef *>::const_iterator it = this->classList->begin();
		it != this->classList->end(); ++it)
	{
		if (krt_strequ((*it)->name, name))
		{
			lastCompileError = KCSE_DUPLICATE_CLASS;
			return NULL;
		}
	}
	
	for (std::vector<DelegateDef *>::const_iterator it = this->delegateList->begin();
		it != this->delegateList->end(); ++it)
	{
		if (krt_strequ((*it)->name, name))
		{
			lastCompileError = KCSE_DUPLICATE_DELEGATE;
			return NULL;
		}
	}

	ClassBuilder *builder = new ClassBuilder(this, attrs, name);
	builder->size = sizeof(ClassBuilder);

	builder->localIndex = (ktoken16_t)this->classList->size();
	this->classList->push_back(builder);
	
	lastCompileError = KCSE_NO_ERROR;
	return builder;
}

DelegateBuilder * ModuleBuilder::defineDelegate(KCLASSATTRIBUTES attrs, kstring_t name,
                                                const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount)
{
	if (this->baked)
	{
		lastCompileError = KCSE_MODULE_BAKED;
		return NULL;
	}

	for (std::vector<DelegateDef *>::const_iterator it = this->delegateList->begin();
		it != this->delegateList->end(); ++it)
	{
		if (krt_strequ((*it)->name, name))
		{
			lastCompileError = KCSE_DUPLICATE_DELEGATE;
			return NULL;
		}
	}

	for (std::vector<ClassDef *>::const_iterator it = this->classList->begin();
		it != this->classList->end(); ++it)
	{
		if (krt_strequ((*it)->name, name))
		{
			lastCompileError = KCSE_DUPLICATE_CLASS;
			return NULL;
		}
	}

	DelegateBuilder *builder = new DelegateBuilder(this, attrs, name, returnType, params, paramCount);
	builder->size = sizeof(DelegateBuilder);

	builder->localIndex = (ktoken16_t)this->delegateList->size();
	this->delegateList->push_back(builder);
	
	lastCompileError = KCSE_NO_ERROR;
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
	: baked(false), codeSize(0)
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
	DELETE_IF_NOT_NULL(this->fieldBuilderList);
	DELETE_IF_NOT_NULL(this->methodBuilderList);
}

FieldBuilder * ClassBuilder::defineField(KFIELDATTRIBUTES attrs, kstring_t name, const TypeDef *declType)
{
	if (this->baked)
	{
		lastCompileError = KCSE_CLASS_BAKED;
		return NULL;
	}

	for (std::vector<FieldBuilder *>::const_iterator it = this->fieldBuilderList->begin();
		it != this->fieldBuilderList->end(); ++it)
	{
		if (krt_strequ((*it)->name, name))
		{
			lastCompileError = KCSE_DUPLICATE_FIELD;
			return NULL;
		}
	}
	
	for (std::vector<MethodBuilder *>::const_iterator it = this->methodBuilderList->begin();
		it != this->methodBuilderList->end(); ++it)
	{
		if (krt_strequ((*it)->name, name))
		{
			lastCompileError = KCSE_DUPLICATE_METHOD;
			return NULL;
		}
	}

	FieldBuilder *builder = new FieldBuilder(this, attrs, name, declType);
	builder->size = sizeof(FieldBuilder);

	++this->fieldCount;
	if (builder->attrs & KMA_STATIC)
		builder->localIndex = this->sFieldCount++;
	else
		builder->localIndex = this->iFieldCount++;
	
	this->fieldBuilderList->push_back(builder);

	this->moduleBuilder->addField(builder);
	
	lastCompileError = KCSE_NO_ERROR;
	return builder;
}

MethodBuilder * ClassBuilder::defineMethod(KMETHODATTRIBUTES attrs, kstring_t name,
	const TypeDef *returnType, const KPARAMINFO *params, kushort_t paramCount)
{
	if (this->baked)
	{
		lastCompileError = KCSE_CLASS_BAKED;
		return NULL;
	}

	for (std::vector<MethodBuilder *>::const_iterator it = this->methodBuilderList->begin();
		it != this->methodBuilderList->end(); ++it)
	{
		if (krt_strequ((*it)->name, name))
		{
			lastCompileError = KCSE_DUPLICATE_METHOD;
			return NULL;
		}
	}
	
	for (std::vector<FieldBuilder *>::const_iterator it = this->fieldBuilderList->begin();
		it != this->fieldBuilderList->end(); ++it)
	{
		if (krt_strequ((*it)->name, name))
		{
			lastCompileError = KCSE_DUPLICATE_FIELD;
			return NULL;
		}
	}

	if (!this->moduleBuilder->isNative() && (attrs & KMA_NATIVE))
	{
		lastCompileError = KCSE_NATIVE_NOT_ALLOWED;
		return NULL;
	}
	
	MethodBuilder *builder = new MethodBuilder(this, attrs, name, returnType, params, paramCount);
	builder->size = sizeof(MethodBuilder);

	builder->localIndex = this->methodCount++;

	this->methodBuilderList->push_back(builder);

	this->moduleBuilder->addMethod(builder);
	
	lastCompileError = KCSE_NO_ERROR;
	return builder;
}

bool ClassBuilder::bake(void)
{
	if (this->baked)
		return true;

	kuint_t relativeAddr = 0;

	// check for method completeness
	for (std::vector<MethodBuilder *>::const_iterator it = this->methodBuilderList->begin();
		it != this->methodBuilderList->end(); ++it)
	{
		MethodBuilder *builder = *it;
		if (!(builder->attrs & KMA_NATIVE))
		{
			if (!builder->getCodeGenerator()->isFinished())
			{
				lastCompileError = KCSE_NO_METHOD_BODY;
				return false;
			}

			builder->addr = relativeAddr;
			relativeAddr += builder->getCodeGenerator()->getCodeSize();
		}

		this->codeSize += builder->getCodeGenerator()->getCodeSize();
	}
	
	this->baked = true;

	lastCompileError = KCSE_NO_ERROR;
	return true;
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

DelegateBuilder::~DelegateBuilder(void)
{
	DELETE_IF_NOT_NULL(this->paramList);
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
	: codeGen(NULL), addr(0)
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

MethodBuilder::~MethodBuilder(void)
{
	DELETE_IF_NOT_NULL(this->paramList);
	for (std::vector<LocalBuilder *>::const_iterator it = this->localBuilderList.begin();
		it != this->localBuilderList.end(); ++it)
	{
		delete *it;
	}

	DELETE_IF_NOT_NULL(this->codeGen);
}

LocalBuilder * MethodBuilder::declareLocal(const TypeDef *declType)
{
	LocalBuilder *builder = new LocalBuilder;
	builder->declType = declType;
	builder->localIndex = this->localCount++;

	this->localBuilderList.push_back(builder);
	this->classBuilder->moduleBuilder->addLocal(builder);

	if (!(this->attrs & KMA_STATIC))
		++builder->localIndex;

	return builder;
}

CodeGen * MethodBuilder::getCodeGenerator(void)
{
	if (this->attrs & KMA_NATIVE)
	{
		lastCompileError = KCSE_METHOD_IS_NATIVE;
		return NULL;
	}

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
