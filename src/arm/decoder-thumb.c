/* Copyright (c) 2013-2014 Jeffrey Pfau
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include "decoder.h"

#include "decoder-inlines.h"
#include "emitter-thumb.h"
#include "isa-inlines.h"

#define DEFINE_THUMB_DECODER(NAME, MNEMONIC, BODY) \
	static void _ThumbDecode ## NAME (uint16_t opcode, struct ARMInstructionInfo* info) { \
		UNUSED(opcode); \
		info->mnemonic = ARM_MN_ ## MNEMONIC; \
		BODY; \
	}

#define DEFINE_IMMEDIATE_5_DECODER_DATA_THUMB(NAME, MNEMONIC) \
	DEFINE_THUMB_DECODER(NAME, MNEMONIC, \
		info->op3.immediate = (opcode >> 6) & 0x0007; \
		info->op1.reg = opcode & 0x0007; \
		info->op2.reg = (opcode >> 3) & 0x0007; \
		info->affectsCPSR = 1; \
		info->operandFormat = ARM_OPERAND_REGISTER_1 | \
			ARM_OPERAND_AFFECTED_1 | \
			ARM_OPERAND_REGISTER_2 | \
			ARM_OPERAND_IMMEDIATE_3;)

#define DEFINE_IMMEDIATE_5_DECODER_MEM_THUMB(NAME, MNEMONIC, CYCLES, WIDTH) \
	DEFINE_THUMB_DECODER(NAME, MNEMONIC, \
		info->op1.reg = opcode & 0x0007; \
		info->memory.baseReg = (opcode >> 3) & 0x0007; \
		info->memory.offset.immediate = ((opcode >> 6) & 0x001F) * WIDTH; \
		info->memory.width = (enum ARMMemoryAccessType) WIDTH; \
		info->operandFormat = ARM_OPERAND_REGISTER_1 | \
			ARM_OPERAND_AFFECTED_1 | \
			ARM_OPERAND_MEMORY_2; \
		info->memory.format = ARM_MEMORY_REGISTER_BASE | \
			ARM_MEMORY_IMMEDIATE_OFFSET; \
		CYCLES)

DEFINE_IMMEDIATE_5_DECODER_DATA_THUMB(LSL1, LSL)
DEFINE_IMMEDIATE_5_DECODER_DATA_THUMB(LSR1, LSR)
DEFINE_IMMEDIATE_5_DECODER_DATA_THUMB(ASR1, ASR)
DEFINE_IMMEDIATE_5_DECODER_MEM_THUMB(LDR1, LDR, LOAD_CYCLES, 4)
DEFINE_IMMEDIATE_5_DECODER_MEM_THUMB(LDRB1, LDR, LOAD_CYCLES, 1)
DEFINE_IMMEDIATE_5_DECODER_MEM_THUMB(LDRH1, LDR, LOAD_CYCLES, 2)
DEFINE_IMMEDIATE_5_DECODER_MEM_THUMB(STR1, STR, STORE_CYCLES, 4)
DEFINE_IMMEDIATE_5_DECODER_MEM_THUMB(STRB1, STR, STORE_CYCLES, 1)
DEFINE_IMMEDIATE_5_DECODER_MEM_THUMB(STRH1, STR, STORE_CYCLES, 2)

#define DEFINE_DATA_FORM_1_DECODER_THUMB(NAME, MNEMONIC) \
	DEFINE_THUMB_DECODER(NAME, MNEMONIC, \
		info->op1.reg = opcode & 0x0007; \
		info->op2.reg = (opcode >> 3) & 0x0007; \
		info->op3.reg = (opcode >> 6) & 0x0007; \
		info->affectsCPSR = 1; \
		info->operandFormat = ARM_OPERAND_REGISTER_1 | \
			ARM_OPERAND_AFFECTED_1 | \
			ARM_OPERAND_REGISTER_2 | \
			ARM_OPERAND_REGISTER_3;)

DEFINE_DATA_FORM_1_DECODER_THUMB(ADD3, ADD)
DEFINE_DATA_FORM_1_DECODER_THUMB(SUB3, SUB)

#define DEFINE_DATA_FORM_2_DECODER_THUMB(NAME, MNEMONIC) \
	DEFINE_THUMB_DECODER(NAME, MNEMONIC, \
		info->op1.reg = opcode & 0x0007; \
		info->op2.reg = (opcode >> 3) & 0x0007; \
		info->op3.immediate = (opcode >> 6) & 0x0007; \
		info->affectsCPSR = 1; \
		info->operandFormat = ARM_OPERAND_REGISTER_1 | \
			ARM_OPERAND_AFFECTED_1 | \
			ARM_OPERAND_REGISTER_2 | \
			ARM_OPERAND_IMMEDIATE_3;)

DEFINE_DATA_FORM_2_DECODER_THUMB(ADD1, ADD)
DEFINE_DATA_FORM_2_DECODER_THUMB(SUB1, SUB)

#define DEFINE_DATA_FORM_3_DECODER_THUMB(NAME, MNEMONIC, AFFECTED) \
	DEFINE_THUMB_DECODER(NAME, MNEMONIC, \
		info->op1.reg = (opcode >> 8) & 0x0007; \
		info->op2.immediate = opcode & 0x00FF; \
		info->affectsCPSR = 1; \
		info->operandFormat = ARM_OPERAND_REGISTER_1 | \
			AFFECTED | \
			ARM_OPERAND_IMMEDIATE_2;)

DEFINE_DATA_FORM_3_DECODER_THUMB(ADD2, ADD, ARM_OPERAND_AFFECTED_1)
DEFINE_DATA_FORM_3_DECODER_THUMB(CMP1, CMP, ARM_OPERAND_NONE)
DEFINE_DATA_FORM_3_DECODER_THUMB(MOV1, MOV, ARM_OPERAND_AFFECTED_1)
DEFINE_DATA_FORM_3_DECODER_THUMB(SUB2, SUB, ARM_OPERAND_AFFECTED_1)

#define DEFINE_DATA_FORM_5_DECODER_THUMB(NAME, MNEMONIC, AFFECTED) \
	DEFINE_THUMB_DECODER(NAME, MNEMONIC, \
		info->op1.reg = opcode & 0x0007; \
		info->op2.reg = (opcode >> 3) & 0x0007; \
		info->affectsCPSR = 1; \
		info->operandFormat = ARM_OPERAND_REGISTER_1 | \
			AFFECTED | \
			ARM_OPERAND_REGISTER_2;)

DEFINE_DATA_FORM_5_DECODER_THUMB(AND, AND, ARM_OPERAND_AFFECTED_1)
DEFINE_DATA_FORM_5_DECODER_THUMB(EOR, EOR, ARM_OPERAND_AFFECTED_1)
DEFINE_DATA_FORM_5_DECODER_THUMB(LSL2, LSL, ARM_OPERAND_AFFECTED_1)
DEFINE_DATA_FORM_5_DECODER_THUMB(LSR2, LSR, ARM_OPERAND_AFFECTED_1)
DEFINE_DATA_FORM_5_DECODER_THUMB(ASR2, ASR, ARM_OPERAND_AFFECTED_1)
DEFINE_DATA_FORM_5_DECODER_THUMB(ADC, ADC, ARM_OPERAND_AFFECTED_1)
DEFINE_DATA_FORM_5_DECODER_THUMB(SBC, SBC, ARM_OPERAND_AFFECTED_1)
DEFINE_DATA_FORM_5_DECODER_THUMB(ROR, ROR, ARM_OPERAND_AFFECTED_1)
DEFINE_DATA_FORM_5_DECODER_THUMB(TST, TST, ARM_OPERAND_NONE)
DEFINE_DATA_FORM_5_DECODER_THUMB(NEG, NEG, ARM_OPERAND_AFFECTED_1)
DEFINE_DATA_FORM_5_DECODER_THUMB(CMP2, CMP, ARM_OPERAND_NONE)
DEFINE_DATA_FORM_5_DECODER_THUMB(CMN, CMN, ARM_OPERAND_NONE)
DEFINE_DATA_FORM_5_DECODER_THUMB(ORR, ORR, ARM_OPERAND_AFFECTED_1)
DEFINE_DATA_FORM_5_DECODER_THUMB(MUL, MUL, ARM_OPERAND_AFFECTED_1)
DEFINE_DATA_FORM_5_DECODER_THUMB(BIC, BIC, ARM_OPERAND_AFFECTED_1)
DEFINE_DATA_FORM_5_DECODER_THUMB(MVN, MVN, ARM_OPERAND_AFFECTED_1)

#define DEFINE_DECODER_WITH_HIGH_EX_THUMB(NAME, H1, H2, MNEMONIC, AFFECTED, CPSR) \
	DEFINE_THUMB_DECODER(NAME, MNEMONIC, \
		info->op1.reg = (opcode & 0x0007) | H1; \
		info->op2.reg = ((opcode >> 3) & 0x0007) | H2; \
		if (info->op1.reg == ARM_PC) { \
			info->branchType = ARM_BRANCH_INDIRECT; \
		} \
		info->affectsCPSR = CPSR; \
		info->operandFormat = ARM_OPERAND_REGISTER_1 | \
			AFFECTED | \
			ARM_OPERAND_REGISTER_2;)


#define DEFINE_DECODER_WITH_HIGH_THUMB(NAME, MNEMONIC, AFFECTED, CPSR) \
	DEFINE_DECODER_WITH_HIGH_EX_THUMB(NAME ## 00, 0, 0, MNEMONIC, AFFECTED, CPSR) \
	DEFINE_DECODER_WITH_HIGH_EX_THUMB(NAME ## 01, 0, 8, MNEMONIC, AFFECTED, CPSR) \
	DEFINE_DECODER_WITH_HIGH_EX_THUMB(NAME ## 10, 8, 0, MNEMONIC, AFFECTED, CPSR) \
	DEFINE_DECODER_WITH_HIGH_EX_THUMB(NAME ## 11, 8, 8, MNEMONIC, AFFECTED, CPSR)

DEFINE_DECODER_WITH_HIGH_THUMB(ADD4, ADD, ARM_OPERAND_AFFECTED_1, 0)
DEFINE_DECODER_WITH_HIGH_THUMB(CMP3, CMP, ARM_OPERAND_NONE, 1)
DEFINE_DECODER_WITH_HIGH_THUMB(MOV3, MOV, ARM_OPERAND_AFFECTED_1, 0)

#define DEFINE_IMMEDIATE_WITH_REGISTER_DATA_THUMB(NAME, MNEMONIC, REG) \
	DEFINE_THUMB_DECODER(NAME, MNEMONIC, \
		info->op1.reg = (opcode >> 6) & 0x0007; \
		info->op2.reg = REG; \
		info->op3.immediate = (opcode & 0x00FF) << 2; \
		info->operandFormat = ARM_OPERAND_REGISTER_1 | \
			ARM_OPERAND_AFFECTED_1 | \
			ARM_OPERAND_REGISTER_2 | \
			ARM_OPERAND_IMMEDIATE_3;)

#define DEFINE_IMMEDIATE_WITH_REGISTER_MEM_THUMB(NAME, MNEMONIC, REG, CYCLES) \
	DEFINE_THUMB_DECODER(NAME, MNEMONIC, \
		info->op1.reg = (opcode >> 8) & 0x0007; \
		info->memory.baseReg = REG; \
		info->memory.offset.immediate = (opcode & 0x00FF) << 2; \
		info->memory.width = ARM_ACCESS_WORD; \
		info->operandFormat = ARM_OPERAND_REGISTER_1 | \
			ARM_OPERAND_AFFECTED_1 | \
			ARM_OPERAND_MEMORY_2; \
		info->memory.format = ARM_MEMORY_REGISTER_BASE | \
			ARM_MEMORY_IMMEDIATE_OFFSET; \
		CYCLES;)

DEFINE_IMMEDIATE_WITH_REGISTER_MEM_THUMB(LDR3, LDR, ARM_PC, LOAD_CYCLES)
DEFINE_IMMEDIATE_WITH_REGISTER_MEM_THUMB(LDR4, LDR, ARM_SP, LOAD_CYCLES)
DEFINE_IMMEDIATE_WITH_REGISTER_MEM_THUMB(STR3, STR, ARM_SP, STORE_CYCLES)

DEFINE_IMMEDIATE_WITH_REGISTER_DATA_THUMB(ADD5, ADD, ARM_PC)
DEFINE_IMMEDIATE_WITH_REGISTER_DATA_THUMB(ADD6, ADD, ARM_SP)

#define DEFINE_LOAD_STORE_WITH_REGISTER_THUMB(NAME, MNEMONIC, CYCLES, TYPE) \
	DEFINE_THUMB_DECODER(NAME, MNEMONIC, \
		info->memory.offset.reg = (opcode >> 6) & 0x0007; \
		info->op1.reg = opcode & 0x0007; \
		info->memory.baseReg = (opcode >> 3) & 0x0007; \
		info->memory.width = TYPE; \
		info->operandFormat = ARM_OPERAND_REGISTER_1 | \
			ARM_OPERAND_AFFECTED_1 | /* TODO: Remove this for STR */ \
			ARM_OPERAND_MEMORY_2; \
		info->memory.format = ARM_MEMORY_REGISTER_BASE | \
			ARM_MEMORY_REGISTER_OFFSET; \
		CYCLES;)

DEFINE_LOAD_STORE_WITH_REGISTER_THUMB(LDR2, LDR, LOAD_CYCLES, ARM_ACCESS_WORD)
DEFINE_LOAD_STORE_WITH_REGISTER_THUMB(LDRB2, LDR, LOAD_CYCLES, ARM_ACCESS_BYTE)
DEFINE_LOAD_STORE_WITH_REGISTER_THUMB(LDRH2, LDR, LOAD_CYCLES, ARM_ACCESS_HALFWORD)
DEFINE_LOAD_STORE_WITH_REGISTER_THUMB(LDRSB, LDR, LOAD_CYCLES, ARM_ACCESS_SIGNED_BYTE)
DEFINE_LOAD_STORE_WITH_REGISTER_THUMB(LDRSH, LDR, LOAD_CYCLES, ARM_ACCESS_SIGNED_HALFWORD)
DEFINE_LOAD_STORE_WITH_REGISTER_THUMB(STR2, STR, STORE_CYCLES, ARM_ACCESS_WORD)
DEFINE_LOAD_STORE_WITH_REGISTER_THUMB(STRB2, STR, STORE_CYCLES, ARM_ACCESS_BYTE)
DEFINE_LOAD_STORE_WITH_REGISTER_THUMB(STRH2, STR, STORE_CYCLES, ARM_ACCESS_HALFWORD)

// TODO: Estimate memory cycles
#define DEFINE_LOAD_STORE_MULTIPLE_EX_THUMB(NAME, RN, MNEMONIC, DIRECTION, ADDITIONAL_REG) \
	DEFINE_THUMB_DECODER(NAME, MNEMONIC, \
		info->memory.baseReg = RN; \
		info->op1.immediate = (opcode & 0xFF) | ADDITIONAL_REG; \
		if (info->op1.immediate & (1 << ARM_PC)) { \
			info->branchType = ARM_BRANCH_INDIRECT; \
		} \
		info->operandFormat = ARM_OPERAND_MEMORY_1; \
		info->memory.format = ARM_MEMORY_REGISTER_BASE | \
			ARM_MEMORY_WRITEBACK | \
			DIRECTION;)

#define DEFINE_LOAD_STORE_MULTIPLE_THUMB(NAME) \
	DEFINE_LOAD_STORE_MULTIPLE_EX_THUMB(NAME ## IA, (opcode >> 8) & 0x0007, NAME, ARM_MEMORY_INCREMENT_AFTER, 0)

DEFINE_LOAD_STORE_MULTIPLE_THUMB(LDM)
DEFINE_LOAD_STORE_MULTIPLE_THUMB(STM)

#define DEFINE_CONDITIONAL_BRANCH_THUMB(COND) \
	DEFINE_THUMB_DECODER(B ## COND, B, \
		int8_t immediate = opcode; \
		info->op1.immediate = immediate << 1; \
		info->branchType = ARM_BRANCH; \
		info->condition = ARM_CONDITION_ ## COND; \
		info->operandFormat = ARM_OPERAND_IMMEDIATE_1;)

DEFINE_CONDITIONAL_BRANCH_THUMB(EQ)
DEFINE_CONDITIONAL_BRANCH_THUMB(NE)
DEFINE_CONDITIONAL_BRANCH_THUMB(CS)
DEFINE_CONDITIONAL_BRANCH_THUMB(CC)
DEFINE_CONDITIONAL_BRANCH_THUMB(MI)
DEFINE_CONDITIONAL_BRANCH_THUMB(PL)
DEFINE_CONDITIONAL_BRANCH_THUMB(VS)
DEFINE_CONDITIONAL_BRANCH_THUMB(VC)
DEFINE_CONDITIONAL_BRANCH_THUMB(LS)
DEFINE_CONDITIONAL_BRANCH_THUMB(HI)
DEFINE_CONDITIONAL_BRANCH_THUMB(GE)
DEFINE_CONDITIONAL_BRANCH_THUMB(LT)
DEFINE_CONDITIONAL_BRANCH_THUMB(GT)
DEFINE_CONDITIONAL_BRANCH_THUMB(LE)

#define DEFINE_SP_MODIFY_THUMB(NAME, MNEMONIC) \
	DEFINE_THUMB_DECODER(NAME, MNEMONIC, \
		info->op1.reg = ARM_SP; \
		info->op2.immediate = (opcode & 0x7F) << 2; \
		info->operandFormat = ARM_OPERAND_REGISTER_1 | \
			ARM_OPERAND_AFFECTED_1 | \
			ARM_OPERAND_IMMEDIATE_2;)

DEFINE_SP_MODIFY_THUMB(ADD7, ADD)
DEFINE_SP_MODIFY_THUMB(SUB4, SUB)

DEFINE_LOAD_STORE_MULTIPLE_EX_THUMB(POP, ARM_SP, LDM, ARM_MEMORY_INCREMENT_AFTER, 0)
DEFINE_LOAD_STORE_MULTIPLE_EX_THUMB(POPR, ARM_SP, LDM, ARM_MEMORY_INCREMENT_AFTER, 1 << ARM_PC)
DEFINE_LOAD_STORE_MULTIPLE_EX_THUMB(PUSH, ARM_SP, STM, ARM_MEMORY_DECREMENT_BEFORE, 0)
DEFINE_LOAD_STORE_MULTIPLE_EX_THUMB(PUSHR, ARM_SP, STM, ARM_MEMORY_DECREMENT_BEFORE, 1 << ARM_LR)

DEFINE_THUMB_DECODER(ILL, ILL,
	info->operandFormat = ARM_OPERAND_NONE;
	info->traps = 1;)

DEFINE_THUMB_DECODER(BKPT, BKPT,
	info->operandFormat = ARM_OPERAND_NONE;
	info->traps = 1;)

DEFINE_THUMB_DECODER(B, B,
	int16_t immediate = (opcode & 0x07FF) << 5;
	info->op1.immediate = (((int32_t) immediate) >> 4);
	info->operandFormat = ARM_OPERAND_IMMEDIATE_1;
	info->branchType = ARM_BRANCH;)

DEFINE_THUMB_DECODER(BL1, BL,
	int16_t immediate = (opcode & 0x07FF) << 5;
	info->op1.reg = ARM_LR;
	info->op2.reg = ARM_PC;
	info->op3.immediate = (((int32_t) immediate) << 7);
	info->operandFormat = ARM_OPERAND_REGISTER_1 | ARM_OPERAND_AFFECTED_1 |
		ARM_OPERAND_REGISTER_2 | ARM_OPERAND_AFFECTED_2 |
		ARM_OPERAND_IMMEDIATE_3;)

DEFINE_THUMB_DECODER(BL2, BL,
	info->op1.reg = ARM_PC;
	info->op2.reg = ARM_LR;
	info->op3.immediate = (opcode & 0x07FF) << 1;
	info->operandFormat = ARM_OPERAND_REGISTER_1 | ARM_OPERAND_AFFECTED_1 |
		ARM_OPERAND_REGISTER_2 | ARM_OPERAND_IMMEDIATE_3;
	info->branchType = ARM_BRANCH_LINKED;)

DEFINE_THUMB_DECODER(BX, BX,
	info->op1.reg = (opcode >> 3) & 0xF;
	info->operandFormat = ARM_OPERAND_REGISTER_1;
	info->branchType = ARM_BRANCH_INDIRECT;)

DEFINE_THUMB_DECODER(SWI, SWI,
	info->op1.immediate = opcode & 0xFF;
	info->operandFormat = ARM_OPERAND_IMMEDIATE_1;
	info->traps = 1;)

typedef void (*ThumbDecoder)(uint16_t opcode, struct ARMInstructionInfo* info);

static const ThumbDecoder _thumbDecoderTable[0x400] = {
	DECLARE_THUMB_EMITTER_BLOCK(_ThumbDecode)
};

void ARMDecodeThumb(uint16_t opcode, struct ARMInstructionInfo* info) {
	info->execMode = MODE_THUMB;
	info->opcode = opcode;
	info->branchType = ARM_BRANCH_NONE;
	info->traps = 0;
	info->affectsCPSR = 0;
	info->condition = ARM_CONDITION_AL;
	info->sDataCycles = 0;
	info->nDataCycles = 0;
	info->sInstructionCycles = 1;
	info->nInstructionCycles = 0;
	info->iCycles = 0;
	info->cCycles = 0;
	ThumbDecoder decoder = _thumbDecoderTable[opcode >> 6];
	decoder(opcode, info);
}

bool ARMDecodeThumbCombine(struct ARMInstructionInfo* info1, struct ARMInstructionInfo* info2, struct ARMInstructionInfo* out) {
	if (info1->execMode != MODE_THUMB || info1->mnemonic != ARM_MN_BL) {
		return false;
	}
	if (info2->execMode != MODE_THUMB || info2->mnemonic != ARM_MN_BL) {
		return false;
	}
	if (info1->op1.reg != ARM_LR || info1->op2.reg != ARM_PC) {
		return false;
	}
	if (info2->op1.reg != ARM_PC || info2->op2.reg != ARM_LR) {
		return false;
	}
	out->op1.immediate = info1->op3.immediate | info2->op3.immediate;
	out->operandFormat = ARM_OPERAND_IMMEDIATE_1;
	out->execMode = MODE_THUMB;
	out->mnemonic = ARM_MN_BL;
	out->branchType = ARM_BRANCH_LINKED;
	out->traps = 0;
	out->affectsCPSR = 0;
	out->condition = ARM_CONDITION_AL;
	out->sDataCycles = 0;
	out->nDataCycles = 0;
	out->sInstructionCycles = 2;
	out->nInstructionCycles = 0;
	out->iCycles = 0;
	out->cCycles = 0;
	return true;
}
