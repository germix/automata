///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Automata
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
#include "atmt.h"
#include "tape.h"
#include <set.h>
#include <stack.h>
#include <graph.h>
#include <array.h>
#include <stdlib.h>
#include <assert.h>

#include <treefna.h>
#include <treenpda.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

int automata_type(struct automata* autom)
{
	if(!autom)
		return AUTOMATA_TYPE_INVALID;
	return autom->type;
}
char*automata_descr(struct automata* autom)
{
	return autom->descr;
}
void automata_free(struct automata* autom)
{
	if(autom)
	{
		if(autom->graph)						graph_delete(autom->graph);
		
		if(autom->stack)						free(autom->stack);
		
		if(autom->final_states)					free(autom->final_states);
		if(autom->multi_current_states)			free(autom->multi_current_states);
		
		if(autom->multi_current_root)
		{
			switch(autom->type)
			{
				case AUTOMATA_TYPE_FDA:
					treefna_delete(autom->multi_current_root);
					break;
				case AUTOMATA_TYPE_NPDA:
					treenpda_delete(autom->multi_current_root);
					break;
			}
		}
		if(autom->multi_current_nodes)			free(autom->multi_current_nodes);
		
		if(autom->states)
		{
			int i;
			for(i = 0; i < autom->states_count; i++)
			{
				free(autom->states[i].transitions);
				cstring_clear(&autom->states[i].name);
			}
			free(autom->states);
		}
		free(autom);
	}
}
void automata_reset(struct automata* autom, struct tape* tape)
{
	autom->halt_code = HALTCODE_NONE;
	autom->last_state = HALT_STATE;
	autom->current_state = autom->initial_state;

	autom->opers->reset_automata(autom, tape);
}
bool automata_runstep(struct automata* autom, struct tape* tape)
{
	return autom->opers->runstep_automata(autom, tape);
}
int  automata_stacksize(struct automata* autom)
{
//	return autom->stack_size;
	if(autom->stack)
		return autom->stack->top;
	return 0;
}
int  automata_stacksymbol(struct automata* autom, int idx)
{
	/*
	if(autom->stack && (idx >= 0 && autom->stack_size))
	{
		return autom->stack[idx];
	}
	*/
	if(autom->stack && (idx >= 0 && autom->stack->top))
	{
		return autom->stack->buf[idx];
	}
	return 0;
}
int  automata_haltcode(struct automata* autom)
{
	return autom->halt_code;
}
bool automata_is_nondeterministic(struct automata* autom)
{
	return (autom->type == AUTOMATA_TYPE_FNA || autom->type == AUTOMATA_TYPE_NPDA);
}

