/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  thread management routines                                        */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

static thread_manager_t all_threads;
static id_bitmap_t thread_id_map = ID_BITMAP_INITIALIZER;  /*< ����å�ID�Υס���  */


/** ����åɥޥ͡��������������
 */
static void
thread_manager_init(thread_manager_t *tm) {

	init_list_head(&tm->head);
}

/** ����åɥޥ͡�����Υ��ɥ쥹���������
 */
thread_manager_t *
thrmgr_refer_thread_manager(void) {
	return &all_threads; 
}

/** ����åɥޥ͡�����˥���åɤ��ɲä���
    @param[in] tm ����åɥޥ͡�����Υ��ɥ쥹
    @param[in] thr ����åɴ�������
 */
void
thrmgr_thread_manager_add(thread_manager_t *tm, thread_t *thr) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	list_add(&tm->head, &thr->mgr_link);
	psw_restore_interrupt(&psw);
}
/** ����åɥޥ͡����㤫�饹��åɤ������
    @param[in] tm ����åɥޥ͡�����Υ��ɥ쥹
    @param[in] thr ����åɴ�������
 */
void
thrmgr_thread_manager_remove(thread_manager_t *tm, thread_t *thr) {
	psw_t psw;

	psw_disable_interrupt(&psw);
	list_del(&thr->mgr_link);
	psw_restore_interrupt(&psw);
}

/** ����å�ID���饹��åɴ������������
    @param[in] tid  �����оݥ���åɤΥ���å�ID
    @param[in] thrp ����åɴ�������򻲾Ȥ���ݥ����ѿ��Υ��ɥ쥹
    @retval 0       ���ｪλ
    @retval ESRCH   ����åɤ����Ĥ���ʤ��ä�
 */
int
thrmgr_find_thread_by_tid(tid_t tid, thread_t **thrp) {
	int     rc;
	list_t *ptr;
	psw_t psw;
	thread_manager_t *tm;
	thread_t *thr;

	tm = thrmgr_refer_thread_manager();
	
	psw_disable_interrupt(&psw);

	rc = ESRCH;
	if (list_is_empty(&tm->head)) 
		goto out;

	list_for_each(ptr, tm, head) {
		thr = CONTAINER_OF(ptr, thread_t, mgr_link);
		if (thr->tid == tid) {
			rc = 0;
			*thrp = thr;
			goto out;
		}
	}

out:
	psw_restore_interrupt(&psw);
	return rc;
}


/** ����å�ID��ͽ��
    @param[in] id ͽ�󤹤�ID
    @retval EBUSY ���˻��Ѥ���Ƥ���
 */
int
thrmgr_reserve_threadid(tid_t id) {
	return reserve_id(&thread_id_map, id);
}

/** ����å�ID�γ���
    @param[in] tidp ����å�ID�ֵ��ΰ�
    @retval ENOENT ID��Ȥ��ڤä�
 */
int
thrmgr_get_threadid(tid_t *tidp) {
	int rc;

	rc = get_new_id(&thread_id_map, tidp);

	return rc;
}

/** ����å�ID�γ���
    @param[in] tid  �������륹��å�ID
 */
void
thrmgr_put_threadid(tid_t tid) {

	put_id(&thread_id_map, tid);

	return;
}

/** ����åɴ����Ϥν����
 */
void
thrmgr_init_thread_manager(void) {

	thread_manager_init(&all_threads);
}
