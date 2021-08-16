///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Prototipo de funciones de visualizadores
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___VIEWER_H___
#define ___VIEWER_H___
#if defined(_MSC_VER)
#pragma warning(error:4013)
#pragma warning(error:4020)
#pragma warning(disable:4996)
#endif

#ifdef __cplusplus
extern "C" {
#endif

void tape_viewer(struct tape* tape, int x, int y, int width, int height, int arrow_char, int back_color, int border_color, int tape_color, int arrow_color);

void stack_viewer(struct stack* stack, int x, int y, int width, int height, int back_color, int frame_color, int stack_color);

void treefna_viewer(struct treefna* node, int x, int y, int width, int height, int translate_x, int translate_y);

void transition_viewer(struct automata* autom, int x, int y, int width, int height);

void graph_viewer(struct graph* graph, int x, int y, int width, int height, int translate_x, int translate_y);

void treenpda_viewer(struct treenpda* node, int x, int y, int width, int height, int translate_x, int translate_y);

#ifdef __cplusplus
};
#endif

#endif
