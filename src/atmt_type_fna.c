///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Automata Finito No Deterministico
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <atmt.h>
#include <tape.h>
#include <set.h>
#include <stack.h>
#include <treefna.h>

//
// Ver libro: Compiladores principios, técnicas y herramientas
//
static void fna_move_and_closure(struct automata* autom, struct automata_state* states, struct set* currents, int symb, struct set* result)
{
	int i;
	struct stack stack;
	struct set multi_current_nodes_temp;
	struct stack stack_nodes;
	
	//
	// Si se calcula de cerradura con respecto a epsilon,
	// hacer una copia de los estados, sino, crear un nuevo conjunto
	//
	if(symb != CH_EPSILON)
	{
		set_init(result);
		set_init(&multi_current_nodes_temp);
	}
	else
	{
		set_copy(currents, result);
		set_copy(autom->multi_current_nodes, &multi_current_nodes_temp);
	}
	//
	// Poner los estados corrientes en la pila
	//
	stack_init(&stack);
	for(i = 0; i < currents->size; i++)
	{
		stack_push(&stack, currents->elems[i]);
	}
	//
	// Poner los nodos de los estados corrientes en la pila
	//
	stack_init(&stack_nodes);
	for(i = 0; i < autom->multi_current_nodes->size; i++)
	{
		stack_push(&stack_nodes, autom->multi_current_nodes->elems[i]);
	}
	
	//
	// ...
	//
	while(!stack_empty(&stack))
	{
		int q = stack_pop(&stack);
		struct treefna* node = (struct treefna*)stack_pop(&stack_nodes);
		for(i = 0; i < states[q].transitions_count; i++)
		{
			struct automata_transition* t = &states[q].transitions[i];
			int c = t->in;
			int r = t->state;
			if(c == symb && !set_contains(result, r))
			{
				struct treefna* new_node = treefna_new(node, r, symb);

				set_insert(result, r);
				set_insert(&multi_current_nodes_temp, (int)new_node);
				if(symb == CH_EPSILON)
				{
					stack_push(&stack, r);
					stack_push(&stack_nodes, (int)new_node);
				}
			}
		}
	}
	//
	// ...
	//
	{
		if(symb != CH_EPSILON)
		{
			for(i = 0; i < autom->multi_current_nodes->size; i++)
			{
				struct treefna* node = (struct treefna*)autom->multi_current_nodes->elems[i];
				if(!node->first)	// Si no tiene hijos, entonces no hay transiciones, o sea muere
				{
					node->dead = true;
				}
			}
		}
		set_copy(&multi_current_nodes_temp, autom->multi_current_nodes);
	}
}
static void fna_move(struct automata* autom, struct set* currents, int symb, struct set* result)
{
	fna_move_and_closure(autom, autom->states, currents, symb, result);
}
static void fna_epsilon_closure(struct automata* autom, struct set* currents, struct set* result)
{
	fna_move_and_closure(autom, autom->states, currents, CH_EPSILON, result);
}
static void ___fna_reset_automata(struct automata* autom, struct tape* tape)
{
	if(autom->multi_current_root)
	{
		treefna_delete(autom->multi_current_root);
	}
#if 1//!!
	autom->multi_current_root = treefna_new(NULL, autom->initial_state, 0);
#else
	{
		struct treefna*r = treefna_new(0, -1);
		autom->multi_current_root = treefna_new(r, autom->initial_state);
	}
#endif
	set_clear(autom->multi_current_nodes);
	set_insert(autom->multi_current_nodes, (int)autom->multi_current_root);
	//
	// ...
	//
	if(autom->final_states)
	{
		struct set start;

		set_init(&start);
		set_insert(&start, autom->initial_state);	// q0 (estado inicial)
		
		set_clear(autom->multi_current_states);
		fna_epsilon_closure(autom, &start, autom->multi_current_states);
	}
}
static bool ___fna_runstep_automata(struct automata* autom, struct tape* tape)
{
	int ch;
	
	ch = tape_read(tape);
	if(ch == CH_NULL)
	{
		autom->halt_code = set_intersects(autom->multi_current_states, autom->final_states) ? HALTCODE_SUCCESS : HALTCODE_ERROR;
	}
	else
	{
		struct set temp;
		fna_move(autom, autom->multi_current_states, ch, &temp);
		fna_epsilon_closure(autom, &temp, autom->multi_current_states);
		
		if(!set_empty(autom->multi_current_states))
		{
			tape_move(tape, +1);
		}
		else
		{
			autom->halt_code = set_intersects(autom->multi_current_states, autom->final_states) ? HALTCODE_SUCCESS : HALTCODE_ERROR;
		}
	}
	if(autom->halt_code != HALTCODE_NONE)
	{
		int i;

		for(i = 0; i < autom->multi_current_nodes->size; i++)
		{
			struct treefna* node = (struct treefna*)autom->multi_current_nodes->elems[i];
			if(!set_contains(autom->final_states, node->state))
			{
				if(!node->first)
					node->dead = true;
			}
			else
			{
				node->accept = true;
#ifdef USE_TREEFNA_SEMI_ACCEPT
				{
					struct treefna* parent = node->parent;
					while(parent)
					{
						parent->semi_accept = true;
						parent = parent->parent;
					}
				}
#endif
			}
		}
	}
	return (autom->halt_code == HALTCODE_NONE);
}
struct automata_operations automata_operations_for_fna = 
{
	___fna_reset_automata,
	___fna_runstep_automata,
};
