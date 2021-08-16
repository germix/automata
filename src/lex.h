///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Analizador léxico
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___LEX_H___
#define ___LEX_H___

struct lexer
{
//	void (*free)(struct lexer* lex);
	void (*read_token)(struct lexer* lex, struct buffer* buf, struct logger* log);
};

#ifdef __cplusplus
extern "C" {
#endif

extern int  tok;
extern int  tok_chr;
extern int  tok_num;
extern char tok_string[256];
extern int  tok_lineno;

void lex_init(struct lexer* lexer, struct buffer* buffer, struct logger* logger);
void lex_close();

void next();
void match(int t);

#ifdef __cplusplus
};
#endif

#endif
