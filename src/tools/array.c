///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Arreglos
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// Devolver un puntero a un nuevo elemento, e incrementar el vector si es necesario
//
void* array_add(void** ptr, int* count, int data_size)
{
	int rc;
	int nb;
	int nb_alloc;
	void* pp;
	void* data;
	
	rc = 0;
	pp = *ptr;
	nb = *count;
	
	if((nb & (nb - 1)) == 0)
	{
		if(!nb)
			nb_alloc = 1;
		else
			nb_alloc = nb * 2;
		
		if(NULL == (pp = realloc(pp, nb_alloc * data_size)))
			rc = -1;
		else
			*ptr = pp;
	}
	data = 0;
	if(rc == 0)
	{
		data = &(*((char**)ptr))[(*count) * data_size];
		memset(data, 0, data_size);
		*count = nb+1;
	}
	return data;
}

//
// Eliminar el vector, y limpiar
//
void  array_clear(void** ptr, int* count)
{
	free(*ptr);
	*ptr = 0;
	*count = 0;
}

