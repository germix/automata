///////////////////////////////////////////////////////////////////////////////////////////////////
//
// String para C
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#pragma warning(error:4013)
#pragma warning(error:4020)
#pragma warning(error:4133)
#pragma warning(disable:4996)
#endif
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#include <cstring.h>


void cstring_init(struct cstring* s)
{
	memset(s, 0, sizeof(struct cstring));
}
void cstring_clear(struct cstring* s)
{
	free(s->buffer);
	s->len = 0;
	s->alloc = 0;
	s->buffer = NULL;
}
void cstring_sprintf(struct cstring* s, const char* fmt, ...)
{
	static char ach[256*2+1];
	va_list args;
	
	va_start(args, fmt);
	s->len = vsprintf(ach, fmt, args);
	va_end(args);
	// ...
	if((s->len + 32) > s->alloc)
	{
		s->alloc = s->len + 32;
		s->buffer = (char*)realloc(s->buffer, s->alloc + 1);
	}
	strcpy(s->buffer, ach);
}
void cstring_addchar(struct cstring* s, int chr)
{
	if((s->len + 32) > s->alloc)
	{
		s->alloc = s->len + 32;
		s->buffer = (char*)realloc(s->buffer, s->alloc + 1);
	}
	s->buffer[s->len++] = chr;
	s->buffer[s->len] = '\0';
}
void cstring_addsprintf(struct cstring* s, const char* fmt, ...)
{
	int len;
	static char ach[256*2+1];
	va_list args;
	
	va_start(args, fmt);
	len = vsprintf(ach, fmt, args);
	va_end(args);
	// ...
	if((s->len + len) > s->alloc)
	{
		s->alloc = s->len + len;
		s->buffer = (char*)realloc(s->buffer, s->alloc + 1);
	}
	strcpy(&s->buffer[s->len], ach);
	s->len += len;
}



void cstring_copy(const struct cstring* s, struct cstring* dst)
{
	if(dst->buffer)
		free(dst->buffer);
	dst->len = s->len;
	dst->alloc = s->len;
	dst->buffer = (char*)malloc(s->len + 1);
	strcpy(dst->buffer, s->buffer);
}
