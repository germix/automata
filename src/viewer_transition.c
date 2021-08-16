///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Visualización de transiciones
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#pragma warning(disable:4996)
#endif
#include "atmt.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <screen.h>

void state_to_str(struct automata_state* Q, int state, char* buf)
{
	if(state == HALT_STATE)
		strcpy(buf, "\\H");
	else
		strcpy(buf, Q[state].name.buffer);
}
void direction_to_str(int direction, char* buf)
{
	switch(direction)
	{
		case '-':
			buf[0] = '-';
			break;
		case '<':
			buf[1] = CHR_ARROW_LEFT;
//			buf[1] = CHR_ARROW_LEFT_2;
			break;
		case '>':
			buf[0] = CHR_ARROW_RIGHT;
		//	buf[0] = CHR_ARROW_RIGHT_2;
			break;
	}
	buf[1] = 0;
}
void syms_to_str(int* syms, int count, char* buf)
{
	int i;
	char* p = buf;
	
	for(i = 0; i < count; i++)
	{
#if 0//!!!
		if(syms[i] == CH_ZETA)
		{
			*p++ = '\\';
			*p++ = 'Z';
		}
		else if(syms[i] == CH_EPSILON)
		{
			*p++ = '\\';
			*p++ = 'E';
		}
		else
#endif
		if(syms[i] == CH_NULL && i == 0)
		{
			*p++ = ' ';	//  '$';
		}
		else
			*p++ = (unsigned char)syms[i];
	}
	*p = 0;
	strrev(buf);
}

void transition_viewer(struct automata* autom, int x, int y, int width, int height)
{
	int attr = 0x30;
	struct cstring string;
	static unsigned char str_in[10];
	static unsigned char str_out[10];
	static unsigned char str_pop[MAX_POPABLE_CHARS+1];
	static unsigned char str_push[MAX_PUSHABLE_CHARS+1];
	static unsigned char str_state[10];
	static unsigned char str_direction[10];
	
	draw_border(x, y, x+width-1,y+height-1, attr, ' ', BORDER_TYPE_DOUBLE);
	draw_string(x+1, y+1, attr, "TRANSICION:");

	y += ((height-1)/2);
	cstring_init(&string);
	if(autom->last_state != HALT_STATE)
	{
		switch(autom->type)
		{
			case AUTOMATA_TYPE_TMA:
				{
					struct automata_transition_tma tma = autom->last_transition.tma;
					
					syms_to_str(&tma.in, 1, str_in);
					syms_to_str(&tma.out, 1, str_out);
					state_to_str(autom->states, tma.state, str_state);
					direction_to_str(tma.direction, str_direction);

					cstring_sprintf(&string,
							"&(%s, %s) = (%s, %s, %s)",
							autom->states[autom->last_state].name.buffer,
							str_in,
							str_state,
							str_out,
							str_direction);
				}
				break;
			case AUTOMATA_TYPE_PDA:
			case AUTOMATA_TYPE_NPDA:
				{
					struct automata_transition_pda pda = autom->last_transition.pda;
					
					syms_to_str(&pda.in, 1, str_in);
					syms_to_str(&pda.pop, 1, str_pop);
					syms_to_str(pda.push, MAX_PUSHABLE_CHARS, str_push);
					state_to_str(autom->states, pda.state, str_state);

					cstring_sprintf(&string,
							"&(%s, %s, %s) = (%s, %s)",
							autom->states[autom->last_state].name.buffer,
							str_in,
							str_pop,
							str_state,
							str_push);
				}
				break;
			case AUTOMATA_TYPE_FDA:
			case AUTOMATA_TYPE_FNA:
				{
					struct automata_transition fda = autom->last_transition.fda;

					syms_to_str(&fda.in, 1, str_in);
					state_to_str(autom->states, fda.state, str_state);

					cstring_sprintf(&string,
							"&(%s, %s) = %s",
							autom->states[autom->last_state].name.buffer,
							str_in,
							str_state);
				}
				break;
		}
		draw_string_len(x + (width/2)-(string.len/2), y, attr, string.buffer, string.len);
	}
	cstring_clear(&string);
}


