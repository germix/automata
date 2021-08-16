///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Automata Finito Deterministico
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <atmt.h>
#include <tape.h>
#include <set.h>
#include <assert.h>

static void ___fda_reset_automata(struct automata* autom, struct tape* tape)
{
}
static bool ___fda_runstep_automata(struct automata* autom, struct tape* tape)
{
#if 0//!!!
	int i;
	bool found = false;
	struct automata_transition* transition;
	
	for(i = 0; i < autom->states[autom->current_state].transitions_count && !found; i++)
	{
		transition = get_automata_transition(autom, autom->current_state, i, automata_transition);
		if(transition->in == tape_read(tape))
		{
			found = true;
			autom->last_state = autom->current_state;
			autom->last_transition.fda = *transition;
		}
	}
	if(!found)
	{
		autom->halt_code = HALTCODE_ERROR;
	}
	else
	{
		//
		// Cambiar de estado
		//
		if(transition->state == HALT_STATE)
		{
			autom->halt_code = HALTCODE_SUCCESS;
		}
		else
		{
			tape_move(tape, +1);
			autom->current_state = transition->state;
		}
	}
#else
	int ch;
	
	ch = tape_read(tape);
	if(ch == CH_NULL)
	{
		if(!set_contains(autom->final_states, autom->current_state))
			autom->halt_code = HALTCODE_ERROR;
		else
			autom->halt_code = HALTCODE_SUCCESS;
	}
	else
	{
		int i;
		bool found = false;
		struct automata_transition* transition;
		
		for(i = 0; i < autom->states[autom->current_state].transitions_count && !found; i++)
		{
			transition = get_automata_transition(autom, autom->current_state, i, automata_transition);
			if(transition->in == tape_read(tape))
			{
				found = true;
				autom->last_state = autom->current_state;
				autom->last_transition.fda = *transition;
			}
		}
		if(!found)
		{
			autom->halt_code = HALTCODE_ERROR;
		}
		else
		{
			assert(transition->state != HALT_STATE);
			
			//
			// Cambiar de estado
			//
			tape_move(tape, +1);
			autom->current_state = transition->state;
		}
	}
#endif
	return (autom->halt_code == HALTCODE_NONE);
}
struct automata_operations automata_operations_for_fda = 
{
	___fda_reset_automata,
	___fda_runstep_automata,
};

