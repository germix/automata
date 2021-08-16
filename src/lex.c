///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Analizador léxico
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "lex.h"
#include "buf.h"
#include "log.h"
#include "dbg.h"
#include "error.h"

int  tok;
int  tok_chr;
int  tok_num;
char tok_string[256];
int  tok_lineno;

static struct lexer* lex_lexer = 0;
static struct buffer* lex_buffer = 0;
static struct logger* lex_logger = 0;

static struct logger  default_lex_logger =
{
	dbg_print,
	dbg_fatal,
	dbg_error,
	dbg_warning,
};

//
// Obtener el siguiente token
//
void next(void)
{
	tok = 0;
	tok_chr = 0;
	tok_num = 0;
	tok_string[0] = 0;
	// ...
	lex_lexer->read_token(lex_lexer, lex_buffer, lex_logger);
}

//
// Comprueba si es el token esperado, y continua
//
void match(int t)
{
	if(tok == t)
	{
		next();
	}
	else
	{
		if(lex_logger)
			lex_logger->error(ERROR_TEXT_EXPECTED_CHAR(t));
		
		next();		// ¿Continuar?
	}
}

//
// Inicializar el analizador léxico actual
//
void lex_init(struct lexer* lexer, struct buffer* buffer, struct logger* logger)
{
	tok = 0;
	tok_chr = 0;
	tok_num = 0;
	tok_string[0] = 0;
	// ...
	lex_lexer = lexer;
	lex_buffer = buffer;
	if(logger)
		lex_logger = logger;
	else
		lex_logger = &default_lex_logger;
}
void lex_close()
{
	lex_lexer = NULL;
	if(lex_buffer)
	{
		if(lex_buffer->free)
			lex_buffer->free(lex_buffer);
		lex_buffer = NULL;
	}
	lex_logger = NULL;
}

