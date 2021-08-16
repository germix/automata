///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Visualización de la cinta
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "tape.h"
#include "utils.h"
#include "screen.h"

void tape_viewer(struct tape* tape, int x, int y, int width, int height, int arrow_char, int back_color, int border_color, int tape_color, int arrow_color)
{
	int ix;
	int idx;
	int dy;
	int attr;
	
	attr = MAKE_COLOR(back_color, 0xF);
	fill_rect(x, y, x+width-1,y+height-1, back_color, ' ');
	
	dy = (height/2) - (4/2);
	y += dy;
	for(ix = x, idx = 0; ix < width; ix += 4, idx++)
	{
		attr = MAKE_COLOR(back_color, tape_color);
		//
		// Linea 1
		//
		draw_char(ix+0, y+0, attr, 194);
		draw_char(ix+1, y+0, attr, 196);
		draw_char(ix+2, y+0, attr, 196);
		draw_char(ix+3, y+0, attr, 196);
		
		//
		// Linea 2
		//
		draw_char(ix+0, y+1, attr, 179);
		draw_char(ix+1, y+1, attr, ' ');
		draw_char(ix+2, y+1, attr, tape->buffer[idx]);
		draw_char(ix+3, y+1, attr, ' ');
		
		//
		// Linea 3
		//
		draw_char(ix+0, y+2, attr, 193);
		draw_char(ix+1, y+2, attr, 196);
		draw_char(ix+2, y+2, attr, 196);
		draw_char(ix+3, y+2, attr, 196);
		
		//
		// Linea 4
		//
		attr = MAKE_COLOR(back_color, arrow_color);
		draw_char(ix+0, y+3, attr, ' ');
		draw_char(ix+1, y+3, attr, ' ');
		draw_char(ix+2, y+3, attr, (idx == tape->pos) ? arrow_char : ' ');
		draw_char(ix+3, y+3, attr, ' ');
	}
	y -= dy;

	attr = MAKE_COLOR(back_color, border_color);
	draw_border(x, y, x+width-1,y+height-1, attr, 0, BORDER_TYPE_DOUBLE);
}


