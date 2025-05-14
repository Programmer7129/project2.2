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

static queue_t ready_queue = NULL;             // FIFO of ready threads
static struct uthread_tcb *current_thread = NULL;
static ucontext_t scheduler_context;

static void uthread_stub(void) {
    // Run the user function
    current_thread->func(current_thread->arg);
    // When it returns, exit the thread
    uthread_exit();
}


struct uthread_tcb *uthread_current(void)
{
	/* TODO Phase 2/3 */
}

void uthread_yield(void)
{
	/* TODO Phase 2 */
	if (!current_thread)
        return;

    // Put this thread at end of ready queue
    queue_enqueue(ready_queue, current_thread);
    // Switch to scheduler
    swapcontext(&current_thread->context,
                &scheduler_context);
}

void uthread_exit(void)
{
	/* TODO Phase 2 */
	free(current_thread->stack);
    free(current_thread);

    // Pick next thread to run
    struct uthread_tcb *next;
    if (queue_dequeue(ready_queue, (void**)&next) == 0) {
        current_thread = next;
        setcontext(&next->context);
    } else {
        // No threads left: return to scheduler
        setcontext(&scheduler_context);
    }
}

int uthread_create(uthread_func_t func, void *arg)
{
	/* TODO Phase 2 */
	if (!func)
        return -1;

    // Allocate TCB
    struct uthread_tcb *tcb = malloc(sizeof *tcb);
    if (!tcb)
        return -1;

    // Initialize context
    if (getcontext(&tcb->context) == -1) {
        free(tcb);
        return -1;
    }

    // Allocate stack
    tcb->stack = malloc(UTHREAD_STACK_SIZE);
    if (!tcb->stack) {
        free(tcb);
        return -1;
    }

    // Set up context to use the new stack and scheduler as link
    tcb->context.uc_stack.ss_sp   = tcb->stack;
    tcb->context.uc_stack.ss_size = UTHREAD_STACK_SIZE;
    tcb->context.uc_link          = &scheduler_context;

    // Save function and argument
    tcb->func = func;
    tcb->arg  = arg;

    // Arrange for stub to run first
    makecontext(&tcb->context, uthread_stub, 0);

    // Enqueue into ready queue
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
	(void)preempt;  // preemption not implemented in Phase 2

    // 1) Initialize ready queue
    ready_queue = queue_create();
    if (!ready_queue)
        return -1;

    // 2) Save scheduler context (so we can return here)
    if (getcontext(&scheduler_context) == -1) {
        queue_destroy(ready_queue);
        return -1;
    }

    // 3) Create the initial thread
    if (uthread_create(func, arg) == -1) {
        queue_destroy(ready_queue);
        return -1;
    }

    // 4) Scheduling loop: dequeue and run threads
    while (queue_dequeue(ready_queue, (void**)&current_thread) == 0) {
        swapcontext(&scheduler_context,
                    &current_thread->context);
    }

    // 5) All threads finished
    queue_destroy(ready_queue);
    return 0;
}

void uthread_block(void)
{
	/* TODO Phase 3 */
}

void uthread_unblock(struct uthread_tcb *uthread)
{
	/* TODO Phase 3 */
}

