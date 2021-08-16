///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Buffer de un string
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___BUF_STRING_H___
#define ___BUF_STRING_H___

#ifdef __cplusplus
extern "C" {
#endif

struct string
{
	int		len;
	char	buffer[1];
};

struct buffer* create_buffer_string(int bufmax, struct string** ref);
struct buffer* create_buffer_string_from_string(const char* string);

#ifdef __cplusplus
};
#endif

#endif
