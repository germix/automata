///////////////////////////////////////////////////////////////////////////////////////////////////
//
// �rbol para la representaci�n de multiple estados corrientes de un NPDA
//
// Autor: Germ�n Mart�nez
// Materia: Aut�matas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___TREENPDA_H___
#define ___TREENPDA_H___
#include <atmt.h>
#include <stack.h>

struct treenpda
{
	//
	// Datos de arbol
	//
	struct treenpda*	parent;		// Padre
	struct treenpda*	first;		// Primer hijo
	struct treenpda*	last;		// Ultimo hijo
	struct treenpda*	prev;		// Hermano anterior
	struct treenpda*	next;		// Hermano siguiente
	//
	// Datos de automata
	//
	int					state;			// Estado al que representa
	bool				dead;			// Indica que es un estado muerto
	bool				accept;			// Indica que es un estado de aceptaci�n
	int					symbol;			// S�mbolo con el que se hace la transici�n
	int					pop;
	int					push[MAX_PUSHABLE_CHARS];
	struct stack		stack;
	//
	// Datos de visualizaci�n
	//
	int					x;
	int					y;
	int					width;
	int					height;
	char				label[1];
};

#ifdef __cplusplus
extern "C" {
#endif

int treenpda_depth(struct treenpda* node);
void treenpda_delete(struct treenpda* node);

#ifdef __cplusplus
};
#endif

#endif
