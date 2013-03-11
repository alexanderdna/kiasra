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
		nameBuffer[j] = (char)className[i];

	for (knuint_t i = 0; i < len2; ++i, ++j)
		nameBuffer[j] = (char)memberName[i];

	nameBuffer[len] = 0;
	return nameBuffer;
}

//=============================================================

KModuleLoader::KModuleLoader(KEnvironment *env, kstring_t importerPath, kstring_t path, uint32_t hash)
	: moduleLoaders(NULL), err(ModuleLoadError::OK),
	importerPath(importerPath), path(path), hash(hash), libHandle(NULL),
	stream(NULL), pos(0), isCleaned(true),
	loadPhase(ModuleLoadPhase::Initial), module(NULL)
{
	this->stringTable.rows = NULL;
	this->typeTable.rows = NULL;
	this->moduleTable.rows = NULL;
	this->classTable.rows = NULL;
	this->delegateTable.rows = NULL;
	this->fieldTable.rows = NULL;
	this->methodTable.rows = NULL;
	this->paramTable.rows = NULL;
	this->dparamTable.rows = NULL;
	this->localTable.rows = NULL;
	this->code = NULL;

	this->env = env;
}

KModuleLoader::~KModuleLoader()
{
	if (!this->isCleaned)
	{
		this->clean();
		this->isCleaned = true;
	}
}

bool KModuleLoader::load(void)
{
	if (this->open() == false)
		return false;

	if (this->loadMeta() == false)
		return false;

	if (this->bake() == false)
		return false;

	return true;
}

bool KModuleLoader::open(void)
{
	if (this->loadPhase >= ModuleLoadPhase::Opened)
		return true;

	switch (this->path[0])
	{
	case 'd':
		this->attrs = (ModuleAttributes)(KMODA_NATIVE | KMODA_USER);
		break;
	case 'D':
		this->attrs = (ModuleAttributes)(KMODA_NATIVE | KMODA_SYSTEM);
		break;
	case 'k':
		this->attrs = (ModuleAttributes)(KMODA_KIASRA | KMODA_USER);
		break;
	case 'K':
		this->attrs = (ModuleAttributes)(KMODA_KIASRA | KMODA_SYSTEM);
		break;
	default:
		this->err = ModuleLoadError::InvalidPath;
		return false;
	}

	kstring_t absolutePath;

	if (this->attrs & KMODA_USER)
	{
		const kchar_t * lastSlash;
#ifdef ISWIN
		lastSlash = wcsrchr(this->importerPath, '\\');
#else
		lastSlash = wcsrchr(this->importerPath, '/');
#endif

		knuint_t len = lastSlash - this->importerPath + 1;

		absolutePath = krt_strcat(this->importerPath, len, this->path + 1, wcslen(this->path) - 1);
	}
	else
	{
		absolutePath = krt_strcat(this->env->getSystemLibPath(), this->path + 1);
	}

	knuint_t len = wcslen(absolutePath);
	char *pathA = new char[len + 1];
	for (knuint_t i = 0; i < len; ++i)
		pathA[i] = (char)absolutePath[i];
	pathA[len] = 0;

	delete []absolutePath;

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

bool KModuleLoader::loadMeta(void)
{
	if (this->loadPhase >= ModuleLoadPhase::Loaded)
		return true;

	this->isCleaned = false;

	if (this->validateHeader() != ModuleValidationResult::OK)
		return false;

	this->loadStringTable();
	this->loadTypeTable();
	this->loadModuleTable();
	this->loadClassTable();
	this->loadDelegateTable();
	
	this->loadFieldTable();
	this->loadMethodTable();
	
	this->loadParamTable();
	this->loadDelegateParamTable();
	this->loadLocalTable();

	this->loadCode();

	this->loadPhase = ModuleLoadPhase::Loaded;
	return true;
}

bool KModuleLoader::bake(void)
{
	if (this->loadPhase >= ModuleLoadPhase::Baked)
		return true;

	KEnvironment *env = this->env;

	ModuleDef *module = new ModuleDef;
	this->module = module;

	// code stream

	module->code = this->code;

	// native library (if any)

	module->libHandle = this->libHandle;

	// string table

	kuint_t stringCount = this->stringTable.rowCount;
	kstring_t *stringRows = this->stringTable.rows;

	kstring_t *strings = new kstring_t[stringCount];
	module->stringCount = stringCount;
	module->strings = strings;

	for (kuint_t i = 0; i < stringCount; ++i)
		strings[i] = stringRows[i];

	// module table

	kuint_t moduleCount = this->moduleTable.rowCount;
	MetaModuleDef *metaModuleRows = this->moduleTable.rows;

	KModuleLoader **moduleLoaderList = new KModuleLoader* [moduleCount];
	ModuleDef **moduleList = new ModuleDef * [moduleCount];
	module->moduleCount = moduleCount;
	module->moduleList = moduleList;

	for (kuint_t i = 0; i < moduleCount; ++i)
	{
		KModuleLoader *moduleLoader = env->createModuleLoader(this->path,
			strings[metaModuleRows[i].path], metaModuleRows[i].hash);

		if (moduleLoader->loadPhase < ModuleLoadPhase::Baked)
		{
			if (moduleLoader->load() == false)
			{
				this->err = ModuleLoadError::CannotLoadImportedModule;
				return false;
			}
		}

		moduleLoaderList[i] = moduleLoader;
		moduleList[i] = moduleLoader->module;
	}

	// field table

	kushort_t allFieldCount = this->fieldTable.rowCount;
	MetaFieldDef *fieldRows = this->fieldTable.rows;

	FieldDef *allFieldList = new FieldDef [allFieldCount + 1];
	module->fieldCount = allFieldCount;
	module->fieldList = allFieldList;

	for (kushort_t i = 0; i < allFieldCount; ++i)
	{
		FieldDef &fld = allFieldList[i + 1];

		fld.attrs = fieldRows[i].attrs;
		fld.name = strings[fieldRows[i].name];
	}

	// param table

	kushort_t allParamCount = this->paramTable.rowCount;
	MetaParamDef *paramRows = this->paramTable.rows;

	ParamDef *allParamList = new ParamDef [allParamCount + 1];
	module->paramCount = allParamCount;
	module->paramList = allParamList;

	for (kushort_t i = 0; i < allParamCount; ++i)
	{
		ParamDef &param = allParamList[i + 1];

		param.byRef = paramRows[i].byRef != 0;
		param.name = strings[paramRows[i].name];
	}

	// method table

	kushort_t allMethodCount = this->methodTable.rowCount;
	MetaMethodDef *methodRows = this->methodTable.rows;

	MethodDef *allMethodList = new MethodDef [allMethodCount + 1];
	module->methodCount = allMethodCount;
	module->methodList = allMethodList;

	for (kushort_t i = 0; i < allMethodCount; ++i)
	{
		MetaMethodDef &methodRow = methodRows[i];

		MethodDef &met = allMethodList[i + 1];

		met.attrs = methodRow.attrs;
		met.name = strings[methodRow.name];
		
		if (methodRow.attrs & KMA_NATIVE)
			met.func = NULL; //we will load it later
		else
			met.addr = methodRow.addr;

		kushort_t paramCount = 0;
			
		if (methodRow.paramList)
		{
			if (i == allMethodCount - 1)
			{
				paramCount = this->paramTable.rowCount - methodRow.paramList + 1;
			}
			else
			{
				ktoken16_t nextParamList = 0;
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

		for (kushort_t i = methodRow.paramList, j = 0; j < paramCount; ++i, ++j)
			paramList[j] = &allParamList[i];

		met.paramCount = paramCount;
		met.paramList = paramList;
	}

	// class table

	kuint_t allClassCount = this->classTable.rowCount;
	MetaClassDef *classRows = this->classTable.rows;

	ClassDef **allClassList = new ClassDef* [allClassCount];
	MetaClassDef *metaClassList = new MetaClassDef[allClassCount];
	module->classCount = allClassCount;
	module->classList = allClassList;

	memcpy(metaClassList, classRows, sizeof(MetaClassDef) * allClassCount);

	for (kuint_t i = 0; i < allClassCount; ++i)
	{
		const MetaClassDef &classRow = classRows[i];
		ClassDef *cls;

		if (classRow.farIndex)
		{
			cls = moduleList[classRow.moduleIndex]->classList[classRow.farIndex];
		}
		else
		{
			cls = new ClassDef;
			cls->attrs = classRow.attrs;
			cls->name = strings[classRow.name];
			cls->module = module;

			kushort_t fieldCount = 0;
			kushort_t iFieldCount = 0, sFieldCount = 0;
			
			if (classRow.fieldList)
			{
				if (i == allClassCount - 1)
				{
					fieldCount = this->fieldTable.rowCount - classRow.fieldList + 1;
				}
				else
				{
					ktoken16_t nextClassFieldList = 0;
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

			for (kushort_t k = classRow.fieldList, j = 0; j < fieldCount; ++k, ++j)
			{
				FieldDef *fld = &allFieldList[k];

				fld->klass = cls;
				if (fld->attrs & KFA_STATIC)
					fld->localIndex = ++sFieldCount;
				else
					fld->localIndex = ++iFieldCount;

				fieldList[j] = fld;
			}

			FieldDef **iFieldList = new FieldDef* [iFieldCount];
			
			FieldDef **sFieldList = new FieldDef* [sFieldCount];

			for (kushort_t k = 0, j = 0, m = 0; k < fieldCount; ++k)
			{
				if (fieldList[k]->attrs & KFA_STATIC)
					iFieldList[j++] = fieldList[k];
				else
					sFieldList[m++] = fieldList[k];
			}

			cls->fieldCount = fieldCount;
			cls->fieldList = fieldList;

			cls->iFieldCount = iFieldCount;
			cls->iFieldList = iFieldList;

			cls->sFieldCount = sFieldCount;
			cls->sFieldList = sFieldList;

			kushort_t methodCount = 0;
			kushort_t iMethodCount = 0, sMethodCount = 0;
			
			if (classRow.methodList)
			{
				if (i == allClassCount - 1)
				{
					methodCount = this->methodTable.rowCount - classRow.methodList + 1;
				}
				else
				{
					ktoken16_t nextClassMethodList = 0;
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

			for (kushort_t k = classRow.methodList, j = 0; j < methodCount; ++k, ++j)
			{
				MethodDef *met = &allMethodList[k];
				methodList[j + 1] = met;
				met->localIndex = j + 1;

				met->klass = cls;
				if (met->attrs & KMA_STATIC)
					++sMethodCount;
				else
					++iMethodCount;

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

				methodList[j] = met;
			}

			MethodDef **iMethodList = new MethodDef* [iMethodCount];
			
			MethodDef **sMethodList = new MethodDef* [sMethodCount];

			for (kushort_t k = 0, j = 0, m = 0; k < methodCount; ++k)
			{
				if (methodList[k]->attrs & KMA_STATIC)
					iMethodList[j++] = methodList[k];
				else
					sMethodList[m++] = methodList[k];
			}

			cls->methodCount = methodCount;
			cls->methodList = methodList;

			cls->iMethodCount = iMethodCount;
			cls->iMethodList = iMethodList;

			cls->sMethodCount = sMethodCount;
			cls->sMethodList = sMethodList;
		}

		allClassList[i+1] = cls;
	}

	// delegate param table

	kushort_t allDParamCount = this->dparamTable.rowCount;
	MetaParamDef *dparamRows = this->dparamTable.rows;

	ParamDef *allDParamList = new ParamDef [allDParamCount + 1];
	module->dparamCount = allDParamCount;
	module->dparamList = allDParamList;

	for (kushort_t i = 0; i < allDParamCount; ++i)
	{
		ParamDef &dparam = allDParamList[i + 1];

		dparam.byRef = dparamRows[i].byRef != 0;
		dparam.name = strings[dparamRows[i].name];
	}

	// delegate table

	kushort_t allDelegateCount = this->delegateTable.rowCount;
	MetaDelegateDef *delegateRows = this->delegateTable.rows;

	DelegateDef **allDelegateList = new DelegateDef* [allDelegateCount + 1];
	module->delegateCount = allDelegateCount;
	module->delegateList = allDelegateList;

	allDelegateList[0] = NULL;
	for (kushort_t i = 0; i < allDelegateCount; ++i)
	{
		MetaDelegateDef &delegateRow = delegateRows[i];

		DelegateDef *del = new DelegateDef;
		allDelegateList[i + 1] = del;

		del->attrs = delegateRow.attrs;
		del->name = strings[delegateRow.name];

		kushort_t paramCount = 0;
			
		if (delegateRow.paramList)
		{
			if (i == allDelegateCount - 1)
			{
				paramCount = this->dparamTable.rowCount - delegateRow.paramList + 1;
			}
			else
			{
				ktoken16_t nextDParamList = 0;
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

		for (kushort_t k = delegateRow.paramList, j = 0; j < paramCount; ++k, ++j)
			paramList[j] = &allDParamList[k];

		del->paramCount = paramCount;
		del->paramList = paramList;
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
			allTypeList[i] = env->createType((ktypetag_t)typeRow.tag, typeRow.dim, allClassList[typeRow.tok]);
		else if ((typeRow.tag & KT_SCALAR_MASK) == KT_DELEGATE)
			allTypeList[i] = env->createType((ktypetag_t)typeRow.tag, typeRow.dim, allDelegateList[typeRow.tok]);
		else
			allTypeList[i] = env->createType((ktypetag_t)typeRow.tag, typeRow.dim);
	}

	// local table

	kushort_t allLocalCount = this->localTable.rowCount;
	ktoken32_t *localRows = this->localTable.rows;

	const TypeDef **allLocalList = new const TypeDef* [allLocalCount + 1];
	module->localCount = allLocalCount;
	module->localList = allLocalList;

	allLocalList[0] = NULL;
	for (kushort_t i = 0; i < allLocalCount; ++i)
		allLocalList[i+1] = allTypeList[localRows[i]];

	for (kushort_t i = 0; i < allMethodCount; ++i)
	{
		MetaMethodDef &methodRow = methodRows[i];

		MethodDef &met = allMethodList[i + 1];

		kushort_t localCount = 0;
			
		if (methodRow.localList)
		{
			if (i == allMethodCount - 1)
			{
				localCount = this->localTable.rowCount - methodRow.localList + 1;
			}
			else
			{
				ktoken16_t nextLocalList = 0;
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

		for (kushort_t k = methodRow.localList, j = 0; j < localCount; ++k, ++j)
			localList[j] = allLocalList[k];

		met.localCount = localCount;
		met.localList = localList;
	}

	// param decltype

	for (kushort_t i = 0; i < allParamCount; ++i)
		allParamList[i + 1].declType = allTypeList[paramRows[i].declType];

	// delegate param decltype

	for (kushort_t i = 0; i < allDParamCount; ++i)
		allDParamList[i + 1].declType = allTypeList[dparamRows[i].declType];

	// field type

	for (kushort_t i = 0; i < allFieldCount; ++i)
		allFieldList[i + 1].declType = allTypeList[fieldRows[i].declType];

	// method return type

	for (kushort_t i = 0; i < allMethodCount; ++i)
		allMethodList[i + 1].returnType = allTypeList[methodRows[i].returnType];

	// DONE!

	this->loadPhase = ModuleLoadPhase::Baked;
	return true;
}

ModuleDef * KModuleLoader::getModule(void)
{
	return this->module;
}

kstring_t KModuleLoader::getPath(void)
{
	return this->path;
}

void KModuleLoader::onDispose(void)
{
	if (!this->isCleaned)
	{
		this->clean();
		this->isCleaned = true;
	}

	//TODO: release this->module and all its resources
}

void KModuleLoader::clean()
{
	if (this->stringTable.rows)
	{
		delete [] this->stringTable.rows;
		this->stringTable.rows = NULL;
	}

	if (this->typeTable.rows)
	{
		delete [] this->typeTable.rows;
		this->typeTable.rows = NULL;
	}

	if (this->moduleTable.rows)
	{
		delete [] this->moduleTable.rows;
		this->moduleTable.rows = NULL;
	}

	if (this->classTable.rows)
	{
		delete [] this->classTable.rows;
		this->classTable.rows = NULL;
	}

	if (this->delegateTable.rows)
	{
		delete [] this->delegateTable.rows;
		this->delegateTable.rows = NULL;
	}

	if (this->fieldTable.rows)
	{
		delete [] this->fieldTable.rows;
		this->fieldTable.rows = NULL;
	}

	if (this->methodTable.rows)
	{
		delete [] this->methodTable.rows;
		this->methodTable.rows = NULL;
	}

	if (this->paramTable.rows)
	{
		delete [] this->paramTable.rows;
		this->paramTable.rows = NULL;
	}

	if (this->dparamTable.rows)
	{
		delete [] this->dparamTable.rows;
		this->dparamTable.rows = NULL;
	}

	if (this->localTable.rows)
	{
		delete [] this->localTable.rows;
		this->localTable.rows = NULL;
	}

	if (this->code && this->loadPhase < ModuleLoadPhase::Baked)
	{
		delete [] this->code;
		this->code = NULL;
	}

	if (this->stream)
	{
		delete [] this->stream;
		this->stream = NULL;
	}
}

KModuleLoader::ModuleValidationResult KModuleLoader::validateHeader()
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
	uint32_t pos = this->pos;

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

void KModuleLoader::loadStringTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;

	unsigned int rowCount = *(uint32_t *)(stream + pos);
	pos += sizeof(uint32_t);

	kstring_t *rows = new kstring_t[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		_VREAD(length, uint32_t);

		unsigned char *row = new unsigned char[length];
		for (uint32_t j = 0; j < length; ++j, ++pos)
			row[j] = stream[pos];

		rows[i] = (kstring_t)row;
	}

	this->stringTable.rowCount = (uint32_t)rowCount;
	this->stringTable.rows = rows;
	this->pos = pos;
}

void KModuleLoader::loadTypeTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	
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
}

void KModuleLoader::loadModuleTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kstring_t *stringRows = this->stringTable.rows;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaModuleDef *rows = new MetaModuleDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaModuleDef &row = rows[i];

		_VREAD(pathToken, ktoken32_t);
		row.path = pathToken;

		_READ(row.hash, uint32_t);
	}

	this->moduleTable.rowCount = (uint16_t)rowCount;
	this->moduleTable.rows = rows;
	this->pos = pos;
}

void KModuleLoader::loadClassTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kstring_t *stringRows = this->stringTable.rows;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaClassDef *rows = new MetaClassDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaClassDef &row = rows[i];

		_READ(row.attrs, ClassAttributes);

		_VREAD(nameToken, uint32_t);
		row.name = nameToken;

		_READ(row.farIndex, uint16_t);

		if (row.farIndex)
		{
			_READ(row.moduleIndex, ktoken16_t);
		}
		else
		{
			_READ(row.fieldList, ktoken16_t);
			_READ(row.methodList, ktoken16_t);
		}
	}

	this->classTable.rowCount = (uint16_t)rowCount;
	this->classTable.rows = rows;
	this->pos = pos;
}

void KModuleLoader::loadDelegateTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kstring_t *stringRows = this->stringTable.rows;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaDelegateDef *rows = new MetaDelegateDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaDelegateDef &row = rows[i];

		_READ(row.attrs, ClassAttributes);

		_VREAD(nameToken, uint32_t);
		row.name = nameToken;

		_READ(row.farIndex, uint16_t);

		if (row.farIndex)
		{
			_READ(row.moduleIndex, ktoken16_t);
		}
		else
		{
			_READ(row.returnType, ktoken32_t);
			_READ(row.paramList, ktoken16_t);
		}
	}

	this->delegateTable.rowCount = (uint16_t)rowCount;
	this->delegateTable.rows = rows;
	this->pos = pos;
}

void KModuleLoader::loadFieldTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kstring_t *stringRows = this->stringTable.rows;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaFieldDef *rows = new MetaFieldDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaFieldDef &row = rows[i];

		_READ(row.attrs, FieldAttributes);

		_VREAD(nameToken, uint32_t);
		row.name = nameToken;

		_READ(row.declType, ktoken32_t);
	}

	this->fieldTable.rowCount = (uint16_t)rowCount;
	this->fieldTable.rows = rows;
	this->pos = pos;
}

void KModuleLoader::loadMethodTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kstring_t *stringRows = this->stringTable.rows;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaMethodDef *rows = new MetaMethodDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaMethodDef &row = rows[i];

		_READ(row.attrs, MethodAttributes);

		_VREAD(nameToken, uint32_t);
		row.name = nameToken;

		_READ(row.returnType, ktoken32_t);
		_READ(row.paramList, ktoken16_t);
		_READ(row.localList, ktoken16_t);

		_READ(row.addr, knuint_t);
	}

	this->methodTable.rowCount = rowCount;
	this->methodTable.rows = rows;
	this->pos = pos;
}

void KModuleLoader::loadParamTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kstring_t *stringRows = this->stringTable.rows;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

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

	this->paramTable.rowCount = (uint16_t)rowCount;
	this->paramTable.rows = rows;
	this->pos = pos;
}

void KModuleLoader::loadDelegateParamTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kstring_t *stringRows = this->stringTable.rows;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

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

	this->dparamTable.rowCount = (uint16_t)rowCount;
	this->dparamTable.rows = rows;
	this->pos = pos;
}

void KModuleLoader::loadLocalTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	ktoken32_t *rows = new ktoken32_t[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		_READ(rows[i], ktoken32_t);
	}

	this->localTable.rowCount = (uint16_t)rowCount;
	this->localTable.rows = rows;
	this->pos = pos;
}

void KModuleLoader::loadCode()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;

	uint32_t codeSize = *(uint32_t *)(stream + pos);
	pos += sizeof(uint32_t);

	unsigned char *code = new unsigned char[codeSize];
	memcpy(code, stream + pos, (size_t)codeSize);

	this->code = code;
	this->pos = pos + codeSize;
}
