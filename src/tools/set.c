///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Conjunto de enteros
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "set.h"
#include <stdlib.h>
#include <string.h>

struct set* set_new()
{
	return (struct set*)calloc(1, sizeof(struct set));
}
void set_init(struct set* set)
{
	memset(set, 0, sizeof(struct set));
}
void set_clear(struct set* set)
{
	memset(set, 0, sizeof(struct set));
}
int  set_size(const struct set* set)
{
	return set->size;
}
bool set_empty(const struct set* set)
{
	return (set->size == 0);
}
void set_remove(struct set* set, int elem)
{
	int i;
	for(i = 0; i < set->size; i++)
	{
		if(set->elems[i] == elem)
		{
			set->size--;
			for( ; i < set->size; i++)
				set->elems[i] = set->elems[i+1];
			break;
		}
	}
}
void set_insert(struct set* set, int elem)
{
	int i;

	for(i = 0; i < set->size; i++)
	{
		if(set->elems[i] == elem)
			return;
	}
	set->elems[set->size++] = elem;
}
bool set_contains(const struct set* set, int elem)
{
	int i;

	for(i = 0; i < set->size; i++)
	{
		if(set->elems[i] == elem)
			return true;
	}
	return false;
}
bool set_intersects(const struct set* set1, const struct set* set2)
{
	int i;

	for(i = 0; i < set1->size; i++)
	{
		if(set_contains(set2, set1->elems[i]))
			return true;
	}
	return false;
}
void set_copy(const struct set* set, struct set* dst)
{
	int i;
	
	dst->size = set->size;
	for(i = 0; i < set->size; i++)
	{
		dst->elems[i] = set->elems[i];
	}
}
bool set_equals(const struct set* set1, const struct set* set2)
{
	if(set1->size == set2->size)
	{
		int i;
		int j;
		bool found;
		
		for(i = 0; i < set1->size; i++)
		{
			found = false;
			for(j = 0; j < set2->size; j++)
			{
				if(set1->elems[i] == set2->elems[j])
					found = true;
			}
			if(!found)
				return false;
		}
		return true;
	}
	return false;
}
void set_difference(const struct set* set1, const struct set* set2, struct set* dst)
{
	int i;
	
	//
	// Iniciar conjunto
	//
	set_init(dst);
	
	//
	// Insertar los elementos del primer conjunto y que no están en el segundo
	//
	for(i = 0; i < set1->size; i++)
	{
		if(!set_contains(set2, set1->elems[i]))
		{
			set_insert(dst, set1->elems[i]);
		}
	}
}
void set_union(const struct set* set1, const struct set* set2, struct set* dst)
{
	int i;
	
	//
	// Insertar los elementos del primer conjunto
	//
	set_copy(set1, dst);
	
	//
	// Insertar los elementos del segundo conjunto
	//
	for(i = 0; i < set2->size; i++)
	{
		set_insert(dst, set2->elems[i]);
	}
}
void set_intersection(const struct set* set1, const struct set* set2, struct set* dst)
{
	int i;
	
	//
	// Iniciar conjunto
	//
	set_init(dst);
	
	//
	// Insertar los elementos que están en ambos conjuntos
	//
	for(i = 0; i < set1->size; i++)
	{
		if(set_contains(set2, set1->elems[i]))
		{
			set_insert(dst, set1->elems[i]);
		}
	}
}

