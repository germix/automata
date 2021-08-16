///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Screen
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___SCREEN_H___
#define ___SCREEN_H___
#include "borders.h"

#define MAKE_COLOR(b, f)	((b << 4) | f)

#ifdef __cplusplus
extern "C" {
#endif

int init_screen(int w, int h);
void close_screen();
void flush_screen();

void hide_cursor();
void show_cursor();
void move_cursor(int x, int y);

void draw_attr(int x, int y, int attr);
void draw_char(int x, int y, int attr, int chr);
void draw_string(int x, int y, int attr, const char* text);
void draw_string_len(int x, int y, int attr, const char* text, int len);
void draw_string_maxlen(int x, int y, int max, int len, int attr, const char* text);

void fill_rect(int x, int y, int cx, int cy, int attr, int chr);
void draw_border(int x1, int y1, int x2, int y2, int attr, int fill, int type);

void draw_horzline(int x1, int x2, int y, int attr, int chr);
void draw_vertline(int y1, int y2, int x, int attr, int chr);

int screen_width();
int screen_height();

#ifdef __cplusplus
};
#endif

#endif
