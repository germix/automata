///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Conjunto de enteros
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___SET_H___
#define ___SET_H___
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

struct set
{
	int		size;
	int		elems[1024];
};

struct set* set_new();
void set_init(struct set* set);
void set_clear(struct set* set);
int  set_size(const struct set* set);
bool set_empty(const struct set* set);
void set_remove(struct set* set, int elem);
void set_insert(struct set* set, int elem);
bool set_contains(const struct set* set, int elem);
bool set_intersects(const struct set* set1, const struct set* set2);
void set_copy(const struct set* set, struct set* dst);
bool set_equals(const struct set* set1, const struct set* set2);

void set_difference(const struct set* set1, const struct set* set2, struct set* dst);
void set_union(const struct set* set1, const struct set* set2, struct set* dst);
void set_intersection(const struct set* set1, const struct set* set2, struct set* dst);

#ifdef __cplusplus
};
#endif

#endif

