///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Analizador sintáctico para el lenguaje AUTOMATA
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
#include "buf.h"
#include "dbg.h"
#include "atmt.h"
#include "atmt_lex.h"
#include "atmt_parser.h"
#include <array.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <set.h>
#include <stack.h>

struct automata* new_automata(int type)
{
	struct automata* autom = (struct automata*)calloc(1, sizeof(struct automata));
	
	autom->type = type;
	autom->final_states = set_new();
	//
	// ...
	//
	switch(type)
	{
		case AUTOMATA_TYPE_TMA:
			autom->opers = &automata_operations_for_tma;
			autom->deterministic = true;
			break;
		case AUTOMATA_TYPE_FDA:
			autom->opers = &automata_operations_for_fda;
			autom->deterministic = true;
			break;
		case AUTOMATA_TYPE_FNA:
			autom->opers = &automata_operations_for_fna;
			autom->deterministic = false;
			autom->multi_current_root = NULL;
			autom->multi_current_nodes = set_new();
			autom->multi_current_states = set_new();
			break;
		case AUTOMATA_TYPE_PDA:
			autom->opers = &automata_operations_for_pda;
			autom->deterministic = true;
			autom->stack = (struct stack*)malloc(sizeof(struct stack));
			stack_init(autom->stack);
			break;
		case AUTOMATA_TYPE_NPDA:
			autom->opers = &automata_operations_for_npda;
			autom->deterministic = false;
			autom->multi_current_root = NULL;
			autom->multi_current_nodes = set_new();
			autom->multi_current_states = set_new();
			break;
	}
	return autom;
}
struct automata_state* get_automata_state(struct automata* autom, int index, bool allocate)
{
	if(index >= autom->states_count)
	{
		if(!allocate)
		{
			return NULL;
		}
		else
		{
			while(index >= autom->states_count)
			{
				// AVISO: Se considera que la función 'array_add' pone a cero toda la estructura
				//        del estado reservado
				array_add((void**)&autom->states, &autom->states_count, sizeof(struct automata_state));
				//
				// Crear nombre
				//
				{
					int i = (autom->states_count-1);
					cstring_sprintf(&autom->states[i].name, "q%d", i);
				}
			}
		}
	}
	return &autom->states[index];
}
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

//
// Leer configuración
//
static void read_automata_config(struct automata* autom)
{
	while(tok == '<')
	{
		match('<');
		switch(tok)
		{
			case TOK_START_STATE:
				match(TOK_START_STATE);
				match('=');
				if(tok == TOK_STATE)
				{
					autom->initial_state = tok_num;
				}
				match(TOK_STATE);
				break;
			case TOK_FINAL_STATES:
				match(TOK_FINAL_STATES);
				match('=');
				if(tok == '{')
				{
					match('{');
					set_init(autom->final_states);
					while(tok != '}')
					{
						if(tok == TOK_STATE)
						{
							set_insert(autom->final_states, tok_num);
							match(TOK_STATE);
						}
						if(tok != '}')
							match(',');
					}
					match('}');
				}
				break;
		}
		match('>');
	}
}

//
// Leer funciones para Máquinas de Turing
//
static void read_automata_functions_for_tma(struct automata* autom, bool deterministic)
{
	while(tok == '&')
	{
		struct automata_state* state;
		struct automata_transition_tma* transition;
		
		match('&');
		match('(');
		if(tok == TOK_STATE)
		{
			//
			// Obtener estado
			//
			state = get_automata_state(autom, tok_num, true);
			//
			// Obtener una nueva entrada
			//
			transition = new_automata_transition(state, automata_transition_tma);
			
			next();
			match(',');
			
			//
			// Input
			//
			switch(tok)
			{
				case TOK_CHAR:	transition->in = tok_chr;		break;
				case TOK_NULL:	transition->in = CH_NULL;		break;
			}
			next();
			match(')');
			match('=');
			match('(');

			//
			// New State
			//
			switch(tok)
			{
				case TOK_HALT:
					transition->state = HALT_STATE;
					break;
				case TOK_STATE:
					transition->state = tok_num;
					get_automata_state(autom, tok_num, true);	// Forzar existencia del estado
					break;
				default:
					// FIXME: Error
					break;
			}
			next();
			match(',');

			//
			// Output
			//
			switch(tok)
			{
				case TOK_CHAR:	transition->out = tok_chr;		break;
				case TOK_NULL:	transition->out = CH_NULL;		break;
			}
			next();
			match(',');

			//
			// Direction
			//
			transition->direction = tok;
			next();
			match(')');
			match(';');
		}
	}
}

//
// Leer funciones para Automata de Estados Finitos (Determinísticos y No Determinísticos)
//
static void read_automata_functions_for_fsa(struct automata* autom, bool deterministic)
{
	while(tok == '&')
	{
		struct automata_state* state;
		struct automata_transition* transition;
		
		match('&');
		match('(');
		if(tok == TOK_STATE)
		{
			//
			// Obtener estado
			//
			state = get_automata_state(autom, tok_num, true);
			//
			// Obtener una nueva entrada
			//
			transition = new_automata_transition(state, automata_transition);
			
			next();
			match(',');

			//
			// Input
			//
			switch(tok)
			{
				case TOK_CHAR:		transition->in = tok_chr;		break;
				case TOK_NULL:		transition->in = CH_NULL;		break;
				case TOK_EPSILON:
					if(deterministic)
						;	// FIXME: mensaje de error
					else
						transition->in = CH_EPSILON;
					break;
			}
			next();
			match(')');
			match('=');

			//
			// New State
			//
			switch(tok)
			{
				case TOK_HALT:
					transition->state = HALT_STATE;
					break;
				case TOK_STATE:
					transition->state = tok_num;
					get_automata_state(autom, tok_num, true);	// Forzar existencia del estado
					break;
				default:
					// FIXME: Error
					break;
			}
			next();
			match(';');
		}
		else
		{
			// FIXME: Error
		}
	}
}

//
// Leer funciones para Automata de Pila (Determinísticos y No Determinísticos)
//
static void read_automata_functions_for_pda(struct automata* autom, bool deterministic)
{
	while(tok == '&')
	{
		struct automata_state* state;
		struct automata_transition_pda* transition;
		
		match('&');
		match('(');
		if(tok == TOK_STATE)
		{
			//
			// Obtener estado
			//
			state = get_automata_state(autom, tok_num, true);
			//
			// Obtener una nueva entrada
			//
			transition = new_automata_transition(state, automata_transition_pda);
			
			next();
			match(',');
			
			//
			// Input
			//
			switch(tok)
			{
				case TOK_CHAR:		transition->in = tok_chr;		break;
				case TOK_NULL:		transition->in = CH_NULL;		break;
				case TOK_EPSILON:
					if(deterministic)
						;	// FIXME: mensaje de error
					else
						transition->in = CH_EPSILON;
					break;
			}
			next();
			match(',');
			
			//
			// Stack Pop
			//
			switch(tok)
			{
				case TOK_CHAR:		transition->pop = tok_chr;		break;
				case TOK_ZETA:		transition->pop = CH_ZETA;		break;
				case TOK_NULL:		transition->pop = CH_NULL;		break;
				case TOK_EPSILON:	transition->pop = CH_EPSILON;	break;
				default:
					// FIXME: Error
					break;
			}
			next();
			match(')');
			match('=');
			match('(');
			
			//
			// New State
			//
			switch(tok)
			{
				case TOK_HALT:
					transition->state = HALT_STATE;
					break;
				case TOK_STATE:
					transition->state = tok_num;
					get_automata_state(autom, tok_num, true);	// Forzar existencia del estado
					break;
				default:
					// FIXME: Error
					break;
			}
			next();
			match(',');

			//
			// Stack Push
			//
			{
				int i;
				int n;
				int push[MAX_PUSHABLE_CHARS];

				n = 0;
				while(tok != ')' && n < MAX_PUSHABLE_CHARS)
				{
					switch(tok)
					{
						case TOK_CHAR:		push[n] = tok_chr;		break;
						case TOK_ZETA:		push[n] = CH_ZETA;		break;
						case TOK_NULL:		push[n] = CH_NULL;		break;
						case TOK_EPSILON:	push[n] = CH_EPSILON;	break;
						default:
							// FIXME: Error
							break;
					}
					n++;
					next();
				}
				for(i = 0; i < n; i++)
				{
					transition->push[i] = push[n - i - 1];
				}
				transition->push[i] = CH_NULL;
			}
			match(')');
			match(';');
		}
		else
		{
			// FIXME: Error
		}
	}
}

static struct automata* read_automata()
{
	struct automata* autom;
	void (*read_automata_functions)(struct automata* autom, bool deterministic);
	
	next();
	if(tok == TOK_AUTOMATA)
	{
		next();
		
		//
		// Leer tipo
		//
		match('[');
		switch(tok)
		{
			case TOK_AUTOMATA_TMA:
				autom = new_automata(AUTOMATA_TYPE_TMA);
				read_automata_functions = read_automata_functions_for_tma;
				break;
			case TOK_AUTOMATA_FDA:
				autom = new_automata(AUTOMATA_TYPE_FDA);
				read_automata_functions = read_automata_functions_for_fsa;
				break;
			case TOK_AUTOMATA_FNA:
				autom = new_automata(AUTOMATA_TYPE_FNA);
				read_automata_functions = read_automata_functions_for_fsa;
				break;
			case TOK_AUTOMATA_PDA:
				autom = new_automata(AUTOMATA_TYPE_PDA);
				read_automata_functions = read_automata_functions_for_pda;
				break;
			case TOK_AUTOMATA_NPDA:
				autom = new_automata(AUTOMATA_TYPE_NPDA);
				read_automata_functions = read_automata_functions_for_pda;
				break;
		}
		if(autom->type != AUTOMATA_TYPE_INVALID)
		{
			next();
			match(']');
			
			//
			// Leer descripción
			//
			match('[');
			if(tok == TOK_STRING)
			{
				strcpy(autom->descr, tok_string);
			}
			next();
			match(']');
			match('=');
			
			//
			// Leer contenido
			//
			if(tok == '{')
			{
				match('{');
				
				//
				// Leer configuración
				//
				read_automata_config(autom);
				
				//
				// Leer funciones
				//
				read_automata_functions(autom, autom->deterministic);
				
				match('}');
			}
			match(';');
		}
	}
	return autom;
}

struct automata* parser_automata(struct buffer* buf)
{
	struct lexer lex =
	{
		read_token_for_automata,
	};
	struct automata* autom = 0;
	
	lex_init(&lex, buf, NULL);
	
	//
	// Cargar automata
	//
	if(NULL != (autom = read_automata()))
	{
		//
		// Buscar estados de aceptación
		//
		int i;
		int j;
		
		for(i = 0; i < autom->states_count; i++)
		{
			for(j = 0; j < autom->states[i].transitions_count; j++)
			{
				if(autom->states[i].transitions[j].state == HALT_STATE)
				{
					set_insert(autom->final_states, i);
				}
			}
		}
	}
	lex_close();
	
	// ...
	return autom;
}

