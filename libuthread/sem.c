#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "queue.h"
#include "private.h"
#include "sem.h"

struct semaphore {
	size_t count;
    queue_t wait_queue;
};

sem_t sem_create(size_t count)
{
	sem_t sem = malloc(sizeof(struct semaphore));
    if (!sem) return NULL;

    sem->count = count;
    sem->wait_queue = queue_create();
    if (!sem->wait_queue) {
        free(sem);
        return NULL;
    }

    return sem;
}

int sem_destroy(sem_t sem)
{
	if (!sem || queue_length(sem->wait_queue) != 0) return -1;

    queue_destroy(sem->wait_queue);
    free(sem);

    return 0;
}

int sem_down(sem_t sem)
{
	if (!sem) return -1;

    preempt_disable();

    while (sem->count == 0) {
        struct uthread_tcb *curr = uthread_current();
        queue_enqueue(sem->wait_queue, curr);

        preempt_enable();
        uthread_block();
        preempt_disable();
    }

    sem->count--;

    preempt_enable();
    
    return 0;
}

int sem_up(sem_t sem)
{
	if (!sem) return -1;

    preempt_disable();

    struct uthread_tcb *next;
    int unblocked = 0;
    if (queue_dequeue(sem->wait_queue, (void**)&next) == 0) {
        sem->count++;
        uthread_unblock(next);
        unblocked = 1;
    } else {
        sem->count++;
    }

    preempt_enable();

    if (unblocked) uthread_yield();

    return 0;
}

