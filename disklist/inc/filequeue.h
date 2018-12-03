#ifndef FILE_QUEUE_H
#define FILE_QUEUE_H

#include "diskutil.h"

typedef struct QNode {
	struct QNode * next;
	struct QNode * prev;
	struct du_file * file;
} QNode;

typedef struct Queue {
	struct QNode * head;
	struct QNode * tail;
	unsigned int length;
} Queue;

int q_push(Queue * queue, du_file * file);
du_file * p_pop(Queue * queue);
du_file * q_peek(Queue * queue);


#define QUEUE_INITIALIZER (Queue) { .head = NULL, .tail = NULL, .length = 0 }


#endif