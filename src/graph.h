///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Grafo
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___GRAPH_H___
#define ___GRAPH_H___
#include <stdbool.h>
#include <cstring.h>

#define MAX_NODE_LABEL 200
#define MAX_EDGE_LABEL 200

struct point
{
	int		x;
	int		y;
};

struct node
{
	int		x;
	int		y;
	int		width;
	int		height;
	int		xin;				// Por donde entra la flecha
	int		xout;				// Por donde sale la flecha
	int		ysup;				// Posición superior de la nueva arista relacionada
	int		yinf;				// Posición inferior de la nueva arista relacionada

	struct edge*	inedges[300];
	int				inedges_count;
	struct edge*	outedges[300];
	int				outedges_count;

	//
	// Está es la única información relacionada directamente con automatas
	//
	int				state;		// Estado del automata
	bool			accept;		// Estado de aceptación
	bool			initial;	// Estado de inicio

	
	struct cstring	label;
};

struct edge
{
	struct node*	q;
	struct node*	r;
	// ...
	int				ybase;
	int				yline;
//	int				xleft;
//	int				xright;
	
	struct point	points[4];		// 4 es el máximo
	int				points_count;

	
	struct cstring	label;
};
struct graph
{
	int				width;
	int				height;
	/*
	struct node*	nodes;
	int				nodes_count;
	struct edge*	edges;
	int				edges_count;
	*/
	struct node		nodes[300];
	int				nodes_count;
	struct edge		edges[300];
	int				edges_count;

};

#ifdef __cplusplus
extern "C" {
#endif

void edge_init(struct edge* e);
void node_init(struct node* n);

struct graph* graph_new();
void          graph_delete(struct graph* g);
struct node*  graph_addnode(struct graph* g);
struct edge*  graph_addedge(struct graph* g);
struct graph* make_graph(struct automata* autom);

int node_center(struct node* node);
struct point node_inpoint(struct node* node);
struct point node_outpoint(struct node* node);

void make_edge_line(struct edge* e, struct point p1, struct point p2, int ty);

void make_edge_line_pt(struct edge* e, struct point p1, struct point p2, int ty);

#ifdef __cplusplus
};
#endif

#endif
