#include "krt.h"

class ModuleLoader
{
public:
	enum ModuleValidationResult
	{
		MVR_OK = 0,

		MVR_INVALID_MAGIC,
		MVR_INVALID_VERSION,
		MVR_INVALID_MODULE_TYPE,
		MVR_INVALID_MODULE_FORMAT
	};

	enum ModuleType
	{
		MT_WINDOW,
		MT_CONSOLE,
		MT_LIBRARY,

		MT_EXECUTABLE = MT_WINDOW | MT_CONSOLE
	};

private:
	ModuleLoader **modules;

	struct
	{
		uint32_t rowCount;
		kcstring_t *rows;
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
		ktypetoken_t *rows;
	} localTable;

	unsigned char *code;

	ModuleType moduleType;
	bool hasDebugInfo;
	uint32_t hash;

	// two-part token for the entry point
	ktoken_t entryClass;
	ktoken_t entryMethod;

	unsigned char *stream;
	uint32_t pos;
	uint32_t streamLength;
	bool isCleaned;

public:
	ModuleLoader();
	~ModuleLoader();

public:
	bool load(unsigned char *stream, uint32_t streamLength);
	void clean();

private:
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
