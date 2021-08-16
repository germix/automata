///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Gramática
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___GRAMMAR_H___
#define ___GRAMMAR_H___
#include "cstring.h"

struct grammar
{
	struct cstring*		rules;
	int					rules_count;
};

#ifdef __cplusplus
extern "C" {
#endif

void free_grammar(struct grammar* gr);
struct grammar* automata_to_grammar(struct automata* autom);

#ifdef __cplusplus
};
#endif

#endif
