#include "stdafx.h"
#include "module.h"

ModuleLoader::ModuleLoader()
	: modules(NULL), stream(NULL), pos(0), isCleaned(true)
{
	this->stringTable.rows = NULL;
	this->typeTable.rows = NULL;
	this->moduleTable.rows = NULL;
	this->classTable.rows = NULL;
	// TO-DO: other tables
}

ModuleLoader::~ModuleLoader()
{
	if (!this->isCleaned)
	{
		this->clean();
		this->isCleaned == true;
	}
}

bool ModuleLoader::load(unsigned char *stream)
{
	this->isCleaned = false;

	if (this->validateHeader() == false)
		return false;

	this->loadStringTable();
	this->loadTypeTable();
	this->loadModuleTable();
	this->loadClassTable();

	// TO-DO:
	// load delegate table
	// load field table
	// load method table
	// load parameter table
	// load delegate parameter table
	// load local table
	// load code
	// build ModuleDef from all the stuff above
}

void ModuleLoader::clean()
{
	if (this->stringTable.rows)
		delete [] this->stringTable.rows;

	if (this->typeTable.rows)
		delete [] this->typeTable.rows;

	if (this->moduleTable.rows)
		delete [] this->moduleTable.rows;

	if (this->classTable.rows)
		delete [] this->classTable.rows;

	// TO-DO: clean other tables
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
	uint32_t pos = this->pos;

	if (stream[0] != 0xAA
		|| stream[1] != 0xCE
		|| stream[2] != 0xCA
		|| stream[3] != 0xDE)
	{
		return MVR_INVALID_MAGIC;
	}

	if (stream[4] != 0x01 || stream[5] != 0x00)
		return MVR_INVALID_VERSION;

	switch (stream[6])
	{
	case 0x00:
		this->moduleType = MT_WINDOW;
		break;
	case 0x01:
		this->moduleType = MT_CONSOLE;
		break;
	case 0x02:
		this->moduleType = MT_LIBRARY;
		break;

	default:
		return MVR_INVALID_MODULE_TYPE;
	}

	if (stream[7] == 0x01)
		this->hasDebugInfo = true;
	else if (stream[7] == 0x00)
		this->hasDebugInfo = false;
	else
		return MVR_INVALID_MODULE_FORMAT;

	this->hash = *(uint32_t *)(stream + 8);
	pos = 8 + sizeof(uint32_t);

	this->entryClass = *(ktoken_t *)(stream + pos);
	pos += sizeof(ktoken_t);

	this->entryMethod = *(ktoken_t *)(stream + pos);
	pos += sizeof(ktoken_t);

	this->pos = pos;

	return MVR_OK;
}

void ModuleLoader::loadStringTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;

	uint32_t rowCount = *(uint32_t *)(stream + pos);
	pos += sizeof(uint32_t);

	kcstring_t *rows = new kcstring_t[rowCount];

	for (uint32_t i = 0; i < rowCount; ++i)
	{
		uint32_t length = *(uint32_t *)(stream + pos);
		pos += sizeof(uint32_t);

		unsigned char *row = new unsigned char[length];
		for (uint32_t j = 0; j < length; ++j, ++pos)
			row[j] = stream[pos];

		rows[i] = (kcstring_t)row;
	}

	this->stringTable.rowCount = rowCount;
	this->stringTable.rows = rows;
	this->pos = pos;
}

void ModuleLoader::loadTypeTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;

	uint32_t rowCount = *(uint32_t *)(stream + pos);
	pos += sizeof(uint32_t);

	MetaTypeDef *rows = new MetaTypeDef[rowCount];

	for (uint32_t i = 0; i < rowCount; ++i)
	{
		MetaTypeDef &typeDef = rows[i];

		typeDef.tag = *(TypeTag *)(stream + pos);
		pos += sizeof(TypeTag);

		typeDef.dim = *(uint16_t *)(stream + pos);
		pos += sizeof(uint16_t);

		if (typeDef.tag | K_UDT)
		{
			typeDef.token = *(uint16_t *)(stream + pos);
			pos += sizeof(uint16_t);
		}
	}

	this->typeTable.rowCount = rowCount;
	this->typeTable.rows = rows;
	this->pos = pos;
}

void ModuleLoader::loadModuleTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kcstring_t *stringRows = this->stringTable.rows;

	uint16_t rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaModuleDef *rows = new MetaModuleDef[rowCount];

	for (uint16_t i = 0; i < rowCount; ++i)
	{
		MetaModuleDef &row = rows[i];

		uint32_t pathToken = *(uint32_t *)(stream + pos);
		pos += sizeof(uint32_t);

		row.path = stringRows[pathToken];

		row.hash = *(uint32_t *)(stream + pos);
		pos += sizeof(uint32_t);
	}

	this->moduleTable.rowCount = rowCount;
	this->moduleTable.rows = rows;
	this->pos = pos;
}

void ModuleLoader::loadClassTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kcstring_t *stringRows = this->stringTable.rows;
	
	uint16_t rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaClassDef *rows = new MetaClassDef[rowCount];

	for (uint16_t i = 0; i < rowCount; ++i)
	{
		MetaClassDef &row = rows[i];

		row.attrs = *(ClassAttribute *)(stream + pos);
		pos += sizeof(ClassAttribute);

		row.farIndex = *(uint16_t *)(stream + pos);
		pos += sizeof(uint16_t);

		uint32_t nameToken = *(uint32_t *)(stream + pos);
		pos += sizeof(uint32_t);

		row.name = stringRows[nameToken];

		if (row.farIndex == 0)
		{
			row.firstField = *(ktoken_t *)(stream + pos);
			pos += sizeof(ktoken_t);

			row.firstMethod = *(ktoken_t *)(stream + pos);
			pos += sizeof(ktoken_t);
		}
		else
		{
			row.moduleIndex = *(ktoken_t *)(stream + pos);
			pos += sizeof(ktoken_t);
		}
	}

	this->classTable.rowCount = rowCount;
	this->classTable.rows = rows;
	this->pos = pos;
}
