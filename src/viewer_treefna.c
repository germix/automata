///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Visualización del árbol para la representación de multiple estados corrientes de un AFN
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#pragma warning(disable:4996)
#endif
#include "atmt.h"
#include "treefna.h"
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

struct treefna_layout
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
#ifdef USE_TREEFNA_SEMI_ACCEPT
	int			color_line_semi_accept;
	int			color_label_semi_accept;
#endif
};
void treefna_calc_size(struct treefna* node, struct treefna_layout* layout)
{
	struct treefna* child;

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
		treefna_calc_size(child, layout);
	}
}

void align_x(struct treefna* node, struct treefna_layout* layout, int level)
{
	//
	// Acá hago la alineación x
	//
	struct treefna* child;

	node->x = layout->list_of_x[level];
	for(child = node->first; child != NULL; child = child->next)
	{
		align_x(child, layout, level+1);
	}
}
int treefna_calc_position(struct treefna* node, struct treefna_layout* layout, int x, int y, int level)
{
	//
	// V4: En este habría que solucionar la alineación X
	//
	int bottom = 0;
	if(!node)
	{
		return y;
	}
	node->x = x;
	layout->list_of_x[level] = max(layout->list_of_x[level], x);
	
	if(!node->first)
	{
		if(layout->list_of_bottoms[level] == 0)
		{
			node->y = y;
			layout->list_of_bottoms[level] = y + node->height + layout->node_space_y;
		}
		else
		{
#if 0//!!!
			node->y = layout->list_of_bottoms[level];
#else
			if(!(node->parent && node->parent->y > layout->list_of_bottoms[level]))
				node->y = layout->list_of_bottoms[level];
			else
				node->y = y;
#endif
			layout->list_of_bottoms[level] = node->y + node->height + layout->node_space_y;
		}
//		bottom = y + node->height;
		bottom = node->y + node->height;
	}
	else
	{
		if(layout->list_of_bottoms[level] == 0)
		{
			node->y = y;
			layout->list_of_bottoms[level] = y + node->height + layout->node_space_y;
		}
		else
		{
#if 0//!!!
			node->y = layout->list_of_bottoms[level];
#else
			if(!(node->parent && node->parent->y > layout->list_of_bottoms[level]))
				node->y = layout->list_of_bottoms[level];
			else
				node->y = y;
#endif
			layout->list_of_bottoms[level] = node->y + node->height + layout->node_space_y;
		}
		// ...
		{
			int child_x = x + node->width + layout->node_space_x;
			struct treefna* child;
			
			child = node->first;
			bottom = treefna_calc_position(child, layout, child_x, node->y, level+1);
			if(child->y > node->y)
			{
				node->y = child->y;
				layout->list_of_bottoms[level] = node->y + node->height + layout->node_space_y;
			}
			for(child = child->next; child != NULL; child = child->next)
			{
				bottom = treefna_calc_position(child, layout, child_x, bottom, level+1) + layout->node_space_y;
			}
		}
	}
	layout->bound.x1 = min(layout->bound.x1, node->x);
	layout->bound.y1 = min(layout->bound.y1, node->y);
	layout->bound.x2 = max(layout->bound.x2, node->x + node->width);
	layout->bound.y2 = max(layout->bound.y2, node->y + node->height);
	return bottom;
}

static void draw_end_line(struct treefna* node, int y, int line_xm, int line_x2, int line_color)
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

void treefna_draw_node(struct treefna* node, struct treefna_layout* layout, int dx, int dy)
{
	struct treefna* child;
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
#ifdef USE_TREEFNA_SEMI_ACCEPT
	else if(node->semi_accept)
		node_color = layout->color_label_semi_accept;
#endif
	else
		node_color = layout->color_label_normal;

	for(child = node->first; child != NULL; child = child->next)
	{
		treefna_draw_node(child, layout, dx, dy);
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
	if(node->first)	// Si al menos tiene un hijo
	{
		int line_y = node->y + dy;
		int line_len;
		int line_x1;
		int line_x2;
		int line_xm;
#ifdef USE_TREEFNA_SEMI_ACCEPT
		struct treefna* semi = 0;
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
#ifdef USE_TREEFNA_SEMI_ACCEPT
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

#ifdef USE_TREEFNA_SEMI_ACCEPT
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
#ifdef USE_TREEFNA_SEMI_ACCEPT
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
}

void treefna_viewer(struct treefna* node, int x, int y, int width, int height, int translate_x, int translate_y)
{
	int dx;
	int dy;
	struct treefna_layout layout;
	//
	// Calcular layout
	//
	{
		int depth = treefna_depth(node) + 10;	// Agrego 10 por las dudas (¿?)
		
		memset(&layout, 0, sizeof(struct treefna_layout));
		
		layout.has_border = true;
		layout.list_of_x = calloc(1, depth * sizeof(int));
		layout.list_of_bottoms = calloc(1, depth * sizeof(int));
		layout.node_space_x = 5;
		layout.node_space_y = 1;

		treefna_calc_size(node, &layout);
		treefna_calc_position(node, &layout, 0, 0, 0);
		align_x(node, &layout, 0);

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
#ifdef USE_TREEFNA_SEMI_ACCEPT
	layout.color_line_semi_accept = 0x3F;
	layout.color_label_semi_accept = 0x20;
#endif

	fill_rect(x, y, x+width-1, y+height-1, layout.color_line, ' ');
	treefna_draw_node(node, &layout, dx, dy);
	draw_border(x, y, x+width-1, y+height-1, layout.color_line, 0, BORDER_TYPE_DOUBLE);
}

