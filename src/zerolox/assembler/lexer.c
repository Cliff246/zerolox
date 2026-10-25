#include "lexer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define BASE_TOKENS_ALLOCATE 10

char *HRT_token_types_String[] =
{
	HRT_TOKEN_TYPE_LIST(HRT_TOKEN_TYPE_STRING)
};


void allocate_tokens(HRT_Lexer_t *lexer)
{
	if(lexer->m_token_allocated == 0)
	{
		lexer->m_token_allocated = 10;
		lexer->m_token_count = 0;

		lexer->m_token_buffer = malloc(sizeof(HRT_Token_t) * lexer->m_token_allocated);
		if(!lexer->m_token_buffer)
			exit(1);

	}
	else
	{
		lexer->m_token_allocated = (lexer->m_token_allocated * 2) + 1;
		void *temp = realloc(lexer->m_token_buffer, sizeof(HRT_Token_t) *  lexer->m_token_allocated);
		if(temp == NULL)
			exit(1);
		lexer->m_token_buffer = temp;
	}
}


void add_token(HRT_Lexer_t *lexer, HRT_Token_t token)
{
	if(lexer->m_token_count >= lexer->m_token_allocated)
	{
		allocate_tokens(lexer);
	}
	lexer->m_token_buffer[lexer->m_token_count++] = token;
}

void print_token(HRT_Lexer_t *lexer, int position)
{
	if(position < lexer->m_token_count && position >= 0)
	{
		HRT_Token_t token = lexer->m_token_buffer[position];
		if(token.m_start < 0)
		{
			return;
		}
		if(token.m_length + token.m_start > lexer->m_string_length)
		{
			return;
		}
		printf("token %s{%d:%d} [%d,%d]: <",HRT_token_types_String[token.m_type], token.m_row, token.m_column,  token.m_start, token.m_length);
		for(int i = token.m_start; i < token.m_length + token.m_start; ++i)
		{
			if(lexer->m_string[i] == '\n')
			{
				putchar('\\');
				putchar('n');
			}
			else
			{
				putchar(lexer->m_string[i]);
			}
		}
		printf(">\n");
	}

}

HRT_Token_t init_token(HRT_Token_type_t type, int start, int stop, int _column, int _row)
{
	return (HRT_Token_t){.m_type = type, .m_start = start, .m_length = stop - start, .m_column = _column, .m_row = _row};

}



char peek(HRT_Lexer_t *lexer)
{

	if(lexer->m_position >= lexer->m_string_length)
		return 0;
	else
	{
		char ch = lexer->m_string[lexer->m_position];
		if(lexer->m_print_each == true)
		{
			printf("%c", ch);
		}
		return ch;

	}
}

char advance(HRT_Lexer_t *lexer)
{
	lexer->m_position++;

	char pk =  peek(lexer);
	if(pk == '\n')
	{
		lexer->m_row++;
		lexer->m_column = 0;
	}
	else if(pk == '\t')
	{
		lexer->m_column+=4;
	}
	else
	{
		lexer->m_column++;
	}
	return pk;
}

int position(HRT_Lexer_t *lexer)
{
	return lexer->m_position;
}


void add_token_start_word(HRT_Lexer_t *lexer, HRT_Token_type_t type, int start)
{
	int stop = position(lexer);
	HRT_Token_t range = init_token(type, start, stop, lexer->m_tcolumn, lexer->m_trow);

	add_token(lexer, range);
}


void has_character_ctype(HRT_Lexer_t *lexer, int (*ctype)(int c))
{
	char scroll = peek(lexer);
	while(ctype(scroll))
	{
		scroll = advance(lexer);
	}
}

void has_character_string(HRT_Lexer_t *lexer, char *string)
{
	char scroll = peek(lexer);
	while(strchr(string, scroll) && scroll != 0)
	{
		scroll = advance(lexer);
	}
}

void has_character_ctype_or_string(HRT_Lexer_t *lexer, int (*ctype)(int c), char *string)
{
	char scroll = peek(lexer);
	while(ctype(scroll) || (strchr(string, scroll) && scroll != 0))
	{
		scroll = advance(lexer);
	}
}


int HRT_get_token_string_Lexer(HRT_Lexer_t *_lexer, int _position, char *_buffer, int _blength)
{
	if(_position < _lexer->m_token_count && _position >= 0)
	{
		HRT_Token_t token = _lexer->m_token_buffer[_position];
		if(token.m_start < 0)
		{
			return -1;
		}
		if(token.m_length + token.m_start > _lexer->m_string_length || token.m_length > _blength)
		{
			return -1;
		}
		int c = 0;
		for(int i = token.m_start; i < token.m_length + token.m_start && c < _blength; ++i, ++c)
		{
			_buffer[c] = _lexer->m_string[i];
		}
		_buffer[c] = '\0';
		return c;
	}
	return -1;
}

void HRT_print_Lexer(HRT_Lexer_t *lexer)
{

	for(int i = 0; i < lexer->m_token_count; ++i)
	{
		print_token(lexer, i);
	}
}


int HRT_get_substring_Lexer(HRT_Lexer_t *_lexer, int _start, int _length, char *_buffer, int _blength)
{
	if(_start < 0)
	{
		return -1;
	}
	int total = _start + _length;
	if(total >= _lexer->m_string_length || _length > _blength)
	{
		return -1;
	}
	int c = 0;
	for(int i = _start; i < _length && c < _blength; ++c, ++i)
	{
		_buffer[c] = _lexer->m_string[i];
	}
	_buffer[c]= '\0';
	return c;
}


HRT_Lexer_t *HRT_initialize_Lexer(char *_string)
{
	if(!_string)
		exit(1);
	HRT_Lexer_t *lexer = (HRT_Lexer_t *)malloc(sizeof(HRT_Lexer_t));
	if(!lexer)
		exit(1);
	lexer->m_token_allocated = 0;
	allocate_tokens(lexer);

	char *copy = strdup(_string);
	lexer->m_string_length = strlen(copy);
	lexer->m_string = copy;
	lexer->m_position = 0;
	lexer->m_row = 1;
	lexer->m_column = 0;
	lexer->m_print_each = false;
	return lexer;
}

void HRT_lex_Lexer(HRT_Lexer_t *_lexer)
{
	if(peek(_lexer) == '\n')
	{
		_lexer->m_row++;
	}
	bool has_need_space = false;

	while(_lexer->m_position < _lexer->m_string_length)
	{
		_lexer->m_tcolumn = _lexer->m_column;
		_lexer->m_trow = _lexer->m_row;
		int start = position(_lexer);
		char current = peek(_lexer);
		char scroll = advance(_lexer);

		int stop;
		if(current == ' ' || current == '\t')
		{
		}
		else if(current == '\n' )
		{

		}

		else if(isalpha(current) || current == '_' && has_need_space == false)
		{
			has_character_ctype_or_string(_lexer, isalnum, "_");

			add_token_start_word(_lexer, HRT_TOKEN_WORD, start);
		}
		else if(current == '\"')
		{
			while(scroll != '\0' && scroll != '\n' && scroll != '\"')
			{
				scroll = advance(_lexer);
			}
			if(scroll == '\"')
			{
				advance(_lexer);
			}
			else
			{
				stop = position(_lexer);
				printf("STRING OVERLAP %d %d\n", start, stop);
				exit(1);
			}
			stop = position(_lexer);
			HRT_Token_t range = init_token(HRT_TOKEN_STRING, start + 1, stop - 1, _lexer->m_tcolumn, _lexer->m_trow);
			add_token(_lexer, range);

		}
		else if(current == '!')
		{
			if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_EXCLAIM_EQUALS, start);

			}
			else
				add_token_start_word(_lexer, HRT_TOKEN_EXCLAIM, start);

		}
		else if(isdigit(current))
		{
			bool has_hexcode = false, has_zero_start = false;
			if(scroll == 'x' || scroll == 'X')
			{
				has_hexcode = true;
			}

			if(current == '0')
			{
				has_zero_start = true;
			}

			if(has_hexcode && has_zero_start)
			{
				scroll = advance(_lexer);
				const char *hex_chars = "0123456789aAbBcCdDeEfF";
				//printf("%c\n", scroll);
				if(!strchr(hex_chars, scroll) || scroll == 0)
				{
					add_token_start_word(_lexer, HRT_TOKEN_INVALID, start);

				}
				else
				{
					has_character_string(_lexer, (char *)hex_chars);

					add_token_start_word(_lexer, HRT_TOKEN_HEXDECIMAL, start);
				}
			}
			else if(has_hexcode && !has_zero_start)
			{
				add_token_start_word(_lexer, HRT_TOKEN_INVALID, start);
			}
			else if(scroll == '.')
			{
				//floating point
				advance(_lexer);
				has_character_ctype(_lexer, isdigit);
				add_token_start_word(_lexer, HRT_TOKEN_FLOAT, start);
			}
			else
			{
				//integer
				bool has_found_dot = false;
				while(isdigit(scroll) && scroll != '\0' && scroll != '\n')
				{
				 	scroll = advance(_lexer);
					if(scroll == '.')
						has_found_dot = true;
				}
				if(has_found_dot == true)
				{
					advance(_lexer);
					has_character_ctype(_lexer, isdigit);
					add_token_start_word(_lexer, HRT_TOKEN_FLOAT, start);

				}
				else
				{
					add_token_start_word(_lexer, HRT_TOKEN_INT, start);
				}
			}

			has_need_space = true;

		}
		else if(current == '.')
		{
			add_token_start_word(_lexer, HRT_TOKEN_PERIOD, start);
		}
		else if(current == '(')
		{
			add_token_start_word(_lexer, HRT_TOKEN_PARENTHESIS_OPEN, start);
		}
		else if(current == ')')
		{
			add_token_start_word(_lexer, HRT_TOKEN_PARENTHESIS_CLOSED, start);
		}
		else if(current == '[')
		{
			add_token_start_word(_lexer, HRT_TOKEN_SQUAREBRACE_OPEN, start);
		}
		else if(current == ']')
		{
			add_token_start_word(_lexer, HRT_TOKEN_SQUAREBRACE_CLOSED, start);

		}
		else if(current == '{')
		{
			add_token_start_word(_lexer, HRT_TOKEN_CURLYBRACE_CLOSED, start);

		}
		else if(current == '}')
		{
			add_token_start_word(_lexer, HRT_TOKEN_CURLYBRACE_CLOSED, start);

		}
		else if(current == '+')
		{
			if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_ADD_EQUALS, start);

			}
			else if(scroll == '-')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_ADD_ADD, start);

			}
			else
			{
				add_token_start_word(_lexer, HRT_TOKEN_ADD, start);
			}
		}
		else if(current == '-')
		{
			if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_MINUS_EQUALS, start);

			}
			else if(scroll == '-')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_MINUS_MINUS, start);

			}
			else
			{
				add_token_start_word(_lexer, HRT_TOKEN_MINUS, start);
			}
		}
		else if(current == '/')
		{
			if(scroll == '/')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_SLASH_SLASH, start);

			}
			else if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_SLASH_EQUALS, start);

			}
			else if(scroll == ':')
			{
				//printf("found this\n");
				//_lexer->m_print_each = true;
				advance(_lexer);
				while(scroll != '\0' && scroll != '\n' )
				{
					scroll = advance(_lexer);
				}
				//_lexer->m_print_each = false;

			}
			else
			{
				add_token_start_word(_lexer, HRT_TOKEN_SLASH, start);

			}
		}
		else if(current == '*')
		{
			if(scroll == '*')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_STAR_STAR, start);

			}
			else if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_STAR_EQUALS, start);

			}
			else
			{
				add_token_start_word(_lexer, HRT_TOKEN_STAR, start);
			}
		}
		else if(current == ',')
		{
			add_token_start_word(_lexer, HRT_TOKEN_COMMA, start);

		}
		else if(current == ':')
		{
			add_token_start_word(_lexer, HRT_TOKEN_COLON, start);

		}

		else if(current == ';')
		{
			add_token_start_word(_lexer, HRT_TOKEN_SEMICOLON, start);

		}
		else if(current == '@')
		{
			add_token_start_word(_lexer, HRT_TOKEN_AT, start);

		}
		else if(current == '#')
		{
			add_token_start_word(_lexer, HRT_TOKEN_HASHTAG, start);

		}
		else if(current == '=')
		{
			if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_EQUALS_EQUALS, start);
			}
			else if(scroll == '>')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_EQUALS_LESS_THAN, start);
			}
			else
			{
				add_token_start_word(_lexer, HRT_TOKEN_EQUALS, start);
			}
		}
		else if(current == '<')
		{
			if(scroll == '=')
			{

				scroll = advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_LESS_THAN_EQUALS, start);

			}
			else if(scroll == '<')
			{
				scroll = advance(_lexer);
				if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, HRT_TOKEN_LESS_THAN_LESS_THAN_EQUALS, start);

				}
				else
					add_token_start_word(_lexer, HRT_TOKEN_LESS_THAN_LESS_THAN, start);
			}
			else
				add_token_start_word(_lexer, HRT_TOKEN_LESS_THAN, start);
		}
		else if(current == '>')
		{
			if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_GREATER_THAN_EQUALS, start);
			}
			else if(scroll == '>')
			{
				scroll = advance(_lexer);
				if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, HRT_TOKEN_GREATER_THAN_GREATER_THAN_EQUALS, start);

				}
				else
					add_token_start_word(_lexer, HRT_TOKEN_GREATER_THAN_GREATER_THAN, start);
			}
			else
				add_token_start_word(_lexer, HRT_TOKEN_GREATER_THAN, start);
		}
		else if(current == '&')
		{
			if(scroll == '&')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_AMPERSAND_AMPERSAND, start);
			}
			else if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_AMPERSAND_EQUALS, start);
			}
			else
			{
				add_token_start_word(_lexer, HRT_TOKEN_AMPERSAND, start);
			}
		}
		else if(current == '|')
		{
			if(scroll == '|')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_VERTICAL_LINE_VERTICAL_LINE, start);
			}
			else if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_VERTICAL_LINE_EQUALS, start);
			}
			else
			{
				add_token_start_word(_lexer, HRT_TOKEN_VERTICAL_LINE, start);
			}
		}
		else if(current == '%')
		{
			if(current == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_MODULO_EQUALS, start);

			}
			else
			{
				add_token_start_word(_lexer, HRT_TOKEN_MODULO, start);

			}
		}
		else if(current == '^')
		{
			if(current == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, HRT_TOKEN_HAT_EQUALS, start);
			}
			else
			{
				add_token_start_word(_lexer, HRT_TOKEN_HAT, start);

			}
		}
		else if(current == '?')
		{
			add_token_start_word(_lexer, HRT_TOKEN_QUESTION_MARK, start);
		}
		else
		{
			has_need_space = false;
			add_token_start_word(_lexer, HRT_TOKEN_INVALID, start);

		}
		//advance(lexer);

	}

}

void HRT_free_Lexer(HRT_Lexer_t *_lexer)
{
	free(_lexer->m_string);
	free(_lexer->m_token_buffer);
	free(_lexer);
}