#include "filequeue.h"

#include <stdlib.h>
#include <stdio.h>

int q_push(Queue * queue, du_file * file) {
	QNode * new_node = (QNode *) malloc(sizeof(QNode));
	if(new_node == NULL) {
		fprintf(stderr, "Error: malloc.\n");
		exit(1);
	}

	if(queue->head == NULL) {
		queue->head = queue->tail = new_node;
	} else {
		queue->tail->next = new_node;
	}
	new_node->file = file;
	return ++queue->length;
}

du_file * q_pop(Queue * queue) {
	du_file * file = NULL;
	if(queue->head != NULL) {
		file = queue->head->file;
		QNode * temp = queue->head;
		queue->head = queue->head->next;
		free(temp);
		queue->length--;
	}
	return file;
}

du_file * q_peek(Queue * queue) {
	if(queue->head != NULL)
		return queue->head->file;
	return NULL;
}
