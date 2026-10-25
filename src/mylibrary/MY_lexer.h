#ifndef MY_LEXER_H
#define MY_LEXER_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>


#define MY_TOKEN_TYPE_LIST(X)			\
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

#define MY_TOKEN_TYPE_ENUM_NAME(X) MY_TOKEN_ ## X
#define MY_TOKEN_TYPE_ENUM(X) MY_TOKEN_TYPE_ENUM_NAME(X),


typedef enum MY_token_type
{
	MY_TOKEN_TYPE_LIST(MY_TOKEN_TYPE_ENUM)
}MY_token_type_t;

#define MY_TOKEN_TYPE_STRING(X) ("MY_TOKEN_"#X),

extern char *MY_token_type_names[];



typedef struct MY_token
{
	MY_token_type_t type;
	int start;
	int length;
	int column;
	int row;
}MY_token_t;

typedef struct MY_lexer
{
	char *ap__string;
	int string_length;

	int token_allocated;
	int token_count;
	MY_token_t *ap__token_buffer;

	int position;
	int row, column;
	int trow, tcolumn;
	bool print_each;
}MY_lexer_t;

MY_lexer_t *MY_initialize_lexer(char *_string);
int MY_get_token_string(MY_lexer_t *_lexer, int _position, char *_buffer, int _blength);
int MY_get_substring(MY_lexer_t *_lexer, int _start, int _length, char *_buffer, int _blength);
void MY_lex(MY_lexer_t *_lexer);
void MY_print_lexer(MY_lexer_t *lexer);
void MY_free_lexer(MY_lexer_t *_lexer);
#endif
