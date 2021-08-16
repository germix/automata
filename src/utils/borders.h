///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Bordes
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___BORDERS_H___
#define ___BORDERS_H___

//
// Tipos de bordes
//
enum BORDER_TYPE
{
	BORDER_TYPE_NONE,
	BORDER_TYPE_SOLID,
	BORDER_TYPE_SINGLE,
	BORDER_TYPE_DOUBLE,
	// ...
	MAX_BORDER_TYPE,
};

//
// Descriptor de bordes
//
struct border
{
	int c;		// Union centro

	int v;		// Vertical
	int v1;		// Union vertical y1
	int v2;		// Union vertical y2

	int h;		// Horizontal
	int h1;		// Union horizontal x1
	int h2;		// Union horizontal x2

	int e1;		// Corner x1-y1
	int e2;		// Corner x2-y1
	int e3;		// Corner x1-y2
	int e4;		// Corner x2-y2
};


#ifdef __cplusplus
extern "C" {
#endif

struct border borders[];

#ifdef __cplusplus
};
#endif

#endif
