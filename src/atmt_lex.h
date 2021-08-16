///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Analizador léxico para el lenguaje AUTOMATA
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___ATMT_LEX_H___
#define ___ATMT_LEX_H___
#include "lex.h"

#ifdef __cplusplus
extern "C" {
#endif

enum
{
	TOK_EOF = 128,
	TOK_CHAR,				// Atributo: tok_chr
	TOK_NUMBER,				// Atributo: tok_num
	TOK_STRING,				// Atributo: tok_string
	// ...
	TOK_NULL,				// \$
	TOK_HALT,				// \H
	TOK_ZETA,				// \Z
	TOK_EPSILON,			// \E
	// ...
	TOK_STATE,				// q#. Atributo: tok_num
	TOK_AUTOMATA,			// Palabra reservada: "AUTOMATA"
	// ...
	TOK_AUTOMATA_TMA,
	TOK_AUTOMATA_PDA,
	TOK_AUTOMATA_FDA,
	TOK_AUTOMATA_FNA,
	TOK_AUTOMATA_NPDA,
	// ...
	TOK_START_STATE,
	TOK_FINAL_STATES,
};

void read_token_for_automata(struct lexer* lex, struct buffer* buf, struct logger* log);

#ifdef __cplusplus
};
#endif

#endif
