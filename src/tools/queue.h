///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Cola de enteros
//
// Autor: Germán Martínez
// Materia: Autómatas y Lenguajes (2013)
//
///////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef ___QUEUE_H___
#define ___QUEUE_H___

#ifdef __cplusplus
extern "C" {
#endif

#define QUEUE_BUFFER_SIZE		512

struct queue
{
	int		in;
	int		out;
	int		size;
	int		buffer[QUEUE_BUFFER_SIZE];
};

void queue_init(struct queue* q);
int queue_size(const struct queue* q);
int queue_empty(const struct queue* q);
int queue_head(const struct queue* q);
int queue_dequeue(struct queue* q);
void queue_enqueue(struct queue* q, int ch);

#ifdef __cplusplus
};
#endif

#endif
