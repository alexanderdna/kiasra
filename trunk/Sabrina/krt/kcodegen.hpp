#pragma once

#include "kopcodes.h"
#include "kcompile.hpp"

#include <vector>

// Represents a position in the method code stream.
struct Label
{
	kuint_t position;
};

// Represents a branching instruction in the method code stream.
struct Fixup
{
	// The branching target.
	Label  *label;
	// The position of this branching instruction.
	kuint_t position;

	Fixup(Label *label, kuint_t position)
		: label(label), position(position) { }
};

// Code generator.
class CodeGen
{
protected:
	ModuleBuilder *moduleBuilder;
	MethodBuilder *methodBuilder;

	unsigned char *buffer;
	kuint_t size;
	kuint_t capacity;

	std::vector<Label *> labels;
	std::vector<Fixup *> fixups;

	bool isFinished;

protected:
	void ensureCapacity(kuint_t extra);
	inline kshort_t calculateDistance(kuint_t fixupPosition, kuint_t targetPosition);

public:
	CodeGen(MethodBuilder *methodBuilder);
	~CodeGen(void);

	void finish(void);

	const unsigned char * getCodeStream(void);
	kuint_t getCodeSize(void);

	Label * defineLabel(void);
	void markLabel(Label *label);

	void emit(KOPCODES opcode);
	void emit(KOPCODES opcode, kchar_t val);
	void emit(KOPCODES opcode, ksbyte_t val);
	void emit(KOPCODES opcode, kbyte_t val);
	void emit(KOPCODES opcode, kshort_t val);
	void emit(KOPCODES opcode, kushort_t val);
	void emit(KOPCODES opcode, kint_t val);
	void emit(KOPCODES opcode, kuint_t val);
	void emit(KOPCODES opcode, klong_t val);
	void emit(KOPCODES opcode, kulong_t val);
	void emit(KOPCODES opcode, kfloat_t val);
	void emit(KOPCODES opcode, kdouble_t val);
	void emit(KOPCODES opcode, kstring_t val, kuint_t length);
	void emit(KOPCODES opcode, LocalBuilder *builder);
	void emit(KOPCODES opcode, Label *label);
	void emit(KOPCODES opcode, ClassDef *classDef);
	void emit(KOPCODES opcode, DelegateDef *delegateDef);
	void emit(KOPCODES opcode, FieldDef *fieldDef);
	void emit(KOPCODES opcode, MethodDef *methodDef);
	void emit(KOPCODES opcode, const TypeDef *typeDef);
	void emit(KOPCODES opcode, const TypeDef *excType, Label *handlerLabel, Label *leaveLabel);
};
