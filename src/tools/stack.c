///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Pila de enteros
//
// Autor: Germ�n Mart�nez
// Materia: Aut�matas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "stack.h"


void stack_init(struct stack* stk)
{
	stk->top = 0;
}
int stack_size(const struct stack* stk)
{
	return stk->top;
}
int stack_empty(const struct stack* stk)
{
	return (stk->top == 0);
}
void stack_clear(struct stack* stk)
{
	stk->top = 0;
}
int stack_peek(const struct stack* stk)
{
	return stk->buf[stk->top-1];
}
int stack_pop(struct stack* stk)
{
	// AVISO: Llamar a esta funci�n despu�s de verificar que la
	//        pila no est� vacia
	return stk->buf[--stk->top];
}
void stack_push(struct stack* stk, int ch)
{
	stk->buf[stk->top++] = ch;
}
