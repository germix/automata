///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Funciones betas
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

bool compare_transitions(struct automata_state* s1, struct automata_state* s2)
{
	int t1;
	int t2;
	bool found;
	
	if(s1->transitions_count == s2->transitions_count)
	{
		for(t1 = 0; t1 < s1->transitions_count; t1++)
		{
			found = false;
			for(t2 = 0; t2 < s2->transitions_count; t2++)
			{
				if((s1->transitions[t1].in == s2->transitions[t2].in) && (s1->transitions[t1].state == s2->transitions[t2].state))
				{
					found = true;
				}
			}
			if(!found)
				return false;
		}
		return true;
	}
	return false;
}
bool has_transition_with_symb(struct automata_state* s, int in, int* trans)
{
	int i;

	for(i = 0; i < s->transitions_count; i++)
	{
		if(s->transitions[i].in == in)
		{
			*trans = i;
			return true;
		}
	}
	return false;
}

bool compare_states(bool* marked1, bool* marked2, struct automata* autom1, struct automata* autom2, int state1, int state2)
{
	int i;
	int t;
	struct automata_state* s1 = &autom1->states[state1];
	struct automata_state* s2 = &autom1->states[state2];
	
	if(s1->transitions_count != s2->transitions_count)
		return false;
	
	marked1[state1] = true;
	marked2[state2] = true;
	for(i = 0; i < s1->transitions_count; i++)
	{
		if(!has_transition_with_symb(s2, s1->transitions[i].in, &t))
			return false;
		
		if(!marked1[s1->transitions[i].state] && !marked2[s2->transitions[t].state])
		{
			if(!compare_states(marked1, marked2, autom1, autom2, s1->transitions[i].state, s2->transitions[t].state))
				return false;
		}
	}
	return true;
}

//
// Comparar que dos automatas AFD sean iguales, se supone que ambos están minimizados
//
bool automata_is_equals(struct automata* autom1, struct automata* autom2)
{
	int i;
	bool* marked1;
	bool* marked2;

	if(autom1->states_count != autom2->states_count)
		return false;
	
	marked1 = (bool*)calloc(autom1->states_count, sizeof(bool));
	marked2 = (bool*)calloc(autom1->states_count, sizeof(bool));
	for(i = 0; i < autom1->states_count; i++)
	{
		marked1[i] = false;
		marked2[i] = false;
	}
	if(compare_states(marked1, marked2, autom1, autom2, autom1->initial_state, autom2->initial_state))
	{
		for(i = 0; i < autom1->states_count; i++)
		{
			if(!marked1[i] || !marked2[i])
			{
				free(marked1);
				free(marked2);
				return false;
			}
		}
		free(marked1);
		free(marked2);
		return true;
	}
	free(marked1);
	free(marked2);
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

#include <utils.h>
#include <keybd.h>
#include <screen.h>

void test_automata_equals()
{
	bool automata_is_equals(struct automata* autom1, struct automata* autom2);
	struct automata* thompson(const char* string);
	struct automata* fna_to_fda(struct automata* fna_autom);
	struct automata* minimize_fda(struct automata* fda);

//	struct automata* a1 = minimize_fda(fna_to_fda(thompson("0*")));
//	struct automata* a2 = minimize_fda(fna_to_fda(thompson("0*")));

//	struct automata* a1 = minimize_fda(fna_to_fda(thompson("1|0")));
//	struct automata* a2 = minimize_fda(fna_to_fda(thompson("0|1")));

//	struct automata* a1 = minimize_fda(fna_to_fda(thompson("1*|0")));
//	struct automata* a2 = minimize_fda(fna_to_fda(thompson("0|1*")));

//	struct automata* a1 = minimize_fda(fna_to_fda(thompson("1*|00")));
//	struct automata* a2 = minimize_fda(fna_to_fda(thompson("00|1*")));
	
	struct automata* a1 = minimize_fda(fna_to_fda(thompson("1*|(00|b)*")));
	struct automata* a2 = minimize_fda(fna_to_fda(thompson("(b|00)*|1*11")));


	if(automata_is_equals(a1, a2))
		show_message("Automatas Iguales", 0x1E);
	else
		show_message("Automatas Diferentes", 0x4E);
	flush_screen();
	getkey();
	exit(0);
}


void test_automata_inclusion()
{
	flush_screen();
	getkey();
	exit(0);
}

