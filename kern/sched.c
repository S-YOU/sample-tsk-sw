/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  �������塼��                                                      */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

static thread_ready_queue_t rd_queue;

/** �¹Բ�ǽ�ʥ���åɤ����򤹤�
    @retval NULL �������塼���ǽ�ʥ���åɤ����ʤ�(��ǥ������塼����)
    @retval ����ư����륹��åɤΥ���åɴ�������
 */
static thread_t *
select_next_thread(void) {
	psw_t psw;
	thread_t *next;

	psw_disable_interrupt(&psw);
	next = rdq_find_runnable_thread(&rd_queue);
	if (next == NULL)
		next = idle_refer_idle_thread();  /* �¹Բ�ǽ�ʥ���åɤ��ʤ����, �����ɥ륹��åɤ�����  */	
out:
	psw_restore_interrupt(&psw);

	return next;
}

/** ����åɤ򥹥����塼���ǽ�ˤ���
    @param[in] thr ����åɴ�������
 */
void
sched_set_ready(thread_t *thr) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	rdq_add_thread(&rd_queue, thr);  /* ��ǥ������塼���ɲä���  */
	psw_restore_interrupt(&psw);
}

/** ��ǥ������塼���ž����
 */
void
sched_rotate_queue(void) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	rdq_rotate_queue(&rd_queue);
	psw_restore_interrupt(&psw);	
}

/** �������塼������
 */
void
sched_schedule(void) {
	psw_t psw;
	thread_t *prev, *next;

	psw_disable_interrupt(&psw);

	prev = current;
	next = select_next_thread(); 
	if (prev == next) {/* �ǥ����ѥå�����ɬ�פʤ�  */
		goto out;
	}
	current = next;
	thr_thread_switch(prev, next);

	current->status = THR_TSTATE_RUN ;
out:
	psw_restore_interrupt(&psw);	
}

/** �������塼��ν����
 */
void
sched_init(void) {

	rdq_init_ready_queue(&rd_queue);
}
