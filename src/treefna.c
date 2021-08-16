///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Árbol para la representación de multiple estados corrientes de un AFN
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#pragma warning(disable:4996)
#endif
#include "treefna.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

///////////////////////////////////////////////////////////////////////////////////////////////////

struct treefna* treefna_new(struct treefna* parent, int state, int symbol)
{
	static char sz[15];
	int size;
	struct treefna* node;

	sprintf(sz, "q%d", state);
	size = sizeof(struct treefna) + strlen(sz) + 1;
	node = (struct treefna*)malloc(size);
	if(node)
	{
		memset(node, 0, size);
		node->state = state;
		node->symbol = symbol;
		node->parent = parent;
		if(parent)
		{
			if(!parent->first)
			{
				//
				// Si el padre no tiene hijos, 'node' es el primero y el último
				//
				parent->first = node;
				parent->last = node;
			}
			else
			{
				//
				// Agregar 'node' al final de la lista de hijos de 'parent'
				//
				parent->last->next = node;
				node->prev = parent->last;
				parent->last = node;
			}
		}
		strcpy(node->label, sz);
	}
	return node;
}

void treefna_delete(struct treefna* node)
{
	if(node)
	{
		struct treefna* child;
		struct treefna* child_next;
#if 0//!!!
		//
		// Desvincularse de la lista de hermanos
		//
		if(node->prev)
			node->prev->next = node->next;
		if(node->last)
			node->last->prev = node->prev;
#endif
		//
		// Eliminar sus hijos
		//
		for(child = node->first; child != NULL; child = child_next)
		{
			child_next = child->next;
			treefna_delete(child);
		}
		//
		// Eliminar nodo
		//
		free(node);
	}
}

//
// Calcular profundidad
//
int treefna_depth(struct treefna* node)
{
	int md = 0;		// max depth
	if(node->first)
	{
		int cd;		// child depth
		struct treefna* child;

		for(child = node->first; child != NULL; child = child->next)
		{
			if(md < (cd = treefna_depth(child)))
				md = cd;
		}
		md += 1;
	}
	return md;
}

