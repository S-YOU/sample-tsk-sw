/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  ��å���������                                                    */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** ����Υ���åɤ���Υ�å����������뤫��ǧ����
    @param[in] src �����դ��оݤΥ���å�ID
    @param[in] mbp ��å������Хåե��Υ��ɥ쥹�򻲾Ȥ���ݥ����ѿ��Υ��ɥ쥹
    @retval 0 ��å����������Ĥ���, mbp�����֤��줿�ݥ����ѿ��ǥ�å������Хåե��򻲾ȤǤ���褦�ˤ���
    @retval ENOENT ��å������ϸ��Ĥ���ʤ��ä�
 */
static int
msg_recv_from(tid_t src, message_buffer_t **mbp) {
	int               rc;
	message_buffer_t *mb;
	list_t           *lp;
	psw_t            psw;

	psw_disable_interrupt(&psw);
	list_for_each(lp, current, recv_que) {
		mb = CONTAINER_OF(lp, message_buffer_t, link);
		if (mb->sender == src) { /* ��å����������Ĥ��ä��Τ�, ��å���������Ф�  */
			list_del(lp);
			*mbp = mb;
			rc = 0;
			goto out;
		}
	}
	rc = ENOENT;
out:
	psw_restore_interrupt(&psw);
	return rc;
}


/** ��å������Хåե��ν����
    @param[in] mbuf ��å������Хåե��ؤΥݥ���
 */
void 
msg_init_message_buffer(message_buffer_t *mbuf){
	
	memset(mbuf, 0, sizeof(message_buffer_t));
	wque_init_wait_queue(&mbuf->wq);
	init_list_node(&mbuf->link);
}

/** ��å���������
    @param[in] dst �����襹��å�ID
    @param[in] msg ��å������Υ��ɥ쥹
    @retval     0  ���ｪλ
    @retval ESRCH  ����åɤ����Ĥ���ʤ��ä�
    @retval EINVAL ��ʬ���Ȥ��������褦�Ȥ���
 */
int  
msg_send(tid_t dst, message_t *msg){
	int               rc;
	psw_t            psw;
	thread_t        *thr;
	message_buffer_t *mb;

	mb = &current->msg_buf;
	psw_disable_interrupt(&psw);

	if (dst == current->tid) {
		rc = EINVAL;
		goto out;
	}
	
	rc = thrmgr_find_thread_by_tid(dst , &thr);
	if (rc != 0)
		goto out;

	memmove(&mb->msg, msg, sizeof(message_t));
	mb->sender = thr_get_current_tid();
	mb->receiver = dst;
	list_add(&thr->recv_que, &mb->link);
	
	/* ��꤬�����Ԥ��Ǥʤ����, ���Υ������ȥ��塼���Ԥ���碌��  */
	while (!thr_can_receive_message(thr)) {
		wque_wakeup(&thr->recv_wq);         /* ���Υ���åɤ򵯾�����  */
		wque_wait_on_queue(&thr->send_wq); /* ���������Ԥ����塼�ǵ�̲����  */
	}

	wque_wait_on_queue(&mb->wq); /* ��å������μ��դ��ԤĤ���˥�å������Υ��塼���Ԥ���碌��  */

out:
	psw_restore_interrupt(&psw);

	return rc;
}
/** ��å���������
    @param[in] src ����������å�ID
    @param[in] msg ��å������Υ��ɥ쥹
    @retval     0  ���ｪλ
    @retva EINVAL  ������åɤΥ�å��������Ԥ���碌�褦�Ȥ���
 */
int
msg_recv(tid_t src, message_t *msg) {
	int               rc;
	psw_t            psw;
	thread_t        *thr;
	message_buffer_t *mb;
	list_t           *lp;

	if (src ==thr_get_current_tid()) {
		rc = EINVAL;
		goto out;
	}
		
	psw_disable_interrupt(&psw);

	while ( (list_is_empty(&current->recv_que)) ||
	    ( ( src != MSG_RECV_ANY ) && (msg_recv_from(src, &mb) != 0 ) ) ) {
		/* �����Хåե��˥�å��������ʤ����,
		 * �����Ԥ�����åɤ򵯤����Ƶ�̲����
		 */
		wque_wakeup(&current->send_wq);         /* ���Υ���åɤ򵯾�����  */
		wque_wait_on_queue(&current->recv_wq);  /* ��ʬ�μ����Ԥ����塼�ǵ�̲����  */
	}

	if (src == MSG_RECV_ANY)  { /* Ǥ�դΥ���åɤΥ�å���������Ф�  */
		mb = CONTAINER_OF(list_get_top(&current->recv_que), message_buffer_t, link);
	} 

	memmove(msg, &mb->msg, sizeof(message_t));      /*  ��å��������ɤ߼��  */	

	wque_wakeup(&mb->wq);           /* ��å������μ������ԤäƤ��륹��åɤ򵯾�����  */
	
	rc = 0;
out:
	psw_restore_interrupt(&psw);

	return rc;
}
