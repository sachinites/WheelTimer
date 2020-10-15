/*
 * =====================================================================================
 *
 *       Filename:  timerExample.c
 *
 *    Description: This file demonstrates the use of POSIX Timer routines
 *
 *        Version:  1.0
 *        Created:  10/12/2020 11:25:06 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ABHISHEK SAGAR (), sachinites@gmail.com
 *   Organization:  Juniper Networks
 *
 * =====================================================================================
 */

/* To work with the posix timers
 * below two hdr files are necessary 
 **/
#include <signal.h>
#include <time.h>

/* Other standard hdr files we would need */
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h> /* for pause() */
#include <stdint.h>
#include <stdbool.h>

static void
print_current_system_time(){

	time_t t;
	time(&t); 	/*  Get the current system time */
	
	/* Print the current system time,
 	 * Will insert one \n character */
	printf("%s ",ctime(&t)); 
}

/* Example */
typedef struct pair_{

	int a;
	int b;
} pair_t;

pair_t pair = { 10, 20 };

/* The Timer callback function which will be called every
 * time the timer expires. The signature of the function would be :
 * void <fn-name>(union sigval)
 * */
void
timer_callback(union sigval arg){

	print_current_system_time();

	pair_t *pair = (pair_t *) arg.sival_ptr; /*Extract the user data structure*/

	printf("pair : [%u %u]\n", pair->a, pair->b);
}

void
timer_demo(){

	int ret;
	struct sigevent evp;

	/* You can take it as a local variable if you
 	 * wish, in that case we will not free it in
 	 * timer handler fn */
	timer_t timer;
	memset(&timer, 0, sizeof(timer_t));

	/* evp variable is used to setup timer properties*/
	memset(&evp, 0, sizeof(struct sigevent));
	
	/* Fill the the user defined data structure.
 	 * When timer expires, this will be passed as
 	 * argument to the timer callback handler */
	evp.sigev_value.sival_ptr = (void *)&pair;

	/* On timer Expiry, We want kernel to launch the
 	 * timer handler routine in a separate thread context */
	evp.sigev_notify = SIGEV_THREAD;
	
	/* Register the timer hander routine. This routine shall
 	 * be invoked when timer expires*/
	evp.sigev_notify_function = timer_callback; 

	/* Create a timer. It is just a timer initialization, Timer
 	 * is not fired (Alarmed)  */
	ret = timer_create (CLOCK_REALTIME,
						&evp,
						&timer);

	if ( ret < 0) {
		
		printf("Timer Creation failed, errno = %d\n", errno);
		exit(0);
	}

	/* Let us setup the time intervals */

	struct itimerspec ts;

	/* I want the timer to fire for the first time after 5 seconds
 	 * and 0 nano seconds*/
	ts.it_value.tv_sec = 5;
	ts.it_value.tv_nsec = 0;

	ts.it_interval.tv_sec = 3;
	ts.it_interval.tv_nsec = 0;

	/* Now start the timer*/
	ret = timer_settime (timer,
						 0,
						 &ts,
						 NULL);

	if ( ret < 0) {
		
		printf("Timer Start failed, errno = %d\n", errno);
		exit(0);
	}
	else {
		print_current_system_time();
		printf("Timer Alarmed Successfully\n");
	}
}

int
main(int argc, char **argv){

	timer_demo();
	pause();
	return 0;
}
