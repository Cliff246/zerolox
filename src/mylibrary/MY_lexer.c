#include "MY_lexer.h"
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>

#define BASE_TOKENS_ALLOCATE 10

char *MY_token_type_names[] =
{
	MY_TOKEN_TYPE_LIST(MY_TOKEN_TYPE_STRING)
};


static void allocate_tokens(MY_lexer_t *lexer)
{
	if(lexer->token_allocated == 0)
	{
		lexer->token_allocated = BASE_TOKENS_ALLOCATE;
		lexer->token_count = 0;

		lexer->ap__token_buffer = malloc(sizeof(MY_token_t) * lexer->token_allocated);
		if(!lexer->ap__token_buffer)
			exit(1);

	}
	else
	{
		if(lexer->token_allocated > (INT_MAX - 1) / 2)
		{
			exit(1);
		}
		lexer->token_allocated = (lexer->token_allocated * 2) + 1;
		void *temp = realloc(lexer->ap__token_buffer, sizeof(MY_token_t) *  lexer->token_allocated);
		if(temp == NULL)
			exit(1);
		lexer->ap__token_buffer = temp;
	}
}


static void add_token(MY_lexer_t *lexer, MY_token_t token)
{
	if(lexer->token_count >= lexer->token_allocated)
	{
		allocate_tokens(lexer);
	}
	lexer->ap__token_buffer[lexer->token_count++] = token;
}

static void print_token(MY_lexer_t *lexer, int position)
{
	if(position < lexer->token_count && position >= 0)
	{
		MY_token_t token = lexer->ap__token_buffer[position];
		if(token.start < 0)
		{
			return;
		}
		if(token.length + token.start > lexer->string_length)
		{
			return;
		}
		printf("token %s{%d:%d} [%d,%d]: <",MY_token_type_names[token.type], token.row, token.column,  token.start, token.length);
		for(int i = token.start; i < token.length + token.start; ++i)
		{
			if(lexer->ap__string[i] == '\n')
			{
				putchar('\\');
				putchar('n');
			}
			else
			{
				putchar(lexer->ap__string[i]);
			}
		}
		printf(">\n");
	}

}

static MY_token_t init_token(MY_token_type_t type, int start, int stop, int _column, int _row)
{
	return (MY_token_t){.type = type, .start = start, .length = stop - start, .column = _column, .row = _row};

}



static char peek(MY_lexer_t *lexer)
{

	if(lexer->position >= lexer->string_length)
		return 0;
	else
	{
		char ch = lexer->ap__string[lexer->position];
		if(lexer->print_each == true)
		{
			printf("%c", ch);
		}
		return ch;

	}
}

static char advance(MY_lexer_t *lexer)
{
	char consumed = peek(lexer);
	if(lexer->position >= lexer->string_length)
	{
		return 0;
	}
	lexer->position++;
	char pk = peek(lexer);
	if(consumed == '\n')
	{
		lexer->row++;
		lexer->column = 0;
	}
	else if(consumed == '\t')
	{
		lexer->column += 4;
	}
	else
	{
		lexer->column++;
	}
	return pk;
}

static int position(MY_lexer_t *lexer)
{
	return lexer->position;
}


static void add_token_start_word(MY_lexer_t *lexer, MY_token_type_t type, int start)
{
	int stop = position(lexer);
	MY_token_t range = init_token(type, start, stop, lexer->tcolumn, lexer->trow);

	add_token(lexer, range);
}


static void has_character_ctype(MY_lexer_t *lexer, int (*ctype)(int c))
{
	char scroll = peek(lexer);
	while(ctype((unsigned char)scroll))
	{
		scroll = advance(lexer);
	}
}

static void has_character_string(MY_lexer_t *lexer, char *string)
{
	char scroll = peek(lexer);
	while(strchr(string, scroll) && scroll != 0)
	{
		scroll = advance(lexer);
	}
}

static void has_character_ctype_or_string(MY_lexer_t *lexer, int (*ctype)(int c), char *string)
{
	char scroll = peek(lexer);
	while(ctype((unsigned char)scroll) || (strchr(string, scroll) && scroll != 0))
	{
		scroll = advance(lexer);
	}
}


int MY_get_token_string(MY_lexer_t *_lexer, int _position, char *_buffer, int _blength)
{
	if(_position < _lexer->token_count && _position >= 0)
	{
		MY_token_t token = _lexer->ap__token_buffer[_position];
		if(token.start < 0)
		{
			return -1;
		}
		if(token.length + token.start > _lexer->string_length || token.length >= _blength)
		{
			return -1;
		}
		int c = 0;
		for(int i = token.start; i < token.length + token.start && c < _blength; ++i, ++c)
		{
			_buffer[c] = _lexer->ap__string[i];
		}
		_buffer[c] = '\0';
		return c;
	}
	return -1;
}

void MY_print_lexer(MY_lexer_t *lexer)
{

	for(int i = 0; i < lexer->token_count; ++i)
	{
		print_token(lexer, i);
	}
}


int MY_get_substring(MY_lexer_t *_lexer, int _start, int _length, char *_buffer, int _blength)
{
	if(_start < 0 || _length < 0 || _blength <= _length ||
		_start > _lexer->string_length ||
		_length > _lexer->string_length - _start)
	{
		return -1;
	}
	memcpy(_buffer, _lexer->ap__string + _start, (size_t)_length);
	_buffer[_length] = '\0';
	int c = _length;
	return c;
}


MY_lexer_t *MY_initialize_lexer(char *_string)
{
	if(!_string)
		exit(1);
	MY_lexer_t *lexer = (MY_lexer_t *)malloc(sizeof(MY_lexer_t));
	if(!lexer)
		exit(1);
	lexer->token_allocated = 0;
	allocate_tokens(lexer);

	size_t length = strlen(_string);
	if(length > INT_MAX)
	{
		exit(1);
	}
	char *copy = malloc(length + 1);
	if(!copy)
	{
		free(lexer->ap__token_buffer);
		free(lexer);
		exit(1);
	}
	memcpy(copy, _string, length + 1);
	lexer->string_length = strlen(copy);
	lexer->ap__string = copy;
	lexer->position = 0;
	lexer->row = 1;
	lexer->column = 0;
	lexer->print_each = false;
	return lexer;
}

void MY_lex(MY_lexer_t *_lexer)
{


	while(_lexer->position < _lexer->string_length)
	{
		_lexer->tcolumn = _lexer->column;
		_lexer->trow = _lexer->row;
		int start = position(_lexer);
		char current = peek(_lexer);
		char scroll = advance(_lexer);

		int stop;
		if(current == ' ' || current == '\t')
		{
		}
		else if(current == '\n' || current == '\r')
		{

		}

		else if(isalpha((unsigned char)current) || current == '_')
		{
			has_character_ctype_or_string(_lexer, isalnum, "_");

			add_token_start_word(_lexer, MY_TOKEN_WORD, start);
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
				add_token_start_word(_lexer, MY_TOKEN_INVALID, start);
				continue;
			}
			stop = position(_lexer);
			MY_token_t range = init_token(MY_TOKEN_STRING, start + 1, stop - 1, _lexer->tcolumn, _lexer->trow);
			add_token(_lexer, range);

		}
		else if(current == '!')
		{
			if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_EXCLAIM_EQUALS, start);

			}
			else
				add_token_start_word(_lexer, MY_TOKEN_EXCLAIM, start);

		}
		else if(isdigit((unsigned char)current))
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
					add_token_start_word(_lexer, MY_TOKEN_INVALID, start);

				}
				else
				{
					has_character_string(_lexer, (char *)hex_chars);

					add_token_start_word(_lexer, MY_TOKEN_HEXDECIMAL, start);
				}
			}
			else if(has_hexcode && !has_zero_start)
			{
				add_token_start_word(_lexer, MY_TOKEN_INVALID, start);
			}
			else if(scroll == '.')
			{
				//floating point
				advance(_lexer);
				has_character_ctype(_lexer, isdigit);
				add_token_start_word(_lexer, MY_TOKEN_FLOAT, start);
			}
			else
			{
				//integer
				bool has_found_dot = false;
				while(isdigit((unsigned char)scroll) && scroll != '\0' && scroll != '\n')
				{
				 	scroll = advance(_lexer);
					if(scroll == '.')
						has_found_dot = true;
				}
				if(has_found_dot == true)
				{
					advance(_lexer);
					has_character_ctype(_lexer, isdigit);
					add_token_start_word(_lexer, MY_TOKEN_FLOAT, start);

				}
				else
				{
					add_token_start_word(_lexer, MY_TOKEN_INT, start);
				}
			}


		}
		else if(current == '.')
		{
			add_token_start_word(_lexer, MY_TOKEN_PERIOD, start);
		}
		else if(current == '(')
		{
			add_token_start_word(_lexer, MY_TOKEN_PARENTHESIS_OPEN, start);
		}
		else if(current == ')')
		{
			add_token_start_word(_lexer, MY_TOKEN_PARENTHESIS_CLOSED, start);
		}
		else if(current == '[')
		{
			add_token_start_word(_lexer, MY_TOKEN_SQUAREBRACE_OPEN, start);
		}
		else if(current == ']')
		{
			add_token_start_word(_lexer, MY_TOKEN_SQUAREBRACE_CLOSED, start);

		}
		else if(current == '{')
		{
			add_token_start_word(_lexer, MY_TOKEN_CURLYBRACE_OPEN, start);

		}
		else if(current == '}')
		{
			add_token_start_word(_lexer, MY_TOKEN_CURLYBRACE_CLOSED, start);

		}
		else if(current == '+')
		{
			if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_ADD_EQUALS, start);

			}
			else if(scroll == '+')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_ADD_ADD, start);

			}
			else
			{
				add_token_start_word(_lexer, MY_TOKEN_ADD, start);
			}
		}
		else if(current == '-')
		{
			if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_MINUS_EQUALS, start);

			}
			else if(scroll == '-')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_MINUS_MINUS, start);

			}
			else
			{
				add_token_start_word(_lexer, MY_TOKEN_MINUS, start);
			}
		}
		else if(current == '/')
		{
			if(scroll == '/')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_SLASH_SLASH, start);

			}
			else if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_SLASH_EQUALS, start);

			}
			else if(scroll == ':')
			{
				//printf("found this\n");
				//_lexer->print_each = true;
				advance(_lexer);
				while(scroll != '\0' && scroll != '\n' )
				{
					scroll = advance(_lexer);
				}
				//_lexer->print_each = false;

			}
			else
			{
				add_token_start_word(_lexer, MY_TOKEN_SLASH, start);

			}
		}
		else if(current == '*')
		{
			if(scroll == '*')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_STAR_STAR, start);

			}
			else if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_STAR_EQUALS, start);

			}
			else
			{
				add_token_start_word(_lexer, MY_TOKEN_STAR, start);
			}
		}
		else if(current == ',')
		{
			add_token_start_word(_lexer, MY_TOKEN_COMMA, start);

		}
		else if(current == ':')
		{
			add_token_start_word(_lexer, MY_TOKEN_COLON, start);

		}

		else if(current == ';')
		{
			add_token_start_word(_lexer, MY_TOKEN_SEMICOLON, start);

		}
		else if(current == '@')
		{
			add_token_start_word(_lexer, MY_TOKEN_AT, start);

		}
		else if(current == '#')
		{
			add_token_start_word(_lexer, MY_TOKEN_HASHTAG, start);

		}
		else if(current == '=')
		{
			if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_EQUALS_EQUALS, start);
			}
			else if(scroll == '>')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_EQUALS_LESS_THAN, start);
			}
			else
			{
				add_token_start_word(_lexer, MY_TOKEN_EQUALS, start);
			}
		}
		else if(current == '<')
		{
			if(scroll == '=')
			{

				scroll = advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_LESS_THAN_EQUALS, start);

			}
			else if(scroll == '<')
			{
				scroll = advance(_lexer);
				if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_TOKEN_LESS_THAN_LESS_THAN_EQUALS, start);

				}
				else
					add_token_start_word(_lexer, MY_TOKEN_LESS_THAN_LESS_THAN, start);
			}
			else
				add_token_start_word(_lexer, MY_TOKEN_LESS_THAN, start);
		}
		else if(current == '>')
		{
			if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_GREATER_THAN_EQUALS, start);
			}
			else if(scroll == '>')
			{
				scroll = advance(_lexer);
				if(scroll == '=')
				{
					advance(_lexer);
					add_token_start_word(_lexer, MY_TOKEN_GREATER_THAN_GREATER_THAN_EQUALS, start);

				}
				else
					add_token_start_word(_lexer, MY_TOKEN_GREATER_THAN_GREATER_THAN, start);
			}
			else
				add_token_start_word(_lexer, MY_TOKEN_GREATER_THAN, start);
		}
		else if(current == '&')
		{
			if(scroll == '&')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_AMPERSAND_AMPERSAND, start);
			}
			else if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_AMPERSAND_EQUALS, start);
			}
			else
			{
				add_token_start_word(_lexer, MY_TOKEN_AMPERSAND, start);
			}
		}
		else if(current == '|')
		{
			if(scroll == '|')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_VERTICAL_LINE_VERTICAL_LINE, start);
			}
			else if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_VERTICAL_LINE_EQUALS, start);
			}
			else
			{
				add_token_start_word(_lexer, MY_TOKEN_VERTICAL_LINE, start);
			}
		}
		else if(current == '%')
		{
			if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_MODULO_EQUALS, start);

			}
			else
			{
				add_token_start_word(_lexer, MY_TOKEN_MODULO, start);

			}
		}
		else if(current == '^')
		{
			if(scroll == '=')
			{
				advance(_lexer);
				add_token_start_word(_lexer, MY_TOKEN_HAT_EQUALS, start);
			}
			else
			{
				add_token_start_word(_lexer, MY_TOKEN_HAT, start);

			}
		}
		else if(current == '?')
		{
			add_token_start_word(_lexer, MY_TOKEN_QUESTION_MARK, start);
		}
		else
		{
			add_token_start_word(_lexer, MY_TOKEN_INVALID, start);

		}
		//advance(lexer);

	}

}

void MY_free_lexer(MY_lexer_t *_lexer)
{
	if(!_lexer)
	{
		return;
	}
	free(_lexer->ap__string);
	free(_lexer->ap__token_buffer);
	free(_lexer);
}
