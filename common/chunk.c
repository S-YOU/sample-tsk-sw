/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  �ҡ������������(Buddy���르�ꥺ��ˤ��������)         */
/*                                                                    */
/**********************************************************************/
#include <errno.h>

#include "kern/kernel.h"

/** ��������󥯤Υ��������ꥹ�Ȥ���³��ǽ����ǧ����
 */
static int
is_chunk_linkable(chunk_t *c) {
	int rc;
	int idx;

	idx = find_msr_bit_in_size(CHUNK_AREA_SIZE(c));
	
	if ( (CHUNK_AREA_SIZE(c) == 0) || ( !CHUNK_IS_ROUNDUP(c) ) )  {
		rc = EINVAL;  /* ��������, 0�ޤ���, �������ñ�̤�2�ѤˤʤäƤ��ʤ�  */
		goto out;
	}

	if ( idx > FREE_CHUNK_LIST_NR ) {
		rc = E2BIG;  /* ���������礭������  */
		goto out;
	}

	rc = 0;
out:
	return rc;
}

/** �����ꥹ�Ȥ˶����ΰ褬���뤫Ĵ�٤�
    @param[in] hp  �ҡ��״�������
    @param[in] idx �����ꥹ�ȤΥ���ǥå���
    @retval �� �����ꥹ�Ȥ˶����ΰ褬����
    @retval �� �����ꥹ�Ȥ˶����ΰ褬�ʤ�
 */
static inline int
has_free_chunk(heap_t *hp, int idx) {
	free_chunks_t *free_list = GET_FREE_CHUNK_LIST(hp);

	return !(list_is_empty(&free_list->flist[idx]));
}


/** ��������󥯤򥢥꡼�ʤζ�������󥯥ꥹ�Ȥ��ɲä���
    @param[in]             c ����󥯾���
    @retval 0 ����˥ꥹ�Ȥ��ɲäǤ���
    @retval EINVAL ��������, 0�ޤ���, �������ñ�̤�2�ѤˤʤäƤ��ʤ� 
    @retval E2BIG  ���������礭������
    (EINVAL, E2BIG��, is_chunk_linkable������ֵ���)
 */
static int
add_free_chunk(chunk_t *c) {
	int rc;
	int idx;
	heap_t *hp;
	free_chunks_t *free_list;

	rc = is_chunk_linkable(c);
	if (rc != 0)  /* �ե꡼�ꥹ�Ȥ���³���ˤ��äƤ��ʤ�  */
		goto out;

	hp = CHUNK2HEAP(c);
	free_list = GET_FREE_CHUNK_LIST(hp);
	idx = size2index(CHUNK_AREA_SIZE(c));
	
	/*
	 * �����ΰ�ꥹ�ȤˤĤʤ�
	 */
	/* TODO:�����ΰ�������¾��ɬ��  */
	list_add(&free_list->flist[idx], &c->free);
	free_list->bitmap |=  (flist_bitmap_t)(1 << idx);

	rc = 0;
out:
	return rc;
}

/** ����󥯤򥢥꡼�ʤζ�������󥯥ꥹ�Ȥ��鳰��
    @param[in]             c ����󥯾���
    @retval 0 ����˥ꥹ�Ȥ��鳰����
    @retval EINVAL ��������, 0�ޤ���, �������ñ�̤�2�ѤˤʤäƤ��ʤ� 
    @retval E2BIG  ���������礭������
    (EINVAL, E2BIG��, is_chunk_linkable������ֵ���)
 */
static int
remove_free_chunk(chunk_t *c) {
	int rc;
	int idx;
	heap_t *hp;
	free_chunks_t *free_list;

	rc = is_chunk_linkable(c);
	if (rc != 0)  /* �ե꡼�ꥹ�Ȥ���³���ˤ��äƤ��ʤ�  */
		goto out;

	hp = CHUNK2HEAP(c);
	free_list = GET_FREE_CHUNK_LIST(hp);
	idx = size2index(CHUNK_AREA_SIZE(c));
	
	/*
	 * �����ΰ�ꥹ�Ȥ��鳰��
	 */
	/* TODO:�����ΰ�������¾��ɬ��  */
	list_del(&c->free);
	if (list_is_empty(&free_list->flist[idx]))
		free_list->bitmap &= ~((flist_bitmap_t)( 1 << idx ));

	rc = 0;
out:
	return rc;
}

/** �����ꥹ�Ȥ���Ƭ���Ǥ���Ф�
    @param[in] hp  �ҡ��״�������
    @param[in] idx �����ꥹ�ȤΥ���ǥå���
    @retval ��NULL �����ꥹ�Ȥ���Ƭ�����
    @retval NULL   ��������󥯤����ꤵ�줿����ǥ����Υꥹ�Ȥ�¸�ߤ��ʤ�
 */
static chunk_t *
get_first_chunk(heap_t *hp, int idx) {
	free_chunks_t *free_list = GET_FREE_CHUNK_LIST(hp);
	chunk_t *c;

	c = NULL;
	if (has_free_chunk(hp, idx)) {
		c = CONTAINER_OF(list_ref_top(&free_list->flist[idx]),
		    chunk_t, free);
		remove_free_chunk(c);
	}

	return c;
}

/** chunk���ΰ�Ĺ��̾�����
    @param[in]             c ����󥯾���
    @param[in] new_area_size ����󥯤Υ��ꥢ������
    @retval 0      ���ｪλ
    @retval ENOSPC �ΰ�ν̾��˼��Ԥ���
    @note chunk�Υ桼�����ꥵ�������ѹ��������, resize_chunk����Ѥ�,
    �ܴؿ���ľ�ܻ��Ѥ��ʤ�����.
 */
static int
chunk_shrink_size(chunk_t *c, size_t new_area_size, chunk_t **new_chunk_p) {
	int                rc;
	chunk_t          *new;
	chunk_t         *next;
	size_t free_area_size;

	next = CHUNK_NEXT(c);
	free_area_size = CHUNK_AREA_SIZE(c) - new_area_size;  /*  �̾��ˤ�äƤǤ��������ΰ�Υ����� */

	if ( free_area_size < ALLOC_UNIT_SHIFT ) {  /*  �ΰ�ν̾����Ǥ��ʤ����  */
		rc = ENOSPC;
		goto out;
	}

	if (CHUNK_IS_FREE(c))  /*  ��������󥯤ξ���, ���������ѹ������Τ�, �ꥹ�Ȥ��鳰��  */
		remove_free_chunk(c);

	/*
	 �ΰ�̾���
	 |             |                |
	 |<-----c----->|<-���Υ����->|
	 |             |                |
	 �ΰ�̾���
	 |     |       |                |
	 |<-c->|<-new->|<-���Υ����->|
	 |     |       |                |
	 [����󥯤Υꥹ�Ⱦ���]
	 new->prev = �̾����c�Υ��ꥢ������
	 new->next = �����ˤǤ�������󥯤Υ��ꥢ������
	 */
	new = (chunk_t *)(((void *)c) + new_area_size);  /*  �̾��ˤ�äƤǤ��������ΰ�  */
	CHUNK_INIT(new, free_area_size);                 /*  �̾��ˤ�äƤǤ��������ΰ�ξ���򥻥åȤ���  */
	new->prev_size = new_area_size;                  /*  �̾���Υ����������ΰ��c����Ƭ���ɥ쥹��¸�ߤ���  */
	CHUNK_MAKE_FREE(new);                            /* �����ΰ�����ꤹ��  */		

	next->prev_size = free_area_size;                /* �����ݥ��󥿤򹹿�����  */

	CHUNK_SET_AREA_SIZE(new, free_area_size);        /* ���������ΰ�Υ��ꥢ�����������ꤹ��  */
	CHUNK_SET_AREA_SIZE(c, new_area_size);           /* �̾���Υ��������˿��������ΰ褬¸�ߤ���  */
	*new_chunk_p = new;                              /* ���������ΰ���ֵѤ���  */

	if (CHUNK_IS_FREE(c))  /*  ���������ѹ����줿�Τ�, �ꥹ�ȤˤĤʤ�ľ��  */
		add_free_chunk(c);

	rc = 0;

out:
	return rc;
}

/** ��������󥯤�16��2�Τ٤����ʬ�䤹��
    @param[in] c ����󥯴�������Υ��ɥ쥹
 */
static void
fixup_chunk(chunk_t *c) {
	int             rc;
	size_t        size;
	chunk_t *new_chunk;

	/* ���ߤΥ������������ñ�̤�2�Ѥ��ڤ�ͤ᤿�ΰ�(CHUNK_TRANC_SIZE(c))��
	 * ���ߤΥ���󥯤θ��ˤĤ��뤳�Ȥ�, ������ñ�̤�2�Ѥǥ���󥯤���Ǥ���褦�ˤ���.
	 * ���Τ����, ���ߤΥ��ꥢ����������˺��������ΰ�ʬ�����̾�����.
	 */
	for(rc = 0, size = CHUNK_AREA_SIZE(c);
	    (!CHUNK_IS_ROUNDUP(c)) && (size > ALLOC_UNIT_SHIFT);
	    size = CHUNK_AREA_SIZE(c)) {
		rc = chunk_shrink_size(c, CHUNK_AREA_SIZE(c) - CHUNK_TRANC_SIZE(c), &new_chunk);
		if (rc == 0)
			add_free_chunk(new_chunk);
	}
	
}
/** ����󥯤��ΰ�Ĺ���ѹ�����
    @param[in] c ����󥯴�������Υ��ɥ쥹
    @param[in] s �ѹ���Υ���󥯤Υ��ꥢ������
    @retval 0      ���ｪλ
    @retval ENOSPC �ΰ�ν̾��˼��Ԥ���(chunk_shrink_size�����ֵ�)
 */
static int 
resize_chunk(chunk_t *c, size_t s) {
	int               rc;
	chunk_t   *new_chunk;
	size_t new_area_size;
	size_t old_area_size;

	new_area_size = size2alloc(s);
	if ( CHUNK_AREA_SIZE(c) > new_area_size ) {  /*  �ΰ��̾�������  */
		rc = chunk_shrink_size(c, new_area_size, &new_chunk);
		if (rc != 0)
			goto out;
		if (!CHUNK_IS_ROUNDUP(new_chunk)) {
			fixup_chunk(new_chunk); /*  ��������󥯤������ñ�̤�2�Ѥ���������  */
		}
		if ( CHUNK_IS_FREE(new_chunk) && (!list_is_linked(&new_chunk->free)) )
			add_free_chunk(new_chunk);
	}

	rc = 0;

out:
	return rc;
}
/** �����ΰ���������
    @param[in] hp �����ΰ���������ҡ���
    @param[in] s  �桼���ΰ����������
    @param[in] cp ������������󥯤�ؤ������ݥ����ѿ��Υ��ɥ쥹
    @retval      0 ���ｪλ
    @retval ENOMEM �������꤬�ʤ�
    @retval ENOSPC ��������ν̾����˼��Ԥ���
 */
int
get_free_chunk(heap_t *hp, size_t s, chunk_t **cp) {
	int            rc;
	int           idx;
	size_t alloc_size;
	chunk_t        *c;
	psw_t psw;

	psw_disable_interrupt(&psw);
	/*
	 * ��������󥯥ꥹ�Ȥ���γ������ߤ�
	 */
	alloc_size = size2alloc(CALC_AREA_SIZE(s));
	for(idx = size2index(alloc_size);
	    !has_free_chunk(hp, idx); 
	    ++idx) {
		if (idx >= FREE_CHUNK_LIST_NR) {
			rc = ENOMEM;  /*  �����ΰ褬¸�ߤ��ʤ�  */
			goto error_out;
		}
	}
	c = get_first_chunk(hp, idx);	

	rc = 0;
	CHUNK_MAKE_USED(c);     /* ������ˤ���        */
	if ( CHUNK_AREA_SIZE(c) ==  alloc_size ) {
		goto out;  /*  �׵᥵����ʬ���ΰ���ڤ�Ф���  */
	}

	rc = resize_chunk(c, alloc_size);  /* ��������̾�������  */

out:
	psw_restore_interrupt(&psw);
	*cp = c;
	return rc;

error_out:
	psw_restore_interrupt(&psw);
	return rc;
}
/** ����󥯤�������
    @param[in] c ������������
    @retval 0 ���ｪλ
    @retval EINVAL ��������, 0�ޤ���, �������ñ�̤�2�ѤˤʤäƤ��ʤ� 
    @retval E2BIG  ���������礭������
    (EINVAL, E2BIG��, remove_free_chunk������ֵ���)
 */
int
free_chunk(chunk_t *c) {
	int rc;
	chunk_t *buddy;
	chunk_t *self = c;
	psw_t psw;

	psw_disable_interrupt(&psw);

	CHUNK_MAKE_FREE(self);

	for (buddy = CHUNK_GET_BUDDY(self); 
	       CHUNK_IS_FREE(buddy) && CHUNK_BUDDY_MATCHED(self) ;
	     buddy = CHUNK_GET_BUDDY(self) ) {

		rc = remove_free_chunk(buddy);  /*  �Хǥ�������ꥹ�Ȥ�����  */
		if (rc != 0)  /*  ������ԤˤĤ�����ޤǷ�礷������󥯤�ե꡼�ꥹ�Ȥ���³  */
			goto free_out;

		self = CHUNK_GET_EVEN_SIDE(self);  /*  self�����¦�Υ���󥯤˹���  */

		/*  ����󥯤���  */
		CHUNK_SET_AREA_SIZE(self, CHUNK_AREA_SIZE(self) * 2);

		if ( CHUNK_IS_LAST(self) )
			break;  /*  ��礷������󥯤��ǽ�����󥯤ʤ�ȴ����  */
	}

	rc = 0;

free_out:
	add_free_chunk(self);
	psw_restore_interrupt(&psw);

	return rc;
}

/** �ҡ��פ˽������󥯤����ꤹ��
    @param[in] hp �ҡ��״�������
    @param[in] s  �������󥯤��礭��
 */
void
setup_heap_chunks(heap_t *hp, size_t s) {
	chunk_t *c = (chunk_t *)&(hp->heap[0]);
	psw_t psw;

	psw_disable_interrupt(&psw);
	CHUNK_INIT(c, HEAP_SIZE);

	hp->chunk = c;
	add_free_chunk(c);
	psw_restore_interrupt(&psw);

	return;
}
