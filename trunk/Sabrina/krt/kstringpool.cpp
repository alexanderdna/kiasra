#include "kstringpool.hpp"

#include "kstringutils.hpp"

StringPool::StringPool(ModuleBuilder *moduleBuilder)
	: moduleBuilder(moduleBuilder)
{
}


StringPool::~StringPool(void)
{
	for (std::vector<StringInfo>::const_iterator it = this->stringList.begin();
		it != this->stringList.end(); ++it)
	{
		delete [](it->str);
		this->stringList.erase(it);
	}
}

ktoken32_t StringPool::addString(kstring_t s, kuint_t length)
{
	for (std::vector<StringInfo>::const_iterator it = this->stringList.begin();
		it != this->stringList.end(); ++it)
	{
		if (krt_strequ(s, it->str, length, it->len))
			return (ktoken32_t)(it - this->stringList.begin());
	}

	kstring_t newS = krt_strdup(s, length);
	StringInfo si = { newS, length };
	this->stringList.push_back(si);
	return (ktoken32_t)(this->stringList.size() - 1);
}
