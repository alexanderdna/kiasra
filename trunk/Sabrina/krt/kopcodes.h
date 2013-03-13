#ifndef _KOPCODES_H
#define _KOPCODES_H

/*===================================================*/

#ifdef __cplusplus
extern "C" {
#endif

/*===================================================*/

enum OpCodes
{
	OP_NOP = 0,

	OP_LDTRUE,
	OP_LDFALSE,
	OP_LDC_CH,
	OP_LDC_I1,
	OP_LDC_U1,
	OP_LDC_I2,
	OP_LDC_U2,
	OP_LDC_I4,
	OP_LDC_U4,
	OP_LDC_I8,
	OP_LDC_U8,
	OP_LDC_R4,
	OP_LDC_R8,
	OP_LDSTR,

	OP_LDTHIS,
	OP_LDNULL,
	OP_LDTYPE,
	OP_LDMETHOD,

	OP_LDLOC,
	OP_LDLOCA,
	OP_LDARG,
	OP_LDARGA,
	OP_LDFLD,
	OP_LDFLDA,
	OP_LDSFLD,
	OP_LDSFLDA,
	OP_LDELEM,
	OP_LDIND,

	OP_STLOC,
	OP_STARG,
	OP_STFLD,
	OP_STSFLD,
	OP_STELEM,
	OP_STIND,

	OP_POP,
	OP_DUP,

	OP_JMP,
	OP_JE,
	OP_JNE,
	OP_JL,
	OP_JLE,
	OP_JG,
	OP_JGE,
	OP_JTRUE,
	OP_JFALSE,

	OP_ENTER,
	OP_LEAVE,
	OP_THROW,
	
	OP_CALLI,
	OP_CALLS,
	OP_CALLO,
	OP_RET,

	OP_ADD,
	OP_SUB,
	OP_MUL,
	OP_DIV,
	OP_REM,
	OP_NEG,
	OP_EQU,
	OP_NEQ,
	OP_LES,
	OP_LEQ,
	OP_GRT,
	OP_GEQ,
	OP_CAT,
	OP_AND,
	OP_OR,
	OP_XOR,
	OP_NOT,
	OP_SHL,
	OP_SHR,

	OP_ISTRUE,
	OP_ISNULL,
	
	OP_NEWOBJ,
	OP_NEWDEL,
	OP_NEWARR,
	OP_MAKEARR,
	
	OP_CONV_CH,
	OP_CONV_I1,
	OP_CONV_U1,
	OP_CONV_I2,
	OP_CONV_U2,
	OP_CONV_I4,
	OP_CONV_U4,
	OP_CONV_I8,
	OP_CONV_U8,
	OP_CONV_R4,
	OP_CONV_R8,
	OP_CONV_S,
	
	OP_CAST,
	OP_ISINST,

	OP_HLT,
	OP_BRK,
	OP_TRACE,

	OP_OPCODE_COUNT
};

/*===================================================*/

#ifdef __cplusplus
}
#endif

#endif/*_KOPCODES_H*/
