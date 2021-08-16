///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Buffer de un string
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#pragma warning(disable:4996)
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buf.h"
#include "buf_string.h"

struct buffer_string
{
	struct buffer	base;
	// ...
	int				pos;
	int				max;
	struct string	string;
};

void buffer_string_free(struct buffer_string* buf);
void buffer_string_reset(struct buffer_string* buf);
int  buffer_string_getc(struct buffer_string* buf);
int  buffer_string_peekc(struct buffer_string* buf);

void buffer_string_free(struct buffer_string* buf)
{
	if(buf)
	{
		free(buf);
	}
}
void buffer_string_incr(struct buffer_string* buf)
{
	buf->pos++;
}
int  buffer_string_getc(struct buffer_string* buf)
{
	int ch = buffer_string_peekc(buf);
	buf->pos++;
	return ch;
}
int  buffer_string_peekc(struct buffer_string* buf)
{
	if(buf->pos >= buf->string.len)
	{
		buf->pos = buf->string.len;		// Posición del caracter nulo
		return EOF;
	}
	return buf->string.buffer[buf->pos];
}
struct buffer* create_buffer_string(int bufmax, struct string** ref)
{
	struct buffer_string* buf;
	
	buf = (struct buffer_string*)malloc(sizeof(struct buffer_string) + bufmax + 1);
	if(!buf)
	{
		return NULL;
	}
	buf->pos = 0;
	buf->max = bufmax;
	memset(&buf->string, 0, bufmax+1);
	// ...
	*ref = &buf->string;
	
	// Funciones
	buf->base.free = (void (*)(struct buffer*))buffer_string_free;
	buf->base.incr = (void (*)(struct buffer*))buffer_string_incr;
	buf->base.getch = (int  (*)(struct buffer*))buffer_string_getc;
	buf->base.peekch = (int  (*)(struct buffer*))buffer_string_peekc;
	
	return &buf->base;
}
struct buffer* create_buffer_string_from_string(const char* string)
{
	int len = strlen(string);
	struct string* str;
	struct buffer* buf = create_buffer_string(len + 1, &str);
	str->len = len;
	strcpy(str->buffer, string);
	return buf;
}

