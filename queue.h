#include <stdbool.h>

#ifndef QUEUE_INCLUDED
#define QUEUE_INCLUDED

typedef struct Queue_T *Queue_T;

//Client is responsible of freeing the pointers stored in queue
extern Queue_T Queue_new(void);
extern void Queue_free(Queue_T *queue);

extern int Queue_length(Queue_T queue);
bool Queue_empty(Queue_T queue);

extern void Queue_enq(Queue_T queue, void *elem);
extern void *Queue_deq(Queue_T queue);

#endif