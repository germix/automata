///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Visualización de gramática
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "chrs.h"
#include "atmt.h"
#include "set.h"
#include "array.h"
#include "grammar.h"
#include "screen.h"

void grammar_viewer(struct grammar* grammar, int x, int y, int width, int height)
{
	int i;
	int back_color = 0;
	int rule_color = MAKE_COLOR(back_color, 0xF);
	int frame_color = MAKE_COLOR(back_color, 0x7);

	fill_rect(x, y, x+width-1, y+height-1, frame_color, ' ');
	draw_string(x+1, y+1, 0x0F, "Gram\240tica:");
	for(i = 0; i < grammar->rules_count; i++)
	{
		draw_string_len(x+5, y+3+i, rule_color, grammar->rules[i].buffer, grammar->rules[i].len);
	}
	
	draw_border(x, y, x+width-1, y+height-1, frame_color, 0, BORDER_TYPE_DOUBLE);
}
