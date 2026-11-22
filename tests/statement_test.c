#include <assert.h>
#include <stdlib.h>
#include "ASM_Statement.h"

static void check_statement(char *_source, ZLX_ASM_statement_type_t _expected)
{
	MY_Lexer_t *lexer = MY_init_Lexer(_source);
	MY_lex_Lexer(lexer);
	MY_Token_group_t group = MY_init_Token_group(lexer, 0, lexer->token_count);
	assert(ZLX_ASM_classify_statement(group) == _expected);
	if(_expected == ZLX_ASM_statement_type_operation_mnemonic)
	{
		ZLX_ASM_Assembler_t assembler = {0};
		ZLX_ASM_Statement_t *statement = ZLX_ASM_convert_token_to_statement_Assembler(&assembler, group);
		assert(statement && statement->type == _expected);
		free(statement);
	}
	MY_free_Lexer(lexer);
}

static void check_opcode(char *source, ZLX_ISA_instruction_type_t type, unsigned opcode)
{
	MY_Lexer_t *lexer = MY_init_Lexer(source);
	MY_lex_Lexer(lexer);
	MY_Token_group_t group = MY_init_Token_group(lexer, 0, lexer->token_count);
	ZLX_ASM_Assembler_t assembler = {0};
	ZLX_ASM_Statement_t *statement = ZLX_ASM_convert_token_to_statement_Assembler(&assembler, group);
	assert(statement->type == ZLX_ASM_statement_type_operation_mnemonic);
	assert(statement->u__operation.type == ZLX_ASM_operation_type_mnemonic);
	int index = statement->u__operation.u__mnemonic.opcode;
	assert(ZLX_SYTX_mnemonics[index].type == type);
	assert(ZLX_SYTX_mnemonics[index].opcode == opcode);
	free(statement);
	MY_free_Lexer(lexer);
}

int main(void)
{
	check_statement("m.mv r1, r2", ZLX_ASM_statement_type_operation_mnemonic);
	check_statement("\tm.mv r1, r2", ZLX_ASM_statement_type_operation_mnemonic);
	check_statement("		m.mv r1, r2", ZLX_ASM_statement_type_operation_mnemonic);
	check_statement("   m.mv r1, r2", ZLX_ASM_statement_type_operation_mnemonic);
	check_statement(" \t mv r1, r2", ZLX_ASM_statement_type_operation_mnemonic);
	check_statement("\t@jumpto r3", ZLX_ASM_statement_type_operation_semantic);
	check_statement("\tjump:", ZLX_ASM_statement_type_label);
	check_statement("\t%include", ZLX_ASM_statement_type_directive);
	check_statement("", ZLX_ASM_statement_type_invalid);
	check_statement(" \t ", ZLX_ASM_statement_type_invalid);
	check_statement("\tm. r1", ZLX_ASM_statement_type_invalid);
	check_opcode("calli 2", ZLX_ISA_instruction_type_minor, ZLX_ISA_minor_operation_calli);
	check_opcode("dec t0, 32", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_dec);
	check_opcode("getneg a0, 32", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_get_neg);
	check_opcode("get5 a0, 16", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_get_pos);
	check_opcode("j t0", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_j);
	check_opcode("extb t0, t1, 31", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_extb);
	check_opcode("andn t0, t1, t2", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_andn);
	check_opcode("clt t0, t1, t2", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_clt);
	check_opcode("n.cgeu t0, t1, t2", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_cgeu);
	check_opcode("n.add t0, t1, t2", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_add);
	check_opcode("m.add t0, t1", ZLX_ISA_instruction_type_minor, ZLX_ISA_minor_operation_add);

	check_opcode("addi ac, 1", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_inc);
	check_opcode("addi t0, 1", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_inc);
	check_opcode("n.addi t0, t1, 2", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_addi);
	check_opcode("addi t0, t1, 2", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_addi);
	check_opcode("n.slli ac, 1", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_shift_left);
	check_opcode("slli t0, 1", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_shift_left);
	check_opcode("n.slli t0, t1, 2", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_slli);
	check_opcode("slli t0, t1, 2", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_slli);
	check_opcode("srli ac, 1", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_shift_right);
	check_opcode("srli t0, 1", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_shift_right);
	check_opcode("n.srli t0, t1, 2", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_srli);
	check_opcode("srli t0, t1, 2", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_srli);
	check_opcode("srai ac, 1", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_shift_arith);
	check_opcode("srai t0, 1", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_shift_arith);
	check_opcode("n.srai t0, t1, 2", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_srai);
	check_opcode("srai t0, t1, 2", ZLX_ISA_instruction_type_major, ZLX_ISA_major_operation_srai);
	check_opcode("m.bieqc+ x+, 2", ZLX_ISA_instruction_type_minor, ZLX_ISA_minor_operation_bieqc_update);
	check_opcode("bineqc+ y-, 2", ZLX_ISA_instruction_type_minor, ZLX_ISA_minor_operation_bineqc_update);
	check_statement("add+ t0", ZLX_ASM_statement_type_invalid);
	return 0;
}
