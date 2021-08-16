///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Método Thompson
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___THOMPSON_H___
#define ___THOMPSON_H___
#include "set.h"
#include "atmt.h"
#include "graph.h"

struct thompson_graph
{
	int s;			// Estado inicial
	int t;			// Estado terminal

	struct node* ns;	// Nodo de estado inicial
	struct node* nt;	// Nodo de estado terminal
	
	int			x;
	int			y;
	int			width;
	int			height;

	struct set		nodes;
	struct set		edges;
};

#ifdef __cplusplus
extern "C" {
#endif

struct thompson_graph thompson_graph_symb(struct automata* autom, int c);
struct thompson_graph thompson_graph_star(struct automata* autom, struct thompson_graph A);
struct thompson_graph thompson_graph_union(struct automata* autom, struct thompson_graph A, struct thompson_graph B);
struct thompson_graph thompson_graph_concat(struct automata* autom, struct thompson_graph A, struct thompson_graph B);

#ifdef __cplusplus
};
#endif

#endif
