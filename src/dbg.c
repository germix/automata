///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Depurador
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "dbg.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

int errcnt = 0;			// Contador de errores
int errlimit = 100;		// Límite de errores

void dbg_print(const char* fmt, ...)
{
	va_list args;
	
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}
void dbg_fatal(const char* fmt, ...)
{
	va_list args;

#if LANG == SPANISH
#define ERROR_FATAL		"error fatal:"
#else
#define ERROR_FATAL		"fatal error:"
#endif
	fprintf(stderr, ERROR_FATAL);
	
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
	
	exit(1);
}
void dbg_error(const char* fmt, ...)
{
	va_list args;

	if(errcnt++ >= errlimit)
	{
		fprintf(stderr, ERROR_TEXT_TOO_MANY_ERRORS);
		exit(1);
	}
	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}
void dbg_warning(const char* fmt, ...)
{
	va_list args;

	va_start(args, fmt);
	vfprintf(stderr, fmt, args);
	va_end(args);
}
