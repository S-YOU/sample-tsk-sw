/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  thread context handling routines                                  */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

extern void thr_thread_start(void (*_fn)(void *), void   *_arg);

/** ����åɥ����å��˥���åɳ��ϴؿ�����򥻥åȤ���
    @param[in] thr  ����åɴ�������
    @param[in] fn   ����åɤγ��ϴؿ�
    @param[in] arg  ����åɤγ��ϴؿ��ΰ���
 */
void
hal_setup_thread_function(struct _thread *thr, void (*fn)(void *), void *arg) {
	addr_t *sp;
	thread_attr_t *attr = &thr->attr;

	sp = (addr_t *)thr_refer_thread_info(thr);
	--sp;  /* ����åɴ�������ΰ�ľ夫��������Ѥ߾夲��  */

	*sp-- = (addr_t)thr_thread_start;  /* x30 */
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;  /* x25  */
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;  /* x20  */
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;  /* x15 */
	*sp-- = (addr_t)0;  /* x8  */
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;  /* x5 */
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)0;
	*sp-- = (addr_t)arg;
	*sp = (addr_t)fn; /* x0 */

	attr->stack = sp;                 /* �����å��ݥ��󥿤򹹿�����  */
}


