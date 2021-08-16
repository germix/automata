///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Visualización de la pila del automata a pila
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stack.h"
#include "utils.h"
#include "screen.h"

void stack_viewer(struct stack* stack, int x, int y, int width, int height, int back_color, int frame_color, int stack_color)
{
	int i;
	int cnt;
	int attr;
	int arrow_color;

	back_color = 3;
	frame_color = 0;
	stack_color = 0xE;
	arrow_color = 0xA;

	attr = MAKE_COLOR(back_color, frame_color);

	draw_border(x, y, x+width-1, y+height-1, attr, ' ', BORDER_TYPE_DOUBLE);
#if 0//!!!
	x += 1;
	y += 4;
	draw_string(x, y, attr, "PILA:");
	attr = MAKE_COLOR(back_color, stack_color);
	for(i = 0, cnt = stack_size(stack); i < cnt; i++)
	{
		draw_char(x+5+i, y, attr, stack->buf[i]);
	}
#else
	draw_string(x+1, y+1, attr, "PILA:");
	attr = MAKE_COLOR(back_color, stack_color);

	y = y+height-2;
	for(i = 0, cnt = stack_size(stack); i < cnt; i++)
	{
		draw_char(x + width/2, y-i - 1, attr, stack->buf[i]);
	}
	attr = MAKE_COLOR(back_color, arrow_color);
	draw_char(x + width/2-2, y-stack_size(stack), attr, 26);
#endif
}
