///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Minimización de un AFD
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
#include <assert.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

extern bool has_path(struct automata* autom, int from, int to);
extern void get_sigma(struct automata* autom, struct set* sigma);
extern struct automata_state* create_state(struct automata_state** states, int* states_count, struct set* set);


//
// Retorna &(state,simbol)
//
int delta_function(struct automata* autom, int state, int symbol)
{
	int i;
	int r = -1;
	struct automata_state* s = &autom->states[state];
	for(i = 0; (i < s->transitions_count) && (r == -1); i++)
	{
		if(s->transitions[i].in == symbol)
		{
			r = s->transitions[i].state;
		}
	}
//	assert(r != -1);
	return r;
}

void split(struct automata* autom, struct set* B, int a, struct set* B1, struct set* B2, struct set* P)
{
	int i;
	int j;
	int b;
	int s;
	
	set_init(B1);
	set_init(B2);
	
	//
	// Calcular B'
	//
	{
		struct set* G = 0;

		for(i = 0; i < B->size && !G; i++)		// Para cada elemento 'b' que pertenece a B
		{
			b = B->elems[i];
			s = delta_function(autom, b, a);

			for(j = 0; j < P->size && !G; j++)
			{
				if(set_contains((struct set*)P->elems[j], s))
				{
					G = (struct set*)P->elems[j];
					set_insert(B1, b);
				}
			}
		}
		for(; i < B->size; i++)
		{
			b = B->elems[i];
			s = delta_function(autom, b, a);
			for(j = 0; j < P->size; j++)
			{
				if(set_contains(G, s))
				{
					set_insert(B1, b);
				}
			}
		}
	}
	/*
	for(i = 0; i < B->size; i++)		// Para cada elemento 'b' que pertenece a B
	{
		b = B->elems[i];
		s = delta_function(Q, b, a);	// &(b,a)

		for(j = 0; j < P->size; j++)
		{
			if(set_contains((struct set*)P->elems[j], s))	// Si &(b,a) pertenece a alguna particion de P
			{
				set_insert(B1, b);
			}
		}
	}
	*/
	//
	// Calcular B''
	//
	set_difference(B, B1, B2);
}

bool partition_contains(const struct set* p, const struct set* s)
{
	int i;
	struct set* tmp;

	for(i = 0; i < p->size; i++)
	{
		tmp = (struct set*)p->elems[i];
		if(set_equals(tmp, s))
			return true;
	}
	return false;
}
bool compare_partitions(const struct set* P1, const struct set* P2)
{
	if(P1->size == P2->size)
	{
		int i;
		int j;
		struct set* s1;
		struct set* s2;
		bool match;
		
		for(i = 0; i < P1->size; i++)
		{
			s1 = (struct set*)P1->elems[i];
			match = false;
			for(j = 0; j < P2->size && !match; j++)
			{
				s2 = (struct set*)P2->elems[j];
				if(set_equals(s1, s2))
					match = true;
			}
			if(!match)
				return false;
		}
		return true;
	}
	return false;
}

void minimize_now(struct automata* autom, struct automata* minimized)
{
	int i;
	struct set P;		// Partición (conjunto de conjunto de estados)
	struct set S;		// Conjunto de todos los símbolos (Sigma)
	struct set Q;		// Todos los estados
	struct set* F;		// Estados finales
	struct set* N;		// Estados no finales (Q-F)
	
	//
	// Obtener alfabeto
	//
	get_sigma(autom, &S);
	
	//
	// Obtener todos los estados
	//
	set_init(&Q);
	for(i = 0; i < autom->states_count; i++)
		set_insert(&Q, i);
	//
	// Estados finales
	//
	F = set_new();
	set_copy(autom->final_states, F);
	//
	// Estados no finales
	//
	N = set_new();
	set_difference(&Q, F, N);

	//
	// Eliminar estados inaccesibles
	//
	{
		struct set tmp;
		struct set unreachable;	// Conjunto de estados inalcanzables (son los estado que hay que eliminar)
		
		set_init(&unreachable);
		for(i = 0; i < autom->states_count; i++)
		{
			if(!has_path(autom, autom->initial_state, i))	// Comprobar que no haya un camino de q0 (estado inicial) a qi
				set_insert(&unreachable, i);
		}
		
		set_init(&tmp);
		set_difference(F, &unreachable, &tmp);
		set_copy(&tmp, F);
		set_init(&tmp);
		set_difference(N, &unreachable, &tmp);
		set_copy(&tmp, N);
	}
	//
	// ...
	//
	set_init(&P);
	if(!set_empty(F))
		set_insert(&P, (int)F);
	if(!set_empty(N))
		set_insert(&P, (int)N);
	while(1)
	{
		int j;
		struct set Pnew;
		struct set* B;
		struct set* B1;
		struct set* B2;
		bool splitted = false;

		set_init(&Pnew);
		for(i = 0; i < P.size; i++)
		{
			B = (struct set*)P.elems[i];
			splitted = false;
			for(j = 0; j < S.size && !splitted; j++)
			{
				B1 = set_new();
				B2 = set_new();
				split(autom, B, S.elems[j], B1, B2, &P);
				if(B1->size > 0 && B2->size > 0)
				{
					set_insert(&Pnew, (int)B1);
					set_insert(&Pnew, (int)B2);
					free(B);
					splitted = true;
				}
				else
				{
					free(B1);
					free(B2);
				}
			}
			if(!splitted)
				set_insert(&Pnew, (int)B);
		}
		if(compare_partitions(&P, &Pnew))
		{
			break;
		}
		set_copy(&Pnew, &P);
	}
	{
		struct set* B;
		
		//
		// Buscar estado inicial
		//
		for(i = 0; i < P.size; i++)
		{
			B = (struct set*)P.elems[i];
			if(set_contains(B, autom->initial_state))	// Si q0 (estado inicial) está en B
			{
#if 0//!!!
				if(i != 0)
				{
					int tmp = P.elems[0];
					P.elems[i] = tmp;
					P.elems[0] = (int)B;
				}
#else
				minimized->initial_state = i;
#endif
				break;
			}
		}
		//
		// Buscar estados finales
		//
		for(i = 0; i < P.size; i++)
		{
			B = (struct set*)P.elems[i];
			if(set_intersects(autom->final_states, B))
				set_insert(minimized->final_states, i);
		}
		//
		// Crear estados
		//
		for(i = 0; i < P.size; i++)
		{
			B = (struct set*)P.elems[i];
			create_state(&minimized->states, &minimized->states_count, B);
		}
		//
		// Crear las transiciones
		//
		for(i = 0; i < P.size; i++)
		{
			int s;
			int c;
			int j;
			int k;

			B = (struct set*)P.elems[i];
			for(j = 0; j < S.size; j++)
			{
				c = S.elems[j];
				s = delta_function(autom, B->elems[0], c);
				if(s != -1)
				{
					for(k = 0; k < P.size; k++)
					{
						if(set_contains((struct set*)P.elems[k], s))
						{
							struct automata_transition* transition;

							transition = new_automata_transition((&minimized->states[i]), automata_transition);
							transition->in = c;
							transition->state = k;
						}
					}
				}
			}
		}
	}
}

struct automata* minimize_fda(struct automata* fda)
{
	struct automata* minimized;
	
	if(fda->type != AUTOMATA_TYPE_FDA)
	{
		return NULL;
	}
	minimized = new_automata(AUTOMATA_TYPE_FDA);
	sprintf(minimized->descr, "Minimized( %s )", fda->descr);
	
	minimize_now(fda, minimized);
	
	minimized->minimized = true;
	return minimized;
}

