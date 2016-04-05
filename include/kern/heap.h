/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  heap management relevant routines                                 */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_HEAP_H)
#define  _KERN_HEAP_H 
#include <stdint.h>

#include "kern/kern_types.h"
#include "param.h"
#include "kern/list.h"

typedef uint32_t flist_bitmap_t;                                    /*< �ӥåȥޥåפΥǡ�����  */

#define FREE_CHUNK_LIST_NR (sizeof(flist_bitmap_t) * BIT_PER_BYTE)  /*< ��������󥯥ꥹ�Ȥο�  */

struct _chunk;

/** ��������󥯴����ꥹ��
 */
typedef struct _free_chunks{
	flist_bitmap_t                 bitmap;   /*< �����ꥹ�ȤΥӥåȥޥå�  */
	list_head_t flist[FREE_CHUNK_LIST_NR];   /*< ��������󥯤Υꥹ��      */
}free_chunks_t;

/** �ҡ��״�������
 */
typedef struct _heap{
	list_t             list;            /*< �ҡ��פΥ�󥯾���    */
	free_chunks_t   fchunks;            /*< ��������󥯴�������  */
	struct _chunk    *chunk;            /*< ��Ƭ�����          */
	size_t             size;            /*< �ҡ��ץ�����          */
	uint8_t heap[HEAP_SIZE];            /*< �ҡ���                */
}heap_t;

typedef struct _heap_info{
	list_head_t list;                   /*< �ҡ��פΥꥹ��        */
}heap_info_t;

/** �ҡ��״�������ν�����ؼ���
    @param[in] var �ҡ��״���������ѿ�̾
 */
#define HEAP_INITIALIZER(var, sz) {		\
	.list = {&((var).list), &((var).list)},	\
	.fchunks = {0,{0,} },	 	        \
	.chunk = (chunk_t *)(&((var).heap[0])),	\
        .size = (sz),			        \
	.heap = {0, },			        \
 }

/** ��������󥯴���������������
    @param[in] h �ҡ��״�������
    @retval ��������󥯴�������Υ��ɥ쥹
 */
#define GET_FREE_CHUNK_LIST(h)						\
	((free_chunks_t *)(&((h)->fchunks)))

/** �ҡ��פ��礭���򻲾Ȥ���
    @param[in] h �ҡ��״�������
    @retval �ҡ��פ��礭��
 */
#define GET_HEAP_SIZE(h)						\
	((h)->size)

void init_heap(void);
int  size2index(size_t );
heap_t *refer_main_heap(void);
void print_chunks(heap_t *);
#endif  /*   _KERN_HEAP_H   */
