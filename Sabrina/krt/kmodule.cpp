#include "kmodule.hpp"

#include "kconfig.hpp"
#include "krt.h"
#include "kconfig.hpp"
#include "kmeta.hpp"
#include "kenv.hpp"
#include "kobject.hpp"
#include "kstringutils.hpp"

#include <cstdio>
#include <cstring>

// ===I know what I am doing===

#define _READ(DEST, TYPE) \
	DEST = *(TYPE *)(stream + pos); \
	pos += sizeof(TYPE)

#define _VREAD(DEST, TYPE) \
	TYPE DEST = *(TYPE *)(stream + pos); \
	pos += sizeof(TYPE)

// ===I know what I am doing===

//=============================================================

#define MAX_NAME_BUFFER 2048
char nameBuffer[MAX_NAME_BUFFER];

// Gets the symbol name of a native member method, in the form: _Namespace_Class_Member
const char * getFullyQualifiedName(kstring_t className, kstring_t memberName)
{
	knuint_t len1 = wcslen(className);
	knuint_t len2 = wcslen(memberName);
	knuint_t len = len1 + len2 + 2;
	knuint_t j = 1;

	if (len > MAX_NAME_BUFFER)
	{
		if (len2 >= (len - MAX_NAME_BUFFER))
		{
			len2 -= len - MAX_NAME_BUFFER;
			len = MAX_NAME_BUFFER;
		}
		else
		{
			len1 -= len - len2 - MAX_NAME_BUFFER;
			len2 = 0;
			len = len1;
		}
	}

	nameBuffer[0] = '_';
	for (knuint_t i = 0; i < len1; ++i, ++j)
	{
		if (className[i] == L'.')
			nameBuffer[j] = '_';
		else
			nameBuffer[j] = (char)className[i];
	}

	nameBuffer[j++] = '_';
	for (knuint_t i = 0; i < len2; ++i, ++j)
		nameBuffer[j] = (char)memberName[i];

	nameBuffer[len] = 0;
	return nameBuffer;
}

//=============================================================

//public static
ModuleLoader * ModuleLoader::create(KMODULEATTRIBUTES attrs, kstring_t importerPath, kstring_t filename)
{
	kstring_t _dirPath = krt_getdirpath(importerPath);
	kstring_t _importerPath = krt_strdup(importerPath);
	kstring_t _fullPath = krt_strcat(_dirPath, filename);
	kstring_t _filename = krt_getfilename(filename);

	delete []_dirPath;

	ModuleLoader *loader = new ModuleLoader(attrs, _importerPath, _fullPath, _filename);
	if (loader == NULL)
	{
		delete []_importerPath;
		delete []_fullPath;
		delete []_filename;
	}

	return loader;
}

//protected
ModuleLoader::ModuleLoader(KMODULEATTRIBUTES attrs,  kstring_t importerPath, kstring_t fullPath, kstring_t filename)
	: moduleLoaders(NULL), err(ModuleLoadError::OK), attrs(attrs),
	importerPath(importerPath), fullPath(fullPath), filename(filename),
	hash(hash), libHandle(NULL),
	stream(NULL), pos(0), isCleaned(false),
	loadPhase(ModuleLoadPhase::Initial), module(NULL),
	code(NULL)
{
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

//public
ModuleLoader::~ModuleLoader()
{
	if (!this->isCleaned)
	{
		this->clean();
		this->isCleaned = true;
	}
}

bool ModuleLoader::load(void)
{
	if (this->open() == false)
		return false;

	if (this->loadMeta() == false)
		return false;

	if (this->bake() == false)
		return false;

	return true;
}

bool ModuleLoader::open(void)
{
	if (this->loadPhase >= ModuleLoadPhase::Opened)
		return true;

	const char *pathA = krt_wcstostr(this->fullPath);

	FILE *fp;

	unsigned char *stream;
	knuint_t streamLength;

	fp = fopen(pathA, "rb");

	if (!fp)
	{
		delete []pathA;
		this->err = ModuleLoadError::CannotOpen;
		return false;
	}

	if (this->attrs & KMODA_KIASRA)
	{
		fseek(fp, 0, SEEK_END);
		streamLength = ftell(fp);

		stream = new unsigned char[streamLength];

		fseek(fp, 0, SEEK_SET);
		fread(stream, sizeof(unsigned char), streamLength, fp);

		fclose(fp);
		fp = NULL;

		delete []pathA;
	}
	else
	{
		// load Kiasra stream

		fseek(fp, -4, SEEK_END);

		unsigned char rmagic[4] = { };
		fread(rmagic, sizeof(unsigned char), sizeof(rmagic), fp);

		if (rmagic[0] != 0x0A
		|| rmagic[1] != 0xCE
		|| rmagic[2] != 0xCA
		|| rmagic[3] != 0xDE)
		{
			delete []pathA;
			this->err = ModuleLoadError::InvalidLib;
			return false;
		}

		fseek(fp, 0, SEEK_END);
		kuint_t fileSize = ftell(fp);

		fseek(fp, -4 - (long)sizeof(kuint_t), SEEK_END);

		kuint_t offset;
		fread(&offset, sizeof(kuint_t), 1, fp);

		kuint_t dataSize = fileSize - offset - sizeof(rmagic) - sizeof(kuint_t);
		if (dataSize == 0)
		{
			delete []pathA;
			this->err = ModuleLoadError::InvalidLib;
			return false;
		}

		stream = new unsigned char[dataSize];

		fseek(fp, offset, SEEK_SET);
		fread(stream, sizeof(unsigned char), dataSize, fp);

		fclose(fp);
		fp = NULL;

		// load library

#ifdef ISWIN
		this->libHandle = LoadLibraryA(pathA);

		if (!this->libHandle)
		{
			delete []pathA;
			this->err = ModuleLoadError::CannotLoadLib;
			return false;
		}
#else
		this->libHandle = dlopen(pathA, RTLD_LAZY);

		if (!this->libHandle)
		{
			delete []pathA;
			this->err = ModuleLoadError::CannotLoadLib;
			return false;
		}
#endif
		
		delete []pathA;
	}

	this->stream = stream;
	this->streamLength = streamLength;
	this->pos = 0;

	this->loadPhase = ModuleLoadPhase::Opened;
	return true;
}

bool ModuleLoader::loadMeta(void)
{
	if (this->loadPhase >= ModuleLoadPhase::Loaded)
		return true;

	if (this->validateHeader() != ModuleValidationResult::OK)
		return false;

	if (!this->loadStringTable())
		return false;
	if (!this->loadTypeTable())
		return false;
	if (!this->loadModuleTable())
		return false;
	if (!this->loadClassTable())
		return false;
	if (!this->loadDelegateTable())
		return false;
	
	if (!this->loadFieldTable())
		return false;
	if (!this->loadMethodTable())
		return false;
	
	if (!this->loadParamTable())
		return false;
	if (!this->loadDelegateParamTable())
		return false;
	if (!this->loadLocalTable())
		return false;

	if (!this->loadCode())
		return false;

	this->loadPhase = ModuleLoadPhase::Loaded;
	return true;
}

bool ModuleLoader::bake(void)
{
	if (this->loadPhase >= ModuleLoadPhase::Baked)
		return true;

	ModuleDef *module = new ModuleDef;
	this->module = module;

	module->attrs = this->attrs;
	module->path = this->filename;

	// transfer code stream

	module->code = this->code;
	this->code = NULL;

	// native library (if any)

	module->libHandle = this->libHandle;

	// string table

	kuint_t stringCount = this->stringTable.rowCount;
	kstring_t *stringRows = this->stringTable.rows;
	kuint_t *lengthRows = this->stringTable.lengths;

	kstring_t *strings = new kstring_t[stringCount];
	kuint_t *stringLengths = new kuint_t[stringCount];
	module->stringCount = stringCount;
	module->strings = strings;
	module->stringLengths = stringLengths;

	memset(strings, 0, sizeof(kstring_t) * stringCount);

	for (kuint_t i = 0; i < stringCount; ++i)
	{
		// transfer
		strings[i] = stringRows[i];
		stringLengths[i] = lengthRows[i];
	}

	// module table

	kuint_t moduleCount = this->moduleTable.rowCount;
	MetaModuleDef *metaModuleRows = this->moduleTable.rows;

	ModuleLoader **moduleLoaderList = new ModuleLoader* [moduleCount + 1];
	ModuleDef **moduleList = new ModuleDef * [moduleCount + 1];
	module->moduleCount = moduleCount;
	module->moduleList = moduleList;
	this->moduleLoaders = moduleLoaderList;

	memset(moduleLoaderList, 0, sizeof(ModuleLoader *) * (moduleCount + 1));

	for (kuint_t i = 0; i < moduleCount; ++i)
	{
		ModuleLoader *moduleLoader = KEnvironment::createModuleLoader((KMODULEATTRIBUTES)metaModuleRows[i].attrs,
			this->fullPath, strings[metaModuleRows[i].path]);

		if (moduleLoader->loadPhase < ModuleLoadPhase::Baked)
		{
			if (moduleLoader->load() == false)
			{
				this->err = ModuleLoadError::CannotLoadImportedModule;
				return false;
			}
		}

		moduleLoaderList[i + 1] = moduleLoader;
		moduleList[i + 1] = moduleLoader->module;
	}

	// field table

	kuint_t allFieldCount = this->fieldTable.rowCount;
	MetaFieldDef *fieldRows = this->fieldTable.rows;

	FieldDef *allFieldList = new FieldDef [allFieldCount + 1];
	module->fieldCount = allFieldCount;
	module->fieldList = allFieldList;

	for (kuint_t i = 0; i < allFieldCount; ++i)
	{
		FieldDef &fld = allFieldList[i + 1];

		fld.size = sizeof(FieldDef);
		fld.attrs = fieldRows[i].attrs;
		fld.name = strings[fieldRows[i].name];
	}

	// param table

	kuint_t allParamCount = this->paramTable.rowCount;
	MetaParamDef *paramRows = this->paramTable.rows;

	ParamDef *allParamList = new ParamDef [allParamCount + 1];
	module->paramCount = allParamCount;
	module->paramList = allParamList;

	for (kuint_t i = 0; i < allParamCount; ++i)
	{
		ParamDef &param = allParamList[i + 1];

		param.byRef = paramRows[i].byRef != 0;
		param.name = strings[paramRows[i].name];
	}

	// method table

	kuint_t allMethodCount = this->methodTable.rowCount;
	MetaMethodDef *methodRows = this->methodTable.rows;

	MethodDef *allMethodList = new MethodDef [allMethodCount + 1];
	module->methodCount = allMethodCount;
	module->methodList = allMethodList;

	for (kuint_t i = 0; i < allMethodCount; ++i)
	{
		MetaMethodDef &methodRow = methodRows[i];

		MethodDef &met = allMethodList[i + 1];

		met.size = sizeof(MethodDef);
		met.attrs = methodRow.attrs;
		met.name = strings[methodRow.name];
		
		if (methodRow.attrs & KMA_NATIVE)
			met.func = NULL; //we will load it later
		else
			met.addr = methodRow.addr;

		kuint_t paramCount = 0;
			
		if (methodRow.paramList)
		{
			if (i == allMethodCount - 1)
			{
				paramCount = this->paramTable.rowCount - methodRow.paramList + 1;
			}
			else
			{
				ktoken32_t nextParamList = 0;
				for (kuint_t j = i + 1; j < allMethodCount; ++j)
					if (nextParamList = methodRows[j].paramList)
						break;

				if (nextParamList)
					paramCount = nextParamList - methodRow.paramList;
				else
					paramCount = this->paramTable.rowCount - methodRow.paramList + 1;
			}
		}
			
		ParamDef **paramList = new ParamDef* [paramCount];

		for (kuint_t i = methodRow.paramList, j = 0; j < paramCount; ++i, ++j)
			paramList[j] = &allParamList[i];

		met.paramCount = paramCount;
		met.paramList = paramList;
	}

	// class table

	kuint_t allClassCount = this->classTable.rowCount;
	MetaClassDef *classRows = this->classTable.rows;

	ClassDef **allClassList = new ClassDef* [allClassCount];
	bool *extClassFlags = new bool[allClassCount];
	module->classCount = allClassCount;
	module->classList = allClassList;
	module->extClassFlags = extClassFlags;

	memset(allClassList, 0, sizeof(ClassDef *) * allClassCount);
	memset(extClassFlags, 0, sizeof(bool) * allClassCount);

	for (kuint_t i = 0; i < allClassCount; ++i)
	{
		const MetaClassDef &classRow = classRows[i];
		ClassDef *cls;

		if (classRow.moduleIndex)
		{
			cls = moduleList[classRow.moduleIndex]->classList[classRow.farIndex];
			extClassFlags[i] = true;

			allClassList[i] = cls;
		}
		else
		{
			cls = new ClassDef;
			allClassList[i] = cls;

			cls->size = sizeof(ClassDef);
			cls->attrs = classRow.attrs;
			cls->name = strings[classRow.name];
			cls->module = module;

			kuint_t fieldCount = 0;
			kuint_t iFieldCount = 0, sFieldCount = 0;
			
			if (classRow.fieldList)
			{
				if (i == allClassCount - 1)
				{
					fieldCount = this->fieldTable.rowCount - classRow.fieldList + 1;
				}
				else
				{
					ktoken32_t nextClassFieldList = 0;
					for (kuint_t j = i + 1; j < allClassCount; ++j)
						if (nextClassFieldList = classRows[j].fieldList)
							break;

					if (nextClassFieldList)
						fieldCount = nextClassFieldList - classRow.fieldList;
					else
						fieldCount = this->fieldTable.rowCount - classRow.fieldList + 1;
				}
			}
			
			FieldDef **fieldList = new FieldDef* [fieldCount];

			cls->fieldCount = fieldCount;
			cls->fieldList = fieldList;

			for (kuint_t k = classRow.fieldList, j = 0; j < fieldCount; ++k, ++j)
			{
				FieldDef *fld = &allFieldList[k];
				fieldList[j] = fld;

				fld->klass = cls;
				if (fld->attrs & KFA_STATIC)
					fld->localIndex = sFieldCount++;
				else
					fld->localIndex = iFieldCount++;
			}

			FieldDef **iFieldList = new FieldDef* [iFieldCount];

			cls->iFieldCount = iFieldCount;
			cls->iFieldList = iFieldList;
			
			FieldDef **sFieldList = new FieldDef* [sFieldCount];

			cls->sFieldCount = sFieldCount;
			cls->sFieldList = sFieldList;

			for (kuint_t k = 0, j = 0, m = 0; k < fieldCount; ++k)
			{
				if (fieldList[k]->attrs & KFA_STATIC)
					sFieldList[j++] = fieldList[k];
				else
					iFieldList[m++] = fieldList[k];
			}

			//================================

			kuint_t methodCount = 0;
			
			if (classRow.methodList)
			{
				if (i == allClassCount - 1)
				{
					methodCount = this->methodTable.rowCount - classRow.methodList + 1;
				}
				else
				{
					ktoken32_t nextClassMethodList = 0;
					for (kuint_t j = i + 1; j < allClassCount; ++j)
						if (nextClassMethodList = classRows[j].methodList)
							break;

					if (nextClassMethodList)
						methodCount = nextClassMethodList - classRow.methodList;
					else
						methodCount = this->methodTable.rowCount - classRow.methodList + 1;
				}
			}
			
			MethodDef **methodList = new MethodDef* [methodCount];

			cls->methodCount = methodCount;
			cls->methodList = methodList;

			for (kuint_t k = classRow.methodList, j = 0; j < methodCount; ++k, ++j)
			{
				MethodDef *met = &allMethodList[k];
				methodList[j] = met;
				met->localIndex = j;

				met->klass = cls;

				if (met->attrs & KMA_CTOR)
				{
					if (met->attrs & KMA_STATIC)
						cls->cctor = met;
					else
						cls->ctor = met;
				}

				if (met->attrs & KMA_NATIVE)
				{
					// load native function
					const char *fullyQualifiedName = getFullyQualifiedName(cls->name, met->name);
#ifdef ISWIN
					NFUNC func = (NFUNC)GetProcAddress(this->libHandle, fullyQualifiedName);
					if (!func)
					{
						this->err = ModuleLoadError::CannotLoadNativeFunction;
						return false;
					}
#else
					NFUNC func = (NFUNC)dlsym(this->libHandle, fullyQualifiedName);
					if (!func)
					{
						this->err = ModuleLoadError::CannotLoadNativeFunction;
						return false;
					}
#endif

					met->func = func;
				}
			}
		}
	}

	// delegate param table

	kuint_t allDParamCount = this->dparamTable.rowCount;
	MetaParamDef *dparamRows = this->dparamTable.rows;

	ParamDef *allDParamList = new ParamDef [allDParamCount + 1];
	module->dparamCount = allDParamCount;
	module->dparamList = allDParamList;

	for (kuint_t i = 0; i < allDParamCount; ++i)
	{
		ParamDef &dparam = allDParamList[i + 1];

		dparam.byRef = dparamRows[i].byRef != 0;
		dparam.name = strings[dparamRows[i].name];
	}

	// delegate table

	kushort_t allDelegateCount = this->delegateTable.rowCount;
	MetaDelegateDef *delegateRows = this->delegateTable.rows;

	DelegateDef **allDelegateList = new DelegateDef* [allDelegateCount];
	bool *extDelegateFlags = new bool[allDelegateCount];
	module->delegateCount = allDelegateCount;
	module->delegateList = allDelegateList;
	module->extDelegateFlags = extDelegateFlags;

	memset(allDelegateList, 0, sizeof(DelegateDef *) * allDelegateCount);
	memset(extDelegateFlags, 0, sizeof(bool) * allDelegateCount);

	for (kushort_t i = 0; i < allDelegateCount; ++i)
	{
		MetaDelegateDef &delegateRow = delegateRows[i];
		DelegateDef *del;

		if (delegateRow.moduleIndex)
		{
			del = moduleList[delegateRow.moduleIndex]->delegateList[delegateRow.farIndex];
			extDelegateFlags[i] = true;
		
			allDelegateList[i] = del;
		}
		else
		{
			del = new DelegateDef;
			allDelegateList[i] = del;

			del->size = sizeof(DelegateDef);
			del->attrs = delegateRow.attrs;
			del->name = strings[delegateRow.name];

			kuint_t paramCount = 0;
			
			if (delegateRow.paramList)
			{
				if (i == allDelegateCount - 1)
				{
					paramCount = this->dparamTable.rowCount - delegateRow.paramList + 1;
				}
				else
				{
					ktoken32_t nextDParamList = 0;
					for (kuint_t j = i + 1; j < allDelegateCount; ++j)
						if (nextDParamList = delegateRows[j].paramList)
							break;

					if (nextDParamList)
						paramCount = nextDParamList - delegateRow.paramList;
					else
						paramCount = this->dparamTable.rowCount - delegateRow.paramList + 1;
				}
			}
			
			ParamDef **paramList = new ParamDef* [paramCount];

			for (kuint_t k = delegateRow.paramList, j = 0; j < paramCount; ++k, ++j)
				paramList[j] = &allDParamList[k];

			del->paramCount = paramCount;
			del->paramList = paramList;
		}
	}

	// type table

	kuint_t allTypeCount = this->typeTable.rowCount;
	MetaTypeDef *typeRows = this->typeTable.rows;

	const TypeDef **allTypeList = new const TypeDef* [allTypeCount];
	module->typeCount = allTypeCount;
	module->typeList = allTypeList;

	for (kuint_t i = 0; i < allTypeCount; ++i)
	{
		const MetaTypeDef &typeRow = typeRows[i];
		
		if ((typeRow.tag & KT_SCALAR_MASK) == KT_CLASS)
			allTypeList[i] = KEnvironment::createType((KTYPETAG)typeRow.tag, typeRow.dim, allClassList[typeRow.tok]);
		else if ((typeRow.tag & KT_SCALAR_MASK) == KT_DELEGATE)
			allTypeList[i] = KEnvironment::createType((KTYPETAG)typeRow.tag, typeRow.dim, allDelegateList[typeRow.tok]);
		else
			allTypeList[i] = KEnvironment::createType((KTYPETAG)typeRow.tag, typeRow.dim);
	}

	// local table

	kuint_t allLocalCount = this->localTable.rowCount;
	ktoken32_t *localRows = this->localTable.rows;

	const TypeDef **allLocalList = new const TypeDef* [allLocalCount + 1];
	module->localCount = allLocalCount;
	module->localList = allLocalList;

	allLocalList[0] = NULL;
	for (kuint_t i = 0; i < allLocalCount; ++i)
		allLocalList[i+1] = allTypeList[localRows[i]];

	for (kuint_t i = 0; i < allMethodCount; ++i)
	{
		MetaMethodDef &methodRow = methodRows[i];

		MethodDef &met = allMethodList[i + 1];

		kuint_t localCount = 0;
			
		if (methodRow.localList)
		{
			if (i == allMethodCount - 1)
			{
				localCount = this->localTable.rowCount - methodRow.localList + 1;
			}
			else
			{
				ktoken32_t nextLocalList = 0;
				for (kuint_t j = i + 1; j < allMethodCount; ++j)
					if (nextLocalList = methodRows[j].localList)
						break;

				if (nextLocalList)
					localCount = nextLocalList - methodRow.localList;
				else
					localCount = this->localTable.rowCount - methodRow.localList + 1;
			}
		}
			
		const TypeDef **localList = new const TypeDef* [localCount];

		for (kuint_t k = methodRow.localList, j = 0; j < localCount; ++k, ++j)
			localList[j] = allLocalList[k];

		met.localCount = localCount;
		met.localList = localList;
	}

	// param decltype

	for (kuint_t i = 0; i < allParamCount; ++i)
		allParamList[i + 1].declType = allTypeList[paramRows[i].declType];

	// delegate param decltype

	for (kuint_t i = 0; i < allDParamCount; ++i)
		allDParamList[i + 1].declType = allTypeList[dparamRows[i].declType];

	// field type

	for (kuint_t i = 0; i < allFieldCount; ++i)
		allFieldList[i + 1].declType = allTypeList[fieldRows[i].declType];

	// method return type

	for (kuint_t i = 0; i < allMethodCount; ++i)
		allMethodList[i + 1].returnType = allTypeList[methodRows[i].returnType];

	// entry point

	if (this->moduleType != ModuleType::Library)
		module->entryMethod = allClassList[this->entryClass]->methodList[this->entryMethod];
	else
		module->entryMethod = NULL;

	// DONE!

	this->loadPhase = ModuleLoadPhase::Baked;
	return true;
}

ModuleDef * ModuleLoader::getModule(void)
{
	return this->module;
}

kstring_t ModuleLoader::getImporterPath(void)
{
	return this->importerPath;
}

kstring_t ModuleLoader::getFullPath(void)
{
	return this->fullPath;
}

kstring_t ModuleLoader::getFilename(void)
{
	return this->filename;
}

ModuleLoader::ModuleLoadError ModuleLoader::getError(void)
{
	return err;
}

void ModuleLoader::onDispose(void)
{
	if (!this->isCleaned)
	{
		this->clean();
		this->isCleaned = true;
	}
}

void ModuleLoader::clean()
{
	ADELETE_IF_NOT_NULL(this->importerPath);
	ADELETE_IF_NOT_NULL(this->fullPath);
	ADELETE_IF_NOT_NULL(this->filename);

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

	ADELETE_IF_NOT_NULL(this->stream);

	ADELETE_IF_NOT_NULL(this->moduleLoaders);

	if (this->stringTable.rows && this->loadPhase < ModuleLoadPhase::Baked)
	{
		kuint_t count = this->stringTable.rowCount;
		kstring_t *rows = this->stringTable.rows;
		for (kuint_t i = 0; i < count; ++i)
			ADELETE_IF_NOT_NULL(rows[i]);

		delete []rows;
		delete []this->stringTable.lengths;

		this->stringTable.rows = NULL;
		this->stringTable.lengths = NULL;
	}
		ADELETE_IF_NOT_NULL(this->stringTable.rows);

	if (this->code && this->loadPhase < ModuleLoadPhase::Baked)
	{
		delete [] this->code;
		this->code = NULL;
	}

	if (this->loadPhase == ModuleLoadPhase::Baked && this->module)
	{
		delete this->module;
		this->module = NULL;
	}

	this->loadPhase = ModuleLoadPhase::Initial;
}

ModuleLoader::ModuleValidationResult ModuleLoader::validateHeader()
{
	//	VALID HEADER
	//		u1 magic[] = { 0xAA, 0xCE, 0xCA, 0xDE }
	//		u1 versionMajor = 0x01
	//		u1 versionMinor = 0x00
	//		u1 moduleType
	//		u1 debugMode
	//		u4 hash
	//		u4 entry

	unsigned char *stream = this->stream;
	knuint_t pos = this->pos;

	if (stream[0] != 0xAA
		|| stream[1] != 0xCE
		|| stream[2] != 0xCA
		|| stream[3] != 0xDE)
	{
		return ModuleValidationResult::InvalidMagic;
	}

	if (stream[4] != 0x01 || stream[5] != 0x00)
		return ModuleValidationResult::InvalidVersion;

	switch (stream[6])
	{
	case 0x00:
		this->moduleType = ModuleType::Window;
		break;
	case 0x01:
		this->moduleType = ModuleType::Console;
		break;
	case 0x02:
		this->moduleType = ModuleType::Library;
		break;

	default:
		return ModuleValidationResult::InvalidModuleType;
	}

	if (stream[7] == 0x01)
		this->hasDebugInfo = true;
	else if (stream[7] == 0x00)
		this->hasDebugInfo = false;
	else
		return ModuleValidationResult::InvalidModuleFormat;

	// TO-DO: the following hash code is used to check for
	// metadata integrity, so we will need a function to
	// calculate a hash from the collected metadata then
	// compare it with the hash we read here.
	this->hash = *(uint32_t *)(stream + 8);
	pos = 8 + sizeof(uint32_t);

	if (this->moduleType == ModuleType::Library)
	{
		uint32_t entry = *(uint32_t *)(stream + pos);
		pos += sizeof(uint32_t);

		// libraries should not have entry point
		if (entry != 0)
			return ModuleValidationResult::InvalidModuleFormat;
	}
	else
	{
		this->entryClass = *(ktoken16_t *)(stream + pos);
		pos += sizeof(ktoken16_t);

		this->entryMethod = *(ktoken16_t *)(stream + pos);
		pos += sizeof(ktoken16_t);
	}

	this->pos = pos;

	return ModuleValidationResult::OK;
}

bool ModuleLoader::loadStringTable()
{
	unsigned char *stream = this->stream;
	knuint_t pos = this->pos;

	unsigned int rowCount = *(uint32_t *)(stream + pos);
	pos += sizeof(uint32_t);

	kstring_t *rows = new kstring_t[rowCount];
	kuint_t *lengths = new kuint_t[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		_VREAD(length, uint32_t);

		// sizeof(kchar_t)==sizeof(wchar_t) can be 2 (Windows) or 4 (*nix),
		// but for KLI, a character should always be uint16_t.

		kchar_t *row = new kchar_t[length + 1];
		for (uint32_t j = 0; j < length; ++j, pos += sizeof(uint16_t))
			row[j] = (kchar_t)(*(uint16_t *)(stream + pos));
		row[length] = 0;

		rows[i] = row;
		lengths[i] = length;
	}

	this->stringTable.rowCount = (uint32_t)rowCount;
	this->stringTable.rows = rows;
	this->stringTable.lengths = lengths;
	this->pos = pos;

	return true;
}

bool ModuleLoader::loadTypeTable()
{
	unsigned char *stream = this->stream;
	knuint_t pos = this->pos;
	
	unsigned int rowCount = *(uint32_t *)(stream + pos);
	pos += sizeof(uint32_t);

	MetaTypeDef *rows = new MetaTypeDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaTypeDef &row = rows[i];

		_READ(row.tag, uint16_t);

		_READ(row.dim, uint16_t);

		if ((row.tag & KT_SCALAR_MASK) == KT_CLASS || (row.tag & KT_SCALAR_MASK) == KT_DELEGATE)
		{
			_READ(row.tok, uint16_t);
		}
	}

	this->typeTable.rowCount = (uint32_t)rowCount;
	this->typeTable.rows = rows;
	this->pos = pos;

	return true;
}

bool ModuleLoader::loadModuleTable()
{
	unsigned char *stream = this->stream;
	knuint_t pos = this->pos;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaModuleDef *rows = new MetaModuleDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaModuleDef &row = rows[i];

		_READ(row.attrs, uint16_t);
		_READ(row.path, ktoken32_t);
		_READ(row.hash, uint32_t);
	}

	this->moduleTable.rowCount = (uint16_t)rowCount;
	this->moduleTable.rows = rows;
	this->pos = pos;

	return true;
}

bool ModuleLoader::loadClassTable()
{
	unsigned char *stream = this->stream;
	knuint_t pos = this->pos;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaClassDef *rows = new MetaClassDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaClassDef &row = rows[i];

		_READ(row.attrs, uint16_t);

		_VREAD(nameToken, uint32_t);
		row.name = nameToken;

		_READ(row.moduleIndex, uint16_t);

		if (row.moduleIndex)
		{
			_READ(row.farIndex, ktoken16_t);
		}
		else
		{
			_READ(row.fieldList, ktoken32_t);
			_READ(row.methodList, ktoken32_t);
		}
	}

	this->classTable.rowCount = (uint16_t)rowCount;
	this->classTable.rows = rows;
	this->pos = pos;

	return true;
}

bool ModuleLoader::loadDelegateTable()
{
	unsigned char *stream = this->stream;
	knuint_t pos = this->pos;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaDelegateDef *rows = new MetaDelegateDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaDelegateDef &row = rows[i];

		_READ(row.attrs, uint16_t);

		_VREAD(nameToken, uint32_t);
		row.name = nameToken;

		_READ(row.moduleIndex, uint16_t);

		if (row.moduleIndex)
		{
			_READ(row.farIndex, ktoken16_t);
		}
		else
		{
			_READ(row.returnType, ktoken32_t);
			_READ(row.paramList, ktoken32_t);
		}
	}

	this->delegateTable.rowCount = (uint16_t)rowCount;
	this->delegateTable.rows = rows;
	this->pos = pos;

	return true;
}

bool ModuleLoader::loadFieldTable()
{
	unsigned char *stream = this->stream;
	knuint_t pos = this->pos;
	
	unsigned int rowCount = *(uint32_t *)(stream + pos);
	pos += sizeof(uint32_t);

	MetaFieldDef *rows = new MetaFieldDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaFieldDef &row = rows[i];

		_READ(row.attrs, uint16_t);

		_VREAD(nameToken, uint32_t);
		row.name = nameToken;

		_READ(row.declType, ktoken32_t);
	}

	this->fieldTable.rowCount = (uint32_t)rowCount;
	this->fieldTable.rows = rows;
	this->pos = pos;

	return true;
}

bool ModuleLoader::loadMethodTable()
{
	unsigned char *stream = this->stream;
	knuint_t pos = this->pos;
	
	unsigned int rowCount = *(uint32_t *)(stream + pos);
	pos += sizeof(uint32_t);

	MetaMethodDef *rows = new MetaMethodDef[rowCount];

	bool nonNative = !(this->attrs & KMODA_NATIVE);

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaMethodDef &row = rows[i];

		_READ(row.attrs, uint16_t);

		if (nonNative && (row.attrs & KMA_NATIVE))
		{
			// not a native module but has native method
			this->err = ModuleLoadError::InvalidModule;
			return false;
		}

		_VREAD(nameToken, uint32_t);
		row.name = nameToken;

		_READ(row.returnType, ktoken32_t);
		_READ(row.paramList, ktoken32_t);
		_READ(row.localList, ktoken32_t);

		if (!(row.attrs & KMA_NATIVE))
		{
			_READ(row.addr, kuint_t);
		}
	}

	this->methodTable.rowCount = (uint32_t)rowCount;
	this->methodTable.rows = rows;
	this->pos = pos;

	return true;
}

bool ModuleLoader::loadParamTable()
{
	unsigned char *stream = this->stream;
	knuint_t pos = this->pos;
	
	unsigned int rowCount = *(uint32_t *)(stream + pos);
	pos += sizeof(uint32_t);

	MetaParamDef *rows = new MetaParamDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaParamDef &row = rows[i];

		_VREAD(nameToken, uint32_t);
		row.name = nameToken;

		_READ(row.declType, ktoken32_t);
		
		_VREAD(isByRef, uint8_t);
		row.byRef = isByRef!=0;
	}

	this->paramTable.rowCount = (uint32_t)rowCount;
	this->paramTable.rows = rows;
	this->pos = pos;

	return true;
}

bool ModuleLoader::loadDelegateParamTable()
{
	unsigned char *stream = this->stream;
	knuint_t pos = this->pos;
	
	unsigned int rowCount = *(uint32_t *)(stream + pos);
	pos += sizeof(uint32_t);

	MetaParamDef *rows = new MetaParamDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaParamDef &row = rows[i];

		_VREAD(nameToken, uint32_t);
		row.name = nameToken;

		_READ(row.declType, ktoken32_t);
		
		_VREAD(isByRef, uint8_t);
		row.byRef = isByRef!=0;
	}

	this->dparamTable.rowCount = (uint32_t)rowCount;
	this->dparamTable.rows = rows;
	this->pos = pos;

	return true;
}

bool ModuleLoader::loadLocalTable()
{
	unsigned char *stream = this->stream;
	knuint_t pos = this->pos;
	
	unsigned int rowCount = *(uint32_t *)(stream + pos);
	pos += sizeof(uint32_t);

	ktoken32_t *rows = new ktoken32_t[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		_READ(rows[i], ktoken32_t);
	}

	this->localTable.rowCount = (uint32_t)rowCount;
	this->localTable.rows = rows;
	this->pos = pos;

	return true;
}

bool ModuleLoader::loadCode()
{
	unsigned char *stream = this->stream;
	knuint_t pos = this->pos;

	uint32_t codeSize = *(uint32_t *)(stream + pos);
	pos += sizeof(uint32_t);

	unsigned char *code = new unsigned char[codeSize];
	memcpy(code, stream + pos, (size_t)codeSize);

	this->code = code;
	this->pos = pos + codeSize;

	return true;
}

//=============================================================

ModuleDef::ModuleDef(void)
	: stringCount(0), strings(NULL), stringLengths(NULL),
	moduleCount(0), moduleList(NULL),
	typeCount(0), typeList(NULL),
	classCount(0), classList(NULL), extClassFlags(NULL),
	delegateCount(0), delegateList(NULL), extDelegateFlags(NULL),
	fieldCount(0), fieldList(NULL),
	methodCount(0), methodList(NULL),
	paramCount(0), paramList(NULL),
	dparamCount(0), dparamList(NULL),
	localCount(0), localList(NULL),
	code(NULL),
	staticData(NULL),
	entryMethod(NULL),
	libHandle(NULL)
{
}

ModuleDef::~ModuleDef(void)
{
	kuint_t i, count;

	if (this->strings)
	{
		count = this->stringCount;
		for (i = 0; i < count; ++i)
			ADELETE_IF_NOT_NULL(this->strings[i]);

		delete []this->strings;
		delete []this->stringLengths;

		this->strings = NULL;
		this->stringLengths = NULL;
	}

	if (this->moduleList)
	{
		count = this->moduleCount;
		for (i = 1; i < count; ++i)
			delete this->moduleList[i];

		delete []this->moduleList;
		this->moduleList = NULL;
	}

	if (this->classList)
	{
		count = this->classCount;
		for (i = 0; i < count; ++i)
			// delete only internal classes
			if (!this->extClassFlags[i])
				DELETE_IF_NOT_NULL(this->classList[i]);

		delete []this->classList;
		delete []this->extClassFlags;

		this->classList = NULL;
		this->extClassFlags = NULL;
	}

	if (this->delegateList)
	{
		count = this->delegateCount;
		for (i = 0; i < count; ++i)
			// delete only internal delegates
				if (!this->extDelegateFlags[i])
					DELETE_IF_NOT_NULL(this->delegateList[i]);

		delete []this->delegateList;
		delete []this->extDelegateFlags;

		this->delegateList = NULL;
		this->extDelegateFlags = NULL;
	}

	ADELETE_IF_NOT_NULL(this->typeList);
	ADELETE_IF_NOT_NULL(this->fieldList);
	ADELETE_IF_NOT_NULL(this->methodList);
	ADELETE_IF_NOT_NULL(this->paramList);
	ADELETE_IF_NOT_NULL(this->dparamList);
	ADELETE_IF_NOT_NULL(this->localList);
	ADELETE_IF_NOT_NULL(this->code);

	this->entryMethod = NULL;

	// GC will take care of this
	this->staticData = NULL;

	if (this->libHandle)
	{
#ifdef ISWIN
		FreeLibrary(this->libHandle);
#else
		dlclose(this->libHandle);
#endif
		this->libHandle = NULL;
	}
}

//=============================================================

ClassDef::ClassDef(void)
	: fieldList(NULL), iFieldList(NULL), sFieldList(NULL), methodList(NULL)
{
}

ClassDef::~ClassDef(void)
{
	ADELETE_IF_NOT_NULL(this->fieldList);
	ADELETE_IF_NOT_NULL(this->iFieldList);
	ADELETE_IF_NOT_NULL(this->sFieldList);
	ADELETE_IF_NOT_NULL(this->methodList);
}

//=============================================================

DelegateDef::DelegateDef(void)
	: paramList(NULL)
{
}

DelegateDef::~DelegateDef(void)
{
	ADELETE_IF_NOT_NULL(this->paramList);
}

//=============================================================

MethodDef::MethodDef(void)
	: paramList(NULL), localList(NULL)
{
}

MethodDef::~MethodDef(void)
{
	ADELETE_IF_NOT_NULL(this->paramList);
	ADELETE_IF_NOT_NULL(this->localList);
}
