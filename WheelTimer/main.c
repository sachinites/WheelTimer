#include "WheelTimer.h"
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>


#define MAX_NUMBER_APP_THREADS  	1
#define WHEEL_SIZE 			15
#define WHEEL_TIMER_CLOCK_TIC_INTERVAL  1 // sec

/*Import Library global Variables*/
extern blocked_pool_t gl_blocked_th_pool;

void
generate_general_query(int vlan){
	static int count = 0;
	printf("%s() : Query no : %d : general query is generated for vlan %d\n", __FUNCTION__, count++, vlan);
}

void wrapper_generate_general_query(void *arg, int arg_size){
	
	int vlan = *(int *)arg;
	generate_general_query(vlan);
}

void
generate_pim_hello(int vlan){
	static int count = 0;
	printf("%s() : Query no : %d : pim_hello is generated for vlan %d\n", __FUNCTION__, count++, vlan);
}

void
wrapper_generate_pim_hello(void *arg, int arg_size){
	int vlan = *(int *)arg;
	generate_pim_hello(vlan);
}

void
generate_ospf_hello(int vlan){
	static int count = 0;
	printf("%s() : Query no : %d : ospf_hello is generated for vlan %d\n", __FUNCTION__, count++, vlan);
}

void wrapper_generate_ospf_hello(void *arg, int arg_size){
	
	int vlan = *(int *)arg;
	generate_ospf_hello(vlan);
}

void
main_menu(wheel_timer_t *wt){
	wheel_timer_elem_t *gq_wt_elem = NULL,
			   *pim_hello_wt_elem = NULL,
			   *ospf_hello_wt_elem = NULL;

	do{
		printf("\nMain Menu\n");
		printf("*************\n");
		printf("1. dump Wheel Timer\n");
		printf("2. Register General Query Event\n");
		printf("3. Register Pim Hello Event\n");
		printf("4. Register Ospf  Hello Event\n");
		printf("5. Reset Wheel Timer\n");
		printf("6. de-Register General Query Event\n");
		printf("7. de-Register Pim Hello Event\n");
		printf("8. de-Register Ospf  Hello Event\n");
		printf("9. Exit\n");
		printf("10. Reschedule general query\n");
		printf("11. Reschedule pim hello\n");
		printf("12. Reschedule ospf hello\n");

		int choice;
		scanf("%d", &choice);
		switch(choice){
			case 10:
				{
					int new_time_interval = 0;
					printf("Enter new time interval : ");
					scanf("%d", &new_time_interval);
					wt_elem_reschedule(gq_wt_elem, new_time_interval);
				}
				break;
			case 11:
				{
					int new_time_interval = 0;
					printf("Enter new time interval : ");
					scanf("%d", &new_time_interval);
					wt_elem_reschedule(pim_hello_wt_elem, new_time_interval);
				}
				break;
			case 12:
				{
					int new_time_interval = 0;
					printf("Enter new time interval : ");
					scanf("%d", &new_time_interval);
					wt_elem_reschedule(ospf_hello_wt_elem, new_time_interval);
				}
				break;
			case 6:
				de_register_app_event(wt, gq_wt_elem);
				break;
			case 7:
				de_register_app_event(wt, pim_hello_wt_elem);
				break;
			case 8:
				de_register_app_event(wt, ospf_hello_wt_elem);
				break;
			case 1:
				print_wheel_timer(wt);
				break;
			case 2:
				{
					int vlan_no;
					printf("Enter vlan no : ");
					scanf("%d", &vlan_no);
					char is_recursive;
					printf("Is Recirsive (1/0) ? ");
					scanf("%d", (int *)&is_recursive);
					int time_interval;
					printf("time_interval ? ");
					scanf("%d", &time_interval);
					gq_wt_elem = register_app_event(wt, wrapper_generate_general_query, 
							   (void *)&vlan_no, sizeof(vlan_no), time_interval, is_recursive);

				}
				break;
			case 3:
				{
					int vlan_no;
					printf("Enter vlan no : ");
					scanf("%d", &vlan_no);
					char is_recursive;
					printf("Is Recirsive (1/0) ? ");
					scanf("%d", (int *)&is_recursive);
					int time_interval;
					printf("time_interval ? ");
					scanf("%d", &time_interval);
					pim_hello_wt_elem = register_app_event(wt, wrapper_generate_pim_hello, 
							   (void *)&vlan_no, sizeof(vlan_no), time_interval, is_recursive);

				}
				break;
			case 4:
				{
					int vlan_no;
					printf("Enter vlan no : ");
					scanf("%d", &vlan_no);
					char is_recursive;
					printf("Is Recirsive (1/0) ? ");
					scanf("%d", (int *)&is_recursive);
					int time_interval;
					printf("time_interval ? ");
					scanf("%d", &time_interval);
					ospf_hello_wt_elem = register_app_event(wt, wrapper_generate_ospf_hello, 
							   (void *)&vlan_no, sizeof(vlan_no), time_interval, is_recursive);

				}
				break;

			case 5:
				reset_wheel_timer(wt);
				break;
			case 9:
				exit(0);
			default:
				break;

		} 
	}
	while(1);
}

int
main(int argc, char **argv){

	init_blocked_pool(&gl_blocked_th_pool, MAX_NUMBER_APP_THREADS);
	wheel_timer_t *wt = 
			init_wheel_timer(WHEEL_SIZE, WHEEL_TIMER_CLOCK_TIC_INTERVAL);
	start_wheel_timer(wt);

	/* WT thread is in DETACHED MODE, so below call is useless*/
	//pthread_join(wt->wheel_thread->pthread_handle, &ret_val);

	main_menu(wt);
	return 0;
}

