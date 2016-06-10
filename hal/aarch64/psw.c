/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  Processor Status Word                                             */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** CPU��٥�ǳ���ߤ�ػߤ���
    @param[in] pswp �����߶ػ����Υץ������ơ���������ֵѰ�
 */
void
psw_disable_interrupt(psw_t *pswp) {
	psw_t psw;

	__save_psw(psw);
	*pswp = psw;
}

/** CPU��٥�ǳ���߾��֤���������
    @param[in] pswp �ץ������ơ���������ֵѰ�
 */
void
psw_restore_interrupt(psw_t *pswp) {
	psw_t psw;

	psw = *pswp;
	__restore_psw(psw);
}
