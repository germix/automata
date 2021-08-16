///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Visualización del árbol para la representación de multiple estados corrientes de un NPDA
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#pragma warning(disable:4996)
#endif
#include "atmt.h"
#include "treenpda.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <screen.h>


struct rect
{
	int		x1;
	int		y1;
	int		x2;
	int		y2;
};
#define RECT_WIDTH(r)				(r.x2 - r.x1 + 1)
#define RECT_HEIGHT(r)				(r.y2 - r.y1 + 1)

struct treenpda_layout
{
	struct rect	bound;
	bool		has_border;
	int*		list_of_x;
	int*		list_of_bottoms;
	int			node_space_x;
	int			node_space_y;
	
	//
	// Colores
	//
	int			color_line;
	int			color_label_dead;
	int			color_label_accept;
	int			color_label_normal;
#ifdef USE_treenpda_SEMI_ACCEPT
	int			color_line_semi_accept;
	int			color_label_semi_accept;
#endif
};
void treenpda_calc_size(struct treenpda* node, struct treenpda_layout* layout)
{
	struct treenpda* child;

	if(!node)
		return;
	
	node->width = strlen(node->label);
	node->height = 1;
	
	if(layout->has_border) // Incremento extra para bordes
	{
		node->width += 2;
		node->height += 2;
	}
	for(child = node->first; child != NULL; child = child->next)
	{
		treenpda_calc_size(child, layout);
	}
}

void align_y(struct treenpda* node, struct treenpda_layout* layout, int level)
{
	//
	// Acá hago la alineación y
	//
	struct treenpda* child;

	node->y = layout->list_of_x[level];
	for(child = node->first; child != NULL; child = child->next)
	{
		align_y(child, layout, level+1);
	}
}
int treenpda_calc_position(struct treenpda* node, struct treenpda_layout* layout, int x, int y, int level)
{
	//
	// V4: En este habría que solucionar la alineación X
	//
	int bottom = 0;
#ifdef CALC_POS_1
	if(!node)
		return y;
	node->x = x;
	layout->list_of_x[level] = max(layout->list_of_x[level], x);
#else
	if(!node)
		return x;
	node->y = y;
	layout->list_of_x[level] = max(layout->list_of_x[level], y);
#endif
	
	if(!node->first)
	{
		if(layout->list_of_bottoms[level] == 0)
		{
#ifdef CALC_POS_1
			node->y = y;
			layout->list_of_bottoms[level] = y + node->height + layout->node_space_y;
#else
			node->x = x;
			layout->list_of_bottoms[level] = x + node->width + layout->node_space_x;
#endif
		}
		else
		{
#ifdef CALC_POS_1
#if 0//!!!
			node->y = layout->list_of_bottoms[level];
#else
			if(!(node->parent && node->parent->y > layout->list_of_bottoms[level]))
				node->y = layout->list_of_bottoms[level];
			else
				node->y = y;
#endif
			layout->list_of_bottoms[level] = node->y + node->height + layout->node_space_y;
#else
#if 0//!!!
			node->x = layout->list_of_bottoms[level];
#else
			if(!(node->parent && node->parent->x > layout->list_of_bottoms[level]))
				node->x = layout->list_of_bottoms[level];
			else
				node->x = x;
#endif
			layout->list_of_bottoms[level] = node->x + node->width + layout->node_space_x;
#endif
		}
#ifdef CALC_POS_1
//		bottom = y + node->height;
		bottom = node->y + node->height;
#else
//		bottom = x + node->width;
		bottom = node->x + node->width;
#endif
	}
	else
	{
		if(layout->list_of_bottoms[level] == 0)
		{
#ifdef CALC_POS_1
			node->y = y;
			layout->list_of_bottoms[level] = y + node->height + layout->node_space_y;
#else
			node->x = x;
			layout->list_of_bottoms[level] = x + node->width + layout->node_space_x;
#endif
		}
		else
		{
#ifdef CALC_POS_1
#if 0//!!!
			node->y = layout->list_of_bottoms[level];
#else
			if(!(node->parent && node->parent->y > layout->list_of_bottoms[level]))
				node->y = layout->list_of_bottoms[level];
			else
				node->y = y;
#endif
			layout->list_of_bottoms[level] = node->y + node->height + layout->node_space_y;
#else
#if 0//!!!
			node->x = layout->list_of_bottoms[level];
#else
			if(!(node->parent && node->parent->x > layout->list_of_bottoms[level]))
				node->x = layout->list_of_bottoms[level];
			else
				node->x = x;
#endif
			layout->list_of_bottoms[level] = node->x + node->width + layout->node_space_x;

#endif
		}
		// ...
		{
#ifdef CALC_POS_1
			int child_x = x + node->width + layout->node_space_x;
			struct treenpda* child;
			
			child = node->first;
			bottom = treenpda_calc_position(child, layout, child_x, node->y, level+1);
			if(child->y > node->y)
			{
				node->y = child->y;
				layout->list_of_bottoms[level] = node->y + node->height + layout->node_space_y;
			}
			for(child = child->next; child != NULL; child = child->next)
			{
				bottom = treenpda_calc_position(child, layout, child_x, bottom, level+1) + layout->node_space_y;
			}
#else
			int child_y = y + node->height + layout->node_space_y;
			struct treenpda* child;
			
			child = node->first;
			bottom = treenpda_calc_position(child, layout, node->x, child_y, level+1);
			if(child->x > node->x)
			{
				node->x = child->x;
				layout->list_of_bottoms[level] = node->x + node->width + layout->node_space_x;
			}
			for(child = child->next; child != NULL; child = child->next)
			{
				bottom = treenpda_calc_position(child, layout, bottom, child_y, level+1) + layout->node_space_x;
			}
#endif
		}
	}
	layout->bound.x1 = min(layout->bound.x1, node->x);
	layout->bound.y1 = min(layout->bound.y1, node->y);
	layout->bound.x2 = max(layout->bound.x2, node->x + node->width);
	layout->bound.y2 = max(layout->bound.y2, node->y + node->height);
	return bottom;
}
static void draw_end_line(struct treenpda* node, int y, int line_xm, int line_x2, int line_color)
{
	if((line_x2-line_xm) > 1) draw_char(line_x2, y, line_color, 26);
	if((line_x2-line_xm) > 2)
	{
		int sym;
		sym = node->symbol;
		if(sym == CH_EPSILON)
			sym = 136;	// ê
		draw_char(line_x2-1, y, line_color, sym);
	}
}

void treenpda_draw_node(struct treenpda* node, struct treenpda_layout* layout, int dx, int dy)
{
	struct treenpda* child;
	int line_color;
	int node_color;
	
	//
	// Seleccionar colores
	//
	line_color = layout->color_line;
	if(node->dead)
		node_color = layout->color_label_dead;
	else if(node->accept)
		node_color = layout->color_label_accept;
#ifdef USE_treenpda_SEMI_ACCEPT
	else if(node->semi_accept)
		node_color = layout->color_label_semi_accept;
#endif
	else
		node_color = layout->color_label_normal;

	for(child = node->first; child != NULL; child = child->next)
	{
		treenpda_draw_node(child, layout, dx, dy);
	}

	//
	// Dibujar borde
	//
	if(layout->has_border)
	{
		int x;
		int y;
		int x1 = node->x+dx;
		int y1 = node->y+dy;
		int x2 = x1+node->width-1;
		int y2 = y1+node->height-1;
		int border_color = node_color;

		for(x = x1; x < x2; x++)
		{
			draw_char(x, y1, border_color, CHR_HORZ);
			draw_char(x, y2, border_color, CHR_HORZ);
		}
		for(y = y1; y < y2; y++)
		{
			draw_char(x1, y, border_color, CHR_VERT);
			draw_char(x2, y, border_color, CHR_VERT);
		}
		draw_char(x1, y1, border_color, CHR_TOPLEFT);
		draw_char(x2, y1, border_color, CHR_TOPRIGHT);
		draw_char(x1, y2, border_color, CHR_BOTTOMLEFT);
		draw_char(x2, y2, border_color, CHR_BOTTOMRIGHT);
	}
	{
		int x = node->x+dx;
		int y = node->y+dy;
		if(layout->has_border)
		{
			x++;
			y++;
		}
		draw_string(x, y, node_color, node->label);
	}
	//
	// Dibujar lineas
	//
#ifdef CALC_POS_1
	if(node->first)	// Si al menos tiene un hijo
	{
		int line_y = node->y + dy;
		int line_len;
		int line_x1;
		int line_x2;
		int line_xm;
#ifdef USE_treenpda_SEMI_ACCEPT
		struct treenpda* semi = 0;
#endif

		line_y = (node->y+dy) + node->height/2;
		line_x1 = (node->x+dx) + node->width;
		line_x2 = (node->first->x+dx) - 1;
		line_xm = (line_x1+1);
		line_len = node->first->x - (node->x + node->width);

		//
		// Dibujar línea vertical
		//
		if(node->first->next)	// Sólo si la cantidad de hijos > 1
		{
#ifdef USE_treenpda_SEMI_ACCEPT
			for(child = node->first; child != NULL && semi == NULL; child = child->next)
			{
				if(child->semi_accept)
					semi = child;
			}
			if(semi)
			{
				int y;
				int y1 = (node->first->y+dy)+(node->first->height/2);
				int y2 = (semi->y +dy)+(semi->height /2);

				line_color = layout->color_line_semi_accept;
				for(y = y1; y < y2; y++)
					draw_char(line_xm, y, line_color, CHR_VERT);

				line_color = layout->color_line;
				y1 = y2;
				y2 = (node->last->y +dy)+(node->last->height /2);
				for(y = y1; y < y2; y++)
					draw_char(line_xm, y, line_color, CHR_VERT);
			}
			else
			{
				int y;
				int y1 = (node->first->y+dy)+(node->first->height/2);
				int y2 = (node->last->y +dy)+(node->last->height /2);
				for(y = y1; y < y2; y++)
					draw_char(line_xm, y, line_color, CHR_VERT);
			}
#else
			int y;
			int y1 = (node->first->y+dy)+(node->first->height/2);
			int y2 = (node->last->y +dy)+(node->last->height /2);
			for(y = y1; y < y2; y++)
				draw_char(line_xm, y, line_color, CHR_VERT);
			
			
		//	draw_char(line_xm, line_y, line_color, CHR_TOPVERT);
#endif
		}
		//
		// Dibujar líneas horizontales de conexión
		//
		for(child = node->first; child != NULL; child = child->next)
		{
			int x;
			int y;
			int chr;

#ifdef USE_treenpda_SEMI_ACCEPT
			if(child->semi_accept || child->accept)
				line_color = layout->color_line_semi_accept;
			else
				line_color = layout->color_line;
#endif

			y = (child->y+dy)+(child->height/2);
			if(child == node->first)
			{
				x = line_x1;
				if(!child->next)
					chr = CHR_HORZ;
				else
					chr = CHR_TOPVERT;
			}
			else
			{
				x = line_xm;
				if(child != node->last)
					chr = CHR_LEFTHORZ;
				else
					chr = CHR_BOTTOMLEFT;
			}
			for( ; x < line_x2; x++)
			{
				draw_char(x, y, line_color, CHR_HORZ);
			}
			draw_char(line_xm, y, line_color, chr);
			//
			// Dibujar flecha
			//
			if((line_x2-line_xm) > 1)
				draw_char(line_x2, y, line_color, 26);
			//
			// Dibujar símbolo de la transición
			//
			if((line_x2-line_xm) > 2)
			{
				int sym;
				sym = child->symbol;
				if(sym == CH_EPSILON)
					sym = 136;	// ê
				draw_char(line_x2-1, y, line_color, sym);
			}
		}
#ifdef USE_treenpda_SEMI_ACCEPT
		if(semi && semi != semi->first)
		{
			int x;
			int y = node->y + 1 + dy;

			line_color = layout->color_line_semi_accept;
			for(x = line_x1; x < line_xm; x++)
			{
				draw_char(x, y, line_color, CHR_HORZ);
			}
			draw_char(line_xm, y, line_color, CHR_TOPVERT);
		}
#endif
	}
#else
	if(node->first)	// Si al menos tiene un hijo
	{
		int line_x = node->x + dx;
		int line_len;
		int line_y1;
		int line_y2;
		int line_ym;

		line_x = (node->x+dx) + node->width/2;
		line_y1 = (node->y+dy) + node->height;
		line_y2 = (node->first->y+dy) - 1;
		line_ym = (line_y1+1);
		line_len = node->first->y - (node->y + node->height);

		//
		// Dibujar línea horizontal
		//
		if(node->first->next)	// Sólo si la cantidad de hijos > 1
		{
			int x;
			int x1 = (node->first->x+dx)+(node->first->width/2);
			int x2 = (node->last->x +dx)+(node->last->width /2);
			for(x = x1; x < x2; x++)
				draw_char(x, line_ym, line_color, CHR_HORZ);
			
		}
		//
		// Dibujar líneas verticales de conexión
		//
		for(child = node->first; child != NULL; child = child->next)
		{
			int x;
			int y;
			int chr;

			x = (child->x+dx)+(child->width/2);
			if(child == node->first)
			{
				y = line_y1;
				if(!child->next)
					chr = CHR_VERT;
				else
					chr = CHR_LEFTHORZ;
			}
			else
			{
				y = line_ym;
				if(child != node->last)
					chr = CHR_TOPVERT;
				else
					chr = CHR_TOPRIGHT;
			}
			for( ; y < line_y2; y++)
			{
				draw_char(x, y, line_color, CHR_VERT);
			}
			draw_char(x, line_ym, line_color, chr);
			//
			// Dibujar flecha
			//
			if((line_y2-line_ym) > 1)
				draw_char(x, line_y2, line_color, CHR_ARROW_DOWN);
			//
			// Dibujar símbolo de la transición
			//
			if((line_y2-line_ym) > 2)
			{
				{
					void syms_to_str(int* syms, int count, char* buf);
				
					static char sz[256];
					static char sz_in[156];
					static char sz_pop[156];
					static char sz_push[156];
					
					syms_to_str(&child->symbol, 1, sz_in);
					syms_to_str(&child->pop, 1, sz_pop);
					if(child->push[0] == CH_EPSILON)
					{sz_push[0] = CH_EPSILON;sz_push[1] = 0; }
					else
					{
						syms_to_str(child->push, MAX_PUSHABLE_CHARS, sz_push);
					}
					sprintf(sz, "%s, %s/%s", sz_in, sz_pop, sz_push);
					draw_string(x-2, line_y2-1, line_color, sz);
				}
			}
		}
	}
#endif
}

void treenpda_viewer(struct treenpda* node, int x, int y, int width, int height, int translate_x, int translate_y)
{
	int dx;
	int dy;
	struct treenpda_layout layout;
	//
	// Calcular layout
	//
	{
		int depth = treenpda_depth(node) + 10;	// Agrego 10 por las dudas (¿?)
		
		memset(&layout, 0, sizeof(struct treenpda_layout));
		
		layout.has_border = true;
		layout.list_of_x = calloc(1, depth * sizeof(int));
		layout.list_of_bottoms = calloc(1, depth * sizeof(int));
#ifdef CALC_POS_1
		layout.node_space_x = 5;
		layout.node_space_y = 1;
#else
		layout.node_space_x = 1;
		layout.node_space_y = 5;
#endif

		treenpda_calc_size(node, &layout);
		treenpda_calc_position(node, &layout, 0, 0, 0);
		align_y(node, &layout, 0);

		free(layout.list_of_x);
		free(layout.list_of_bottoms);
	}
	//
	// Visualizar
	//
	dx = x+1+(width-2)/2 - RECT_WIDTH(layout.bound)/2;
#if 1
	dy = y+3;
#else
	dy = y+1+(height-2)/2 - RECT_HEIGHT(layout.bound)/2;
#endif
	dx += translate_x;
	dy += translate_y;
	
	layout.color_line = 0x30;
	layout.color_label_dead = 0x4E;
	layout.color_label_accept = 0x2E;
	layout.color_label_normal = 0x30;
#ifdef USE_treenpda_SEMI_ACCEPT
	layout.color_line_semi_accept = 0x3F;
	layout.color_label_semi_accept = 0x20;
#endif

	fill_rect(x, y, x+width-1, y+height-1, layout.color_line, ' ');
	treenpda_draw_node(node, &layout, dx, dy);
	draw_border(x, y, x+width-1, y+height-1, layout.color_line, 0, BORDER_TYPE_DOUBLE);
}

