/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  OS��malloc���󥿡��ե�����                                        */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** ��������ؿ�
    @param[in] size �������������
    @return    ������������Υ桼���ΰ�
    @note 0�Х��ȤǤ�Ǿ����������ñ�̤������Ƥ�
 */
void *
kmalloc(size_t size) {
	int     rc;
	heap_t *hp = refer_main_heap();
	size_t   s = (size == 0) ? (MALIGN_SIZE) : (size);  
	chunk_t *c;
	
	rc = get_free_chunk(hp, s, &c);
	if (rc != 0)
		goto error_out;

	return CHUNK2PTR(c);

error_out:
	return NULL;
}
/** �������������ΰ��������
    @param[in] m ������������ΰ�
 */
void
kfree(void *m) {

	free_chunk(PTR2CHUNK(m));
}

