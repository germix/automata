///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Ventana de mensaje
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "utils.h"
#include "screen.h"

void show_message(const char* message, int color)
{
	struct line
	{
		int pos;
		int len;
		const char* str;
	};
	struct line lines_array[5];
	int         lines_count;
	int x;
	int y;
	int width;
	int height;

	//
	// Calcular posicionamiento, largo e inicio de cada linea
	//
	{
		int i = 0;
		bool quit = false;
		struct line* line;
		const char* s = (const char*)message;
		const char* p = (const char*)message;
		int w = screen_width();

		while(!quit)
		{
			if(*p == '\n' || *p == '\0')
			{
				line = &lines_array[i];
				
				if(*s == '\n')
				{
					line->str = 0;
					line->len = 0;
					line->pos = 0;
				}
				else
				{
					line->str = s;
					line->len = p-s;
					line->pos = (w/2)-(line->len/2);
				}
				i++;
				s = p+1;
			}
			if(*p == '\0')
				quit = true;
			p++;
		}
		lines_count = i;
	}
	//
	// Calcular tamaño de ventana
	//
	{
		int w = 0;
		int h = lines_count;
		int i;
		struct line* line;

		for(i = 0; i < lines_count; i++)
		{
			line = &lines_array[i];
			w = max(w, line->len);
		}
		w += 2+4;
		h += 2+4;
		for(i = 0; i < lines_count; i++)
		{
			line = &lines_array[i];
			if(line->len)
				line->pos = (w/2)-(line->len/2);
		}
		x = (screen_width()/2)-(w/2);
		y = (screen_height()/2)-(h/2);
		width = w;
		height = h;
	}
	//
	// Mostrar
	//
	{
		int i;
		draw_border(x, y, x+width-1, y+height-1, color, ' ', BORDER_TYPE_DOUBLE);
		
		y = y + (height/2)-(lines_count/2);
		for(i = 0; i < lines_count; i++)
		{
			struct line* line = &lines_array[i];
			if(line->str)
			{
				draw_string_len(x + line->pos, y + i, color, line->str, line->len);
			}
		}
	}
}
