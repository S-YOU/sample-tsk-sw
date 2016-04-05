/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  �����ͥ륳�󥽡������                                            */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

static kconsole_list_t kcons = KCONSOLE_LIST_INITILIZER(kcons);

/** ��ʸ�����ϴؿ�
    @param[in] ch ���Ϥ���ʸ��
 */
void
kputchar(int ch){
	psw_t psw;
	kconsole_list_t *kcp = &kcons;
	kconsole_t *con;
	list_t *cp;

	psw_disable_interrupt(&psw);
	list_for_each(cp, kcp, head) {
		con = CONTAINER_OF(cp, kconsole_t, link);
		if (con->putchar != NULL) {
			con->putchar(ch);
		}
	}
	psw_restore_interrupt(&psw);
}

/** ���̥��ꥢ
 */
void
kcls(void){
	psw_t psw;
	kconsole_list_t *kcp = &kcons;
	kconsole_t *con;
	list_t *cp;

	psw_disable_interrupt(&psw);
	list_for_each(cp, kcp, head) {
		con = CONTAINER_OF(cp, kconsole_t, link);
		if (con->cls != NULL) {
			con->cls();
		}
	}
	psw_restore_interrupt(&psw);
}

/** ����������ֹ���
    @param[in] x X��ɸ
    @param[in] y Y��ɸ
 */
void
klocate(int x, int y){
	psw_t psw;
	kconsole_list_t *kcp = &kcons;
	kconsole_t *con;
	list_t *cp;

	psw_disable_interrupt(&psw);
	list_for_each(cp, kcp, head) {
		con = CONTAINER_OF(cp, kconsole_t, link);
		if (con->locate != NULL) {
			con->locate(x, y);
		}
	}
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
