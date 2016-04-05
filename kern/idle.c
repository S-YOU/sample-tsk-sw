/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  �����ɥ륹��å�                                                  */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

static thread_t idle_thread;        /* �����ɥ륹��åɤΥ���åɴ�������            */
thread_t *current = &idle_thread;   /* �����Ȥν���ͤ򥢥��ɥ륹��åɤ����ꤹ��  */

/** �����ɥ륹��å�
    @note ��λ��������åɤβ����Ԥ�
 */
void
do_idle_loop(void) {
	thread_t *thr;
	psw_t psw;

	for(;;) {
		sched_schedule();
	}
}

/** �����ɥ륹��åɤΥ���åɴ���������ֵѤ���
 */
thread_t *
idle_refer_idle_thread(void) {

	return &idle_thread;
}

/** �����ɥ륹��åɤν����
 */
void
idle_init_idle(void){
	thread_t *thr = &idle_thread;

	thrmgr_reserve_threadid(0);
	idle_thread.tid = 0;
}
