///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Bordes
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include <borders.h>

struct border borders[] =
{
	// BORDER_TYPE_NONE
	{
		' ',
		
		' ',
		' ',
		' ',

		' ',
		' ',
		' ',

		' ',
		' ',
		' ',
		' ',
	},
	// BORDER_TYPE_SOLID
	{
		219,
		
		219,
		219,
		219,

		219,
		219,
		219,

		219,
		219,
		219,
		219,
	},
	// BORDER_TYPE_SINGLE
	{
		197,
		
		179,
		194,
		193,

		196,
		195,
		180,

		218,
		191,
		192,
		217,
	},
	// BORDER_TYPE_DOUBLE
	{
		/*
		206,

		186,
		204,
		185,

		205,
		203,
		202,

		201,
		187,
		200,
		188,
		*/
		206,

		186,
		203,
		202,

		205,
		204,
		185,

		201,
		187,
		200,
		188,
	},
};

