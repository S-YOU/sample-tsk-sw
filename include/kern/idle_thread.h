/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  idle thread                                                       */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_IDLE_THREAD_H)
#define  _KERN_IDLE_THREAD_H 
#include "kern/thread.h"

extern thread_t *current;  /*< �����ȥ���åɤؤΥݥ���  */

void do_idle_loop(void);
thread_t *idle_refer_idle_thread(void);
void idle_init_idle(void);
#endif  /*  _KERN_IDLE_THREAD_H   */
