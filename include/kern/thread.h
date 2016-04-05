/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  thread definitions                                                */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_THREAD_H)
#define _KERN_THREAD_H

#include "kern/kern_types.h"
#include "kern/list.h"
#include "hal/hal_types.h"
#include "hal/addrinfo.h"

/** ����åɤξ���
 */
#define THR_TSTATE_RUN    (0)  /*< ���˥��� */
#define THR_TSTATE_WAIT   (1)  /*< ���Ԥ���   */
#define THR_TSTATE_EXIT   (2)  /*< ��λ��   */
#define THR_TSTATE_DEAD   (3)  /*< ��λ�Ԥ���   */

typedef uint64_t  preempt_state_t;  /*< ����åɥǥ����ѥå����ľ���  */

/** �ǥ����ѥå����ľ���
 */
#define THR_DISPATCH_ENABLE   ((preempt_state_t)(0x0))  /*< �ǥ����ѥå�����       */
#define THR_DISPATCH_DISABLE  ((preempt_state_t)(0x1))  /*< �ǥ����ѥå��ػ�       */
#define THR_DISPATCH_DELAYED  ((preempt_state_t)(0x2))  /*< �ٱ�ǥ����ѥå�ͽ���� */
#define THR_DISPATCH_MASK  ((preempt_state_t)(THR_DISPATCH_DISABLE|THR_DISPATCH_DELAYED)) /*< ���֥ޥ��� */

/** ����å�°������
 */
typedef struct _thread_attr{
	void         *stack_top;  /*< �����å�����Ƭ���ɥ쥹                  */
	void             *stack;  /*< �ץ��������å����Υ����å��ݥ���    */
	size_t       stack_size;  /*< �����å�������                          */
	int                prio;  /*< ͥ����                                  */
}thread_attr_t;

/** ����å�°������������
    @param[in] stk      �����å�����Ƭ���ɥ쥹
    @param[in] stk_size �����å��Υ�����
 */
#define THR_ATTR_INITIALIZER(stk, stk_size) {			\
	.stack = TRUNCATE_ALIGN(stk + stk_size - sizeof(thread_info_t) - 1, stk_size), \
	.stack_size = stk_size,			\
	}


struct _thread_ready_queue;

/** ����åɴ�������
 */
typedef struct _thread{
	thr_state_t        status;  /*< ����åɾ���                */
	tid_t                 tid;  /*< ����å�ID                  */
	list_t               link;  /*< ���塼�ؤΥ��            */
	list_t           mgr_link;  /*< ����åɴ����ѥ��        */
	struct _thread_ready_queue *rdq;  /*< ��ǥ������塼              */
	exit_code_t     exit_code;  /*< ��λ������                  */
	thread_attr_t        attr;  /*< °������                    */
	message_buffer_t  msg_buf;  /*< ��å�������������          */
	wait_queue_t      recv_wq;  /*< ��å����������Ԥ����塼    */
	wait_queue_t      send_wq;  /*< ��å������������Ԥ����塼  */
	list_head_t      recv_que;  /*< ��å��������塼            */
}thread_t;

#define THR_THREAD_INFO_MAGIC                  (0xdeadbeef)  /*< �����å�����򼨤��ޥ��å��ֹ�  */
/** ����åɤ���ǥ����塼�ˤĤʤ��äƤ��뤫��ǧ����
 */
#define THR_THREAD_ON_RDQ(thr) (thr->rdq != NULL)
/** ����åɴ�������(�����å���ʬ)
 */
typedef struct _thread_info{
	preempt_state_t	preempt;   /*< �ǥ����ѥå��ػ߾��ִ�������   */
	addr_t          magic;     /*< �����å������ɽ���ޥ��å��ֹ� */
}thread_info_t;

/** ����åɥ��塼
 */
typedef struct _thread_queue{
	list_head_t head;    /*< ����åɥ��塼�Υإå�  */
}thread_queue_t;

thread_info_t *thr_refer_thread_info(thread_t *);
void thr_thread_switch(thread_t *, thread_t *);
void thr_unlink_thread(thread_t *);
int thr_create_thread(thread_t **, thread_attr_t *, void (*start)(void *), void *);
void thr_exit_thread(int );
int thr_destroy_thread(thread_t *);
void thr_thread_start(void (*)(void *), void   *);
tid_t thr_get_tid(thread_t *);
tid_t thr_get_current_tid(void);

void thr_init_thread_queue(thread_queue_t *);
void thr_add_thread_queue(thread_queue_t *, thread_t *);
void thr_remove_thread_queue(thread_queue_t *, thread_t *);
int thr_thread_queue_empty(thread_queue_t *);
thread_t *thr_thread_queue_get_top(thread_queue_t *);

int thr_can_receive_message(thread_t *);
#endif  /*  _KERN_THREAD_H */
