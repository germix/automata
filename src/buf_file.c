///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Buffer de un archivo
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#if defined(_MSC_VER)
#pragma warning(disable:4996)
#endif
#include "buf.h"
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>

#define IO_BUF_SIZE		1024

struct buffer_file
{
	struct buffer	base;
	// ...
	int				fd;					// Descriptor de archivo
	unsigned char*	buf_ptr;
	unsigned char*	buf_end;
	unsigned char	buffer[IO_BUF_SIZE];
};
void buffer_file_free(struct buffer_file* bf);
void buffer_file_incr(struct buffer_file* bf);
int  buffer_file_getc(struct buffer_file* bf);
int  buffer_file_peekc(struct buffer_file* bf);
struct buffer* buffer_file_create(const char* filename);

void buffer_file_free(struct buffer_file* bf)
{
	if(bf)
	{
		close(bf->fd);
		free(bf);
	}
}
void buffer_file_incr(struct buffer_file* bf)
{
	bf->buf_ptr++;
	if(bf->buf_ptr >= bf->buf_end)
	{
		buffer_file_getc(bf);
	}
}
int  buffer_file_getc(struct buffer_file* bf)
{
	int ch = buffer_file_peekc(bf);
	bf->buf_ptr++;
	return ch;
}
int  buffer_file_peekc(struct buffer_file* bf)
{
	int len;
	
	if(bf->buf_ptr >= bf->buf_end)
	{
		len = IO_BUF_SIZE;
		len = read(bf->fd, bf->buffer, len);
		if(len < 0)
			len = 0;
		
		bf->buf_ptr = bf->buffer;
		bf->buf_end = bf->buffer + len;
	}
	if(bf->buf_ptr < bf->buf_end)
	{
		return (bf->buf_ptr[0] & 0xFF);
	}
	bf->buf_ptr = bf->buf_end;
	return EOF;
}
struct buffer* buffer_file_create(const char* filename)
{
	int fd;
	struct buffer_file* bf;
	
	fd = open(filename, O_RDONLY | O_BINARY);
	if(fd < 0)
		return NULL;
	bf = (struct buffer_file*)malloc(sizeof(struct buffer_file));
	if(!bf)
	{
		close(fd);
		return NULL;
	}
	bf->fd = fd;
	bf->buf_ptr = bf->buffer;
	bf->buf_end = bf->buffer;
	
	// Funciones
	bf->base.free = (void (*)(struct buffer*))buffer_file_free;
	bf->base.incr = (void (*)(struct buffer*))buffer_file_incr;
	bf->base.getch = (int  (*)(struct buffer*))buffer_file_getc;
	bf->base.peekch = (int  (*)(struct buffer*))buffer_file_peekc;
	
	return &bf->base;
}
