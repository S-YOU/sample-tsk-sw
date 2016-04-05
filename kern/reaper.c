/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  ����åɲ������                                                  */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

static thread_t *reaper_thread;
static reaper_thread_info_t reaper_info;

/** ����åɲ������å�
    @note ��λ��������åɤβ����Ԥ�
 */
static void
reaper(void *nouse) {
	thread_t *thr;
	psw_t psw;

	
	for(;;) {
		psw_disable_interrupt(&psw);
		/*
		 * ��λ��������åɤ��˴�����
		 */
		while (!thr_thread_queue_empty(&(reaper_info.reaper_queue))) {

			thr = thr_thread_queue_get_top(&(reaper_info.reaper_queue));
			thr_destroy_thread(thr);
		}
		wque_wait_on_queue(&(reaper_info.wq));  /*  �����׵᤬����ޤǵ�̲����  */
		psw_restore_interrupt(&psw);
	}
}
/** ��λ���֤Υ���åɤ���Ͽ����
    @param[in] thr ��λ��������å�
 */
void
reaper_add_exit_thread(thread_t *thr) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	if (thr->status != THR_TSTATE_EXIT)
		goto out;
	thr->status = THR_TSTATE_DEAD;  /* ����åɾ�����˴���ǽ�ʾ��֤����ܤ���    */

	thr_add_thread_queue(&(reaper_info.reaper_queue), thr);  /*< ���塼���ɲ�    */
	wque_wakeup(&(reaper_info.wq));                          /*< ����åɤ򵯾�  */
out:
	psw_restore_interrupt(&psw);
}

/** �������åɤν����
 */
void
reaper_init_thread(void){
	thread_attr_t attr;

	/*
	 * ������������
	 */
	thr_init_thread_queue(&(reaper_info.reaper_queue));
	wque_init_wait_queue(&(reaper_info.wq));
	
	/*
	 * ����å�����
	 */
	memset(&attr, 0, sizeof(thread_attr_t));

	attr.prio = REAPER_THREAD_PRIO;  /*< ͥ�����դ��Υ����ƥ�ץ���  */
	thrmgr_reserve_threadid(REAPER_THREAD_TID); /*< ����å�ID��ͽ�󤹤�  */

	thr_create_thread(&reaper_thread, &attr, reaper, NULL);

	thrmgr_put_threadid(reaper_thread->tid);  /*< ��ư���ꤵ�줿ID���ֵѤ���  */
	reaper_thread->tid = REAPER_THREAD_TID;   /*< ����å�ID�����ꤹ��  */
}
