#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "MY_Lexer.h"


static void check_tokens(void)
{
    char source[] = "12 _label:\n\t{} ++ +- %= ^= 0xAf 42 1.5 \"hi\" /: comment\nnext";
    MY_token_type_t types[] =
    {
        MY_token_type_int, MY_token_type_word, MY_token_type_colon,
        MY_token_type_curlybrace_open, MY_token_type_curlybrace_closed,
        MY_token_type_add_add, MY_token_type_add, MY_token_type_minus,
        MY_token_type_modulo_equals, MY_token_type_hat_equals,
        MY_token_type_hexdecimal, MY_token_type_int, MY_token_type_float,
        MY_token_type_string, MY_token_type_word
    };
    MY_Lexer_t *lexer = MY_init_Lexer(source);
    MY_lex_Lexer(lexer);
    assert(lexer->token_count == (int)(sizeof(types) / sizeof(types[0])));
    for(int i = 0; i < lexer->token_count; ++i)
    {
        assert(lexer->ap__token_buffer[i].type == types[i]);
    }
    assert(lexer->ap__token_buffer[3].row == 2);
    assert(lexer->ap__token_buffer[3].column == 4);
    assert(lexer->ap__token_buffer[14].row == 3);
    assert(lexer->ap__token_buffer[14].column == 0);
    char exact[2] = {'x', 'y'};
    assert(MY_get_token_string_Lexer(lexer, 0, exact, 2) == -1);
    assert(exact[1] == 'y');
    char buffer[32];
    assert(MY_get_token_string_Lexer(lexer, 1, buffer, sizeof(buffer)) == 6);
    assert(strcmp(buffer, "_label") == 0);
    assert(MY_get_substring_Lexer(lexer, 3, 6, buffer, sizeof(buffer)) == 6);
    assert(strcmp(buffer, "_label") == 0);
    assert(MY_get_substring_Lexer(lexer, lexer->string_length - 4, 4, buffer, sizeof(buffer)) == 4);
    assert(strcmp(buffer, "next") == 0);
    assert(MY_get_substring_Lexer(lexer, 1, -1, buffer, sizeof(buffer)) == -1);
    assert(MY_get_substring_Lexer(lexer, 0, 2, exact, 2) == -1);
    MY_free_Lexer(lexer);
}

int main(void)
{
    check_tokens();
    char source[] = "\nfoo\nbar\r\n\"unfinished";
    MY_Lexer_t *lexer = MY_init_Lexer(source);
    MY_lex_Lexer(lexer);
    assert(lexer->ap__token_buffer[0].row == 2);
    assert(lexer->ap__token_buffer[0].column == 0);
    assert(lexer->ap__token_buffer[1].row == 3);
    assert(lexer->ap__token_buffer[lexer->token_count - 1].type == MY_token_type_invalid);
    MY_free_Lexer(lexer);
    MY_free_Lexer(NULL);
    puts("lexer tests passed");
    return 0;
}
