/*
 * =====================================================================================
 *
 *       Filename:  timerlib.h
 *
 *    Description: This file is a wrapper over Timer POSIX Timer library 
 *
 *        Version:  1.0
 *        Created:  10/12/2020 01:47:16 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ABHISHEK SAGAR (), sachinites@gmail.com
 *   Organization:  Juniper Networks
 *
 * =====================================================================================
 */

#ifndef __TIMER_WRAP__
#define __TIMER_WRAP__

#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef enum{

	TIMER_INIT,
	TIMER_DELETED,
	TIMER_PAUSED,
	TIMER_CANCELLED,
	TIMER_RESUMED,
	TIMER_RUNNING,
} TIMER_STATE_T;

typedef struct Timer_{

	/* Timer config */
    timer_t posix_timer;
    void *user_arg;
	unsigned long exp_timer;		/* in milli-sec */
	unsigned long sec_exp_timer;	/* in milli-sec */
	uint32_t thresdhold;			/* No of times to invoke the timer callback */
	void (*cb)(struct Timer_ *, void *); /* Timer Callback  */
	bool exponential_backoff;

	/* place holder value to store
 	 * dynamic attributes of timer */
	unsigned long time_remaining;	/* Time left for paused timer for next expiration */
	uint32_t invocation_counter; 
	struct itimerspec ts;
	unsigned long exp_back_off_time;
	TIMER_STATE_T timer_state;
} Timer_t;

Timer_t*
setup_timer(
    void (*timer_cb)(Timer_t*, void *), /*   Timer Callback with user data*/
    unsigned long exp_timer,            /*   First expiration time interval in msec */
    unsigned long sec_exp_timer,        /*   Subsequent expiration time interval in msec */
    uint32_t threshold,                 /*   Max no of expirations, 0 for infinite*/
    void *user_arg,                     /*   Arg to timer callback */
    bool exponential_backoff);          /*   Is Timer Exp backoff*/


static inline TIMER_STATE_T
timer_get_current_state(Timer_t *timer){

    return timer->timer_state;
}

static inline void
timer_set_state(Timer_t *timer, TIMER_STATE_T timer_state){

    timer->timer_state = timer_state;
}

void
resurrect_timer(Timer_t *timer);

void
start_timer(Timer_t *timer);

#endif /* __TIMER_WRAP__  */
