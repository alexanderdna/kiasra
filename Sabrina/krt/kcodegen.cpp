#include "kcodegen.hpp"

#include <cstdlib>

#define DEFAULT_CODE_BUFFER	16

#define EMIT_OPCODE(opcode)	this->buffer[this->size++] = (unsigned char)opcode
#define EMIT_VAL(val, t)	do { *(t *)(this->buffer + this->size) = (t)val; this->size += sizeof(t); } while(0)

//public
CodeGen::CodeGen(MethodBuilder *methodBuilder)
	: methodBuilder(methodBuilder),
	size(0), capacity(DEFAULT_CODE_BUFFER),
	isFinished(false)
{
	this->moduleBuilder = methodBuilder->classBuilder->moduleBuilder;
	this->buffer = new unsigned char[this->capacity];
}

//public
CodeGen::~CodeGen(void)
{
	if (this->buffer)
	{
		delete []this->buffer;
		this->buffer = NULL;
	}

	for (std::vector<Label *>::const_iterator it = this->labels.begin();
		it != this->labels.end(); ++it)
	{
		delete *it;
		this->labels.erase(it);
	}

	for (std::vector<Fixup *>::const_iterator it = this->fixups.begin();
		it != this->fixups.end(); ++it)
	{
		delete *it;
		this->fixups.erase(it);
	}
}


//public
void CodeGen::finish(void)
{
	if (this->isFinished)
		return;

	for (std::vector<Fixup *>::const_iterator it = this->fixups.begin();
		it != this->fixups.end(); ++it)
	{
		Fixup *fixup = *it;
		Label *label = fixup->label;
		kuint_t position = fixup->position;

		*(kshort_t *)(this->buffer + position) = this->calculateDistance(position, label->position);
	}

	this->isFinished = true;
}


//public
const unsigned char * CodeGen::getCodeStream(void)
{
	return this->buffer;
}

//public
kuint_t CodeGen::getCodeSize(void)
{
	return this->size;
}


//public
Label * CodeGen::defineLabel(void)
{
	Label *label = new Label;
	this->labels.push_back(label);

	return label;
}

//public
void CodeGen::markLabel(Label *label)
{
	label->position = this->size;
}


//public
void CodeGen::emit(KOPCODES opcode)
{
	this->ensureCapacity(1);
	EMIT_OPCODE(opcode);
}

//public
void CodeGen::emit(KOPCODES opcode, kchar_t val)
{
	this->ensureCapacity(1+sizeof(val));
	EMIT_OPCODE(opcode);
	EMIT_VAL(val, kchar_t);
}

//public
void CodeGen::emit(KOPCODES opcode, ksbyte_t val)
{
	this->ensureCapacity(1+sizeof(ksbyte_t));
	EMIT_OPCODE(opcode);
	EMIT_VAL(val, ksbyte_t);
}

//public
void CodeGen::emit(KOPCODES opcode, kbyte_t val)
{
	this->ensureCapacity(1+sizeof(kbyte_t));
	EMIT_OPCODE(opcode);
	EMIT_VAL(val, kbyte_t);
}

//public
void CodeGen::emit(KOPCODES opcode, kshort_t val)
{
	this->ensureCapacity(1+sizeof(kshort_t));
	EMIT_OPCODE(opcode);
	EMIT_VAL(val, kshort_t);
}

//public
void CodeGen::emit(KOPCODES opcode, kushort_t val)
{
	this->ensureCapacity(1+sizeof(kushort_t));
	EMIT_OPCODE(opcode);
	EMIT_VAL(val, kushort_t);
}

//public
void CodeGen::emit(KOPCODES opcode, kint_t val)
{
	this->ensureCapacity(1+sizeof(kint_t));
	EMIT_OPCODE(opcode);
	EMIT_VAL(val, kint_t);
}

//public
void CodeGen::emit(KOPCODES opcode, kuint_t val)
{
	this->ensureCapacity(1+sizeof(kuint_t));
	EMIT_OPCODE(opcode);
	EMIT_VAL(val, kuint_t);
}

//public
void CodeGen::emit(KOPCODES opcode, klong_t val)
{
	this->ensureCapacity(1+sizeof(klong_t));
	EMIT_OPCODE(opcode);
	EMIT_VAL(val, klong_t);
}

//public
void CodeGen::emit(KOPCODES opcode, kulong_t val)
{
	this->ensureCapacity(1+sizeof(kulong_t));
	EMIT_OPCODE(opcode);
	EMIT_VAL(val, kulong_t);
}

//public
void CodeGen::emit(KOPCODES opcode, kfloat_t val)
{
	this->ensureCapacity(1+sizeof(kfloat_t));
	EMIT_OPCODE(opcode);
	EMIT_VAL(val, kfloat_t);
}

//public
void CodeGen::emit(KOPCODES opcode, kdouble_t val)
{
	this->ensureCapacity(1+sizeof(kdouble_t));
	EMIT_OPCODE(opcode);
	EMIT_VAL(val, kdouble_t);
}

//public
void CodeGen::emit(KOPCODES opcode, kstring_t val, kuint_t length)
{
	this->ensureCapacity(1+sizeof(ktoken32_t));
	EMIT_OPCODE(opcode);
	ktoken32_t token = this->moduleBuilder->addString(val, length);
	EMIT_VAL(token, ktoken32_t);
}

//public
void CodeGen::emit(KOPCODES opcode, LocalBuilder *builder)
{
	this->ensureCapacity(1+sizeof(kushort_t));
	EMIT_OPCODE(opcode);
	EMIT_VAL(builder->localIndex, kushort_t);
}

//public
void CodeGen::emit(KOPCODES opcode, Label *label)
{
	this->ensureCapacity(1+sizeof(kshort_t));

	Fixup *fixup = new Fixup(label, this->size + 3);
	this->fixups.push_back(fixup);

	EMIT_OPCODE(opcode);
	this->size += sizeof(kshort_t);
}

//public
void CodeGen::emit(KOPCODES opcode, ClassDef *classDef)
{
	this->ensureCapacity(1+sizeof(ktoken16_t));

	EMIT_OPCODE(opcode);
	if (classDef->size == sizeof(ClassDef))
	{
		this->moduleBuilder->referenceClass(classDef);
		EMIT_VAL(this->moduleBuilder->importedClassMap->operator[](classDef), ktoken16_t);
	}
	else
	{
		EMIT_VAL(((ClassBuilder *)classDef)->localIndex, ktoken16_t);
	}
}

//public
void CodeGen::emit(KOPCODES opcode, DelegateDef *delegateDef)
{
	this->ensureCapacity(1+sizeof(ktoken16_t));

	EMIT_OPCODE(opcode);
	if (delegateDef->size == sizeof(DelegateDef))
	{
		this->moduleBuilder->referenceDelegate(delegateDef);
		EMIT_VAL(this->moduleBuilder->importedDelegateMap->operator[](delegateDef), ktoken16_t);
	}
	else
	{
		EMIT_VAL(((DelegateBuilder *)delegateDef)->localIndex, ktoken16_t);
	}
}

//public
void CodeGen::emit(KOPCODES opcode, FieldDef *fieldDef)
{
	if (fieldDef->attrs & KFA_STATIC)
	{
		this->ensureCapacity(1+sizeof(ktoken16_t)+sizeof(ktoken16_t));

		EMIT_OPCODE(opcode);
		
		// load class then field
		if (fieldDef->size == sizeof(FieldDef))
		{
			this->moduleBuilder->referenceClass(fieldDef->klass);
			EMIT_VAL(this->moduleBuilder->importedClassMap->operator[](fieldDef->klass), ktoken16_t);
			EMIT_VAL(fieldDef->localIndex, ktoken16_t);
		}
		else
		{
			EMIT_VAL(((FieldBuilder *)fieldDef)->classBuilder->localIndex, ktoken16_t);
			EMIT_VAL(((FieldBuilder *)fieldDef)->localIndex, ktoken16_t);
		}
	}
	else
	{
		this->ensureCapacity(1+sizeof(ktoken16_t)+sizeof(ktoken16_t));
		
		EMIT_OPCODE(opcode);

		// load field only
		if (fieldDef->size == sizeof(FieldDef))
		{
			this->moduleBuilder->referenceClass(fieldDef->klass);
			EMIT_VAL(fieldDef->localIndex, ktoken16_t);
		}
		else
		{
			EMIT_VAL(((FieldBuilder *)fieldDef)->localIndex, ktoken16_t);
		}
	}
}

//public
void CodeGen::emit(KOPCODES opcode, MethodDef *methodDef)
{
	this->ensureCapacity(1+sizeof(ktoken16_t)+sizeof(ktoken16_t));

	EMIT_OPCODE(opcode);

	if (methodDef->size == sizeof(MethodDef))
	{
		this->moduleBuilder->referenceClass(methodDef->klass);
		EMIT_VAL(this->moduleBuilder->importedClassMap->operator[](methodDef->klass), ktoken16_t);
		EMIT_VAL(methodDef->localIndex, ktoken16_t);
	}
	else
	{
		EMIT_VAL(((MethodBuilder *)methodDef)->classBuilder->localIndex, ktoken16_t);
		EMIT_VAL(((MethodBuilder *)methodDef)->localIndex, ktoken16_t);
	}
}

//public
void CodeGen::emit(KOPCODES opcode, const TypeDef *typeDef)
{
	this->ensureCapacity(1+sizeof(ktoken32_t));
	EMIT_OPCODE(opcode);
	EMIT_VAL(typeDef->localIndex, ktoken32_t);
}

//public
void CodeGen::emit(KOPCODES opcode, const TypeDef *excType, Label *handlerLabel, Label *leaveLabel)
{
	this->ensureCapacity(1+sizeof(ktoken32_t)+sizeof(kshort_t)+sizeof(kshort_t));
	
	Fixup *f1 = new Fixup(handlerLabel, this->size + 3);
	this->fixups.push_back(f1);

	Fixup *f2 = new Fixup(leaveLabel, this->size + 5);
	this->fixups.push_back(f2);
	
	EMIT_OPCODE(opcode);
	EMIT_OPCODE(excType->localIndex);

	this->size += sizeof(kshort_t) * 2;
}

//protected
void CodeGen::ensureCapacity(kuint_t extra)
{
	if (this->size + extra > this->capacity)
	{
		kuint_t newCapacity = this->capacity * 2;
		unsigned char *newBuffer = new unsigned char[newCapacity];
		memcpy(newBuffer, this->buffer, this->capacity);

		delete []this->buffer;

		this->capacity = newCapacity;
		this->buffer = newBuffer;
	}
}

//protected
inline kshort_t CodeGen::calculateDistance(kuint_t fixupPosition, kuint_t targetPosition)
{
	return (targetPosition > fixupPosition) ?
		(kshort_t)(targetPosition - fixupPosition) :
		(kshort_t)(-(kint_t)(fixupPosition - targetPosition));
}
