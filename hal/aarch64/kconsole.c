/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  �����ͥ륳�󥽡������                                            */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

static kconsole_list_t kcons = KCONSOLE_LIST_INITILIZER(kcons);
volatile unsigned int * const UART0DR = (unsigned int *) 0x09000000;
 
/** ��ʸ�����ϴؿ�
    @param[in] ch ���Ϥ���ʸ��
 */
void
kputchar(int ch){
	psw_t psw;

	psw_disable_interrupt(&psw);
	*UART0DR = (unsigned int)(ch); /* Transmit char */
	psw_restore_interrupt(&psw);
}

/** �����ͥ륳�󥽡�����Ͽ
    @param[in] conp ���󥽡������
    @retval     0  ���ｪλ
    @retval EINVAL conp��NULL�ޤ���putchar�ϥ�ɥ餬��Ͽ����Ƥ��ʤ�
 */
int 
register_kconsole(kconsole_t *conp) {
	int    rc;
	psw_t psw;	
	kconsole_list_t *kcp = &kcons;
	
	psw_disable_interrupt(&psw);
	if ( (conp == NULL) || (conp->putchar == NULL) ) {
		rc = EINVAL;
		goto out;
	}
	list_add(&kcp->head, &conp->link);
	rc = 0;
out:
	psw_restore_interrupt(&psw);
	return rc;
}

/** �����ͥ륳�󥽡�����Ͽ����
    @param[in] conp ���󥽡������
 */
void
unregister_kconsole(kconsole_t *conp) {
	int    rc;
	psw_t psw;	
	kconsole_list_t *kcp = &kcons;
	
	psw_disable_interrupt(&psw);
	list_del(&conp->link);
	psw_restore_interrupt(&psw);

	return ;
}
