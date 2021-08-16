///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Tape
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___TAPE_H___
#define ___TAPE_H___
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct tape
{
	int		pos;
	int		size;
	char	buffer[1024];
};

bool tape_init(struct tape* tape, const char* buf, int pos, int size);
void tape_move(struct tape* tape, int dir);
int  tape_read(struct tape* tape);
void tape_write(struct tape* tape, int chr);

#ifdef __cplusplus
};
#endif

#endif
