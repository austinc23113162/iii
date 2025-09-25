#include "queue.h"
#include "seq.h"
#include "mem.h"
#include "assert.h"

#include <stdlib.h>

struct Queue_T {
        Seq_T seq;
};

Queue_T Queue_new(void)
{
        Queue_T queue;
        NEW(queue);
        queue->seq = Seq_new(0);

        return queue;
}

int Queue_length(Queue_T queue)
{
        assert(queue != NULL);
        return Seq_length(queue->seq);
}

bool Queue_empty(Queue_T queue)
{
        assert(queue != NULL);
        return Seq_length(queue->seq) == 0;
}

void Queue_enq(Queue_T queue, void *elem)
{
        assert(queue != NULL);
        Seq_addhi(queue->seq, elem);
}

void *Queue_deq(Queue_T queue)
{
        assert(queue != NULL);
        return Seq_remlo(queue->seq);
}

void Queue_free(Queue_T *queue)
{
        assert(queue != NULL && *queue != NULL);
        Seq_free(&(*queue)->seq);
        FREE(*queue);
}