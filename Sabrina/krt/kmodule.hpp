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
		uint16_t rowCount;
		MetaFieldDef *rows;
	} fieldTable;

	struct
	{
		uint16_t rowCount;
		MetaMethodDef *rows;
	} methodTable;

	struct
	{
		uint16_t rowCount;
		MetaParamDef *rows;
	} paramTable;

	struct
	{
		uint16_t rowCount;
		MetaParamDef *rows;
	} dparamTable;

	struct
	{
		uint16_t rowCount;
		ktoken32_t *rows;
	} localTable;

	KMODULEATTRIBUTES attrs;

	kstring_t importerPath;
	kstring_t fullPath;
	kstring_t filename;

	ModuleLoader **moduleLoaders;

	ModuleLoadError err;

	unsigned char *code;

	ModuleType moduleType;
	bool hasDebugInfo;
	uint32_t hash;

	// two-part token for the entry point
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
	static ModuleLoader * create(KMODULEATTRIBUTES attrs, kstring_t importerPath, kstring_t filename);

protected:
	ModuleLoader(KMODULEATTRIBUTES attrs, kstring_t importerPath, kstring_t fullPath, kstring_t filename);
public:
	~ModuleLoader(void);

public:
	bool load(void);

	ModuleDef * getModule(void);

	kstring_t getImporterPath(void);
	kstring_t getFullPath(void);
	kstring_t getFilename(void);

	ModuleLoadError getError(void);

	void onDispose(void);

protected:
	bool open(void);
	bool loadMeta(void);
	bool bake(void);
	void clean();

	ModuleValidationResult validateHeader();
	void loadStringTable();
	void loadTypeTable();
	void loadModuleTable();
	void loadClassTable();
	void loadDelegateTable();
	void loadFieldTable();
	void loadMethodTable();
	void loadParamTable();
	void loadDelegateParamTable();
	void loadLocalTable();
	void loadCode();
};
