///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Programa principal
//
// Autor: Germ�n Mart�nez
// Materia: Aut�matas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#pragma warning(error:4013)
#pragma warning(error:4020)
#pragma warning(disable:4996)
#endif
#include <stdlib.h>
#include "keybd.h"
#include "screen.h"

#include "tape.h"
#include "utils.h"

#include <assert.h>
#include <string.h>

#include "atmt.h"
#include "atmt_parser.h"

#include "viewer.h"
#include <graph.h>

#define COLOR_MSG_ERROR		0x4E
#define COLOR_MSG_OK	0x1F

void test_automata_equals();
void test_automata_inclusion();

///////////////////////////////////////////////////////////////////////////////////////////////////

//
// Estructura principal de la aplicaci�n
//
struct application
{
	struct tape			tape;
	struct automata*	automata;

#define MAX_STRING_SIZE	20
#define TAPE_WIDTH 4
	char				string[MAX_STRING_SIZE+1];

	int					trans_x;
	int					trans_y;
};

bool dialog_text_entry(char* text, int max, const char* label)
{
	int x = 5;
	int y = 20;
	int width = screen_width()-(x*2)-1;
	int height = 7;
//	int height = screen_height()-(y*2)-1-5;
	
	y += 2;

	draw_border(x, y, x+width, y+height, 0x70, ' ', BORDER_TYPE_DOUBLE);
	flush_screen();
	
	draw_string(x+2, y+2+1, 0x70, label);
	return text_entry(x+2, y+2+2, width-4+1, true, 0x1F, text, max);
}
struct automata* load_automata(const char* filename)
{
	struct buffer* buf = buffer_file_create(filename);
	if(buf)
	{
		struct automata* autom = parser_automata(buf);
		//
		// Si el existe y es v�lido, crear el grafo
		//
		if(autom && autom->type != AUTOMATA_TYPE_INVALID)
		{
			autom->graph = make_graph(autom);
		}
		return autom;
	}
	return NULL;
}
void load_testing(struct application* app)
{
	struct
	{
		char* file;
		char* string;
	}test_data[] =
	{
		// M�quinas de Turing
		{	"../test/ej_tma.txt",					"010111"		},
		
		// Automatas a pila
		{	"../test/ej_pda.txt",					"00111"			},
		
		// Automatas finitos determin�sticos
		{	"../test/ej_fda.txt",					"10101"			},
		{	"../test/ej_fda 2.txt",					"abbabb"		},
		{	"../test/ej_fda 3.txt",					"abbabb"		},
		{	"../test/ej_fda tema4b.txt",			"abbabb"		},
		
		// Automatas finitos no determin�sticos
		{	"../test/ej_fna.txt",					"00101"			},
		{	"../test/ej_fna 2.txt",					"abbbb"			},
		
		// Automatas para testear grafos
		{	"../test/ej_for_graph_fna.txt",			"00101"			},
	//	{	"../test/ej_for_graph_fna.txt",			"001101"		},
		{	"../test/ej_for_graph_pda.txt",			"00101"			},
	//	{	"../test/ej_for_graph_pda.txt",			"001101"		},
		
		// Otros automatas
		{	"../test/cpp_hexadecimal_literal.txt",	""				},
		// ...
		{	"../test/ej_npda.txt",					"1111"			},
		// ...
		{	"../test/zero or one.txt",				""				},
		{	"../test/kleene plus (fda).txt",		""				},
		{	"../test/kleene plus (fna).txt",		""				},
	};
	int index = 11;
	if(index >= 0 && index < (sizeof(test_data)/sizeof(test_data[0])))
	{
		app->automata = load_automata(test_data[index].file);
		if(app->automata)
		{
			strcpy(app->string, test_data[index].string);
		}
	}
}

void app_main(struct application* app);
void show_main(struct application* app);
bool keybd_main(struct application* app);

void app_running(struct application* app);
void show_running(struct application* app);
bool keybd_running(struct application* app);

void app_graph(struct application* app);

void app_graph(struct application* app)
{
	int dx = 0;
	int dy = 0;
	int key;
	do
	{
		graph_viewer(app->automata->graph, 0, 0, screen_width(), screen_height(), dx, dy);
		flush_screen();
		switch(key = getkey())
		{
			case KEY_ARROW_UP:
				dy--;
				break;
			case KEY_ARROW_DOWN:
				dy++;
				break;
			case KEY_ARROW_LEFT:
				dx--;
				break;
			case KEY_ARROW_RIGHT:
				dx++;
				break;
		}
	}
	while(key != KEY_ESC);
}
void app_main(struct application* app)
{
	bool exit = false;
	
	load_testing(app);
	do
	{
		show_main(app);
		exit = keybd_main(app);
	}
	while(!exit);

	automata_free(app->automata);
}
void show_main(struct application* app)
{
	int i;
	int x = 2;
	int y = 1;
//	int attr = 0x0E;
//	int attr = 0xF0;
//	int attr = 0x1E;
//	int attr = 0x0A;
//	int attr = 0x02;
	int attr = 0x0F;

	int w = screen_width() - 1;
	
	draw_border(0, 0, screen_width()-1, screen_height()-1, attr, ' ', BORDER_TYPE_DOUBLE);
	draw_string(x, y++, attr, "\'Q\'     - Salir");
	draw_string(x, y++, attr, "\'C\'     - Cargar cadena");
	draw_string(x, y++, attr, "\'A\'     - Cargar automata");
	draw_string(x, y++, attr, "\'E\'     - Cargar expresion regular");
	draw_string(x, y++, attr, "\'G\'     - Mostrar grafo");
	switch(automata_type(app->automata))
	{
		case AUTOMATA_TYPE_FDA:
			draw_string(x, y++, attr, "\'R\'     - Mostrar gramatica");
			draw_string(x, y++, attr, "\'M\'     - Minimizar");
			draw_string(x, y++, attr, "\'V\'     - \250Es Vacio?");
			draw_string(x, y++, attr, "\'I\'     - \250Es Infinito?");
			break;
		case AUTOMATA_TYPE_FNA:
			draw_string(x, y++, attr, "\'D\'     - Transformar a Deterministico");
			draw_string(x, y++, attr, "\'V\'     - \250Es Vacio?");
			draw_string(x, y++, attr, "\'I\'     - \250Es Infinito?");
			break;
	}
	draw_string(x, y++, attr, "<ENTER> - Ejecutar automata");

	draw_char(0, y, attr, 204);
	for(i = 1; i < w; i++)
	{
		draw_char(i, y, attr, 205);
	}
	draw_char(w, y++, attr, 185);

	draw_string(x, y++, attr, "Cadena:");
	draw_string(x+4, y++, attr, app->string);
	y++;
	draw_string(x, y++, attr, "Tipo:");
	if(!app->automata)
		y++;
	else
	{
		switch(automata_type(app->automata))
		{
			case AUTOMATA_TYPE_TMA:
				draw_string(x+4, y++, attr, "Maquina de Turing");
				break;
			case AUTOMATA_TYPE_PDA:
				draw_string(x+4, y++, attr, "Automata a Pila");
				break;
			case AUTOMATA_TYPE_NPDA:
				draw_string(x+4, y++, attr, "Automata a Pila No Deterministico");
				break;
			case AUTOMATA_TYPE_FDA:
				draw_string(x+4, y++, attr, "Automata Finito Deterministico");
				break;
			case AUTOMATA_TYPE_FNA:
				draw_string(x+4, y++, attr, "Automata Finito No Deterministico");
				break;
		}
	}
	y++;
	draw_string(x, y++, attr, "Descripcion:");
	draw_string(x+4, y++, attr, (app->automata ? automata_descr(app->automata) : ""));

	flush_screen();
}
bool keybd_main(struct application* app)
{
	switch(getkey())
	{
		case 'q':
		case 'Q':
			//
			// Salir
			//
			return true;
		case 'c':
		case 'C':
			//
			// Cargar cadena
			//
			{
				char str[MAX_STRING_SIZE+1];
				
#if 0//!!!
				str[0] = 0;
#else
				strcpy(str, app->string);
#endif
				if(dialog_text_entry(str, MAX_STRING_SIZE, "Cadena:"))
				{
					strcpy(app->string, str);
				}
			}
			break;
		case 'a':
		case 'A':
			//
			// Cargar automata
			//
			{
				char filename[256];
				
				filename[0] = 0;
				if(dialog_text_entry(filename, 256, "Nombre de archivo:"))
				{
					struct automata* new_automata = load_automata(filename);
					if(new_automata)
					{
						automata_free(app->automata);
						app->automata = new_automata;
					}
				}
			}
			break;
		case 'g':
		case 'G':
			if(app->automata)
				app_graph(app);
			break;
		case 'd':
		case 'D':
			if(automata_type(app->automata) == AUTOMATA_TYPE_FNA)
			{
				extern struct automata* fna_to_fda(struct automata* fna_autom);
				struct automata* fda = fna_to_fda(app->automata);
				if(!fda)
				{
					show_message("No se pudo transformar el AFN en un AFD", COLOR_MSG_ERROR);
				}
				else
				{
					automata_free(app->automata);
					app->automata = fda;
					app->automata->graph = make_graph(app->automata);
					show_message("Se ha transformado existosamente el AFN en un AFD", COLOR_MSG_OK);
				}
				flush_screen();
				getkey();
			}
			break;
		case 'm':
		case 'M':
			if(automata_type(app->automata) == AUTOMATA_TYPE_FDA)
			{
				if(app->automata->minimized)
				{
					show_message("El automata ya esta minimizado", 0x2F);
				}
				else
				{
					extern struct automata* minimize_fda(struct automata* fda);
					struct automata* minimized = minimize_fda(app->automata);
					if(!minimized)
					{
						show_message("No se pudo minimizar el AFD", COLOR_MSG_ERROR);
					}
					else
					{
						automata_free(app->automata);
						app->automata = minimized;
						app->automata->graph = make_graph(app->automata);
						show_message("Se ha minimizado existosamente el AFD", COLOR_MSG_OK);
					}
				}
				flush_screen();
				getkey();
			}
			break;
		case KEY_ENTER:
			if(app->automata)
				app_running(app);
			break;
		case 'e':
		case 'E':
			//
			// Cargar expresi�n regular
			//
			{
				struct automata* thompson(const char* string);
				char str[MAX_STRING_SIZE+1];
				
				str[0] = 0;
				if(dialog_text_entry(str, MAX_STRING_SIZE, "Expresion regular:"))
				{
					struct automata* new_automata = thompson(str);
					if(new_automata)
					{
						automata_free(app->automata);
						app->automata = new_automata;
						if(!new_automata->graph)
						{
							new_automata->graph = make_graph(new_automata);
						}
					}
				}
			}
			break;
		case 'v':
		case 'V':
			if(automata_type(app->automata) == AUTOMATA_TYPE_FDA || automata_type(app->automata) == AUTOMATA_TYPE_FNA)
			{
				if(automata_is_empty(app->automata))
					show_message("El automata es vacio", COLOR_MSG_OK);
				else
					show_message("El automata NO es vacio", 0xE0);
				flush_screen();
				getkey();
			}
			break;
		case 'i':
		case 'I':
			if(automata_type(app->automata) == AUTOMATA_TYPE_FDA || automata_type(app->automata) == AUTOMATA_TYPE_FNA)
			{
				if(automata_is_infinite(app->automata))
					show_message("El automata es infinito", COLOR_MSG_OK);
				else
					show_message("El automata NO es infinito", 0xE0);
				flush_screen();
				getkey();
			}
			break;
		case 'r':
		case 'R':
			if(automata_type(app->automata) == AUTOMATA_TYPE_FDA)
			{
				void free_grammar(struct grammar* gr);
				struct grammar* automata_to_grammar(struct automata* autom);
				void grammar_viewer(struct grammar* grammar, int x, int y, int width, int height);
				
				struct grammar* gr = automata_to_grammar(app->automata);
				grammar_viewer(gr, 0, 0, screen_width(), screen_height());
				flush_screen();
				getkey();
				free_grammar(gr);
			}
			break;
	}
	return false;
}
void app_running(struct application* app)
{
	int x = 7;
	int y = 7;
	int width = screen_width()-(x*2)-1;
	int height = screen_height()-(y*2)-1;
	int len;
	int pos;
	bool exit;
	int attr;
	char* message;
	
	app->trans_x = 0;
	app->trans_y = 0;
	//
	// Iniciar tape
	//
	len = strlen(app->string);
	pos = ((screen_width()-2)/(TAPE_WIDTH*2))-(len/2);
	tape_init(&app->tape, app->string, pos, len);
	//
	// Iniciar automata
	//
	automata_reset(app->automata, &app->tape);
	do
	{
		show_running(app);
		exit = keybd_running(app);
	}
	while(!exit);

	show_running(app);

	switch(automata_haltcode(app->automata))
	{
		case HALTCODE_NONE:
			message = NULL;
			break;
		case HALTCODE_SUCCESS:
			attr = COLOR_MSG_OK;
			message = "La cadena ha sido reconocida";
			break;
		case HALTCODE_ERROR:
			attr = COLOR_MSG_ERROR;
			message = "ERROR: La NO cadena ha sido reconocida";
			break;
		default:
			assert(0);
			break;
	}
	if(message)
	{
		show_message(message, attr);
		flush_screen();
		
		getkey();
	}
#if 0
	show_running(app);
	getkey();
#else
	do
	{
		show_running(app);
		exit = keybd_running(app);
	}
	while(!exit);
#endif
}
void show_running(struct application* app)
{
	int x = 0;
	int width = screen_width();
	int height = screen_height();
	//
	// Tape Viewer
	//
	int tape_width = width;
	int tape_height = 10;
	int arrow_char = 24;
//	int arrow_char = 30;
	//
	// Stack Viewer
	//
	//
	// Colores
	//
	int back_color = 0;
	int frame_color = 0x8;
	int tape_color = 0xF;
	int arrow_color = 0xA;
	int stack_color = 0xB;
	int border_color = 0x8;
	//
	// ...
	//
	int tape_viewer_y = 0;
	int tape_viewer_height = 10;
	int trans_viewer_y = tape_viewer_height;
	int trans_viewer_height = 5;
	int tree_viewer_y = (tape_viewer_height+(automata_is_nondeterministic(app->automata) ? 0 : trans_viewer_height));
	int tree_viewer_height = height-tree_viewer_y;
	int stack_viewer_y = (tape_viewer_height+(automata_is_nondeterministic(app->automata) ? 0 : trans_viewer_height));
	int stack_viewer_height = height-stack_viewer_y;

	fill_rect(0, 0, width, height, MAKE_COLOR(back_color, 0xF), ' ');
	switch(automata_type(app->automata))
	{
		case AUTOMATA_TYPE_PDA:
			stack_viewer(
				app->automata->stack,
				x,
				stack_viewer_y,
				width,
				stack_viewer_height,
				back_color,
				frame_color,
				stack_color);
			break;
		case AUTOMATA_TYPE_FNA:
			if(app->automata->multi_current_root)
			{
				treefna_viewer(
					app->automata->multi_current_root,
					x,
					tree_viewer_y,
					width,
					tree_viewer_height,
					app->trans_x,
					app->trans_y);
			}
			break;
		case AUTOMATA_TYPE_NPDA:
			if(app->automata->multi_current_root)
			{
				void treenpda_viewer(struct treenpda* node, int x, int y, int width, int height, int translate_x, int translate_y);

				treenpda_viewer(
					(struct treenpda*)app->automata->multi_current_root,
					x,
					tree_viewer_y,
					width,
					tree_viewer_height,
					app->trans_x,
					app->trans_y);
			}
			break;
	}
	//
	// Dibujar tape
	//
	{
		tape_viewer(&app->tape,
			x,
			tape_viewer_y,
			width,
			tape_viewer_height,
			arrow_char,
			back_color,
			frame_color,
			tape_color,
			arrow_color);
	}
	//
	// Dibujar transiciones
	//
	if(automata_type(app->automata) != AUTOMATA_TYPE_FNA && automata_type(app->automata) != AUTOMATA_TYPE_NPDA)
	{
		transition_viewer(app->automata, x, trans_viewer_y, width, trans_viewer_height);
	}
	flush_screen();
}
bool keybd_running(struct application* app)
{
	int dx = 0;
	int dy = 0;
	int key;
	bool exit;

	do
	{
		exit = true;
		switch(key = getkey())
		{
			case KEY_ESC:
				break;
			case KEY_ENTER:
				if(automata_haltcode(app->automata) == HALTCODE_NONE)
				{
					automata_runstep(app->automata, &app->tape);
				}
				break;
			case KEY_ARROW_UP:
				app->trans_y--;
				break;
			case KEY_ARROW_DOWN:
				app->trans_y++;
				break;
			case KEY_ARROW_LEFT:
				app->trans_x--;
				break;
			case KEY_ARROW_RIGHT:
				app->trans_x++;
				break;
			default:
				exit = false;
				break;
		}
	}
	while(!exit);
	
	return (key == KEY_ESC || (key == KEY_ENTER && automata_haltcode(app->automata) != HALTCODE_NONE));
}
void main(void)
{
	struct application app;
	
//	init_screen(80, 25);
//	init_screen(120, 40);
//	init_screen(120, 50);
	init_screen(180, 50);
	
	memset(&app, 0, sizeof(struct application));
	
	app_main(&app);
//	test_automata_equals();
//	test_automata_inclusion();
	
	
	close_screen();
}
