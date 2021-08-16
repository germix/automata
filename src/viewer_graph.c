///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Visualización del grafo
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "utils.h"
#include "graph.h"
#include "screen.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>


//#define TESTING_COLOR


void draw_node(int x, int y, int width, int height, const char* label, bool has_border, bool double_border, int node_color, int border_color)
{
	//
	// Dibujar borde
	//
	if(has_border)
	{
		draw_border(x, y, x+width-1, y+height-1, border_color, 0, (double_border) ? BORDER_TYPE_DOUBLE : BORDER_TYPE_SINGLE);
	}
	//
	// Dibujar etiqueta
	//
	if(has_border)
	{
		x++;
		y++;
	}
	draw_string(x, y, node_color, label);
}
void draw_edgelabel(int x1, int x2, int y, struct edge* e, int label_color, int epsilon_color)
{
	int w = ((x1 < x2) ? (x2-x1+1) : (x1-x2+1))-2;
	int len = e->label.len;
	int x = ((x1 < x2) ? x1 : x2) + ((w/2) - (len/2));
	
	draw_string(x + 1, y, (((unsigned char)e->label.buffer[0] == 136) ? epsilon_color : label_color), e->label.buffer);
}
void draw_edge(struct edge* e, int dx, int dy, int line_color, int label_color, int epsilon_color)
{
	int i;
	struct point* p1;
	struct point* p2;
	
	for(i = 0; i < (e->points_count-1); i++)
	{
		p1 = &e->points[i];
		p2 = &e->points[i+1];
		
		if(p1->y == p2->y)
		{
			if(p1->x != p2->x)
			{
				draw_horzline(p1->x + dx, p2->x + dx, p1->y + dy, line_color, CHR_HORZ);
				//
				// Draw label
				//
				if(p1->y <= e->points[0].y)
					draw_edgelabel(p1->x + dx, p2->x + dx, p1->y + dy - 1, e, label_color, epsilon_color);
				else
					draw_edgelabel(p1->x + dx, p2->x + dx, p1->y + dy + 1, e, label_color, epsilon_color);
			}
		}
		else
		{
			if(p1->x == p2->x)
			{
				draw_vertline(p1->y + dy, p2->y + dy, p1->x + dx, line_color, CHR_VERT);
			}
		}
	}
	if(e->points_count == 2)
	{
	}
	else if(e->points_count == 3)
	{
		struct point* p1 = &e->points[0];
		struct point* p2 = &e->points[1];
		struct point* p3 = &e->points[2];
		struct point* p4 = &e->points[3];
		
		if(p1->x == p2->x)
		{
			if(p1->y > p2->y)
				draw_char(p2->x+dx, p2->y+dy, line_color, CHR_TOPLEFT);
			else
				draw_char(p2->x+dx, p2->y+dy, line_color, CHR_BOTTOMLEFT);
		}
		else
		{
			draw_char(p1->x+dx, p1->y+dy, line_color, CHR_HORZ);
			if(p2->y < p3->y)
				draw_char(p2->x+dx, p2->y+dy, line_color, CHR_TOPRIGHT);
			else
				draw_char(p2->x+dx, p2->y+dy, line_color, CHR_BOTTOMRIGHT);
		}
	}
	else if(e->points_count == 4)
	{
	}
}
static void draw_in_intersection(struct node* n, int dx, int dy, int line_color)
{
	int chr;
	bool inf = false;
	bool sup = false;
	bool equal = false;
	struct point inpoint = node_inpoint(n);
	
	{
		int i;
		int center = node_center(n);
		
		for(i = 0; i < n->inedges_count; i++)
		{
			struct edge* e = n->inedges[i];
			
			if(e->points[1].y < center)
				sup = true;
			else if(e->points[1].y > center)
				inf = true;
			else if(e->points[1].y == center)
				equal = true;
		}
	}
	if(equal)
	{
			 if(sup && inf)		chr = CHR_CROSS;
		else if(!sup && inf)	chr = CHR_TOPVERT;
		else if(sup && !inf)	chr = CHR_BOTTOMVERT;
		else					chr = CHR_HORZ;
	}
	else
	{
			 if(sup && inf)		chr = CHR_LEFTHORZ;
		else if(!sup && inf)	chr = CHR_TOPLEFT;
	//	else if(sup && !inf)	chr = (n->initial) ? CHR_BOTTOMVERT : 0;
		else if(sup && !inf)	chr = (n->initial) ? CHR_BOTTOMVERT : CHR_BOTTOMLEFT;
		else					chr = (n->initial) ? CHR_HORZ : 0;
	}
	draw_char(inpoint.x+dx, inpoint.y+dy, line_color, chr);
}
static void draw_out_intersection(struct node* n, int dx, int dy, int line_color)
{
	int x;
	int chr;
	bool inf = false;
	bool sup = false;
	bool equal = false;
	struct point outpoint = node_outpoint(n);

	//
	// ...
	//
	for(x = n->x + n->width; x < outpoint.x; x++)
	{
#ifdef TESTING_COLOR
		draw_char(x + dx, n->y + 1 + dy, 0xCF, CHR_HORZ);
#else
		draw_char(x + dx, n->y + 1 + dy, line_color, CHR_HORZ);
#endif
	}
	{
		int i;
		int center = node_center(n);
		
		for(i = 0; i < n->outedges_count; i++)
		{
			struct edge* e = n->outedges[i];
			
			if(e->points[1].y < center)
				sup = true;
			else if(e->points[1].y > center)
				inf = true;
			else if(e->points[1].y == center)
				equal = true;
		}
	}
	if(equal)
	{
			 if(sup && inf)		chr = CHR_CROSS;
		else if(!sup && inf)	chr = CHR_TOPVERT;
		else if(sup && !inf)	chr = CHR_BOTTOMVERT;
		else					chr = CHR_HORZ;
	}
	else
	{
			 if(sup && inf)		chr = CHR_RIGHTHORZ;
		else if(!sup && inf)	chr = CHR_TOPRIGHT;
		else if(sup && !inf)	chr = CHR_BOTTOMRIGHT;
		else					chr = 0;
	}
	draw_char(outpoint.x+dx, outpoint.y+dy, line_color, chr);
}
//
// Comprueba si 'y' tiene superior e inferior, y si 'y' es está en algún punto
//
// ACLARACIÓN: El nombre de esta función puede no ser correcta
//
void check_edge_bounds(struct edge** edges, int count, int y, bool* inf, bool* sup, bool* equal)
{
	int i;
	struct edge* e;
	
	*sup = false;
	*inf = false;
	*equal = false;
	for(i = 0; i < count; i++)
	{
		e = edges[i];
		
		if(e->points[1].y < y)
			*sup = true;
		else if(e->points[1].y > y)
			*inf = true;
		else if(e->points[1].y == y)
			*equal = true;
	}
}
static void draw_vertex_intersection(struct node* n, int dx, int dy, int line_color)
{
	int i;
	struct edge* e;

	for(i = 0; i < n->outedges_count; i++)
	{
		int xin;
		int xout;
		bool inner_inf;
		bool inner_sup;
		bool inner_equal;
		bool outer_inf;
		bool outer_sup;
		bool outer_equal;
		int chr1 = 0;
		int chr2 = 0;
		int color1 = 0;
		int color2 = 0;
		
		e = n->outedges[i];
		if(e->ybase == e->yline)
			continue;
		xin = node_inpoint(e->r).x;
		xout = node_outpoint(e->q).x;
		check_edge_bounds(e->r->inedges, e->r->inedges_count, e->yline, &inner_inf, &inner_sup, &inner_equal);
		check_edge_bounds(e->q->outedges, e->q->outedges_count, e->yline, &outer_inf, &outer_sup, &outer_equal);
		
		if(xout > xin)
		{
			if(e->points_count == 3)
			{
			}
			else
			{
				chr1 = (inner_sup ? CHR_LEFTHORZ : CHR_TOPLEFT);
				color1 = 0x3F;
				
				chr2 = (outer_sup ? CHR_RIGHTHORZ : CHR_TOPRIGHT);
				color2 = 0x4F;
			}
		}
		else
		{
			if(e->points_count == 3)
			{
			}
			else
			{
				chr1 = (outer_inf ? CHR_LEFTHORZ : CHR_BOTTOMLEFT);
				color1 = 0x1F;
				
				chr2 = (inner_inf ? CHR_RIGHTHORZ : CHR_BOTTOMRIGHT);
				color2 = 0x2F;
			}
		}
#ifdef TESTING_COLOR
#else
		color1 = line_color;
		color2 = line_color;
#endif
		if(xout < xin)
		{
			if(chr1)
				draw_char(xout + dx, e->yline + dy, color1, chr1);
			if(chr2)
				draw_char(xin + dx, e->yline + dy, color2, chr2);
		}
		else
		{
			if(chr2)
				draw_char(xout + dx, e->yline + dy, color1, chr2);
			if(chr1)
				draw_char(xin + dx, e->yline + dy, color2, chr1);
		}
	}
}


void graph_viewer(struct graph* graph, int x, int y, int width, int height, int translate_x, int translate_y)
{
	int i;
	int dx, dy;
	int back_color = 0;
	int line_color = MAKE_COLOR(back_color, 0xF);
	int node_color = 0x37;
	int label_color = MAKE_COLOR(back_color, 0xE);
	int frame_color = MAKE_COLOR(back_color, 0x7);
	int epsilon_color = MAKE_COLOR(back_color, 0xD);

	fill_rect(x, y, x+width-1, y+height-1, frame_color, ' ');
	
	if(!graph)
		return;
	if(!graph->nodes_count)
	{
		return;
	}
	//
	// Calcular desplazamiento
	//
	dx = translate_x + (((width-2)/2) - (graph->width/2));
	dy = translate_y + (((height-2)/2) - (graph->height/2));
	
	//
	// Dibujar aristas
	//
	for(i = 0; i < graph->edges_count; i++)
	{
		draw_edge(&graph->edges[i], dx, dy, line_color, label_color, epsilon_color);
	}
	//
	// Dibujar nodos, e intersección y codos de aristas
	//
	for(i = 0; i < graph->nodes_count; i++)
	{
		struct node* n = &graph->nodes[i];
		struct point inpoint = node_inpoint(n);
		
		//
		// Dibujar nodo
		//
		draw_node(n->x + dx, n->y + dy, n->width, n->height, n->label.buffer, true, n->accept, node_color, node_color);
		
		//
		// Dibujar flecha de entrada
		//
		if(n->inedges_count > 0 || n->initial)
		{
			if(n->initial)
			{
				draw_char(inpoint.x+dx+1, inpoint.y+dy, line_color, 16);
				draw_char(inpoint.x+dx-1, inpoint.y+dy, line_color, CHR_HORZ);
			}
			else
				draw_char(inpoint.x+dx+1, inpoint.y+dy, line_color, CHR_ARROW_RIGHT);
		}
		//
		// Dibujar intersección de aristas de entrada
		//
	//	if(n->inedges_count)
		{
#ifdef TESTING_COLOR
			draw_in_intersection(n, dx, dy, 0xAF);
#else
			draw_in_intersection(n, dx, dy, line_color);
#endif
		}
		//
		// Dibujar intersección de aristas de salida
		//
		if(n->outedges_count)
		{
#ifdef TESTING_COLOR
			draw_out_intersection(n, dx, dy, 0xCF);
#else
			draw_out_intersection(n, dx, dy, line_color);
#endif
		}
		//
		// Dibujar los codos de las aristas que pasan por arriba y por abajo
		//
		if(n->outedges_count)
		{
			draw_vertex_intersection(n, dx, dy, line_color);
		}
	}
//	draw_border(x+dx-1, y+dy-1, x+dx+graph->width, y+dy+graph->height, frame_color, 0, BORDER_TYPE_DOUBLE);

	draw_border(x, y, x+width-1, y+height-1, frame_color, 0, BORDER_TYPE_DOUBLE);
}

