///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Analizador léxico para el lenguaje AUTOMATA
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "buf.h"
#include "log.h"
#include "error.h"
#include "atmt_lex.h"

#include <stdlib.h>
#include <string.h>

static int is_eof(int c)		{ return (c == EOF); }
static int is_eol(int c)		{ return (c == '\n'); }
static int is_space(int c)		{ return (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r'); }
static int is_digit(int c)		{ return (c >= '0' && c <= '9'); }
static int is_alpha(int c)		{ return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')); }
static int is_identifier(int c)	{ return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')); }

static int read_int(struct buffer* buf, int ch)
{
	char* p;
	static char sz[256];
	
	p = sz;
	*p = ch;
	do
	{
		ch = buf->peekch(buf);
		if(!is_digit(ch))
			*(++p) = '\0';
		else
			*(++p) = buf->getch(buf);
	}
	while(*p != '\0');

	return atoi(sz);
}
static void read_ident(struct buffer* buf, char ch, char* sz)
{
	char* p;
	
	p = sz;
	*p = ch;
	do
	{
		ch = buf->peekch(buf);
		if(!is_identifier(ch))
			*(++p) = '\0';
		else
			*(++p) = buf->getch(buf);
	}
	while(*p != '\0');
}

void read_token_for_automata(struct lexer* lex, struct buffer* buf, struct logger* log)
{
	int ch;
	char* ptr;
	
	while(tok == 0)
	{
		ch = buf->getch(buf);
		// ...
		if(is_eof(ch))
		{
			tok = TOK_EOF;
		}
		else if(is_eol(ch))
		{
			tok_lineno++;
		}
		else if(is_space(ch))
		{
		}
		else if(is_identifier(ch))
		{
			if(ch == 'q' && is_digit(buf->peekch(buf)))
			{
				ch = buf->getch(buf);
				tok = TOK_STATE;
				tok_num = read_int(buf, ch);
			}
			else
			{
				read_ident(buf, ch, tok_string);
				if(0 == strcmp(tok_string, "AUTOMATA"))			tok = TOK_AUTOMATA;
				else if(0 == strcmp(tok_string, "PDA"))			tok = TOK_AUTOMATA_PDA;
				else if(0 == strcmp(tok_string, "FDA"))			tok = TOK_AUTOMATA_FDA;
				else if(0 == strcmp(tok_string, "FNA"))			tok = TOK_AUTOMATA_FNA;
				else if(0 == strcmp(tok_string, "TMA"))			tok = TOK_AUTOMATA_TMA;
				else if(0 == strcmp(tok_string, "NPDA"))		tok = TOK_AUTOMATA_NPDA;
				else if(0 == strcmp(tok_string, "StartState"))	tok = TOK_START_STATE;
				else if(0 == strcmp(tok_string, "FinalStates"))	tok = TOK_FINAL_STATES;
			}
		}
		//
		// Leer caracter especial
		//
		else if(ch == '\\')
		{
			switch(ch = buf->getch(buf))
			{
				case '$':	tok = TOK_NULL;			break;
				case 'Z':	tok = TOK_ZETA;			break;
				case 'H':	tok = TOK_HALT;			break;
				case 'E':	tok = TOK_EPSILON;		break;
				default:
					log->error(ERROR_TEXT_UNRECOGNIZED_SPECIAL_CHAR(ch));
					break;
			}
		}
		//
		// Leer un "literal char"
		//
		else if(ch == '\'')
		{
			tok = TOK_CHAR;
			
			ch = buf->getch(buf);
			tok_chr = ch;
			ch = buf->getch(buf);
			if(ch != '\'')
			{
				log->error(ERROR_TEXT_EXPECTED_CHAR('\''));
			}
		}
		//
		// Leer un "literal string"
		//
		else if(ch == '\"')
		{
			tok = TOK_STRING;
			ptr = tok_string;
			do
			{
				ch = buf->getch(buf);
				*ptr = (ch != '\"' && ch != EOF) ? ch : '\0';
			}
			while(*ptr++ != 0);
		}
		//
		// Leer comentario (estilo C)
		//
		else if(ch == '/')
		{
			if((ch = buf->getch(buf)) == '/')
			{
				do
				{
					ch = buf->getch(buf);
				}
				while(ch != '\n');
			}
			else
			{
				log->error(ERROR_TEXT_EXPECTED_CHAR('/'));
			}
		}
		else
		{
			switch(ch)
			{
				case '[':
				case ']':
				case '{':
				case '}':
				case '(':
				case ')':
				case '<':
				case '>':
				case '-':
				case ',':
				case ';':
				case '=':
				case '&':
					tok = ch;
					break;
			}
		}
	}
}

