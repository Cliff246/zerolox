#include <assert.h>
#include <limits.h>
#include <string.h>
#include "MY_Lexer.h"

int main(void)
{
	char source[] = "header:\n    m.mv r12, r3\n";
	MY_Lexer_t *lexer = MY_init_Lexer(source);
	MY_lex_Lexer(lexer);
	char buffer[64];
	int start = -1;
	for(int i = 0; i < lexer->token_count; ++i)
	{
		if(MY_get_token_string_Lexer(lexer, i, buffer, sizeof(buffer)) == 1 && strcmp(buffer, "m") == 0)
		{
			start = i;
			break;
		}
	}
	assert(start > 0);
	MY_Token_group_t group = MY_init_Token_group(lexer, start, 8);
	assert(MY_get_substring_Token_group(group, 0, 3, buffer, sizeof(buffer)) == 4);
	assert(strcmp(buffer, "m.mv") == 0);
	assert(MY_get_substring_Token_group(group, 4, 4, buffer, sizeof(buffer)) == 7);
	assert(strcmp(buffer, "r12, r3") == 0);
	assert(MY_get_substring_Token_group(group, 0, 8, buffer, sizeof(buffer)) == 12);
	assert(strcmp(buffer, "m.mv r12, r3") == 0);
	assert(MY_get_token_string_Token_group(group, 4, buffer, sizeof(buffer)) == 3);
	assert(strcmp(buffer, "r12") == 0);
	char exact[5];
	assert(MY_get_substring_Token_group(group, 0, 3, exact, sizeof(exact)) == 4);
	assert(strcmp(exact, "m.mv") == 0);
	assert(MY_get_substring_Token_group(group, 0, 3, exact, 4) == -1);
	group.length = 3;
	assert(MY_get_substring_Token_group(group, 2, 2, buffer, sizeof(buffer)) == -1);
	assert(MY_get_substring_Token_group(group, -1, 1, buffer, sizeof(buffer)) == -1);
	assert(MY_get_substring_Token_group(group, 0, 0, buffer, sizeof(buffer)) == -1);
	assert(MY_get_substring_Token_group(group, 0, -1, buffer, sizeof(buffer)) == -1);
	assert(MY_get_substring_Token_group(group, 0, INT_MAX, buffer, sizeof(buffer)) == -1);
	assert(MY_get_substring_Token_group(group, INT_MAX, 1, buffer, sizeof(buffer)) == -1);
	assert(MY_get_substring_Token_group(group, 0, 1, NULL, sizeof(buffer)) == -1);
	assert(MY_get_token_string_Token_group(group, 3, buffer, sizeof(buffer)) == -1);
	assert(MY_get_token_string_Token_group(group, 0, NULL, 0) == -1);
	group.start = INT_MAX;
	assert(MY_get_substring_Token_group(group, 0, 1, buffer, sizeof(buffer)) == -1);
	group.cp__lexer = NULL;
	assert(MY_get_substring_Token_group(group, 0, 1, buffer, sizeof(buffer)) == -1);
	// The original substring function continues to use character units.
	assert(MY_get_substring_Lexer(lexer, 0, 6, buffer, sizeof(buffer)) == 6);
	assert(strcmp(buffer, "header") == 0);
	MY_free_Lexer(lexer);
	return 0;
}
