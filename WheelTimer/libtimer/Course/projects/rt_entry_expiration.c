/*
 * =====================================================================================
 *
 *       Filename:  rt_entry_expiration.c
 *
 *    Description: This file illustrates how to age out rt table entry 
 *
 *        Version:  1.0
 *        Created:  10/27/2020 04:59:45 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  ABHISHEK SAGAR (), sachinites@gmail.com
 *   Organization:  Juniper Networks
 *
 * =====================================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include "rt.h"

static rt_table_t rt;

static void
rt_entry_delete_on_timer_expiry(Timer_t *timer, void *user_data){

	rt_entry_t *rt_entry = (rt_entry_t *)user_data;
	delete_timer(rt_entry->exp_timer);
	free(rt_entry->exp_timer);
	rt_entry->exp_timer = NULL;
	assert(rt_delete_rt_entry(&rt, rt_entry->rt_entry_keys.dest, 
								   rt_entry->rt_entry_keys.mask));
}

int
main(int argc, char **argv){

	rt_init_rt_table(&rt);

	rt_add_new_rt_entry(&rt, "122.1.1.1", 32, "10.1.1.1", "eth0", rt_entry_delete_on_timer_expiry);
    rt_add_new_rt_entry(&rt, "122.1.1.2", 32, "10.1.1.2", "eth1", rt_entry_delete_on_timer_expiry);
    rt_add_new_rt_entry(&rt, "122.1.1.3", 32, "10.1.1.3", "eth2", rt_entry_delete_on_timer_expiry);
    rt_add_new_rt_entry(&rt, "122.1.1.4", 32, "10.1.1.4", "eth3", rt_entry_delete_on_timer_expiry);
    rt_add_new_rt_entry(&rt, "122.1.1.5", 32, "10.1.1.5", "eth4", rt_entry_delete_on_timer_expiry);

	while(1){
		printf("1. Add rt table entry\n");
		printf("2. Update rt table entry\n");
		printf("3. Delete rt table entry\n");
		printf("4. Dump rt table\n");

		int choice;
		printf("Enter Choice :");
		scanf("%d", &choice);
		fflush(stdin);
        switch(choice){
            case 1:
                {
                    char dest[16];
                    uint8_t mask;
                    char oif[32];
                    char gw[16];
                    printf("Enter Destination :");
                    scanf("%s", dest);
                    printf("Mask : ");
                    scanf("%hhd", &mask);
                    printf("Enter oif name :");
                    scanf("%s", oif);
                    printf("Enter Gateway IP :");
                    scanf("%s", gw);
                    if(!rt_add_new_rt_entry(&rt, dest, mask,
                        gw, oif, rt_entry_delete_on_timer_expiry)){
                        printf("Error : Could not add an entry\n");
                    }
				}
				break;
			case 2:
				break;
			case 3:
				break;
			case 4:
				rt_dump_rt_table(&rt);
			default:
				;
		}
	}
	return 0;
}
