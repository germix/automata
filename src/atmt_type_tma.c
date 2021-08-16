///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Maquina de Turing
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <atmt.h>
#include <tape.h>
#include <set.h>

static void ___tma_reset_automata(struct automata* autom, struct tape* tape)
{
}
static bool ___tma_runstep_automata(struct automata* autom, struct tape* tape)
{

	int i;
	bool found = false;
	struct automata_transition_tma* transition;
	
	for(i = 0; i < autom->states[autom->current_state].transitions_count && !found; i++)
	{
		transition = get_automata_transition(autom, autom->current_state, i, automata_transition_tma);
		// ...
		if(transition->in == tape_read(tape))
		{
			found = true;
			autom->last_state = autom->current_state;
			autom->last_transition.tma = *transition;
		}
	}
	if(!found)
	{
		autom->halt_code = HALTCODE_ERROR;
	}
	else
	{
		//
		// Escribir
		//
		tape_write(tape, transition->out);
		
		//
		// Mover
		//
		switch(transition->direction)
		{
			case '<':	tape_move(tape, -1);	break;
			case '>':	tape_move(tape, +1);	break;
		}
		//
		// Cambiar de estado
		//
		if(transition->state == HALT_STATE)
		{
			autom->halt_code = HALTCODE_SUCCESS;
		}
		else
		{
			autom->current_state = transition->state;
		}
	}
	return (autom->halt_code == HALTCODE_NONE);
}
struct automata_operations automata_operations_for_tma = 
{
	___tma_reset_automata,
	___tma_runstep_automata,
};

