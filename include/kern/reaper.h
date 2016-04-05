/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  reaper thread                                                     */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_REAPER_H)
#define  _KERN_REAPER_H 
#include "kern/thread.h"
#include "kern/wqueue.h"

#define REAPER_THREAD_TID  (1)  /*< �������åɤΥ���å�ID  */
#define REAPER_THREAD_PRIO (7)  /*< �������åɤ�ͥ����      */
typedef struct _reaper_thread_info{
	thread_queue_t reaper_queue;  /*< ����оݤΥ���å�  */
	wait_queue_t             wq;  /*< ����׵��Ԥ����塼  */
}reaper_thread_info_t;

void reaper_add_exit_thread(thread_t *);
void reaper_init_thread(void);
#endif  /*  _KERN_REAPER_H   */
