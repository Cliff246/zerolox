#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "MY_lexer.h"


static void check_tokens(void)
{
    char source[] = "12 _label:\n\t{} ++ +- %= ^= 0xAf 42 1.5 \"hi\" /: comment\nnext";
    MY_token_type_t types[] =
    {
        MY_TOKEN_INT, MY_TOKEN_WORD, MY_TOKEN_COLON,
        MY_TOKEN_CURLYBRACE_OPEN, MY_TOKEN_CURLYBRACE_CLOSED,
        MY_TOKEN_ADD_ADD, MY_TOKEN_ADD, MY_TOKEN_MINUS,
        MY_TOKEN_MODULO_EQUALS, MY_TOKEN_HAT_EQUALS,
        MY_TOKEN_HEXDECIMAL, MY_TOKEN_INT, MY_TOKEN_FLOAT,
        MY_TOKEN_STRING, MY_TOKEN_WORD
    };
    MY_lexer_t *lexer = MY_initialize_lexer(source);
    MY_lex(lexer);
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
    assert(MY_get_token_string(lexer, 0, exact, 2) == -1);
    assert(exact[1] == 'y');
    char buffer[32];
    assert(MY_get_token_string(lexer, 1, buffer, sizeof(buffer)) == 6);
    assert(strcmp(buffer, "_label") == 0);
    assert(MY_get_substring(lexer, 3, 6, buffer, sizeof(buffer)) == 6);
    assert(strcmp(buffer, "_label") == 0);
    assert(MY_get_substring(lexer, lexer->string_length - 4, 4, buffer, sizeof(buffer)) == 4);
    assert(strcmp(buffer, "next") == 0);
    assert(MY_get_substring(lexer, 1, -1, buffer, sizeof(buffer)) == -1);
    assert(MY_get_substring(lexer, 0, 2, exact, 2) == -1);
    MY_free_lexer(lexer);
}

int main(void)
{
    check_tokens();
    char source[] = "\nfoo\nbar\r\n\"unfinished";
    MY_lexer_t *lexer = MY_initialize_lexer(source);
    MY_lex(lexer);
    assert(lexer->ap__token_buffer[0].row == 2);
    assert(lexer->ap__token_buffer[0].column == 0);
    assert(lexer->ap__token_buffer[1].row == 3);
    assert(lexer->ap__token_buffer[lexer->token_count - 1].type == MY_TOKEN_INVALID);
    MY_free_lexer(lexer);
    MY_free_lexer(NULL);
    puts("lexer tests passed");
    return 0;
}
