#ifndef ZLX_ASM_LEXER_H
#define ZLX_ASM_LEXER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>


#define ZLX_ASM_TOKEN_TYPE_LIST(X)			\
	X(EMPTY)							\
	X(INVALID)							\
	X(WORD)								\
	X(INT)								\
	X(FLOAT)							\
	X(HEXDECIMAL)						\
	X(SYMBOL)							\
	X(STRING)							\
	X(EXCLAIM)							\
	X(EXCLAIM_EQUALS)					\
	X(PARENTHESIS_OPEN)					\
	X(PARENTHESIS_CLOSED)				\
	X(CURLYBRACE_OPEN)					\
	X(CURLYBRACE_CLOSED)				\
	X(SQUAREBRACE_OPEN)					\
	X(SQUAREBRACE_CLOSED)				\
	X(ADD)								\
	X(ADD_ADD)							\
	X(ADD_EQUALS)						\
	X(MINUS)							\
	X(MINUS_MINUS)						\
	X(MINUS_EQUALS)						\
	X(SLASH)							\
	X(SLASH_SLASH)						\
	X(SLASH_EQUALS)						\
	X(STAR)								\
	X(STAR_STAR)						\
	X(STAR_EQUALS)						\
	X(COMMA)							\
	X(COLON)							\
	X(SEMICOLON)						\
	X(EQUALS)							\
	X(EQUALS_EQUALS)					\
	X(EQUALS_LESS_THAN)					\
	X(LESS_THAN_EQUALS)					\
	X(LESS_THAN)						\
	X(LESS_THAN_LESS_THAN)				\
	X(LESS_THAN_LESS_THAN_EQUALS)		\
	X(GREATER_THAN)						\
	X(GREATER_THAN_EQUALS)				\
	X(GREATER_THAN_GREATER_THAN)		\
	X(GREATER_THAN_GREATER_THAN_EQUALS)	\
	X(VERTICAL_LINE)					\
	X(VERTICAL_LINE_VERTICAL_LINE)		\
	X(VERTICAL_LINE_EQUALS)				\
	X(AMPERSAND)						\
	X(AMPERSAND_AMPERSAND)				\
	X(AMPERSAND_EQUALS)					\
	X(MODULO)							\
	X(MODULO_EQUALS)					\
	X(HAT)								\
	X(HAT_EQUALS)						\
	X(QUESTION_MARK)					\
	X(PERIOD)							\
	X(AT)								\
	X(HASHTAG)							\

#define ZLX_ASM_TOKEN_TYPE_ENUM_NAME(X) ZLX_ASM_TOKEN_ ## X
#define ZLX_ASM_TOKEN_TYPE_ENUM(X) ZLX_ASM_TOKEN_TYPE_ENUM_NAME(X),


typedef enum ZLX_ASM_token_type
{
	ZLX_ASM_TOKEN_TYPE_LIST(ZLX_ASM_TOKEN_TYPE_ENUM)
}ZLX_ASM_token_type_t;

#define ZLX_ASM_TOKEN_TYPE_STRING(X) ("ZLX_ASM_TOKEN_"#X),

extern char *ZLX_ASM_token_type_names[];



typedef struct ZLX_ASM_token
{
	ZLX_ASM_token_type_t m_type;
	int m_start;
	int m_length;
	int m_column;
	int m_row;
}ZLX_ASM_token_t;

typedef struct ZLX_ASM_lexer
{
	char *m_string;
	int m_string_length;

	int m_token_allocated;
	int m_token_count;
	ZLX_ASM_token_t *m_token_buffer;

	int m_position;
	int m_row, m_column;
	int m_trow, m_tcolumn;
	bool m_print_each;
}ZLX_ASM_lexer_t;

ZLX_ASM_lexer_t *ZLX_ASM_initialize_lexer(char *_string);
int ZLX_ASM_get_token_string(ZLX_ASM_lexer_t *_lexer, int _position, char *_buffer, int _blength);
int ZLX_ASM_get_substring(ZLX_ASM_lexer_t *_lexer, int _start, int _length, char *_buffer, int _blength);
void ZLX_ASM_lex(ZLX_ASM_lexer_t *_lexer);
void ZLX_ASM_print_lexer(ZLX_ASM_lexer_t *lexer);
void ZLX_ASM_free_lexer(ZLX_ASM_lexer_t *_lexer);
#endif
