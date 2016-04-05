/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  thread ready queue                                                */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** ��ǥ������塼�˥���åɤ��ɲä���
    @param[in] rdq �ɲ���Υ�ǥ������塼
    @param[in] thr ����åɴ�������
 */
void 
rdq_add_thread(thread_ready_queue_t *rdq, thread_t *thr){
	psw_t psw;
	thread_attr_t *attr = &thr->attr;

	psw_disable_interrupt(&psw);
	list_add(&rdq->head[attr->prio], &thr->link);
	rdq->bitmap |= (1 << attr->prio);
	thr->rdq = rdq;
	psw_restore_interrupt(&psw);
}

/** ��ǥ������塼���饹��åɤ�������
    @param[in] rdq ������Υ�ǥ������塼
    @param[in] thr ����åɴ�������
    @note �ޥ���ץ��������θ����, ��ǥ������塼������˻��ꤹ��褦�ˤ��Ƥ���
 */
void 
rdq_remove_thread(thread_ready_queue_t *rdq, thread_t *thr){
	psw_t psw;
	thread_attr_t *attr = &thr->attr;

	psw_disable_interrupt(&psw);
	thr_unlink_thread(thr);
	if (list_is_empty(&rdq->head[attr->prio]))
		rdq->bitmap &= ~(1 << attr->prio);
	thr->rdq = NULL;
	psw_restore_interrupt(&psw);
}

/** �桼������åɤΥ�ǥ������塼���ž����
    @param[in] rdq ��ž�����ǥ������塼
 */
void 
rdq_rotate_queue(thread_ready_queue_t *rdq) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	list_rotate(&rdq->head[RDQ_USER_QUE_IDX]);
	psw_restore_interrupt(&psw);
}

/** ��ǥ������塼����ư���ǽ�ʥ���åɤ��������
    @param[in] rdq ��ǥ������塼
    @retval NULL ��ǥ������塼�������ä�
    @retval ����åɴ�������Υ��ɥ쥹 ��ǥ������塼��μ¹Բ�ǽ����å�
 */
thread_t *
rdq_find_runnable_thread(thread_ready_queue_t *rdq){
	psw_t psw;
	thread_t *thr;
	int idx;

	psw_disable_interrupt(&psw);

	idx = find_msr_bit(rdq->bitmap);
	if (idx == 0) {
		thr = NULL;
		goto out;
	}

	thr = CONTAINER_OF(list_ref_top(&rdq->head[rdq_index2prio(idx)]), thread_t, link);
	
out:
	psw_restore_interrupt(&psw);
	
	return thr;
}

/** ��ǥ������塼����������
    @param[in] que ��ǥ������塼��������
 */
void
rdq_init_ready_queue(thread_ready_queue_t *que) {
	int i;

	que->bitmap = 0;  /*< ư���ǽ�ʥ���åɤ��ʤ����֤ʤΤ�0�����ꤹ��  */
	for(i = 0; i < RDQ_PRIORITY_MAX; ++i){
		init_list_head(&que->head[i]);  /*< ��ͥ���٤Υ��塼����������  */
	}
}
