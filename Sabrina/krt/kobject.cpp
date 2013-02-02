#include "stdafx.h"

#include "kobject.h"
#include <cstring>

KObject::KObject()
	: type(NULL), vObj(NULL), length(0), vm(NULL)
{
}

KObject::~KObject()
{
	this->cleanUp();
}

KObject::KObject(const KObject &src)
	: type(src.type), length(src.length), vm(src.vm)
{
	this->accept(src);
}

KObject & KObject::operator=(const KObject &src)
{
	if (this != &src)
	{
		this->cleanUp();

		this->type = src.type;
		this->length = src.length;
		this->vm = src.vm;

		this->accept(src);
	}

	return *this;
}

KObject::KObject(KMachine *vm, const TypeDef *type)
{
	this->init(vm, type);
}

const TypeDef * KObject::getType() const
{
	return this->type;
}

const kbool_t KObject::getBool() const
{
	return this->vBool;
}

const kint_t KObject::getInt() const
{
	return this->vInt;
}

const kfloat_t & KObject::getFloat() const
{
	return this->vFloat;
}

const kstring_t KObject::getString() const
{
	return this->vString;
}

KObject & KObject::getElement(uint32_t index)
{
	return this->vObj[index];
}

KObject & KObject::getField(ktoken_t index)
{
	return this->vObj[index];
}

const uint32_t KObject::getLength() const
{
	return this->length;
}

void KObject::init(KMachine *vm, const TypeDef *type)
{
	this->type = type;
	this->vm = vm;
	this->length = 0;
	
	this->vFloat = 0;
}

void KObject::cleanUp()
{
	if (this->type != NULL && this->vObj != NULL)
	{
		const TypeDef *type = this->type;

		if (type->classDef != NULL || type->dim != 0)
		{
			// class, delegate or array
			this->vm->freeObjects(this->vObj);
			this->vObj = NULL;
		}
		else if (type->tag == KT_STRING)
		{
			// string
			delete [] this->vString;
			this->vString = NULL;
		}
	}
}

void KObject::accept(const KObject &src)
{
	const TypeDef *type = this->type;

	if (type->classDef != NULL || type->dim != 0)
	{
		// class, delegate or array
		this->vObj = src.vObj;
		this->vm->referObjects(this->vObj);
	}
	else if (type->tag == KT_STRING && src.vString != NULL)
	{
		// non-null string
		kstring_t s = new wchar_t[this->length];
		wmemcpy(s, src.vString, this->length);
		this->vString = s;
	}
	else
	{
		// value type or null string
		this->vFloat = src.vFloat;
	}
}
