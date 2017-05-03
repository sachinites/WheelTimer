#include "WheelTimer.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include <time.h>
#include "LinkedListApi.h"

#define TH_JOINABLE	1
#define TH_DETACHED	0

extern blocked_pool_t gl_blocked_th_pool;

wheel_timer_t*
init_wheel_timer(int wheel_size, int clock_tic_interval){
	wheel_timer_t *wt = calloc(1, sizeof(wheel_timer_t) + 
				wheel_size*sizeof(ll_t *));

	wt->clock_tic_interval = clock_tic_interval;
	wt->wheel_size = wheel_size;

	pthread_mutex_init(&wt->wheel_timer_mutex, NULL);
	wt->wheel_thread = calloc(1, sizeof(_pthread_t));
	pthread_init(wt->wheel_thread, 0, TH_DETACHED);

	int i = 0;
	for(; i < wheel_size; i++)
		wt->slots[i] = init_singly_ll();

	return wt;
}

void
de_register_app_event(wheel_timer_t *wt, wheel_timer_elem_t *wt_elem){
	if(!wt_elem) return;
	pause_wheel_timer(wt);
	wt_elem->is_alive = 0;
	resume_wheel_timer(wt);	
}


static void*
wheel_fn(void *arg){

	wheel_timer_t *wt = (wheel_timer_t *)arg;
	wheel_timer_elem_t *wt_elem = NULL;
	int absolute_slot_no = 0, i =0;
	ll_t *slot_list = NULL;
	singly_ll_node_t *head = NULL, *next_node = NULL;

	while(1){
		wt->current_clock_tic++;
		wt->current_clock_tic = (wt->current_clock_tic % wt->wheel_size);
		if(wt->current_clock_tic == 0)
			wt->current_cycle_no++;

		sleep(wt->clock_tic_interval);
		tentative_wait(wt->wheel_thread, &wt->wheel_timer_mutex);

		slot_list = wt->slots[wt->current_clock_tic];
		head = GET_HEAD_SINGLY_LL(slot_list);
		int node_count = GET_NODE_COUNT_SINGLY_LL(slot_list);
		absolute_slot_no = GET_WT_CURRENT_ABS_SLOT_NO(wt);
		printf("Wheel Timer Time = %d : ", absolute_slot_no * wt->clock_tic_interval);
		if(!node_count)
			printf("\n");
		for(i = 0; i < node_count; i++){
			next_node = GET_NEXT_NODE_SINGLY_LL(head);
			wt_elem = (wheel_timer_elem_t *)head->data;
		
			if(wt_elem->is_alive == 0){
				singly_ll_remove_node(slot_list, head);
				free_wheel_timer_element(wt_elem);	
				free(head);
				head = next_node;
				continue;
			}
	
			if(wt->current_cycle_no == wt_elem->execute_cycle_no){
				wt_elem->app_callback(wt_elem->arg, wt_elem->arg_size);
				if(wt_elem->is_recurrence){
					/*relocate*/
					int next_abs_slot_no  = absolute_slot_no + (wt_elem->time_interval/wt->clock_tic_interval);
					int next_cycle_no     = next_abs_slot_no / wt->wheel_size;
					int next_slot_no      = next_abs_slot_no % wt->wheel_size;
					wt_elem->execute_cycle_no 	 = next_cycle_no;
					if(next_slot_no == wt->current_clock_tic){
						head = next_node;
						continue; 
					}
					singly_ll_remove_node(slot_list, head);
					singly_ll_add_node(wt->slots[next_slot_no], head);
				}
				else{
					free_wheel_timer_element((wheel_timer_elem_t *)head->data);
					singly_ll_delete_node(slot_list, head);
				}
			}
			head = next_node;
		}
	}
	return NULL;
}

wheel_timer_elem_t *
register_app_event(wheel_timer_t *wt,
                   app_call_back call_back,
                   void *arg,
		   int arg_size,
                   int time_interval,
                   char is_recursive){

	if(!wt || !call_back) return NULL;
	wheel_timer_elem_t *wt_elem = calloc(1, sizeof(wheel_timer_elem_t));
	
	wt_elem->time_interval = time_interval;
	wt_elem->app_callback  = call_back;
	wt_elem->arg 	       = calloc(1, arg_size);
	memcpy(wt_elem->arg, arg, arg_size);
	wt_elem->arg_size      = arg_size;
	wt_elem->is_recurrence = is_recursive;

	/*Stop the Wheel timer Thread here*/
	pause_wheel_timer(wt);

	int wt_absolute_slot = GET_WT_CURRENT_ABS_SLOT_NO(wt);
	int registration_next_abs_slot = wt_absolute_slot + (wt_elem->time_interval/wt->clock_tic_interval);
	int cycle_no = registration_next_abs_slot / wt->wheel_size;
	int slot_no  = registration_next_abs_slot % wt->wheel_size;
	wt_elem->execute_cycle_no = cycle_no;
	wt_elem->is_alive = 1;
	singly_ll_add_node_by_val(wt->slots[slot_no], wt_elem);
	//printf("Wheel Timer snapshot on New Registration\n");
	//print_wheel_timer(wt);

	resume_wheel_timer(wt);
	return wt_elem;
}

void
free_wheel_timer_element(wheel_timer_elem_t *wt_elem){
	free(wt_elem->arg);
	free(wt_elem);
}


void
print_wheel_timer(wheel_timer_t *wt){
	int i = 0, j = 0;
	ll_t* slot_list = NULL;
	wheel_timer_elem_t *wt_elem = NULL;
	singly_ll_node_t *head = NULL;

	printf("Printing Wheel Timer DS\n");
	printf("wt->current_clock_tic  = %d\n", wt->current_clock_tic);
	printf("wt->clock_tic_interval = %d\n", wt->clock_tic_interval);
	printf("wt->wheel_size         = %d\n", wt->wheel_size);
	printf("wt->current_cycle_no   = %d\n", wt->current_cycle_no);
	printf("wt->wheel_thread       = %p\n", wt->wheel_thread);
	printf("printing slots : \n");

	for(; i < wt->wheel_size; i++){
		slot_list = wt->slots[i];
		printf("	slot_list[%d] : count : %d\n", i, GET_NODE_COUNT_SINGLY_LL(slot_list));
		head = GET_HEAD_SINGLY_LL(slot_list);
		for(j = 0 ; j < GET_NODE_COUNT_SINGLY_LL(slot_list); j++){
			wt_elem = (wheel_timer_elem_t *)head->data;
			if(!wt_elem){
				printf("	NULL\n");
				head = GET_NEXT_NODE_SINGLY_LL(head);
				continue;
			}
			printf("		wt_elem->time_interval		= %d\n",  wt_elem->time_interval);
			printf("                wt_elem->execute_cycle_no	= %d\n",  wt_elem->execute_cycle_no);
			printf("                wt_elem->app_callback		= %p\n",  wt_elem->app_callback);
			printf("                wt_elem->arg			= %p\n",  wt_elem->arg);
			printf("                wt_elem->is_recurrence		= %d\n",  wt_elem->is_recurrence);
			head = GET_NEXT_NODE_SINGLY_LL(head);
		}
	}
}


void
start_wheel_timer(wheel_timer_t *wt){

	_pthread_t *thread = wt->wheel_thread;
	if (pthread_create(&thread->pthread_handle, &thread->attr, wheel_fn, (void*)wt))
	{
		printf("Wheel Timer Thread initialization failed, exiting ... \n");
		exit(0);
	}
}

void
reset_wheel_timer(wheel_timer_t *wt){
	wt->current_clock_tic = 0;
	wt->current_cycle_no  = 0;
}

void
pause_wheel_timer(wheel_timer_t *wt){
	send_wait_order(wt->wheel_thread);
}

void
resume_wheel_timer(wheel_timer_t *wt){
	signal_t(wt->wheel_thread);	
}

void
wt_elem_reschedule(wheel_timer_elem_t *wt_elem, int new_time_interval){
	if(!wt_elem)	return;
	if(new_time_interval <= 0)
		return;

	wt_elem->time_interval = new_time_interval;
}
