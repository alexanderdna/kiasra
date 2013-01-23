#include "stdafx.h"
#include "module.h"

// ===I know what I am doing===

#define _READ(DEST, TYPE) \
	DEST = *(TYPE *)(stream + pos); \
	pos += sizeof(TYPE);

#define _VREAD(DEST, TYPE) \
	TYPE DEST = *(TYPE *)(stream + pos); \
	pos += sizeof(TYPE);

// ===I know what I am doing===

ModuleLoader::ModuleLoader()
	: modules(NULL), stream(NULL), pos(0), isCleaned(true)
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
}

ModuleLoader::~ModuleLoader()
{
	if (!this->isCleaned)
	{
		this->clean();
		this->isCleaned = true;
	}
}

bool ModuleLoader::load(unsigned char *stream, uint32_t streamLength)
{
	this->stream = stream;
	this->pos = 0;
	this->streamLength = streamLength;
	this->isCleaned = false;

	if (this->validateHeader() == false)
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

	// TO-DO:
	// build ModuleDef from all the stuff above

	return true;
}

void ModuleLoader::clean()
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

	if (this->code)
	{
		delete [] this->code;
		this->code = NULL;
	}
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

	unsigned int rowCount = *(uint32_t *)(stream + pos);
	pos += sizeof(uint32_t);

	kcstring_t *rows = new kcstring_t[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		_VREAD(length, uint32_t);

		unsigned char *row = new unsigned char[length];
		for (uint32_t j = 0; j < length; ++j, ++pos)
			row[j] = stream[pos];

		rows[i] = (kcstring_t)row;
	}

	this->stringTable.rowCount = (uint32_t)rowCount;
	this->stringTable.rows = rows;
	this->pos = pos;
}

void ModuleLoader::loadTypeTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	
	unsigned int rowCount = *(uint32_t *)(stream + pos);
	pos += sizeof(uint32_t);

	MetaTypeDef *rows = new MetaTypeDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaTypeDef &row = rows[i];

		_READ(row.tag, TypeTag);

		_READ(row.dim, uint16_t);

		if (row.tag & K_UDT)
		{
			_READ(row.token, uint16_t);
		}
	}

	this->typeTable.rowCount = (uint32_t)rowCount;
	this->typeTable.rows = rows;
	this->pos = pos;
}

void ModuleLoader::loadModuleTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kcstring_t *stringRows = this->stringTable.rows;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaModuleDef *rows = new MetaModuleDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaModuleDef &row = rows[i];

		_VREAD(pathToken, uint32_t);
		row.path = stringRows[pathToken];

		_READ(row.hash, uint32_t);
	}

	this->moduleTable.rowCount = (uint16_t)rowCount;
	this->moduleTable.rows = rows;
	this->pos = pos;
}

void ModuleLoader::loadClassTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kcstring_t *stringRows = this->stringTable.rows;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaClassDef *rows = new MetaClassDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaClassDef &row = rows[i];

		_READ(row.attrs, ClassAttribute);

		_VREAD(nameToken, uint32_t);
		row.name = stringRows[nameToken];

		_READ(row.farIndex, uint16_t);

		if (row.farIndex)
		{
			_READ(row.moduleIndex, ktoken_t);
		}
		else
		{
			_READ(row.firstField, ktoken_t);
			_READ(row.firstMethod, ktoken_t);
		}
	}

	this->classTable.rowCount = (uint16_t)rowCount;
	this->classTable.rows = rows;
	this->pos = pos;
}

void ModuleLoader::loadDelegateTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kcstring_t *stringRows = this->stringTable.rows;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaDelegateDef *rows = new MetaDelegateDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaDelegateDef &row = rows[i];

		_READ(row.attrs, ClassAttribute);

		_VREAD(nameToken, uint32_t);
		row.name = stringRows[nameToken];

		_READ(row.farIndex, uint16_t);

		if (row.farIndex)
		{
			_READ(row.moduleIndex, uint16_t);
		}
		else
		{
			_READ(row.returnType, ktypetoken_t);
			_READ(row.firstParam, ktoken_t);
		}
	}

	this->delegateTable.rowCount = (uint16_t)rowCount;
	this->delegateTable.rows = rows;
	this->pos = pos;
}

void ModuleLoader::loadFieldTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kcstring_t *stringRows = this->stringTable.rows;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaFieldDef *rows = new MetaFieldDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaFieldDef &row = rows[i];

		_READ(row.attrs, FieldAttribute);

		_VREAD(nameToken, uint32_t);
		row.name = stringRows[nameToken];

		_READ(row.declType, ktypetoken_t);
	}

	this->fieldTable.rowCount = (uint16_t)rowCount;
	this->fieldTable.rows = rows;
	this->pos = pos;
}

void ModuleLoader::loadMethodTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kcstring_t *stringRows = this->stringTable.rows;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaMethodDef *rows = new MetaMethodDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaMethodDef &row = rows[i];

		_READ(row.attrs, MethodAttribute);

		_VREAD(nameToken, uint32_t);
		row.name = stringRows[nameToken];

		_READ(row.returnType, ktypetoken_t);
		_READ(row.firstParam, ktoken_t);
		_READ(row.firstLocal, ktoken_t);

		_READ(row.address, uint32_t);
	}

	this->methodTable.rowCount = rowCount;
	this->methodTable.rows = rows;
	this->pos = pos;
}

void ModuleLoader::loadParamTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kcstring_t *stringRows = this->stringTable.rows;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaParamDef *rows = new MetaParamDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaParamDef &row = rows[i];

		_VREAD(nameToken, uint32_t);
		row.name = stringRows[nameToken];

		_READ(row.declType, ktypetoken_t);
		
		_VREAD(isByRef, uint8_t);
		row.byRef = isByRef!=0;
	}

	this->paramTable.rowCount = (uint16_t)rowCount;
	this->paramTable.rows = rows;
	this->pos = pos;
}

void ModuleLoader::loadDelegateParamTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	kcstring_t *stringRows = this->stringTable.rows;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	MetaParamDef *rows = new MetaParamDef[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		MetaParamDef &row = rows[i];

		_VREAD(nameToken, uint32_t);
		row.name = stringRows[nameToken];

		_READ(row.declType, ktypetoken_t);
		
		_VREAD(isByRef, uint8_t);
		row.byRef = isByRef!=0;
	}

	this->dparamTable.rowCount = (uint16_t)rowCount;
	this->dparamTable.rows = rows;
	this->pos = pos;
}

void ModuleLoader::loadLocalTable()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;
	
	unsigned int rowCount = *(uint16_t *)(stream + pos);
	pos += sizeof(uint16_t);

	ktypetoken_t *rows = new ktypetoken_t[rowCount];

	for (unsigned int i = 0; i < rowCount; ++i)
	{
		_READ(rows[i], ktypetoken_t);
	}

	this->localTable.rowCount = (uint16_t)rowCount;
	this->localTable.rows = rows;
	this->pos = pos;
}

void ModuleLoader::loadCode()
{
	unsigned char *stream = this->stream;
	uint32_t pos = this->pos;

	uint32_t codeSize = this->streamLength - pos;

	unsigned char *code = new unsigned char[codeSize];
	for (unsigned int i = 0, j = pos; i < codeSize; ++i, ++j)
		code[i] = stream[j];

	this->code = code;
	this->pos = pos + codeSize;
}
