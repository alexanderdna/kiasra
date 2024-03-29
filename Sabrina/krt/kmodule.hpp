#pragma once

#include "krt.h"
#include "kmeta.hpp"
#include "kobject.hpp"

class KEnvironment;

class ModuleLoader
{
public:
	enum class ModuleLoadError
	{
		OK = 0,

		InvalidPath,
		InvalidModule,
		InvalidLib,
		CannotOpen,
		CannotLoadLib,
		CannotLoadImportedModule,
		CannotLoadNativeFunction,
	};

	enum class ModuleValidationResult
	{
		OK = 0,

		InvalidMagic,
		InvalidVersion,
		InvalidModuleType,
		InvalidModuleFormat,
	};

	enum class ModuleLoadPhase
	{
		Initial = 0,
		Opened,
		Loaded,
		Baked,
	};

	enum class ModuleType
	{
		Window,
		Console,
		Library,
	};

protected:
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

	KMODULEATTRIBUTES attrs;

	const char *fullpath;
	const char *dirpath;
	const char *filename;

	ModuleLoader **moduleLoaders;

	ModuleLoadError err;

	unsigned char *code;

	ModuleType moduleType;
	bool hasDebugInfo;
	uint32_t hash;

	ktoken16_t entryClass;
	ktoken16_t entryMethod;

	unsigned char *stream;
	knuint_t pos;
	knuint_t streamLength;

	bool isCleaned;
	
#ifdef ISWIN
	HMODULE     libHandle;
#else
	void       *libHandle;
#endif

	ModuleLoadPhase loadPhase;

	ModuleDef *module;

public:
	static ModuleLoader * create(const char *fullpath, bool isNative);

protected:
	ModuleLoader(const char *fullpath, const char *dirpath, const char *filename, bool isNative);
public:
	~ModuleLoader(void);

public:
	bool load(void);

	ModuleDef * getModule(void);

	const char * getFullPath(void);
	const char * getDirPath(void);
	const char * getFilename(void);

	ModuleLoadError getError(void);

	void onDispose(void);

protected:
	bool open(void);
	bool loadMeta(void);
	bool bake(void);
	void clean();

	ModuleValidationResult validateHeader();
	bool loadStringTable();
	bool loadTypeTable();
	bool loadModuleTable();
	bool loadClassTable();
	bool loadDelegateTable();
	bool loadFieldTable();
	bool loadMethodTable();
	bool loadParamTable();
	bool loadDelegateParamTable();
	bool loadLocalTable();
	bool loadCode();
};
