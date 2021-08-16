///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Funciones útiles sobre automatas
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
#include <string.h>
#include <atmt.h>
#include <set.h>
#include <stack.h>
#include <array.h>
#include <queue.h>


extern struct automata* fna_to_fda(struct automata* fna_autom);
extern struct automata* minimize_fda(struct automata* fda);


#if 1
bool has_path(struct automata* autom, int from, int to)
{
	int i;
	int q;
	struct queue w;
	struct automata_state* s;
	struct automata_transition* t;
	bool* visited;

	visited = (bool*)calloc(autom->states_count, sizeof(bool));
	queue_init(&w);
	queue_enqueue(&w, from);
	while(!queue_empty(&w))
	{
		q = queue_dequeue(&w);
		if(q == to)
		{
			free(visited);
			return true;
		}
		visited[q] = true;

		s = &autom->states[q];
		for(i = 0; i < s->transitions_count; i++)
		{
			t = &s->transitions[i];
			if(t->state != HALT_STATE && !visited[t->state])
				queue_enqueue(&w, t->state);
		}
	}
	free(visited);
	return false;
}
#else
static bool has_path_recur(struct automata* autom, bool* visited, int from, int to)
{
	if(from == to)
	{
		return true;
	}
	else
	{
		int i;
		struct automata_state* s;
		struct automata_transition* t;

		visited[from] = true;
		s = &autom->states[from];
		for(i = 0; i < s->transitions_count; i++)
		{
			t = &s->transitions[i];
			if(t->state != HALT_STATE && !visited[t->state] && has_path_recur(autom, visited, t->state, to))
			{
				return true;
			}
		}
	}
	return false;
}
bool has_path(struct automata* autom, int from, int to)
{
	bool has;
	bool* visited;

	visited = (bool*)calloc(autom->states_count, sizeof(bool));
	has = has_path_recur(autom, visited, from, to);
	free(visited)
	return has;
}
#endif
bool has_cycle(struct automata* autom, int state)
{
	int i;
	struct automata_state* s;
	struct automata_transition* t;
	
	s = &autom->states[state];
	for(i = 0; i < s->transitions_count; i++)
	{
		t = &s->transitions[i];
		if(has_path(autom, t->state, state))
			return true;
	}
	return false;
}

bool automata_is_empty(struct automata* autom)
{
	bool empty = true;

	if(autom->type == AUTOMATA_TYPE_FDA || autom->type == AUTOMATA_TYPE_FNA)
	{
		int i;
		struct automata* fda;
		struct automata* minimized;
		
		if(autom->type == AUTOMATA_TYPE_FNA)
		{
			fda = fna_to_fda(autom);
			minimized = minimize_fda(fda);
			automata_free(fda);
		}
		else
		{
			minimized = minimize_fda(autom);
		}
		for(i = 0; i < minimized->final_states->size && empty; i++)
		{
			if(has_path(autom, autom->initial_state, autom->final_states->elems[i]))
			{
				empty = false;
			}
		}
		automata_free(minimized);
	}
	return empty;
}

bool automata_is_infinite(struct automata* autom)
{
	bool infinite = false;

	if(autom->type == AUTOMATA_TYPE_FDA || autom->type == AUTOMATA_TYPE_FNA)
	{
		int i;
		int state;
		struct automata* fda;
		struct automata* minimized;
		
		if(autom->type == AUTOMATA_TYPE_FNA)
		{
			fda = fna_to_fda(autom);
			minimized = minimize_fda(fda);
			automata_free(fda);
		}
		else
		{
			minimized = minimize_fda(autom);
		}
		
		for(state = 0; state < autom->states_count && !infinite; state++)
		{
			//
			// Comprobar si tiene ciclo
			//
			if(has_cycle(autom, state))
			{
				//
				// Comprobar si este estado es accesible desde el estado inicial
				//
				if(has_path(autom, autom->initial_state, state))
				{
					//
					// Comprobar si este estado llega a algún estado final
					//
					for(i = 0; i < autom->final_states->size && !infinite; i++)
					{
						if(has_path(autom, state, autom->final_states->elems[i]))
						{
							infinite = true;
						}
					}
				}
			}
		}
		automata_free(minimized);
	}
	return infinite;
}
