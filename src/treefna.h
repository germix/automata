///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Árbol para la representación de multiple estados corrientes de un AFN
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___TREEFNA_H___
#define ___TREEFNA_H___
#include <stdbool.h>

//#define USE_TREEFNA_SEMI_ACCEPT

struct treefna
{
	//
	// Datos de arbol
	//
	struct treefna*		parent;		// Padre
	struct treefna*		first;		// Primer hijo
	struct treefna*		last;		// Ultimo hijo
	struct treefna*		prev;		// Hermano anterior
	struct treefna*		next;		// Hermano siguiente
	//
	// Datos de automata
	//
	int					state;			// Estado al que representa
	bool				dead;			// Indica que es un estado muerto
	bool				accept;			// Indica que es un estado de aceptación
	int					symbol;			// Símbolo con el que se hace la transición
#ifdef USE_TREEFNA_SEMI_ACCEPT
	bool				semi_accept;	// Indica que tiene como descendiente un estado de aceptación
#endif
	//
	// Datos de visualización
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

struct treefna* treefna_new(struct treefna* parent, int state, int symbol);
void treefna_delete(struct treefna* node);
int treefna_depth(struct treefna* node);

#ifdef __cplusplus
};
#endif

#endif
