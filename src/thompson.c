///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Método Thompson
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
#include <ctype.h>
#include <string.h>
#include <assert.h>
#include "array.h"
#include "stack.h"

#include "chrs.h"
#include "thompson.h"

//#define TRANS_1

#define SPACE_BETWEEN_NODES		7
#define SPACE_BETWEEN_LINES		3

void make_edge_line_pt_v2(struct edge* e, struct point p1, struct point p2, int type);


///////////////////////////////////////////////////////////////////////////////////////////////////


static int create_state(struct automata* autom)
{
	int idx = autom->states_count;
	struct automata_state* s = (struct automata_state*)array_add((void**)&autom->states, &autom->states_count, sizeof(struct automata_state));
	
	cstring_sprintf(&s->name, "q%d", idx);
	return idx;
}

void thompson_graph_move(struct thompson_graph* G, int dx, int dy)
{
	int i;
	int j;
	struct node* n;
	struct edge* e;
	
	G->x += dx;
	G->y += dy;
	for(i = 0; i < G->nodes.size; i++)
	{
		n = (struct node*)G->nodes.elems[i];
		n->x += dx;
		n->y += dy;
	}
	for(i = 0; i < G->edges.size; i++)
	{
		e = (struct edge*)G->edges.elems[i];
		e->ybase += dy;
		e->yline += dy;
		for(j = 0; j < e->points_count; j++)
		{
			e->points[j].x += dx;
			e->points[j].y += dy;
		}
	}
}
static struct node* create_state_node(struct automata* autom, struct set* nodes, int s)
{
	struct node* n;

	n = graph_addnode(autom->graph);
	n->state = s;
	cstring_copy(&autom->states[s].name, &n->label);
	n->width = n->label.len + 2;
	n->height = 1 + 2;
	n->xin = -2;
	n->xout = n->width + 1;
	
	set_insert(nodes, (int)n);
	
	return n;
}

//
// Crear un grafo con un estado inicial y uno terminal 
//
struct thompson_graph thompson_graph_new(struct automata* autom, bool with_states)
{
	struct thompson_graph G;
	
	memset(&G, 0, sizeof(struct thompson_graph));
	set_init(&G.nodes);
	set_init(&G.edges);
	if(!with_states)
	{
		G.s = -1;
		G.t = -1;
		G.ns = NULL;
		G.nt = NULL;
	}
	else
	{
		G.s = create_state(autom);
		G.t = create_state(autom);
		if(autom->graph)
		{
			G.ns = create_state_node(autom, &G.nodes, G.s);
			G.nt = create_state_node(autom, &G.nodes, G.t);
		}
	}
	return G;
}
//
// Crear una transición desde 's' a 't' con el símbolo 'c'
//
void thompson_graph_add_transition(struct automata* autom, struct thompson_graph* G, int s, int t, int c, struct node* ns, struct node* nt, int type, int ty)
{
	struct automata_transition* transition;
	
	transition = new_automata_transition((&autom->states[s]), automata_transition);
	transition->in = c;
	transition->state = t;
	if(autom->graph)
	{
		struct edge* e;
		
		e = graph_addedge(autom->graph);
		cstring_sprintf(&e->label, "%c", c);
		// ...
		e->q = &autom->graph->nodes[s];
		e->q->outedges[e->q->outedges_count++] = e;
		e->r = &autom->graph->nodes[t];
		e->r->inedges[e->r->inedges_count++] = e;
		
		set_insert(&G->edges, (int)e);
#if 0
		if(!type)
		{
			e->ybase = node_outpoint(ns).y;
#ifdef TRANS_1
			e->yline = node_outpoint(ns).y + ty;
			make_edge_line_pt(e, node_outpoint(ns), node_inpoint(nt), ty);
#else
			e->yline = ty;
			make_edge_line(e, node_outpoint(ns), node_inpoint(nt), ty);
#endif
		}
		else
		{
			switch(type)
			{
				case CHR_TOPLEFT:
				case CHR_BOTTOMLEFT:
					e->ybase = node_outpoint(ns).y;
					e->yline = node_inpoint(nt).y;
					break;
				case CHR_TOPRIGHT:
				case CHR_BOTTOMRIGHT:
					e->ybase = node_inpoint(nt).y;
					e->yline = node_outpoint(ns).y;
					break;
			}
			make_edge_line_pt_v2(e, node_outpoint(ns), node_inpoint(nt), type);
		}
#else
		
		if(node_outpoint(ns).y == node_inpoint(nt).y)
		{
			e->ybase = node_outpoint(ns).y;
#ifdef TRANS_1
			e->yline = node_outpoint(ns).y + ty;
			make_edge_line_pt(e, node_outpoint(ns), node_inpoint(nt), ty);
#else
			e->yline = ty;
			make_edge_line(e, node_outpoint(ns), node_inpoint(nt), ty);
#endif
		}
		else
		{
			switch(type)
			{
				case CHR_TOPLEFT:
				case CHR_BOTTOMLEFT:
					e->ybase = node_outpoint(ns).y;
					e->yline = node_inpoint(nt).y;
					break;
				case CHR_TOPRIGHT:
				case CHR_BOTTOMRIGHT:
					e->ybase = node_inpoint(nt).y;
					e->yline = node_outpoint(ns).y;
					break;
			}
			make_edge_line_pt_v2(e, node_outpoint(ns), node_inpoint(nt), type);
		}
#endif
	}
}

//
// Grafo para el símbolo 'c', 'c' puede de epsilon
//
struct thompson_graph thompson_graph_symb(struct automata* autom, int c)
{
	struct thompson_graph G;
	
	G = thompson_graph_new(autom, true);
	//
	// ...
	//
	G.ns->x = 0;
	G.ns->y = 0;
	G.nt->x = 0 + G.ns->width + SPACE_BETWEEN_NODES;
	G.nt->y = 0;
	G.width = G.nt->x + G.nt->width;
	G.height = max(G.ns->height, G.nt->height);
	//
	// ...
	//
#ifdef TRANS_1
	thompson_graph_add_transition(autom, &G, G.s, G.t, c, G.ns, G.nt, 0, 0);
#else
	thompson_graph_add_transition(autom, &G, G.s, G.t, c, G.ns, G.nt, 0, 1);
#endif
	return G;
}

//
// Cierre estrella de 'A'
//
struct thompson_graph thompson_graph_star(struct automata* autom, struct thompson_graph A)
{
	struct thompson_graph G;
	int middle;
	
	G = thompson_graph_new(autom, true);
	{
		struct set tmp;
		
		set_union(&G.nodes, &A.nodes, &tmp);
		set_copy(&tmp, &G.nodes);
		set_union(&G.edges, &A.edges, &tmp);
		set_copy(&tmp, &G.edges);
	}
	middle = (A.height/2) - (max(G.ns->height,G.nt->height)/2) + SPACE_BETWEEN_LINES;
//	middle = A.y + SPACE_BETWEEN_LINES;
	if(!(A.height%2))
		middle++;
//	G.height = (A.x + A.height) + (SPACE_BETWEEN_LINES+SPACE_BETWEEN_LINES);

	G.ns->x = 0;
	G.ns->y = middle;
	thompson_graph_move(&A, G.ns->x + G.ns->width + SPACE_BETWEEN_NODES, SPACE_BETWEEN_LINES);
	G.nt->x = A.nt->x + A.nt->width + SPACE_BETWEEN_NODES;
	G.nt->y = middle;
	G.width = G.nt->x + G.nt->width;
	G.height = A.y + A.height + (SPACE_BETWEEN_LINES);
	//
	// ...
	//
#ifdef TRANS_1
	thompson_graph_add_transition(autom, &G, G.s, G.t, CH_EPSILON, G.ns, G.nt, 0, SPACE_BETWEEN_LINES);
	thompson_graph_add_transition(autom, &G, G.s, A.s, CH_EPSILON, G.ns, A.ns, 0, 0);
	thompson_graph_add_transition(autom, &G, A.t, A.s, CH_EPSILON, A.nt, A.ns, 0, -SPACE_BETWEEN_LINES);
	thompson_graph_add_transition(autom, &G, A.t, G.t, CH_EPSILON, A.nt, G.nt, 0, 0);
#else
//	thompson_graph_add_transition(autom, &G, G.s, G.t, CH_EPSILON, G.ns, G.nt, 0, A.y+A.height+SPACE_BETWEEN_LINES-2);
//	thompson_graph_add_transition(autom, &G, G.s, A.s, CH_EPSILON, G.ns, A.ns, 0, A.ns->y+1);
//	thompson_graph_add_transition(autom, &G, A.t, A.s, CH_EPSILON, A.nt, A.ns, 0, A.y-SPACE_BETWEEN_LINES+1);
//	thompson_graph_add_transition(autom, &G, A.t, G.t, CH_EPSILON, A.nt, G.nt, 0, A.ns->y+1);

	thompson_graph_add_transition(autom, &G, G.s, A.s, CH_EPSILON, G.ns, A.ns, CHR_TOPLEFT, A.ns->y+1);
	thompson_graph_add_transition(autom, &G, A.t, G.t, CH_EPSILON, A.nt, G.nt, CHR_TOPRIGHT, A.ns->y+1);
	thompson_graph_add_transition(autom, &G, A.t, A.s, CH_EPSILON, A.nt, A.ns, 0, A.y-SPACE_BETWEEN_LINES+1);
	thompson_graph_add_transition(autom, &G, G.s, G.t, CH_EPSILON, G.ns, G.nt, 0, A.y+A.height+SPACE_BETWEEN_LINES-2);
#endif
	
	return G;
}
//
// Unión de 'A' y 'B'
//
struct thompson_graph thompson_graph_union(struct automata* autom, struct thompson_graph A, struct thompson_graph B)
{
	int y;
	struct thompson_graph G;
	
	y = A.height;
	G = thompson_graph_new(autom, true);
	{
		struct set tmp;
		
		set_union(&G.nodes, &A.nodes, &tmp);
		set_copy(&tmp, &G.nodes);
		set_union(&G.nodes, &B.nodes, &tmp);
		set_copy(&tmp, &G.nodes);

		set_union(&G.edges, &A.edges, &tmp);
		set_copy(&tmp, &G.edges);
		set_union(&G.edges, &B.edges, &tmp);
		set_copy(&tmp, &G.edges);
	}
	G.height = (A.y + A.height) + (B.y + B.height) + 3;

	G.ns->x = 0;
	G.ns->y = y;
	thompson_graph_move(&A, G.ns->x + G.ns->width + SPACE_BETWEEN_NODES, 0);
	thompson_graph_move(&B, G.ns->x + G.ns->width + SPACE_BETWEEN_NODES, A.height + 3);
	G.nt->x = max(A.nt->x + A.nt->width, B.nt->x + B.nt->width) + SPACE_BETWEEN_NODES;
	G.nt->y = y;
	G.width = G.nt->x + G.nt->width;
	G.height = B.y + B.height;

#ifdef TRANS_1
	thompson_graph_add_transition(autom, &G, G.s, A.s, CH_EPSILON, G.ns, A.ns, CHR_TOPLEFT, 0);
	thompson_graph_add_transition(autom, &G, G.s, B.s, CH_EPSILON, G.ns, B.ns, CHR_BOTTOMLEFT, 0);
	thompson_graph_add_transition(autom, &G, A.t, G.t, CH_EPSILON, A.nt, G.nt, CHR_TOPRIGHT, 0);
	thompson_graph_add_transition(autom, &G, B.t, G.t, CH_EPSILON, B.nt, G.nt, CHR_BOTTOMRIGHT, 0);
#else
	thompson_graph_add_transition(autom, &G, G.s, A.s, CH_EPSILON, G.ns, A.ns, CHR_TOPLEFT, 0);
	thompson_graph_add_transition(autom, &G, G.s, B.s, CH_EPSILON, G.ns, B.ns, CHR_BOTTOMLEFT, 0);
	thompson_graph_add_transition(autom, &G, A.t, G.t, CH_EPSILON, A.nt, G.nt, CHR_TOPRIGHT, 0);
	thompson_graph_add_transition(autom, &G, B.t, G.t, CH_EPSILON, B.nt, G.nt, CHR_BOTTOMRIGHT, 0);

#endif
	
	return G;
}

//
// Concatenación de 'A' y 'B'
//
struct thompson_graph thompson_graph_concat(struct automata* autom, struct thompson_graph A, struct thompson_graph B)
{
	struct thompson_graph G;
	int dy;
	
	G = thompson_graph_new(autom, false);
	{
		struct set tmp;
		
		set_union(&G.nodes, &A.nodes, &tmp);
		set_copy(&tmp, &G.nodes);
		set_union(&G.nodes, &B.nodes, &tmp);
		set_copy(&tmp, &G.nodes);

		set_union(&G.edges, &A.edges, &tmp);
		set_copy(&tmp, &G.edges);
		set_union(&G.edges, &B.edges, &tmp);
		set_copy(&tmp, &G.edges);
	}
	dy = max(A.ns->y, B.ns->y);
	
	G.s = A.s;
	G.t = B.t;
	G.ns = A.ns;
	G.nt = B.nt;
	thompson_graph_move(&A, 0, dy - A.ns->y);
	thompson_graph_move(&B, A.x + A.width + SPACE_BETWEEN_NODES, dy - B.ns->y);
	G.width = B.x + B.width;
	G.height = max(A.y+A.height, B.y+B.height);

	thompson_graph_add_transition(autom, &G, A.t, B.s, CH_EPSILON, A.nt, B.ns, 0, A.ns->y+1);
	return G;
}
