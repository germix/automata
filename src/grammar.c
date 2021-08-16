///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Gramática
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chrs.h"
#include "atmt.h"
#include "set.h"
#include "array.h"
#include "grammar.h"

void free_grammar(struct grammar* gr)
{
	int i;
	
	for(i = 0; i < gr->rules_count; i++)
	{
		cstring_clear(&gr->rules[i]);
	}
	free(gr->rules);
	free(gr);
}
struct grammar* automata_to_grammar(struct automata* autom)
{
	int i;
	int j;
	struct automata_state* s;
	struct automata_transition* t;
	struct cstring* buf;
	struct grammar* grammar;
	struct set finals;
//	int chr_sep = '|';
	int chr_sep = CHR_VERT;
	
	grammar = (struct grammar*)calloc(1, sizeof(struct grammar));
	
	for(i = 0; i < autom->states_count; i++)
	{
		s = &autom->states[i];
		if(s->transitions_count)
		{
			set_init(&finals);
			
			buf = (struct cstring*)array_add(&grammar->rules, &grammar->rules_count, sizeof(struct cstring));
			cstring_addsprintf(buf, "%s %c", s->name.buffer, CHR_ARROW_RIGHT);
			for(j = 0; j < s->transitions_count; j++)
			{
				t = &s->transitions[j];
				if(t->state != HALT_STATE)
				{
					if(j == 0)
						cstring_addsprintf(buf, " %c%s", t->in, (autom->states[t->state].name.buffer));
					else
						cstring_addsprintf(buf, " %c %c%s", chr_sep, t->in, (autom->states[t->state].name.buffer));
					
					if(set_contains(autom->final_states, t->state))
					{
						set_insert(&finals, t->in);
					}
				}
			}
			for(j = 0; j < finals.size; j++)
			{
				cstring_addsprintf(buf, " %c %c", chr_sep, finals.elems[j]);
			}
		}
	}
	return grammar;
}


