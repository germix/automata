///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Pila de enteros
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___STACK_H___
#define ___STACK_H___

#ifdef __cplusplus
extern "C" {
#endif

struct stack
{
	int top;
	int buf[1024];
};
void stack_init(struct stack* stk);
int stack_size(const struct stack* stk);
int stack_empty(const struct stack* stk);
void stack_clear(struct stack* stk);
int stack_peek(const struct stack* stk);
int stack_pop(struct stack* stk);
void stack_push(struct stack* stk, int ch);

#ifdef __cplusplus
};
#endif

#endif
