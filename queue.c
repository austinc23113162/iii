/*
 *     queue.c

 *     Authors: Austin Chang achang14, Tanner Vales tvales01
 *     Date:    9/25/2025
 *
 *     This is the implementation of a queue abstraction using Hanson's sequence
 *     The client is responsible for freeing the memory stored in the queue
 * 
 */

#include "queue.h"
#include "seq.h"
#include "mem.h"
#include "assert.h"

#include <stdlib.h>

/* Queue_T is a data strucuted built on Seq_T with limited functions */
struct Queue_T {
        Seq_T seq;
};

/********** Queue_new ********
 *
 * Allocates, initializes, and returns a new queue
 * 
 * Parameters:
 *      none
 *
 * Return:
 *      the Queue_T that is created
 *
 * Notes:
 *      the client must free the elements stored in the queue and call 
 *      Queue_free to free the queue itself
 * 
 ************************/
Queue_T Queue_new(void)
{
        Queue_T queue;
        NEW(queue);
        queue->seq = Seq_new(0);

        return queue;
}

/********** Queue_length ********
 *
 * Returns the length of the queue
 * 
 * Parameters:
 *      Queue_T queue: the queue to check
 *
 * Return:
 *      the length of the given queue
 *
 * Notes:
 *      CRE if queue is null
 * 
 ************************/
int Queue_length(Queue_T queue)
{
        assert(queue != NULL);
        return Seq_length(queue->seq);
}

/********** Queue_empty ********
 *
 * Check if the given queue is empty
 * 
 * Parameters:
 *      Queue_T queue: the queue to check
 *
 * Return:
 *      true if queue is empty, false otherwise
 *
 * Notes:
 *      CRE if queue is null
 * 
 ************************/
bool Queue_empty(Queue_T queue)
{
        assert(queue != NULL);
        return Seq_length(queue->seq) == 0;
}

/********** Queue_enq ********
 *
 * Enqueues an element to the queue
 * 
 * Parameters:
 *      Queue_T queue: the queue to use
 *      void *elem:    pointer to the element to be enqueue
 *
 * Return:
 *      none
 *
 * Notes:
 *      CRE if queue is null
 * 
 ************************/
void Queue_enq(Queue_T queue, void *elem)
{
        assert(queue != NULL);
        Seq_addhi(queue->seq, elem);
}

/********** Queue_deq ********
 *
 * Dequeue and return the first element of the queue
 * 
 * Parameters:
 *      Queue_T queue: the queue to dequeue
 *
 * Return:
 *      the first element of the queue
 *
 * Notes:
 *      CRE if queue is null
 * 
 ************************/
void *Queue_deq(Queue_T queue)
{
        assert(queue != NULL);
        return Seq_remlo(queue->seq);
}

/********** Queue_free ********
 *
 * Frees the queue
 * 
 * Parameters:
 *      Queue_T queue: the queue to free
 *
 * Return:
 *      none
 *
 * Notes:
 *      CRE if queue or *queue is null
 * 
 *      does NOT free the memory of the elements stored in the queue
 * 
 ************************/
void Queue_free(Queue_T *queue)
{
        assert(queue != NULL && *queue != NULL);
        Seq_free(&(*queue)->seq);
        FREE(*queue);
}