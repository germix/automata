///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Caja de entrada de texto
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "keybd.h"
#include "screen.h"
#include <ctype.h>
#include <string.h>
#include <stdbool.h>

bool text_entry(int x, int y, int width, bool arrows, int attr, char* text, int max)
{
	int key;
	int chr;
	int cx;
	int pos = 0;
	int len = 0;
	int cur_x = 0;
	int idx_x = 0;
	bool exit = false;

	cx = width;
	
	if(!(cx >= 4))		// 4 como minimo, 2 para flechas, un caracter y un espacio
		return false;
	if(arrows)
	{
		x++;
		cx -= 2;
	}
	pos = len = strlen(text);
	show_cursor();
	while(!exit)
	{
		// 
		// Ajustar el indice dentro del texto
		// 
		if(pos < idx_x)								// Cuando se mueve hacia la izquierda
			idx_x -= (idx_x - pos);
		else if((pos-(cx-1)) > idx_x)				// Cuando se mueve hacia la derecha
			idx_x += ((pos-(cx-1)) - idx_x);
		
		if((idx_x > 0) && ((len - idx_x+1) < cx))	// Util cuando se borra
		{
			idx_x -= (cx - (len - idx_x+1));
		}
		// 
		// Ajustar la posicion del cursor en el editor
		// 
		cur_x = pos - idx_x;
		
		move_cursor(x+cur_x, y);

		draw_string_maxlen(x, y, cx, len, attr, &text[idx_x]);
		
		if(arrows)
		{
			draw_char(x-1, y, attr, (idx_x == 0) ? ' ' : CHAR_ARROW_LEFT);
			draw_char(x+cx, y, attr, ((len - idx_x) <= cx) ? ' ' : CHAR_ARROW_RIGHT);
		}
		flush_screen();

		switch(key = getkey())
		{
			case KEY_ESC:
			case KEY_ENTER:
				exit = true;
				break;
			case KEY_END:
				pos = len;
				break;
			case KEY_HOME:
				pos = 0;
				break;
			case KEY_ARROW_LEFT:
				if(pos > 0)
					pos--;
				break;
			case KEY_ARROW_RIGHT:
				if(pos < len)
					pos++;
				break;
			case KEY_BACKSPACE:
				if(pos > 0)
				{
					memmove(&text[pos-1], &text[pos], len-pos);
					pos--;
					text[--len] = 0;
				}
				break;
			default:
				chr = key;
				if(len < max && chr < 256 && isprint(chr))
				{
					memmove(&text[pos+1], &text[pos], len-pos + 1);
					text[pos++] = chr;
					text[++len] = 0;
				}
				break;
		}
	}
	hide_cursor();
	text[len] = 0;
	return (key == KEY_ENTER);
}

