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
	return 0;
}
