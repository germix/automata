///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Buffer
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___BUF_H___
#define ___BUF_H___

#ifndef EOF
#define EOF (-1)
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif
#endif

struct buffer
{
	void	(*free)(struct buffer* buf);		// Libera el buffer
	void	(*incr)(struct buffer* buf);		// Avanza posición en el buffer
	int		(*getch)(struct buffer* buf);		// Devuelve el caracter actual y avanza
	int		(*peekch)(struct buffer* buf);		// Devuelve el caracter actual y no avanza
};

#endif
