///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Errores
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___ERROR_H___
#define ___ERROR_H___


//
// Dudas:
//
// (*) ¿El ERROR_TEXT_UNRECOGNIZED_SPECIAL_CHAR, es un error de sintaxis?
//

#if LANG == SPANISH
#define ERROR_TEXT_TOO_MANY_ERRORS					"demasiados errores\n"
#define ERROR_TEXT_EXPECTED_NUMBER_OF_STATE			"error de sintaxis: se esperaba número de estado\n"
#define ERROR_TEXT_EXPECTED_CHAR(ch)				"error de sintaxis: se esperaba \'%c\'\n", ch

#define ERROR_TEXT_UNRECOGNIZED_SPECIAL_CHAR(ch)	"no se reconoce caracter especial \'%c\'\n", ch
#else
#define ERROR_TEXT_TOO_MANY_ERRORS					"too many errors\n"

#define ERROR_TEXT_EXPECTED_NUMBER_OF_STATE			"syntax error: expected number of state\n"
#define ERROR_TEXT_EXPECTED_CHAR(ch)				"syntax error: expected \'%c\'\n", ch

#define ERROR_TEXT_UNRECOGNIZED_SPECIAL_CHAR(ch)	"unrecognized special character \'%c\'\n", ch
#endif

#endif
