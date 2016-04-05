/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  �ҡ��״���                                                        */
/*                                                                    */
/**********************************************************************/
#include "kern/kernel.h"

static heap_t global_heap = HEAP_INITIALIZER(global_heap, HEAP_SIZE);  /*< ���ҡ���  */

/** �ե꡼�ꥹ�Ȥ���������
    @param[in] fc �ե꡼�ꥹ�Ⱦ���Υ��ɥ쥹
 */
static void
init_free_chunk_bitmap(free_chunks_t *fc) {
	int i;
	psw_t psw;

	psw_disable_interrupt(&psw);
	memset(&fc->bitmap,0, sizeof(flist_bitmap_t));
	for(i = 0;i < FREE_CHUNK_LIST_NR; ++i) {
		init_list_head(&fc->flist[i]);
	}
	psw_restore_interrupt(&psw);
}

/** �ҡ��״����������������
    @param[in] hp   �ҡ��״�������Υ��ɥ쥹
    @param[in] size �ҡ����ΰ�Ĺ
 */
static void
init_heap_node(heap_t *hp, size_t size) {
	chunk_t *c;
	psw_t psw;

	psw_disable_interrupt(&psw);

	init_list_node(&hp->list);
	memset(&hp->heap, 0, size);
	hp->size  = size;
	hp->chunk = (chunk_t *)&(hp->heap[0]);
	init_free_chunk_bitmap(&hp->fchunks);
	setup_heap_chunks(hp, size);

	psw_restore_interrupt(&psw);
}

/** ���ꥵ��������ӥåȥޥåפΥ���ǥ����򻻽Ф���
    @param[in] size �������ꥵ����
    @return �ӥåȥޥåפΥ���ǥ���
 */
int
size2index(size_t size) {
	int           idx;
	size_t alloc_size;
	size_t       mask;

	alloc_size = roundup_size(size); /* �׵���ꥵ���������������ñ�̤��ڤ�夲��  */

	idx = find_msr_bit_in_size(alloc_size) - INDEX_SHIFT; /* �Ǿ�̥ӥåȤ򸵤˥���ǥ������֤򻻽�  */

        /*
	 * �Ǿ�̥ӥåȰʳ���Ω�äƤ����飱���礭���������������Ƥ�  
	 */
	mask = index2size(idx) - 1;  
	if (alloc_size & mask)
		++idx;
	
	return idx;
}

/** �ҡ�����Υ���󥯤ξ��֤�ɽ������
    @param[in] hp �ҡ��״�������
 */
void
print_chunks(heap_t *hp) {
	chunk_t *c;
	psw_t psw;

	psw_disable_interrupt(&psw);
	kprintf("[HEAP:%p]\n", hp->chunk);
	for(c = hp->chunk; ;c = CHUNK_NEXT(c)) {
		kprintf("(chunk, size, attr, prev, next, side, buddy) = "
		       "(0x%lx, %lu, 0x%x, 0x%lx, 0x%lx, %s, 0x%lx)\n",
		       c, CHUNK_AREA_SIZE(c), CHUNK_ATTR(c), CHUNK_PREV(c), CHUNK_NEXT(c), 
		       ( (CHUNK_BUDDY_SIDE(c) == CHUNK_BUDDY_ODD) ? ("ODD") : ("EVEN") ),
		       CHUNK_GET_BUDDY(c));

		kprintf("(heap-size, chunk-pos, chunk-size, chunk-last)=(%lu, %lu, %lu, %lu)\n",
		       GET_HEAP_SIZE(CHUNK2HEAP(c)), CHUNK_OFFSET_POS(c), CHUNK_AREA_SIZE(c), 
		       CHUNK_OFFSET_POS(c) + CHUNK_AREA_SIZE(c));

		if (CHUNK_IS_LAST(c))
			break;
	}
	psw_restore_interrupt(&psw);
}

/** �ᥤ��ҡ��פδ���������ֵѤ���
    @return �ᥤ��ҡ��פδ�������Υ��ɥ쥹
 */
heap_t *
refer_main_heap(void) {

	return &global_heap;
}

/** �ᥤ��ҡ��פδ����������������
 */
void
init_heap(void) {

	init_heap_node(&global_heap, HEAP_SIZE);
	return;
}
