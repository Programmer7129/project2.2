#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <string.h>
#include <string.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100
static struct sigaction old_action;
static struct itimerval old_timer;
static bool preemption_enabled = false;

static void sighandler(int sig) {
    if (sig == SIGVTALRM) {
        uthread_yield();
    }
}

void preempt_disable(void)
{
	/* TODO Phase 4 */
	if (!preemption_enabled) return;

    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);

    // Block SIGVTALRM
    sigprocmask(SIG_BLOCK, &set, NULL);
	
}

void preempt_enable(void)
{
	if (!preemption_enabled) return;
	
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGVTALRM);

    // Unblock SIGVTALRM
    sigprocmask(SIG_UNBLOCK, &set, NULL);
}

void preempt_start(bool preempt)
{
	/* TODO Phase 4 */
	if (!preempt) return;

    preemption_enabled = true;

    // Set up signal handler using sigaction
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);         // Don't block any other signals
    sa.sa_flags = 0;

    if (sigaction(SIGVTALRM, &sa, &old_action) == -1) {
        perror("sigaction");
        exit(1);
    }

    // send SIGVTALRM at HZ (100 Hz → 10ms interval)
    struct itimerval timer;
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 1000000 / HZ; 
    timer.it_value = timer.it_interval;     

    if (setitimer(ITIMER_VIRTUAL, &timer, &old_timer) == -1) {
        perror("setitimer");
        exit(1);
    }
}

void preempt_stop(void)
{
	/* TODO Phase 4 */
	if (!preemption_enabled) return;

    // Restore old signal handler
    if (sigaction(SIGVTALRM, &old_action, NULL) == -1) {
        perror("sigaction restore");
        exit(1);
    }

    // Stop timer
    if (setitimer(ITIMER_VIRTUAL, &old_timer, NULL) == -1) {
        perror("setitimer restore");
        exit(1);
    }

    preemption_enabled = false;
}

