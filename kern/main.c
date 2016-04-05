/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  main routine                                                      */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** �����ͥ�ν����
 */
void
kern_init(void) {
	
	init_heap();
	idle_init_idle();
	thrmgr_init_thread_manager();
	sched_init();

	reaper_init_thread();
	
	user_init();

	do_idle_loop();
}

#if !defined(CONFIG_HAL)
int 
main(int argc, char *argv[]) {

	init_pseudo_console();
	kern_init();
}
#endif  /*  CONFIG_HAL  */
