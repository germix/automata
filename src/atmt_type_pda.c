///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Automata a Pila Deterministico
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <atmt.h>
#include <tape.h>
#include <set.h>
#include <stack.h>

static void ___pda_reset_automata(struct automata* autom, struct tape* tape)
{
	if(autom->stack)
	{
		stack_clear(autom->stack);
		stack_push(autom->stack, CH_ZETA);
	}
}
static bool ___pda_runstep_automata(struct automata* autom, struct tape* tape)
{
	int i;
	bool found = false;
	struct automata_transition_pda* transition;

#if 0//!!!
	if(tape_read(tape) == CH_NULL)				// Si se llegó el fin de la cadena
	{
		if(stack_size(autom->stack) && stack_peek(autom->stack) == CH_ZETA)
		{
			// Aceptar si está "vacia", o sea sólo tiene \Z
			autom->halt_code = HALTCODE_SUCCESS;
		}
		else
			autom->halt_code = HALTCODE_ERROR;
	}
	else
#endif
	{
		//
		// Primero buscar una transición con entrada que no sea épsilon
		//
		for(i = 0; i < autom->states[autom->current_state].transitions_count && !found; i++)
		{
			transition = get_automata_transition(autom, autom->current_state, i, automata_transition_pda);
			// ...
			if(transition->in != CH_EPSILON
				&& transition->in == tape_read(tape)
				&& (transition->pop == CH_EPSILON || transition->pop == stack_peek(autom->stack)))
			{
				found = true;
				autom->last_state = autom->current_state;
				autom->last_transition.pda = *transition;
			}
		}
		//
		// Si no se encontró una transición, buscar con entrada épsilon
		//
		if(!found)
		{
			for(i = 0; i < autom->states[autom->current_state].transitions_count && !found; i++)
			{
				transition = get_automata_transition(autom, autom->current_state, i, automata_transition_pda);
				// ...
				if(transition->in == CH_EPSILON
					&& (transition->pop == CH_EPSILON || transition->pop == stack_peek(autom->stack)))
				{
					found = true;
					autom->last_state = autom->current_state;
					autom->last_transition.pda = *transition;
				}
			}
		}
		if(!found)
		{
			autom->halt_code = HALTCODE_ERROR;
		}
		else
		{
			int i;
			int chb;		// Caracter de entrada
			int chz;		// Caracter de la pila
			
			//
			// Leer entrada
			//
			if(transition->in != CH_EPSILON)
			{
				chb = tape_read(tape);
				tape_move(tape, +1);
			}
			//
			// Quitar elemento de la pila
			//
			if(transition->pop != CH_EPSILON)
			{
				chz = stack_pop(autom->stack);
			}
			//
			// Insertar elementos en la pila
			//
			if(transition->push[0] != CH_EPSILON)
			{
				for(i = 0; i < MAX_PUSHABLE_CHARS && transition->push[i] != CH_NULL; i++)
				{
					stack_push(autom->stack, transition->push[i]);
				}
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
	}
	return (autom->halt_code == HALTCODE_NONE);
}
struct automata_operations automata_operations_for_pda = 
{
	___pda_reset_automata,
	___pda_runstep_automata,
};

