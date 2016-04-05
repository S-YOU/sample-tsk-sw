/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  Inter thread message                                              */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_MESSAGE_H)
#define  _KERN_MESSAGE_H 
#include "kern/kern_types.h"
#include "kern/wqueue.h"

#define MSG_ENT_NR      (7)  /*< ��å������Хåե��Υ���ȥ��  */

#define MSG_RECV_ANY    (0)  /*< Ǥ�դΥ���åɤ���Υ�å�����������դ���  */

typedef struct _message{
	msg_ent_t    buf[MSG_ENT_NR]; /*< ��å���������                 */
}message_t;

typedef struct _message_buffer{
	tid_t         sender;        /*< ��������åɤΥ���å�ID       */
	tid_t       receiver;        /*< ��������åɤΥ���å�ID       */
	message_t        msg;        /*< ��å���������                 */
	wait_queue_t      wq;        /*< ����å��Ԥ����塼             */
	list_t          link;        /*< ������å��������塼�ؤΥ�� */
}message_buffer_t;

void msg_init_message_buffer(message_buffer_t *);
int  msg_send(tid_t dst, message_t *);
int  msg_recv(tid_t src, message_t *);
#endif  /*  _KERN_MESSAGE_H   */
