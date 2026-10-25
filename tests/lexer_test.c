#include "lexer.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>

static void check_tokens(void)
{
    char source[] = "12 _label:\n\t{} ++ +- %= ^= 0xAf 42 1.5 \"hi\" /: comment\nnext";
    ZLX_ASM_token_type_t types[] =
    {
        ZLX_ASM_TOKEN_INT, ZLX_ASM_TOKEN_WORD, ZLX_ASM_TOKEN_COLON,
        ZLX_ASM_TOKEN_CURLYBRACE_OPEN, ZLX_ASM_TOKEN_CURLYBRACE_CLOSED,
        ZLX_ASM_TOKEN_ADD_ADD, ZLX_ASM_TOKEN_ADD, ZLX_ASM_TOKEN_MINUS,
        ZLX_ASM_TOKEN_MODULO_EQUALS, ZLX_ASM_TOKEN_HAT_EQUALS,
        ZLX_ASM_TOKEN_HEXDECIMAL, ZLX_ASM_TOKEN_INT, ZLX_ASM_TOKEN_FLOAT,
        ZLX_ASM_TOKEN_STRING, ZLX_ASM_TOKEN_WORD
    };
    ZLX_ASM_lexer_t *lexer = ZLX_ASM_initialize_lexer(source);
    ZLX_ASM_lex(lexer);
    assert(lexer->m_token_count == (int)(sizeof(types) / sizeof(types[0])));
    for(int i = 0; i < lexer->m_token_count; ++i)
    {
        assert(lexer->m_token_buffer[i].m_type == types[i]);
    }
    assert(lexer->m_token_buffer[3].m_row == 2);
    assert(lexer->m_token_buffer[3].m_column == 4);
    assert(lexer->m_token_buffer[14].m_row == 3);
    assert(lexer->m_token_buffer[14].m_column == 0);
    char exact[2] = {'x', 'y'};
    assert(ZLX_ASM_get_token_string(lexer, 0, exact, 2) == -1);
    assert(exact[1] == 'y');
    char buffer[32];
    assert(ZLX_ASM_get_token_string(lexer, 1, buffer, sizeof(buffer)) == 6);
    assert(strcmp(buffer, "_label") == 0);
    assert(ZLX_ASM_get_substring(lexer, 3, 6, buffer, sizeof(buffer)) == 6);
    assert(strcmp(buffer, "_label") == 0);
    assert(ZLX_ASM_get_substring(lexer, lexer->m_string_length - 4, 4, buffer, sizeof(buffer)) == 4);
    assert(strcmp(buffer, "next") == 0);
    assert(ZLX_ASM_get_substring(lexer, 1, -1, buffer, sizeof(buffer)) == -1);
    assert(ZLX_ASM_get_substring(lexer, 0, 2, exact, 2) == -1);
    ZLX_ASM_free_lexer(lexer);
}

int main(void)
{
    check_tokens();
    char source[] = "\nfoo\nbar\r\n\"unfinished";
    ZLX_ASM_lexer_t *lexer = ZLX_ASM_initialize_lexer(source);
    ZLX_ASM_lex(lexer);
    assert(lexer->m_token_buffer[0].m_row == 2);
    assert(lexer->m_token_buffer[0].m_column == 0);
    assert(lexer->m_token_buffer[1].m_row == 3);
    assert(lexer->m_token_buffer[lexer->m_token_count - 1].m_type == ZLX_ASM_TOKEN_INVALID);
    ZLX_ASM_free_lexer(lexer);
    ZLX_ASM_free_lexer(NULL);
    puts("lexer tests passed");
    return 0;
}
