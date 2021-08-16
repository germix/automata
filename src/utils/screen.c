///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Screen
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "screen.h"
#include <windows.h>

#ifndef MIN
#define MIN(a,b)  (((a)<(b))?(a):(b))
#endif

typedef struct SCREEN
{
	int			Width;
	int			Height;
	HANDLE		hStdout;
	HANDLE		hConsole;
	CHAR_INFO*	chInfo;
}SCREEN;

static SCREEN Screen;


int init_screen(int w, int h)
{
	SCREEN* s = &Screen;
	
	// 
	// Obtener la salida estandard y crear un nuevo buffer de pantalla
	// 
	s->hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	s->hConsole = CreateConsoleScreenBuffer(
					GENERIC_READ | GENERIC_WRITE,
					0,
					NULL,
					CONSOLE_TEXTMODE_BUFFER,
					NULL);
	
	if(s->hStdout == INVALID_HANDLE_VALUE || s->hConsole == INVALID_HANDLE_VALUE)
	{
//		printf("CreateConsoleScreenBuffer (%d)\n", GetLastError());
		return -1;
	}
	// 
	// Establecer el nuevo buffer de pantalla como activo
	// 
	if(!SetConsoleActiveScreenBuffer(s->hConsole))
	{
//		printf("SetConsoleActiveScreenBuffer (%d)\n", GetLastError());
		CloseHandle(s->hConsole);
		return -1;
	}
	s->Width = w;
	s->Height = h;
	s->chInfo = (CHAR_INFO*)malloc(sizeof(CHAR_INFO) * (w*h));
#if 1	//!!!
	{
		CONSOLE_SCREEN_BUFFER_INFOEX iex;

		iex.cbSize = sizeof(iex);
		GetConsoleScreenBufferInfoEx(s->hConsole, &iex);
		iex.bFullscreenSupported = 1;
		iex.dwSize.X = s->Width;
		iex.dwSize.Y = s->Height;
		iex.srWindow.Left   = 0;
		iex.srWindow.Top    = 0;
		iex.srWindow.Right  = s->Width;
		iex.srWindow.Bottom = s->Height;
		iex.dwMaximumWindowSize.X = s->Width;
		iex.dwMaximumWindowSize.Y = s->Height;
		SetConsoleScreenBufferInfoEx(s->hConsole, &iex);
	}
#endif
	hide_cursor();
	return 0;
}
void close_screen()
{
	SCREEN* s = &Screen;
	
	if(s->hConsole)
	{
		CloseHandle(s->hConsole);
	}
	free(s->chInfo);
}
void flush_screen()
{
	SCREEN* s = &Screen;
	int x1= 0;
	int y1 = 0;
	int x2 = s->Width;
	int y2 = s->Height;
	// ...
	COORD BufSize;
	COORD BufCoord;
	SMALL_RECT WriteRect;

	BufSize.X = s->Width;
	BufSize.Y = s->Height;
	
	BufCoord.X = x1;
	BufCoord.Y = y1;

	WriteRect.Left		 = x1;
	WriteRect.Top		 = y1;
	WriteRect.Right		 = x2;
	WriteRect.Bottom	 = y2;
	
	WriteConsoleOutput(
					s->hConsole,
					s->chInfo,
					BufSize,
					BufCoord,
					&WriteRect);
}
void hide_cursor()
{
	CONSOLE_CURSOR_INFO info;
	GetConsoleCursorInfo(Screen.hConsole, &info);
	info.bVisible = 0;
	SetConsoleCursorInfo(Screen.hConsole, &info);
}
void show_cursor()
{
	CONSOLE_CURSOR_INFO info;
	GetConsoleCursorInfo(Screen.hConsole, &info);
	info.bVisible = 1;
	SetConsoleCursorInfo(Screen.hConsole, &info);
}
void move_cursor(int x, int y)
{
	COORD c;
	c.X = x;
	c.Y = y;
	SetConsoleCursorPosition(Screen.hConsole, c);
}
void draw_attr(int x, int y, int attr)
{
	SCREEN* s = &Screen;
	// ...
	CHAR_INFO* chi;

	if((x >= 0 && x < s->Width) && (y >= 0 && y < s->Height))
	{
		chi = &s->chInfo[(y*s->Width) + x];
		chi->Attributes = attr;
	}
}
void draw_char(int x, int y, int attr, int chr)
{
	SCREEN* s = &Screen;
	// ...
	CHAR_INFO* chi;

	if((x >= 0 && x < s->Width) && (y >= 0 && y < s->Height))
	{
		chi = &s->chInfo[(y*s->Width) + x];
		chi->Attributes = attr;
		chi->Char.AsciiChar = chr;
	}
}
void draw_string(int x, int y, int attr, const char* text)
{
	draw_string_len(x, y, attr, text, strlen(text));
}
void draw_string_len(int x, int y, int attr, const char* text, int len)
{
	SCREEN* s = &Screen;
	// ...
	int i;
	CHAR_INFO* chi;

	if((x >= 0 && x < s->Width) && (y >= 0 && y < s->Height))
	{
		len = MIN(s->Width-x, len);
		if(len == 0)
			return;
		
		chi = &s->chInfo[(y*s->Width) + x];
		for(i = 0; i < len; i++, chi++)
		{
			chi->Attributes = attr;
			chi->Char.AsciiChar = text[i];
		}
	}
}
void draw_string_maxlen(int x, int y, int max, int len, int attr, const char* text)
{
SCREEN* s = &Screen;
	// ...
	int i;
	CHAR_INFO* chi;
	
	if(max == 0)
		return;
	
	if((x >= 0 && x < s->Width) && (y >= 0 && y < s->Height))
	{
		if(len > max)
			len = max;

		chi = &s->chInfo[(y*s->Width) + x];
		for(i = 0; i < len; i++, chi++)
		{
			chi->Attributes = attr;
			chi->Char.AsciiChar = text[i];
		}
		for(i = len; i < max; i++, chi++)
		{
			chi->Attributes = attr;
			chi->Char.AsciiChar = ' ';
		}
		if(len < max)
			len = max;
	}
}

void draw_border_pattern(struct border* b)
{
	int i;
	int x = 50;
	for(i = 0; i < 11; i++)
	{
		draw_char(x, i*2, 0x35, ((int*)b)[i]);
	}
}

void fill_rect(int x, int y, int cx, int cy, int attr, int chr)
{
	SCREEN* s = &Screen;

	if((x >= 0 && x < s->Width) && (y >= 0 && y < s->Height))
	{
		int x1, x2;
		int y1, y2;
		CHAR_INFO* chi;

		x1 = x;
		y1 = y;
		x2 = MIN(x+cx, s->Width);
		y2 = MIN(y+cy, s->Height);
		for(y = y1; y < y2; y++)
		{
			chi = &s->chInfo[(y * s->Width) + x1];
			for(x = x1; x < x2; x++, chi++)
			{
				chi->Attributes = attr;
				chi->Char.AsciiChar = chr;
			}
		}
	}
}

void draw_border(int x1, int y1, int x2, int y2, int attr, int fill, int type)
{
	if(type >= 0 && type < MAX_BORDER_TYPE)
	{
		int x;
		int y;
		struct border* b = &borders[type];
		
		for(y = y1; y < y2; y++)
		{
			draw_char(x1, y, attr, b->v);
			draw_char(x2, y, attr, b->v);
		}
		for(x = x1; x < x2; x++)
		{
			draw_char(x, y1, attr, b->h);
			draw_char(x, y2, attr, b->h);
		}
		draw_char(x1, y1, attr, b->e1);
		draw_char(x2, y1, attr, b->e2);
		draw_char(x1, y2, attr, b->e3);
		draw_char(x2, y2, attr, b->e4);
		if(fill)
		{
			fill_rect(x1+1, y1+1, x2-x1-1, y2-y1-1, attr, fill);
		}
	}
}

void draw_horzline(int x1, int x2, int y, int attr, int chr)
{
	int x;
	if(!(x1 < x2))
	{
		x = x1;
		x1 = x2;
		x2 = x;
	}
	for(x = x1; x <= x2; x++)
	{
		draw_char(x, y, attr, chr);
	}
}
void draw_vertline(int y1, int y2, int x, int attr, int chr)
{
	int y;
	if(!(y1 < y2))
	{
		y = y1;
		y1 = y2;
		y2 = y;
	}
	for(y = y1; y <= y2; y++)
	{
		draw_char(x, y, attr, chr);
	}
}

int screen_width()
{
	return Screen.Width;
}
int screen_height()
{
	return Screen.Height;
}

///////////////////////////////////////////////////////////////////////////////////////////////////


