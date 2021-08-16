///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Cola de enteros
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#include "queue.h"
#include <string.h>
#include <assert.h>

void queue_init(struct queue* q)
{
	memset(q, 0, sizeof(struct queue));
}
int queue_size(const struct queue* q)
{
	return q->size;
}
int queue_empty(const struct queue* q)
{
	return (q->in == q->out);
}
int queue_head(const struct queue* q)
{
	return q->buffer[q->out];
}
int queue_dequeue(struct queue* q)
{
	int ch;
	
	assert((q->in != q->out));
	
	ch = q->buffer[q->out];
	q->out = (q->out + 1) & (QUEUE_BUFFER_SIZE - 1);
	q->size--;
	return ch;
}
void queue_enqueue(struct queue* q, int ch)
{
	if(((q->in + 1) & (QUEUE_BUFFER_SIZE - 1)) != q->out)
	{
		q->size++;
		q->buffer[q->in] = ch;
		q->in = (q->in + 1) & (QUEUE_BUFFER_SIZE - 1);
	}
}

