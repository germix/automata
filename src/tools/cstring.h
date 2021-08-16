///////////////////////////////////////////////////////////////////////////////////////////////////
//
// String para C
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___CSTRING_H___
#define ___CSTRING_H___

struct cstring
{
	int		len;
	int		alloc;
	char*	buffer;
};

#ifdef __cplusplus
extern "C" {
#endif

void cstring_init(struct cstring* s);
void cstring_clear(struct cstring* s);
void cstring_sprintf(struct cstring* s, const char* fmt, ...);

void cstring_addchar(struct cstring* s, int chr);
void cstring_addsprintf(struct cstring* s, const char* fmt, ...);

void cstring_copy(const struct cstring* s, struct cstring* dst);

#ifdef __cplusplus
};
#endif

#endif
