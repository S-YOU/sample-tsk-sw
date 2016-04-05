/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  ready queue definitions                                           */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_READY_QUE_H)
#define _KERN_READY_QUE_H
#include "kern/list.h"

#define RDQ_SYS_PRIORITY  (7)  /*< �����ƥॹ��åɤ�ͥ����  */
#define RDQ_USER_PRIORITY (1)  /*< �桼������åɤ�ͥ����    */
#define RDQ_PRIORITY_MAX (RDQ_USER_PRIORITY + RDQ_SYS_PRIORITY)
#define RDQ_USER_QUE_IDX  (0)  /*< �桼������åɤΥ���ǥ���    */
/** ��ǥ������塼
 */
typedef struct _thread_ready_queue{
	int                         bitmap;    /*< ��ǥ������塼�Υӥåȥޥå�  */
	list_head_t head[RDQ_PRIORITY_MAX];    /*< ����åɥ��塼�Υإå�        */
}thread_ready_queue_t;

/** �ӥåȥޥåפΥ���ǥ�������ͥ���٤򻻽Ф���
    @note �ӥåȥޥåפΥ���ǥ�����, ư���ǽ����åɤʤ���0��ɽ�������,
    1����Ĥ����Ƥ���Τ�, -1����ͥ���٤򻻽Ф���
 */
#define rdq_index2prio(idx) ((idx) - 1)

void rdq_add_thread(thread_ready_queue_t *, thread_t *);
void rdq_remove_thread(thread_ready_queue_t *, thread_t *);
void rdq_rotate_queue(thread_ready_queue_t *);
thread_t *rdq_find_runnable_thread(thread_ready_queue_t *);
void rdq_init_ready_queue(thread_ready_queue_t *);
#endif  /*  _KERN_READY_QUE_H  */
