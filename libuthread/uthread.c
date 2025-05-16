#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"
#include "queue.h"

#define UTHREAD_STACK_SIZE 32768

struct uthread_tcb {
	/* TODO Phase 2 */
	ucontext_t context;        
    void      *stack;          
    uthread_func_t func;       
    void      *arg; 
};

static queue_t ready_queue = NULL;
static struct uthread_tcb *current_thread = NULL;
static ucontext_t scheduler_context;

static void uthread_stub(void) {
    current_thread->func(current_thread->arg);
    uthread_exit();
}


struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
    return current_thread;
}

void uthread_yield(void)
{
	/* TODO Phase 2 */
	if (!current_thread)
        return;

    preempt_disable();
    queue_enqueue(ready_queue, current_thread);
    preempt_enable();
    swapcontext(&current_thread->context, &scheduler_context);
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
	free(current_thread->stack);
    free(current_thread);

    preempt_disable();

    struct uthread_tcb *next;
    if (queue_dequeue(ready_queue, (void**)&next) == 0) {
        current_thread = next;
        preempt_enable();
        setcontext(&next->context);
    } else {
        preempt_enable();
        setcontext(&scheduler_context);
    }
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
	if (!func)
        return -1;

    struct uthread_tcb *tcb = malloc(sizeof *tcb);
    if (!tcb)
        return -1;

    if (getcontext(&tcb->context) == -1) {
        free(tcb);
        return -1;
    }

    tcb->stack = malloc(UTHREAD_STACK_SIZE);
    if (!tcb->stack) {
        free(tcb);
        return -1;
    }

    tcb->context.uc_stack.ss_sp   = tcb->stack;
    tcb->context.uc_stack.ss_size = UTHREAD_STACK_SIZE;
    tcb->context.uc_link          = &scheduler_context;

    tcb->func = func;
    tcb->arg  = arg;

    makecontext(&tcb->context, uthread_stub, 0);

    if (queue_enqueue(ready_queue, tcb) == -1) {
        free(tcb->stack);
        free(tcb);
        return -1;
    }

    return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
	preempt_start(preempt);

    ready_queue = queue_create();
    if (!ready_queue)
        return -1;

    if (getcontext(&scheduler_context) == -1) {
        queue_destroy(ready_queue);
        return -1;
    }

    if (uthread_create(func, arg) == -1) {
        queue_destroy(ready_queue);
        return -1;
    }

    while (queue_dequeue(ready_queue, (void**)&current_thread) == 0) {
        swapcontext(&scheduler_context,
                    &current_thread->context);
    }

    queue_destroy(ready_queue);
    
    preempt_stop();

    return 0;
}

void uthread_block(void)
{
	/* TODO Phase 3 */
    preempt_disable();

    struct uthread_tcb *next;
    struct uthread_tcb *prev = current_thread;

    if (queue_dequeue(ready_queue, (void**)&next) == 0) {
        current_thread = next;
        preempt_enable();
        swapcontext(&prev->context, &next->context);
    } else {
        preempt_enable();
        setcontext(&scheduler_context);
    }
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
    if (!uthread) return;

    preempt_disable();  
    queue_enqueue(ready_queue, uthread);
    preempt_enable(); 
}

