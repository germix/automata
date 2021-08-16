///////////////////////////////////////////////////////////////////////////////////////////////////
//
// �rbol para la representaci�n de multiple estados corrientes de un NPDA
//
// Autor: Germ�n Mart�nez
// Materia: Aut�matas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <treenpda.h>

int treenpda_depth(struct treenpda* node)
{
	int md = 0;		// max depth
	if(node->first)
	{
		int cd;		// child depth
		struct treenpda* child;

		for(child = node->first; child != NULL; child = child->next)
		{
			if(md < (cd = treenpda_depth(child)))
				md = cd;
		}
		md += 1;
	}
	return md;
}
void treenpda_delete(struct treenpda* node)
{
	if(node)
	{
		struct treenpda* child;
		struct treenpda* child_next;
		
		//
		// Eliminar sus hijos
		//
		for(child = node->first; child != NULL; child = child_next)
		{
			child_next = child->next;
			treenpda_delete(child);
		}
		//
		// Eliminar nodo
		//
		free(node);
	}
}
