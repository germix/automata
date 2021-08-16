///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Automata
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___ATMT_H___
#define ___ATMT_H___
#if defined(_MSC_VER)
#pragma warning(error:4013)
#pragma warning(error:4020)
#pragma warning(error:4133)
#pragma warning(disable:4996)
#endif
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cstring.h>

#define MAX_POPABLE_CHARS	1
#define MAX_PUSHABLE_CHARS	5

#define HALT_STATE (-1)

//
// Códigos de detención
//
enum
{
	HALTCODE_NONE,				// Indica no está detenido
	HALTCODE_ERROR,				// Indica que se detuvo con error
	HALTCODE_SUCCESS,			// Indica que se detuvo sin error (en un estado de aceptación)
//	HALTCODE_NOSTATES,			// Indica que se detuvo porque él automata no tiene estados
	HALTCODE_NOTIMPLEMENTED,
};

//
// Caracteres especiales
//
enum
{
	CH_NULL		 = 0,			// \$
	CH_HALT		 = 1,			// \H
	CH_ZETA		 = 21,			// \Z
	CH_EPSILON	 = 136,			// \E
};

//
// Tipos de automatas
//
enum AUTOMATA_TYPE
{
	AUTOMATA_TYPE_INVALID = -1,
	AUTOMATA_TYPE_TMA,			// Turing Machine
	AUTOMATA_TYPE_PDA,			// Pushdown Automata
	AUTOMATA_TYPE_FDA,			// Finite Deterministic Automata
	AUTOMATA_TYPE_FNA,			// Finite Non-Deterministic Automata
	AUTOMATA_TYPE_NPDA,			//  Non-DeterministicPushdown Automata
};


//
// Transición básica de un automata
//
struct automata_transition
{
	int		in;							// Caracter de lectura
	int		state;						// Indice del siguiente estado
};

//
// Transición extendida para un automata PDA
//
struct automata_transition_pda
{
	int		in;							// Caracter de lectura
	int		state;						// Indice del siguiente estado
	// ...
	int		pop;						// Caracter que se toma de la pila
	int		push[MAX_PUSHABLE_CHARS+1];	// Caracteres que se colocan en la pila
};

//
// Transición extendida para un automata TMA
//
struct automata_transition_tma
{
	int		in;							// Caracter de lectura
	int		state;						// Indice del siguiente estado
	// ...
	int		out;						// Caracter de escritura
	int		direction;					// Dirección de movimiento del cabezal
};


//
// Estructura que representa un estado con sus entradas
//
struct automata_state
{
	struct automata_transition*		transitions;
	int								transitions_count;
//	char							name[50];
	struct cstring					name;
};

//
// Estructura de operaciones específicas para cada tipo de automata
//
struct automata_operations
{
	void (*reset_automata)(struct automata* autom, struct tape* tape);
	bool (*runstep_automata)(struct automata* autom, struct tape* tape);
};

struct automata
{
	// Tipo
	int						type;
	
	// Descripción
	char					descr[256];
	

	//
	// Estados
	//
	struct automata_state*	states;
	int						states_count;
	int						current_state;
	
	int						halt_code;


	//
	// Información para un PDA
	//
	struct stack*			stack;


	//
	// Información para un AFN
	//
	struct set*				final_states;
	struct set*				multi_current_states;
	// ...
//	struct treefna*			multi_current_root;
	void*					multi_current_root;		// treefna ó treenpda
	struct set*				multi_current_nodes;

	//
	// Información para el visualizador de transiciones
	//
	int last_state;
	union
	{
		struct automata_transition		fda;
		struct automata_transition_pda	pda;
		struct automata_transition_tma	tma;
	}last_transition;

	//
	// Grafo.
	// Es opcional, por lo tanto lo creo por separado, pero lo guardo en el automata
	// La razón de esto es que se podría generar varias formas de visualizarlo
	//
	struct graph*			graph;

	int						initial_state;
	bool					minimized;


	//
	// Operaciones
	//
	bool deterministic;
	struct automata_operations*	opers;
};

///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef __cplusplus
extern "C" {
#endif

int  automata_type(struct automata* autom);
char*automata_descr(struct automata* autom);
void automata_free(struct automata* autom);
void automata_reset(struct automata* autom, struct tape* tape);
bool automata_runstep(struct automata* autom, struct tape* tape);

int  automata_stacksize(struct automata* autom);
int  automata_stacksymbol(struct automata* autom, int idx);

int  automata_haltcode(struct automata* autom);

bool automata_is_empty(struct automata* autom);
bool automata_is_infinite(struct automata* autom);

bool automata_is_nondeterministic(struct automata* autom);

///////////////////////////////////////////////////////////////////////////////////////////////////

struct automata* new_automata(int type);

#define new_automata_transition(state, type_trans)	\
	(struct type_trans*)array_add((void**)&(state)->transitions, &state->transitions_count, sizeof(struct type_trans));

#define get_automata_transition(autom, state, index, type_trans)	\
	(struct type_trans*)&((char*)(autom->states[state].transitions))[index*sizeof(struct type_trans)];

struct automata_state* get_automata_state(struct automata* autom, int index, bool allocate);

///////////////////////////////////////////////////////////////////////////////////////////////////


extern struct automata_operations automata_operations_for_tma;
extern struct automata_operations automata_operations_for_pda;
extern struct automata_operations automata_operations_for_fda;
extern struct automata_operations automata_operations_for_fna;
extern struct automata_operations automata_operations_for_npda;

#ifdef __cplusplus
};
#endif

#endif

