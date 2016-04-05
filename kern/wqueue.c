/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  wait queue relevant routines                                      */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** ����åɤ��Ԥ����塼���ɲä���
    @param[in] wq  �Ԥ����塼
    @param[in] ep  �Ԥ����塼�Υ���ȥ�
    @param[in] thr �Ԥ����֤�����륹��å�
 */
void 
wque_add_thread(wait_queue_t *wq, wait_queue_entry_t *ep, struct _thread  *thr){
	psw_t psw;

	psw_disable_interrupt(&psw);

	list_del(&thr->link);
	init_list_node(&ep->link);

	ep->thr = thr;
	list_add(&wq->head, &ep->link);

	psw_restore_interrupt(&psw);
}

/** �Ԥ����塼���饨��ȥ�������
    @param[in] wq  �Ԥ����塼
    @param[in] ep �Ԥ����塼�Υ���ȥ�
 */
void 
wque_remove_entry(wait_queue_t *wq, wait_queue_entry_t *ep){
	psw_t psw;

	psw_disable_interrupt(&psw);

	list_del(&ep->link);
	init_list_node(&ep->link);
	ep->thr = NULL;

	psw_restore_interrupt(&psw);
}

/** �Ԥ����塼����������
    @param[in] wq  �Ԥ����塼
    @param[in] ent �Ԥ����塼�Υ���ȥ�
    @param[in] thr �Ԥ����֤�����륹��å�
 */
void 
wque_init_wait_queue(wait_queue_t *wq) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	init_list_head(&wq->head);
	psw_restore_interrupt(&psw);
}

/** ������åɤ��Ԥ����塼����ꤷ�ƻ��ݤ��Ԥ���碌��
    @param[in] wq  �Ԥ����塼
 */
void 
wque_wait_on_queue(wait_queue_t *wq) {
	psw_t psw;
	wait_queue_entry_t ent, *ep = &ent;

	init_list_node(&ep->link);

	psw_disable_interrupt(&psw);
	if (THR_THREAD_ON_RDQ(current))
		rdq_remove_thread(current->rdq, current);
	wque_add_thread(wq, ep, current);
	
	ep->thr->status = THR_TSTATE_WAIT;

	sched_schedule() ;

	wque_remove_entry(wq, ep);

	psw_restore_interrupt(&psw);
}

/** �Ԥ����塼�ǵ�̲���Ƥ��륹��åɤ򵯾�����
    @param[in] wq  �Ԥ����塼
 */
void 
wque_wakeup(wait_queue_t *wq) {
	psw_t                psw;
	wait_queue_entry_t   *ep;

	psw_disable_interrupt(&psw);
	while(!list_is_empty(&wq->head)) {
		ep = CONTAINER_OF(list_get_top(&wq->head), wait_queue_entry_t, link);

		list_del(&ep->link);
		init_list_node(&ep->link);
		ep->thr->status = THR_TSTATE_RUN;
		sched_set_ready(ep->thr); 
		ep->thr = NULL;
	}
	
	psw_restore_interrupt(&psw);
}

/** �������ȥ��塼�����Ǥ��뤳�Ȥ��ǧ����
    @param[in] wq �������ȥ��塼
    @retval �� �������ȥ��塼�����Ǥ���
    @retval �� �������ȥ��塼�����Ǥʤ�
 */
int  
is_wque_empty(wait_queue_t *wq) {
	int    rc;
	psw_t psw;
	
	psw_disable_interrupt(&psw);
	rc = list_is_empty(&wq->head);
	psw_restore_interrupt(&psw);
	
	return rc;
}
