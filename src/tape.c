///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Tape
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "tape.h"
#include <string.h>

#define MAX_TAPE_SIZE	1024

bool tape_init(struct tape* tape, const char* buf, int pos, int size)
{
	tape->pos = pos;
	tape->size = size;
	memset(&tape->buffer[0], 0, pos);
	memcpy(&tape->buffer[pos], buf, size);
	memset(&tape->buffer[pos+size], 0, MAX_TAPE_SIZE - (pos+size));
	
	return true;
}
void tape_move(struct tape* tape, int dir)
{
	tape->pos += dir;
}
int  tape_read(struct tape* tape)
{
	return tape->buffer[tape->pos];
}
void tape_write(struct tape* tape, int chr)
{
	tape->buffer[tape->pos] = chr;
}