#pragma once

#include "kcompile.hpp"

#include <map>
#include <vector>

class StringPool
{
public:
	struct StringInfo
	{
		kstring_t str;
		kuint_t len;
	};

protected:
	ModuleBuilder *moduleBuilder;

	std::vector<StringInfo> stringList;

public:
	StringPool(ModuleBuilder *moduleBuilder);
	~StringPool(void);

	ktoken32_t addString(kstring_t s, kuint_t length);

	std::vector<StringInfo> * getStringList(void);
};

