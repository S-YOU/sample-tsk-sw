/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  type definitions                                                  */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_KERN_TYPES_H)
#define _KERN_KERN_TYPES_H
#include <stdint.h>
#include <stddef.h>

#define BIT_PER_BYTE       (8)  /*<  �Х��ȥޥ��������              */

typedef uint64_t         obj_id_t;  /*< ID����                       */
typedef uint64_t      exit_code_t;  /*< ����åɽ�λ������           */
typedef int           thr_state_t;  /*< ����åɾ���                 */
typedef char *            caddr_t;  /*< �������ɥ쥹�򼨤���         */
typedef uint64_t        msg_ent_t;  /*< ��å�����1����ȥ�Υ�����  */
typedef obj_id_t            tid_t;  /*< ����å�ID    */
#endif  /*  _KERN_KERN_TYPES_H  */
