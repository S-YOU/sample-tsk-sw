/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  thread relevant  routines                                         */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** �ǥ����ѥå����ľ��֤����ꤹ��
    @param[in] tinfo ����åɾ���Υ��ɥ쥹
 */
static void
enable_dispatch(thread_info_t *tinfo) {

	tinfo->preempt &= ~THR_DISPATCH_DISABLE;
}

/** �ǥ����ѥå��ػ߾��֤����ꤹ��
    @param[in] tinfo ����åɾ���Υ��ɥ쥹
 */
static void
disable_dispatch(thread_info_t *tinfo) {

	tinfo->preempt |= THR_DISPATCH_DISABLE;
}

/** �ٱ�ǥ����ѥå�ͽ���Ω�Ƥ�
    @param[in] tinfo ����åɾ���Υ��ɥ쥹
 */
static void
set_delay_dispatch(thread_info_t *tinfo) {

	tinfo->preempt |= THR_DISPATCH_DELAYED;
}

/** �ٱ�ǥ����ѥå�ͽ��򥯥ꥢ����
    @param[in] tinfo ����åɾ���Υ��ɥ쥹
 */
static void
clr_delay_dispatch(thread_info_t *tinfo) {

	tinfo->preempt &= ~THR_DISPATCH_DELAYED;
}

/** �ٱ�ǥ����ѥå�ͽ��򥯥ꥢ����
    @param[in] tinfo ����åɾ���Υ��ɥ쥹
 */
static void
clr_thread_info(thread_info_t *tinfo) {

	tinfo->preempt &= ~THR_DISPATCH_MASK;
}

/** ����åɤΥ����å����ɥ쥹�����ꤹ��
    @param[in] thr       ����åɴ�������
    @param[in] stack_top �����å�����Ƭ���ɥ쥹
    @param[in] size      �����å��Υ�����
 */
static void
set_thread_stack(thread_t *thr, void *stack_top, size_t size) {
	thread_attr_t *attr = &thr->attr;
	thread_info_t *tinfo;

	attr->stack_top = stack_top;
	attr->stack_size = size;
	tinfo = thr_refer_thread_info(thr);
	clr_thread_info(tinfo);
	tinfo->magic = THR_THREAD_INFO_MAGIC;
	attr->stack = (void *)TRUNCATE_ALIGN(((void *)(tinfo)) - 1, MALIGN_SIZE);  /* �����å���������  */
}

/** ����åɤ򳫻Ϥ���
 */
void
thr_thread_start(void (*fn)(void *), void   *arg){

	__psw_enable_interrupt();  /* �����ߤ���Ĥ���  */
	fn(arg);                /* ����åɳ��ϴؿ���ƤӽФ�  */
	thr_exit_thread(0);        /* ����åɳ��ϴؿ����鵢�ä��鼫����åɤ�λ����  */
	/* �����ˤ���ʤ�. */

	return ;
}

/** ����åɴ�������Υ�󥯤��곰��
    @param[in] thr ����åɴ�������
 */
void
thr_unlink_thread(thread_t *thr){
	psw_t psw;

	psw_disable_interrupt(&psw);
	list_del(&thr->link);
	psw_restore_interrupt(&psw);
}

/** ����åɴ�������Υ����å�������ʬ�򻲾Ȥ���
    @param[in] thr ����åɴ�������
 */
thread_info_t *
thr_refer_thread_info(thread_t *thr) {
	thread_attr_t *attr = &thr->attr;

	return 	(thread_info_t *)(attr->stack_top + attr->stack_size - sizeof(thread_info_t));
}

/** ����åɤΥ���ƥ����ȥ����å���Ԥ�
    @param[in] prev �����å�����륹��åɤΥ���åɴ�������
    @param[in] next �����å����륹��åɤΥ���åɴ�������
 */
void 
thr_thread_switch(thread_t *prev, thread_t *next) {

	hal_do_context_switch(&(((thread_attr_t *)(&prev->attr))->stack),
	    &(((thread_attr_t *)(&next->attr))->stack));
	return;
}
/** ����åɤ���������
    @param[in] thrp  ����åɴ�������Υݥ����ѿ��Υ��ɥ쥹
    @param[in] attrp ����å�°������
    @param[in] start ����åɳ��ϴؿ��Υ��ɥ쥹
    @param[in] arg   ����åɳ��ϴؿ��ΰ���
    @retval 0 ���ｪλ
    @retval ENOENT ����å�ID�򤹤٤ƻȤ��ڤä�
    @retval ENOMEM ����å�������ɬ�פʥ��꤬��­���Ƥ���
 */
int
thr_create_thread(thread_t **thrp, thread_attr_t *attrp, void (*start)(void *), void *arg){
	int               rc;
	void   *thread_stack;
	thread_t        *thr;
	size_t    stack_size;

	/*
	 * �����å��Υ��åȥ��å�
	 */
	if ( (attrp == NULL) || (attrp->stack_top == NULL) || (attrp->stack_size == 0) ) {

		stack_size = STACK_SIZE;
		thread_stack = kmalloc(stack_size);
		if (thread_stack == NULL) {
			rc = ENOMEM;
			goto out;
		}
	} else {
		thread_stack = attrp->stack_top;
		stack_size = attrp->stack_size;
	}

	thr = kmalloc(sizeof(thread_t));
	if (thr == NULL) {
		rc = ENOMEM;
		goto free_stack_out;
	}

	memset(thr, 0, sizeof(thread_t));

	set_thread_stack(thr, thread_stack, stack_size);

	rc = thrmgr_get_threadid(&thr->tid);
	if (rc != 0)
		goto free_thread_out;  /* ID�����˼��Ԥ���  */

	/*
	 * °������Υ����å�
	 */
	if ( (attrp != NULL) &&
	    ( (attrp->prio < RDQ_PRIORITY_MAX) && (attrp->prio > RDQ_USER_QUE_IDX) ) )
		(&thr->attr)->prio = attrp->prio;  /*  ͥ���٤����ꤹ��  */
	else
		(&thr->attr)->prio = 0;

	hal_setup_thread_function(thr, start, arg);  /* ����åɳ��ϥ��ɥ쥹�����ꤹ��  */
	
	init_list_node(&thr->link);
	thr->exit_code = 0;

	/*
	 * ��å����������������ν����
	 */
	wque_init_wait_queue(&thr->recv_wq);
	wque_init_wait_queue(&thr->send_wq);
	init_list_head(&thr->recv_que);
	msg_init_message_buffer(&thr->msg_buf);

	thr->status = THR_TSTATE_RUN;

	thrmgr_thread_manager_add(thrmgr_refer_thread_manager(), thr);

	sched_set_ready(thr);  /* ��ǥ������塼���ɲä���  */

	rc = 0;
	*thrp = thr;
	
out:
	return rc;

free_thread_out:
	kfree(thr);

free_stack_out:
	kfree(thread_stack);

	return rc;
}

/** ������åɤν�λ
    @param[in] code ��λ������
 */
void
thr_exit_thread(int code){
	psw_t psw;

	psw_disable_interrupt(&psw);
	thr_unlink_thread(current);   /* ��ǥ������塼���鳰��  */
	current->exit_code = (exit_code_t)code; /* ��λ�����ɤ�����  */
	current->status = THR_TSTATE_EXIT;  /* ����åɤ�λ���֤ˤ���  */
	thrmgr_thread_manager_remove(thrmgr_refer_thread_manager(), current);  /* ����åɰ������������  */

	reaper_add_exit_thread(current);  /*< �������åɤ˥���åɤβ������ꤹ��  */
out:	
	psw_restore_interrupt(&psw);

	sched_schedule();  /* ������åɽ�λ��ȼ���������塼��  */

	return;
}

/** ����åɤ��˴�
   @param[in] thr �˴����륹��åɤΥ���åɴ������� 
   @retval EBUSY ��λ���Ƥ��ʤ�����åɤ��˴����褦�Ȥ���
 */
int
thr_destroy_thread(thread_t *thr){
	int rc;
	psw_t psw;
	thread_attr_t *attr = &thr->attr;

	psw_disable_interrupt(&psw);
	if ( thr->status != THR_TSTATE_DEAD ) {
		rc = EBUSY;
		goto out;
	}

	thrmgr_put_threadid(thr->tid); /* ID���ֵ�          */
	kfree(attr->stack_top);  /* �����å��γ���          */
	kfree(thr);              /* ����åɴ�������γ���  */
out:	
	psw_restore_interrupt(&psw);
	return rc;
}

/** ����å�ID�γ���
    @param[in] thr ����åɴ�������
    @retval    ����å�ID
 */
tid_t
thr_get_tid(thread_t *thr) {

	return thr->tid;
}

/** ������å�ID�γ���
    @param[in] thr ����åɴ�������
    @retval    ����å�ID
 */
tid_t
thr_get_current_tid(void) {

	return current->tid;
}

/** ����åɥ��塼�ν����
    @param[in] que ������оݤΥ���åɥ��塼
 */
void
thr_init_thread_queue(thread_queue_t *que) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	init_list_head(&que->head);
	psw_restore_interrupt(&psw);
}


/** ����åɥ��塼�˥���åɤ��ɲä���
    @param[in] que �ɲ���Υ���åɥ��塼
    @param[in] thr ����åɴ�������
 */
void 
thr_add_thread_queue(thread_queue_t *que, thread_t *thr){
	psw_t psw;

	psw_disable_interrupt(&psw);
	list_add(&que->head, &thr->link);
	psw_restore_interrupt(&psw);
}

/** ����åɥ��塼���饹��åɤ��곰��
    @param[in] que �ɲ���Υ���åɥ��塼
    @param[in] thr ����åɴ�������
 */
void 
thr_remove_thread_queue(thread_queue_t *que, thread_t *thr){
	psw_t psw;

	psw_disable_interrupt(&psw);
	list_del(&thr->link);
	psw_restore_interrupt(&psw);
}

/** ����åɥ��塼�����Ǥ��뤳�Ȥ��ǧ����
    @param[in] que ��ǧ�оݤΥ���åɥ��塼
    @retval �� ����åɥ��塼�����Ǥ���
    @retval �� ����åɥ��塼�����Ǥʤ�
 */
int 
thr_thread_queue_empty(thread_queue_t *que) {
	int rc;
	psw_t psw;

	psw_disable_interrupt(&psw);
	rc = list_is_empty(&que->head);
	psw_restore_interrupt(&psw);

	return rc;
}

/** ����åɥ��塼����Ƭ���Ǥ���Ф�
    @param[in] que ���Ф��оݤΥ���åɥ��塼
    @return    ����åɴ�������Υ��ɥ쥹  ����åɥ��塼����Ƭ���Ǥ�������
    @return    NULL                        ����åɥ��塼�����ξ��
 */
thread_t *
thr_thread_queue_get_top(thread_queue_t *que) {
	thread_t *thr;
	psw_t psw;
	
	psw_disable_interrupt(&psw);

	if (list_is_empty(&que->head)) 
		thr =  NULL;
	else 
		thr = CONTAINER_OF(list_get_top(&que->head), thread_t, link);
	
	psw_restore_interrupt(&psw);

	return thr;
}

/** ��å����������Ԥ���Ǥ��뤳�Ȥ��ǧ����
    @param[in] thr ����åɴ�������
    @retval �� ��å����������Ԥ���Ǥ���
    @retval �� ��å����������Ԥ���Ǥʤ�
 */
int
thr_can_receive_message(thread_t *thr) {
	int rc;
	psw_t psw;
	
	psw_disable_interrupt(&psw);

	rc = is_wque_empty(&thr->recv_wq);

	psw_restore_interrupt(&psw);
	return rc;
}

