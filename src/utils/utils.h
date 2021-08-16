///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Utilitarios
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___UTILS_H___
#define ___UTILS_H___
#include "chrs.h"
#include <stdbool.h>

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#ifndef countof
#define countof(a) (sizeof(a) / sizeof(a[0]))
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _MSC_VER
void clrscr();
#endif

bool text_entry(int x, int y, int width, bool arrows, int attr, char* text, int max);

void show_message(const char* message, int color);

struct buffer* buffer_file_create(const char* filename);

#ifdef __cplusplus
extern "C" {
#endif

#endif
