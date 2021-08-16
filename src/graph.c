///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Grafo
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
#include "chrs.h"
#include "atmt.h"
#include "graph.h"
#include "set.h"
#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define YBASE	1
#define SPACE_BETWEEN_LINES		3

void edge_init(struct edge* e)
{
	memset(e, 0, sizeof(struct edge));
}
void node_init(struct node* n)
{
	memset(n, 0, sizeof(struct node));
}
struct graph* graph_new()
{
	return (struct graph*)calloc(1, sizeof(struct graph));
}
void          graph_delete(struct graph* g)
{
	int i;
	struct node* n;
	struct edge* e;

	for(i = 0; i < g->nodes_count; i++)
	{
		n = &g->nodes[i];
		cstring_clear(&n->label);
	}
	for(i = 0; i < g->edges_count; i++)
	{
		e = &g->edges[i];
		cstring_clear(&e->label);
	}
	free(g);
}
struct node*  graph_addnode(struct graph* g)
{
	return &g->nodes[g->nodes_count++];
}
struct edge*  graph_addedge(struct graph* g)
{
	return &g->edges[g->edges_count++];
}

void make_node_edges(struct automata* autom, struct graph* graph)
{
	extern void syms_to_str(int* syms, int count, char* buf);
	extern void direction_to_str(int direction, char* buf);
	// ...
	int i;
	int j;
	struct edge* e;
	static unsigned char str_in[10];
	static unsigned char str_out[10];
	static unsigned char str_pop[MAX_POPABLE_CHARS+1];
	static unsigned char str_push[MAX_PUSHABLE_CHARS+1];
	static unsigned char str_direction[10];

	for(i = 0; i < autom->states_count; i++)
	{
		for(j = 0; j < autom->states[i].transitions_count; j++)
		{
			switch(autom->type)
			{
				case AUTOMATA_TYPE_FDA:
				case AUTOMATA_TYPE_FNA:
					{
						struct automata_transition* fa = get_automata_transition(autom, i, j, automata_transition);
						if(fa->state == HALT_STATE)
						{
							graph->nodes[i].accept = true;
							continue;
						}
						e = graph_addedge(graph);
						e->q = &graph->nodes[i];
						e->r = &graph->nodes[fa->state];
						
						syms_to_str(&fa->in, 1, str_in);
						cstring_sprintf(&e->label, "%s", str_in);
					}
					break;
				case AUTOMATA_TYPE_TMA:
					{
						struct automata_transition_tma* tma = get_automata_transition(autom, i, j, automata_transition_tma);
						if(tma->state == HALT_STATE)
						{
							graph->nodes[i].accept = true;
							continue;
						}
						e = graph_addedge(graph);
						e->q = &graph->nodes[i];
						e->r = &graph->nodes[tma->state];

						syms_to_str(&tma->in, 1, str_in);
						syms_to_str(&tma->out, 1, str_out);
						direction_to_str(tma->direction, str_direction);

						cstring_sprintf(&e->label, "(%s,%s,%s)",
								str_in,
								str_out,
								str_direction);
					}
					break;
				case AUTOMATA_TYPE_PDA:
				case AUTOMATA_TYPE_NPDA:
					{
						struct automata_transition_pda* pda = get_automata_transition(autom, i, j, automata_transition_pda);
						if(pda->state == HALT_STATE)
						{
							graph->nodes[i].accept = true;
							continue;
						}
						e = graph_addedge(graph);
						e->q = &graph->nodes[i];
						e->r = &graph->nodes[pda->state];
						
						syms_to_str(&pda->in, 1, str_in);
						syms_to_str(&pda->pop, 1, str_pop);
						syms_to_str(pda->push, MAX_PUSHABLE_CHARS, str_push);
						cstring_sprintf(&e->label, "(%s,%s,%s)",
							str_in,
							str_pop,
							str_push);
					}
					break;
			}
			e->q->outedges[e->q->outedges_count++] = e;
			e->r->inedges[e->r->inedges_count++] = e;
		}
	}
}

void calc_node_position(struct graph* graph)
{
	int i;
	int j;
	int x = 2;
	struct node* n;
	struct edge* e;
	int max_edge_label_len;	// Es para que no se superponga las líneas con la etiqueta

	for(i = 0; i < graph->nodes_count; i++)
	{
		int extra_space = 0;		
		n = &graph->nodes[i];
		
		n->x = x;
		n->y = 0;
		n->xin = -2;
		n->xout = n->width + 1;
		n->ysup = n->y - 2;
		n->yinf = n->y + n->height + 1;
		
		graph->width = x + n->width;

	
		//
		// Calcular 'xout', para el caso de los bucles
		// Calcular 'max_edge_label_len', para los demás casos
		//
		max_edge_label_len = 0;
		for(j = 0; j < n->outedges_count; j++)
		{
			e = n->outedges[j];
			if(n->state == e->r->state)
			{
				int t = (n->xout - (n->xin+1));
				int len = e->label.len;
				if(len > t)
					n->xout += (len - t);
			}
			else
			{
				max_edge_label_len = max(max_edge_label_len, e->label.len);
			}
		}
		//
		// ...
		//
		x += (n->xout+2) + max_edge_label_len + (2+1);
	}
}

struct graph* make_graph(struct automata* autom)
{
	int i;
	int j;
	struct graph* graph;
	int needed_width;
	
	graph = graph_new();
	
	//
	// Crear todos los nodos que representan a los estados
	//
	for(i = 0; i < autom->states_count; i++)
	{
		struct node* n = graph_addnode(graph);
		n->state = i;
		cstring_copy(&autom->states[i].name, &n->label);

		//
		// Comprobar si es un estado final
		//
		if(set_contains(autom->final_states, i))
			n->accept = true;
		//
		// Comprobar si es un estado inicial
		//
		if(i == autom->initial_state)
			n->initial = true;
	}
	//
	// Crear todas las aristas que representan a las transiciones
	//
#if 1
	make_node_edges(autom, graph);
#else
	for(i = 0; i < autom->states_count; i++)
	{
		static unsigned char str_in[10];
		static unsigned char str_out[10];
		static unsigned char str_pop[MAX_POPABLE_CHARS+1];
		static unsigned char str_push[MAX_PUSHABLE_CHARS+1];
		static unsigned char str_direction[10];

		extern void syms_to_str(int* syms, int count, char* buf);
		extern void direction_to_str(int direction, char* buf);
		
		for(j = 0; j < autom->states[i].transitions_count; j++)
		{
			struct edge* e;
			switch(autom->type)
			{
				case AUTOMATA_TYPE_FDA:
				case AUTOMATA_TYPE_FNA:
					{
						//struct automata_transition* fa = get_automata_transition(autom, i, j, automata_transition);
						struct automata_transition* fa = &autom->states[i].transitions[j];
						if(fa->state == HALT_STATE)
						{
							graph->nodes[i].accept = true;
							continue;
						}
						e = graph_addedge(graph);
						e->q = &graph->nodes[i];
						e->r = &graph->nodes[fa->state];
						
#if 0//!!!
						e->label_len = sprintf(e->label, "%c", fa->in);
#else
						syms_to_str(&fa->in, 1, str_in);
						e->label_len = sprintf(e->label, "%s", str_in);
#endif
					}
					break;
				case AUTOMATA_TYPE_TMA:
					{
						struct automata_transition_tma* tma = get_automata_transition(autom, i, j, automata_transition_tma);
						if(tma->state == HALT_STATE)
						{
							graph->nodes[i].accept = true;
							continue;
						}
						e = graph_addedge(graph);
						e->q = &graph->nodes[i];
						e->r = &graph->nodes[tma->state];

#if 0//!!!
						e->label_len = sprintf(e->label, "(%c,%c,%c)", tma->in, tma->out, tma->direction);
#else
						syms_to_str(&tma->in, 1, str_in);
						syms_to_str(&tma->out, 1, str_out);
						direction_to_str(tma->direction, str_direction);

					//	e->label_len = sprintf(e->label, "%s,%s,%s",
						e->label_len = sprintf(e->label, "(%s,%s,%s)",
								str_in,
								str_out,
								str_direction);
#endif
					}
					break;
				case AUTOMATA_TYPE_PDA:
				case AUTOMATA_TYPE_NPDA:
					{
						struct automata_transition_pda* pda = get_automata_transition(autom, i, j, automata_transition_pda);
						if(pda->state == HALT_STATE)
						{
							graph->nodes[i].accept = true;
							continue;
						}
						e = graph_addedge(graph);
						e->q = &graph->nodes[i];
						e->r = &graph->nodes[pda->state];
						
#if 0//!!!
						e->label_len = sprintf(e->label, "(%c,%c,%c)", pda->in, pda->pop, pda->push[0]);	// FIXME: el push puede ser mas de un símbolo
#else
						syms_to_str(&pda->in, 1, str_in);
						syms_to_str(&pda->pop, 1, str_pop);
						syms_to_str(pda->push, MAX_PUSHABLE_CHARS, str_push);
					//	e->label_len = sprintf(e->label, "%s,%s,%s",
						e->label_len = sprintf(e->label, "(%s,%s,%s)",
							str_in,
							str_pop,
							str_push);
#endif
					}
					break;
			}
			e->q->outedges[e->q->outedges_count++] = e;
			e->r->inedges[e->r->inedges_count++] = e;
		}
	}
#endif
	//
	// Calcular tamaño de nodos
	//
	for(i = 0; i < graph->nodes_count; i++)
	{
		struct node* n = &graph->nodes[i];
		
		n->width = n->label.len + 2;
		n->height = 1 + 2;
	}
	//
	// Calcular ancho mínimo necesario para etiqueta de arista
	//
	switch(autom->type)
	{
		case AUTOMATA_TYPE_TMA:
			needed_width = 7;
			break;
		case AUTOMATA_TYPE_PDA:
		case AUTOMATA_TYPE_NPDA:
			needed_width = 7;
			break;
		case AUTOMATA_TYPE_FDA:
		case AUTOMATA_TYPE_FNA:
			needed_width = 1;
			break;
	}
	needed_width += 2;

	//
	// Calcular posicionamiento de nodos
	//
#if 1
	calc_node_position(graph);
#else
	{
		int x = 2;
		for(i = 0; i < graph->nodes_count; i++)
		{
			struct node* n = &graph->nodes[i];
			int extra_space = 0;		// El espacio extra es para que no se superponga las líneas con la etiqueta
			
			n->x = x;
			n->y = 0;
			n->xin = -2;
			n->xout = n->width + 1;
			n->ysup = n->y - 2;
			n->yinf = n->y + n->height + 1;
			
			graph->width = x + n->width;
			
#if 0//!!!
			x += (n->width + needed_width + 4);
#else
#if 0//!!!
			//
			// Recorrer todos los nodos que salen de 'n'
			// y encontrar los bucles y los nodos que son los siguientes de 'n' en cuanto a indice
			//
			for(j = 0; j < n->outedges_count; j++)
			{
				struct edge* e = n->outedges[j];
				if((n->state == e->r->state) || ((n->state+1) ==  e->r->state))
				{
					int t = (n->xout - n->xin);
					int len = e->label_len;
					if(n->state == e->r->state)
						len += 1;
					if(len > t)
					{
						extra_space = max(extra_space, len - t);
					}
				}
			}
			if(extra_space)
			{
				n->xout += extra_space;
			}
#else
			for(j = 0; j < n->outedges_count; j++)
			{
				struct edge* e = n->outedges[j];
				if(n->state == e->r->state)
				{
					int t = (n->xout - (n->xin+1));
					int len = e->label_len;
					if(len > t) extra_space = max(extra_space, len - t);
				}
			}
			if(extra_space)
			{
				n->xout += extra_space;
			}
			for(j = 0; j < n->outedges_count; j++)
			{
				struct edge* e = n->outedges[j];
				if((n->state+1) == e->r->state)
				{
					int t = (n->xout - n->xin);
					int len = e->label_len;
					if(len > t) extra_space = max(extra_space, len - t);
				}
			}
#endif
			x += (n->width + needed_width + 4 + extra_space);
#endif
		}
	}
#endif
	//
	// Calcular posicionamiento de aristas
	//
	for(i = 0; i < graph->edges_count; i++)
	{
		struct edge* e = &graph->edges[i];
		
		e->ybase = 0+1;
		if((e->q->state+1) == (e->r->state))
		{
			int s;
			int yline = e->ybase;

			//
			// Buscar si ya hay alguna arista de este tipo
			//
			bool b = false;
			for(s = 0; s < i; s++)
			{
				struct edge* tmp = &graph->edges[s];
				if(tmp->q->state == e->q->state && tmp->r->state == e->r->state)
				{
					b = true;
					yline = max(yline, tmp->yline);
				}
			}
			if(b)
				yline += SPACE_BETWEEN_LINES;
			e->yline = yline;
			// ...
			if(yline != e->ybase)
			{
				e->q->yinf = yline + SPACE_BETWEEN_LINES;
				e->r->yinf = yline + SPACE_BETWEEN_LINES;
			}
		}
		else if((e->q->state+1) < (e->r->state))		// Ir por abajo
		{
			int s;
			int yline = e->ybase;
			
			for(s = e->q->state; s <= e->r->state; s++)
			{
				yline = max(yline, graph->nodes[s].yinf);
			}
			e->yline = yline;
			// ...
			e->q->yinf = yline + SPACE_BETWEEN_LINES;
			e->r->yinf = yline + SPACE_BETWEEN_LINES;
		}
		else if((e->q->state+1) > (e->r->state))	// Ir por arriba
		{
			int s;
			int yline = e->ybase;
			
			for(s = e->r->state; s <= e->q->state; s++)
			{
				yline = min(yline, graph->nodes[s].ysup);
			}
			e->yline = yline;
			// ...
			e->q->ysup = yline - SPACE_BETWEEN_LINES;
			e->r->ysup = yline - SPACE_BETWEEN_LINES;
		}
		make_edge_line(e, node_outpoint(e->q), node_inpoint(e->r), e->yline);
	}
	//
	// Calcular altura
	//
	{
		int inf, sup;
		struct node* n;
		struct edge* e;

		//
		// Buscar cota inferior y superior
		//
#if 0
		inf = graph->nodes[0].yinf;
		sup = graph->nodes[0].ysup;
		for(i = 1; i < graph->nodes_count; i++)
		{
			inf = max(inf, graph->nodes[i].yinf);
			sup = min(sup, graph->nodes[i].ysup);
		}
#else
		inf = graph->nodes[0].yinf;
		sup = graph->nodes[0].ysup;
		for(i = 1; i < graph->nodes_count; i++)
		{
			inf = max(inf, graph->nodes[i].yinf);
			sup = min(sup, graph->nodes[i].ysup);
		}
#endif
		//
		// Mover nodos
		//
		for(i = 0; i < graph->nodes_count; i++)
		{
			n = &graph->nodes[i];
			n->y += -sup;
		}
		//
		// Mover aristas
		//
		for(i = 0; i < graph->edges_count; i++)
		{
			e = &graph->edges[i];
			e->ybase += -sup;
			e->yline += -sup;
			for(j = 0; j < e->points_count; j++)
			{
				e->points[j].y += -sup;
			}
		}
		graph->height = abs(inf-sup);
	}
//	graph->width += (3+3);
//	graph->height += (3+3);
	return graph;
}


int node_center(struct node* node)
{
	return node->y + 1;
}
struct point node_inpoint(struct node* node)
{
	struct point p;
	p.x = node->x + node->xin;
	p.y = node->y + 1;	// node_center()
	return p;
}
struct point node_outpoint(struct node* node)
{
	struct point p;
	p.x = node->x + node->xout;
	p.y = node->y + 1;	// node_center()
	return p;
}


void make_edge_line(struct edge* e, struct point p1, struct point p2, int ty)
{
	int n;
	struct point* p = e->points;
	int x1 = p1.x;
	int y1 = p1.y;
	int x2 = p2.x;
	int y2 = p2.y;

	p->x = x1;
	p->y = y1;
	if(y1 == y2)
	{
		if(ty == y2)
		{
			n = 2;
		}
		else
		{
			n = 4;
			p++;
			p->x = x1;
			p->y = ty;
			p++;
			p->x = x2;
			p->y = ty;
		}
		p++;
		p->x = x2;
		p->y = y2;
	}
	else
	{
		n = 3;
		p++;
		p->x = x1;
		p->y = y2;
		p++;
		p->x = x2;
		p->y = y2;
	}
	e->points_count = n;
}


void make_edge_line_pt(struct edge* e, struct point p1, struct point p2, int ty)
{
	int n;
	struct point* p = e->points;
	int x1 = p1.x;
	int y1 = p1.y;
	int x2 = p2.x;
	int y2 = p2.y;
	
	p->x = x1;
	p->y = y1;
	if(y1 == y2)
	{
		if(!ty)
		{
			n = 2;
		}
		else
		{
			n = 4;
			p++;
			p->x = x1;
			p->y = y1 + ty;
			p++;
			p->x = x2;
			p->y = y1 + ty;
		}
		p++;
		p->x = x2;
		p->y = y2;
	}
	else
	{
		n = 3;
		if(y1 < y2)
		{
			p++;
			p->x = x1;
			p->y = y2;
		}
		else
		{
			p++;
			p->x = x2;
			p->y = y1;
		}
		p++;
		p->x = x2;
		p->y = y2;
	}
	e->points_count = n;
}

void make_edge_line_pt_v2(struct edge* e, struct point p1, struct point p2, int type)
{
	struct point* p = e->points;
	int x1 = p1.x;
	int y1 = p1.y;
	int x2 = p2.x;
	int y2 = p2.y;

	p->x = x1;
	p->y = y1;
	if(y1 != y2)
	{
		switch(type)
		{
			case CHR_TOPLEFT:
			case CHR_BOTTOMLEFT:
				p++;
				p->x = x1;
				p->y = y2;
				break;
			case CHR_TOPRIGHT:
			case CHR_BOTTOMRIGHT:
				p++;
				p->x = x2;
				p->y = y1;
				break;
			default:
				break;
		}
	}
	p++;
	p->x = x2;
	p->y = y2;
	e->points_count = (p-e->points) + 1;
}

