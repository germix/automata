///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Convertir un AFN a un AFD
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
#include <atmt.h>
#include <set.h>
#include <stack.h>
#include <array.h>
#include <queue.h>


void get_sigma(struct automata* autom, struct set* sigma)
{
	int i;
	int j;
	
	set_init(sigma);
	//
	// Obtener el conjunto de todas las posibles entradas
	//
	for(i = 0; i < autom->states_count; i++)
	{
		struct automata_state* state = &autom->states[i];
		for(j = 0; j < state->transitions_count; j++)
		{
			switch(state->transitions[j].in)
			{
				case CH_NULL:
				case CH_HALT:
				case CH_ZETA:
				case CH_EPSILON:
					break;
				default:
					set_insert(sigma, state->transitions[j].in);
					break;
			}
		}
	}
	//
	// Ordenar los elementos
	//
	for(i = 0; i < sigma->size; i++)
	{
		for(j = i+1; j < sigma->size; j++)
		{
			if(sigma->elems[i] > sigma->elems[j])
			{
				int tmp = sigma->elems[i];
				sigma->elems[i] = sigma->elems[j];
				sigma->elems[j] = tmp;
			}
		}
	}
}

struct automata_state* create_state(struct automata_state** states, int* states_count, struct set* set)
{
	struct automata_state* s = (struct automata_state*)array_add((void**)states, states_count, sizeof(struct automata_state));
	
	//
	// Construir nombre del estado
	//
	switch(set_size(set))
	{
		case 0:
			// ???
			break;
		case 1:
			cstring_sprintf(&s->name, "q%d", set->elems[0]);
			break;
		default:
			{
				int i;
				
			//	cstring_addchar(&s->name, '{');
			//	cstring_addchar(&s->name, '\'');
				cstring_addchar(&s->name, '\"');
				for(i = 0; i < set->size; i++)
				{
					if(i == 0)
						cstring_addsprintf(&s->name, "q%d", set->elems[i]);
					else
						cstring_addsprintf(&s->name, ",q%d", set->elems[i]);
				}
			//	cstring_addchar(&s->name, '}');
			//	cstring_addchar(&s->name, '\'');
				cstring_addchar(&s->name, '\"');
			}
			break;
	}
	return s;
}

void move(struct automata_state* states, struct set* currents, int symb, struct set* result)
{
	int i;
	struct stack stack;
	
	//
	// Crear un nuevo conjunto
	//
	set_init(result);
	
	//
	// Poner los estados corrientes en la pila
	//
	stack_init(&stack);
	for(i = 0; i < currents->size; i++)
	{
		stack_push(&stack, currents->elems[i]);
	}
	
	//
	// ...
	//
	while(!stack_empty(&stack))
	{
		int q = stack_pop(&stack);
		for(i = 0; i < states[q].transitions_count; i++)
		{
			struct automata_transition* t = &states[q].transitions[i];
			int c = t->in;
			int r = t->state;
			if(c == symb && !set_contains(result, r))
			{
				set_insert(result, r);
			}
		}
	}
}
void epsilon_closure(struct automata_state* states, struct set* currents, struct set* result)
{
	int i;
	struct stack stack;
	
	//
	// Hacer una copia de los estados
	//
	set_copy(currents, result);
	
	//
	// Poner los estados corrientes en la pila
	//
	stack_init(&stack);
	for(i = 0; i < currents->size; i++)
	{
		stack_push(&stack, currents->elems[i]);
	}
	
	//
	// ...
	//
	while(!stack_empty(&stack))
	{
		int q = stack_pop(&stack);
		for(i = 0; i < states[q].transitions_count; i++)
		{
			struct automata_transition* t = &states[q].transitions[i];
			int c = t->in;
			int r = t->state;
			if(c == CH_EPSILON && !set_contains(result, r))
			{
				set_insert(result, r);
				stack_push(&stack, r);
			}
		}
	}
}

struct data
{
	int			state;
	struct set	fna_states;
};

void add_transition(struct automata* fda, struct data* t, int a, struct data* u)
{
	struct automata_state* state = get_automata_state(fda, t->state, true);
	struct automata_transition* transition;
	
	transition = new_automata_transition(state, automata_transition);
	transition->in = a;
	transition->state = u->state;
}

struct automata* fna_to_fda(struct automata* fna_autom)
{
	int i;
	int j;
	struct data* t;
	struct data* u;
	struct set symbols;
#ifdef UNMARKED_WITH_STACK
	struct stack unmarked;		// Representa los estados no marcado de Destados, popear un estado, representa marcarlo
#else
	struct queue unmarked;
#endif
	struct automata* fda_autom;
	struct set dstates;
	
	if(fna_autom->type != AUTOMATA_TYPE_FNA)
	{
		return NULL;
	}
	set_init(&symbols);
	set_init(&dstates);
#ifdef UNMARKED_WITH_STACK
	stack_init(&unmarked);
#else
	queue_init(&unmarked);
#endif

	//
	// Crear AFD
	//
	fda_autom = new_automata(AUTOMATA_TYPE_FDA);
	sprintf(fda_autom->descr, "AFN->AFD ( %s )", fna_autom->descr);
	
	//
	// Obtener el conjunto de todas las posibles entradas
	//
	get_sigma(fna_autom, &symbols);

	//
	// Inicialmente, cerradura-e(s0) es el único estado dentro de Destados, y está sin marcar
	//
	{
		struct set s0;
		
		t = (struct data*)calloc(1, sizeof(struct data));
		t->state = fda_autom->states_count;
		
		set_init(&s0);
		set_init(&t->fna_states);
		set_insert(&s0, fna_autom->initial_state);
		epsilon_closure(fna_autom->states, &s0, &t->fna_states);
		
		set_insert(&dstates, (int)t);
#ifdef UNMARKED_WITH_STACK
		stack_push(&unmarked, (int)t);
#else
		queue_enqueue(&unmarked, (int)t);
#endif
		create_state(&fda_autom->states, &fda_autom->states_count, &t->fna_states);
		//
		// ...
		//
		fda_autom->initial_state = 0;
		if(set_intersects(&t->fna_states, fna_autom->final_states))
		{
			set_insert(fda_autom->final_states, t->state);
		}
	}
	
	//
	// Mientras haya estados sin marcar en Destados, hacer:
	//
#ifdef UNMARKED_WITH_STACK
	while(!stack_empty(&unmarked))
#else
	while(!queue_empty(&unmarked))
#endif
	{
#ifdef UNMARKED_WITH_STACK
		t = (struct data*)stack_pop(&unmarked);	// Al quitar de la pila de no-marcados, indica que está marcado
#else
		t = (struct data*)queue_dequeue(&unmarked);
#endif
		
		for(i = 0; i < symbols.size; i++)	// Recorrer cada símbolo de entrada
		{
			bool found;
			struct set move_result;
			struct set closure_result;		// U
			
			set_init(&move_result);
			set_init(&closure_result);
			move(fna_autom->states, &t->fna_states, symbols.elems[i], &move_result);
			epsilon_closure(fna_autom->states, &move_result, &closure_result);
			
			found = false;
			if(!set_empty(&closure_result))
			{
				for(j = 0; j < dstates.size && !found; j++)
				{
					u = (struct data*)dstates.elems[j];
					if(set_equals(&closure_result, &u->fna_states))
					{
						found = true;
					}
				}
				if(!found)
				{
					u = (struct data*)calloc(1, sizeof(struct data));
					
					u->state = fda_autom->states_count;
					set_copy(&closure_result, &u->fna_states);
					
					set_insert(&dstates, (int)u);
#ifdef UNMARKED_WITH_STACK
					stack_push(&unmarked, (int)u);
#else
					queue_enqueue(&unmarked, (int)u);
#endif
					create_state(&fda_autom->states, &fda_autom->states_count, &u->fna_states);
					
					//
					// Verificar si es un estado final, si al menos
					// un estado de que compone al nuevo estado u está como estado final en el AFN
					//
					if(set_intersects(&u->fna_states, fna_autom->final_states))
					{
						set_insert(fda_autom->final_states, u->state);
					}
				}
				add_transition(fda_autom, t, symbols.elems[i], u);
			}
		}
	}
	for(i = 0; i < dstates.size; i++)
	{
		//
		// Comprobar si es el estado inicial
		//
		/*
		struct data* d = (struct data*)dstates.elems[i];
		
		if(set_contains(&d->fna_states, fna_autom->initial_state))
		{
			fda_autom->initial_state = d->state;
		}
		*/
		free((struct data*)dstates.elems[i]);
	}
	return fda_autom;
}

