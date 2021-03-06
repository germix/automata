///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Parser de una expresión regular para contruir un automata con el Método Thompson
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#pragma warning(error:4013)
#pragma warning(error:4020)
#pragma warning(error:4133)
#pragma warning(disable:4996)
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "thompson.h"
#include "lex.h"
#include "buf.h"
#include "log.h"
#include "buf_string.h"


//
// Gramática para reconocer expresiones regulares:
//
// E -> U
// U -> U|C │ C
// C -> C.S │ S
// S -> F* │ F
// F -> (E) │ symbol
//
//
// Eliminando la recursividad por la izquierda, entonces...
//
// E  -> U
// U  -> CU'
// U' -> |CU' │ epsilon
// C  -> SC'
// C' -> .SC' │ epsilon
// S  -> FS'
// S' -> *S' │ epsilon
// F  -> (E) │ symbol
//

enum
{
	TOK_EOF = 128,
	TOK_CHAR,				// Atributo: tok_chr
	TOK_NUMBER,				// Atributo: tok_num
	TOK_STRING,				// Atributo: tok_string
	TOK_IDENTIFIER,			// Atributo: tok_string
};

struct thompson_graph proc_E(struct automata* autom);
struct thompson_graph proc_U(struct automata* autom);
struct thompson_graph proc_U_prima(struct automata* autom, struct thompson_graph graph);
struct thompson_graph proc_C(struct automata* autom);
struct thompson_graph proc_C_prima(struct automata* autom, struct thompson_graph graph);
struct thompson_graph proc_S(struct automata* autom);
struct thompson_graph proc_S_prima(struct automata* autom, struct thompson_graph graph);
struct thompson_graph proc_F(struct automata* autom);

struct thompson_graph proc_E(struct automata* autom)
{
	return proc_U(autom);
}
struct thompson_graph proc_U(struct automata* autom)
{
	return proc_U_prima(autom, proc_C(autom));
}
struct thompson_graph proc_U_prima(struct automata* autom, struct thompson_graph graph)
{
	switch(tok)
	{
		case '|':
			match('|');
			graph = thompson_graph_union(autom, graph, proc_U_prima(autom, proc_C(autom)));
			break;
	}
	return graph;
}
struct thompson_graph proc_C(struct automata* autom)
{
	return proc_C_prima(autom, proc_S(autom));
}
struct thompson_graph proc_C_prima(struct automata* autom, struct thompson_graph graph)
{
	switch(tok)
	{
		case '.':
			match('.');
			graph = thompson_graph_concat(autom, graph, proc_C_prima(autom, proc_S(autom)));
			break;
	}
	return graph;
}
struct thompson_graph proc_S(struct automata* autom)
{
	return proc_S_prima(autom, proc_F(autom));
}
struct thompson_graph proc_S_prima(struct automata* autom, struct thompson_graph graph)
{
	switch(tok)
	{
		case '*':
			match('*');
		//	graph = thompson_graph_star(autom, graph);
			graph = thompson_graph_star(autom, proc_S_prima(autom, graph));
			break;
	}
	return graph;
}
struct thompson_graph proc_F(struct automata* autom)
{
	int chr;
	struct thompson_graph graph;
	
	switch(tok)
	{
		case '(':
			match('(');
			graph = proc_E(autom);
			match(')');
			break;
		case TOK_CHAR:
			chr = tok_chr;
			match(TOK_CHAR);
			graph = thompson_graph_symb(autom, chr);
			break;
		default:
		case TOK_EOF:
			assert(0);
			graph.s = -1;
			graph.t = -1;
			break;
	}
	return graph;
}

static int is_eof(int c)		{ return (c == EOF); }
static int is_eol(int c)		{ return (c == '\n'); }
static int is_space(int c)		{ return (c == ' ' || c == '\t' || c == '\v' || c == '\f' || c == '\r'); }
static int is_digit(int c)		{ return (c >= '0' && c <= '9'); }
static int is_alpha(int c)		{ return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')); }
static int is_identifier(int c)	{ return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_')); }

static void read_token_for_regular_expr(struct lexer* lex, struct buffer* buf, struct logger* log)
{
	int ch;
	
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
		else if(is_digit(ch) || is_identifier(ch))
		{
			tok = TOK_CHAR;
			tok_chr = ch;
		}
		else
		{
			switch(ch)
			{
				case '(':
				case ')':
				case '+':		// Cierre positivo
				case '*':		// Cierre estrella
				case '|':		// Unión
				case '.':		// Concatenación
					tok = ch;
					break;
			}
		}
	}
}

//
// Agregar operador de concatenación
//
void expand_concat(const char* string, char* result)
{
	int i;
	int r = 0;
	int len = strlen(string);
	int chleft;
	int chright;
	
	for(i = 0; i < len-1; i++)
	{
		chleft = string[i];
		chright = string[i+1];
		
		result[r++] = chleft;
		if((isalnum(chleft)) || (chleft == ')') || (chleft == '*'))
		{
			if((isalnum(chright)) || (chright == '('))
				result[r++] = '.';
		}
	}
	result[r++] = string[len-1];
	result[r++] = 0;
}

//
// Algoritmo Thompson para convertir una expresión regular a un automata
//
struct automata* thompson(const char* string)
{
	struct lexer lex =
	{
		read_token_for_regular_expr,
	};
	struct buffer* buf;
	struct automata* autom;
	char expanded[256];

	//
	// Crear AFN
	//
	autom = new_automata(AUTOMATA_TYPE_FNA);
	sprintf(autom->descr, "ER->AFN ( %s )", string);
	
	//
	// Agregar operadores de concatenación
	//
	expand_concat(string, expanded);
	
	//
	// Leer expresión regular y crear estados y transiciones con el método de thompson
	//
	autom->graph = graph_new();
	if(NULL != (buf = create_buffer_string_from_string(expanded)))
	{
		lex_init(&lex, buf, NULL);
		{
			struct thompson_graph G;

			next();
			G = proc_E(autom);

			autom->initial_state = G.s;
			set_insert(autom->final_states, G.t);
			G.ns->initial = true;
			G.nt->accept = true;
			autom->graph->width = G.width;
			autom->graph->height = G.height;
		}
		lex_close();
	}
	return autom;
}

