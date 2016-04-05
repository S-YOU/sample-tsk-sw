/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  wait queue definitions                                            */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_WQUEUE_H)
#define  _KERN_WQUEUE_H 
#include "kern/list.h"

struct _thread;

/** �������ȥ��塼
 */
typedef struct _wait_queue{
	list_head_t head;   /*< �Ԥ����塼�Υإå�  */
}wait_queue_t;

typedef struct _wait_queue_entry{
	list_t          link;   /*< �Ԥ����塼�إåɤؤΥ��   */
	struct _thread  *thr;   /*< ����åɴ�������ؤΥݥ��� */
}wait_queue_entry_t;


void wque_add_thread(wait_queue_t *, wait_queue_entry_t *, struct _thread  *);
void wque_remove_entry(wait_queue_t *, wait_queue_entry_t *);
void wque_init_wait_queue(wait_queue_t *);

void wque_wait_on_queue(wait_queue_t *);
void wque_wakeup(wait_queue_t *);
int  is_wque_empty(wait_queue_t *);
#endif  /*  _KERN_WQUEUE_H   */
