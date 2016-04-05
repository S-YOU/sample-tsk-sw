/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  thread context handling routines                                  */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

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
	*sp-- = (addr_t)arg;
	*sp-- = (addr_t)fn;
	*sp-- = (addr_t)thr_exit_thread;  /* thr_exit thread����륳��ƥ����Ȥ�ǰ�Τ����Ѥ�Ǥ���  */
	*sp = (addr_t)__start_thread;     /* thread_start�ؿ��ƤӽФ��ѤΥ��롼 */
	attr->stack = sp;                 /* �����å��ݥ��󥿤򹹿�����  */
}


