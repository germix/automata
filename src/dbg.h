///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Depurador
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___DBG_H___
#define ___DBG_H___

#ifdef __cplusplus
extern "C" {
#endif

void dbg_print(const char* fmt, ...);
void dbg_fatal(const char* fmt, ...);
void dbg_error(const char* fmt, ...);
void dbg_warning(const char* fmt, ...);

#ifdef __cplusplus
};
#endif

#endif
