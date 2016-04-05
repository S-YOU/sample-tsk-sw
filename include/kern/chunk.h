/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  memory chunk relevant routines                                    */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_CHUNK_H)
#define  _KERN_CHUNK_H 

#include "kern/list.h"
#include "kern/heap.h"
#include "hal/addrinfo.h"

/** �������ñ��/�������ñ��
 */
#define ALLOC_UNIT_SHIFT                    (4)  /*<  ����γ������ñ�̤�16( = 2^4 )�Х��� �Ȥ���  */
#define INDEX_SHIFT      (ALLOC_UNIT_SHIFT + 1)  /*<  ��������������ꥹ�ȥӥåȥޥåפΥ���ǥ�����Ф����ե���  */
#define ALLOC_UNIT        (1<<ALLOC_UNIT_SHIFT)  /*<  �������ñ��  */

/** ����󥯤ξ��֥ӥå�(������������2�ӥåȤ���Ѥ���)
 */
#define CHUNK_USE_BIT    (0x0)  /*< ������Ǥ��뤳�Ȥ򼨤�  */
#define CHUNK_FREE_BIT   (0x1)  /*< �����ΰ�Ǥ��뤳�Ȥ򼨤�  */
#define CHUNK_EXTRA_BIT  (0x2)  /*< �������ȥ饢�꡼�ʤΥ���󥯤Ǥ��뤳�Ȥ�ɽ��  */
#define CHUNK_ATTR_MASK (CHUNK_FREE_BIT|CHUNK_EXTRA_BIT)

/** �Хǥ��ΰ��ִط�
 */
#define CHUNK_BUDDY_EVEN (0x0)  /*< ����¦�Хǥ�  */
#define CHUNK_BUDDY_ODD  (0x1)  /*< ����¦�Хǥ�  */

/** ����󥯥ǡ�����¤
 */
typedef struct _chunk{
	size_t  prev_size;   /*< ���Υ���󥯤ޤǤΥ�����  */
	size_t       size;   /*< �إå���ޤ��ΰ�Ĺ(=���Υ���󥯤ޤǤΥ�����)  */
	list_t       free;   /*< �����ꥹ�ȴ�������ӳ�����ƻ��ˤ�, free���Фΰ��֤��������Ƥ�  */
}chunk_t;

/** �������Υ桼�������ΰ�ޤǤΥ��ե��å�
 */
#define CHUNK_MEM_OFFSET ((size_t)(&(((chunk_t *)(0))->free)))

/** �����ꥹ�ȤΥӥåȥޥåפΥ���ǥå����������󥯥�������Ф�
    @param[in] idx ����ǥ���
    @return ����󥯥�����
 */
#define index2size(idx) (ALLOC_UNIT * ( 1UL << (idx) ) )  

/** �׵�����̤����������ñ�̤��ڤ�夲��
    @param[in] s �׵᥵����
    @return ����������ꥵ����
 */
#define roundup_size(s)				   \
	( ((s) == 0) ? (MALIGN_SIZE) :		   \
	    (ADDR_NOT_ALIGNED((s), MALIGN_SIZE)) ? \
	    ( ROUNDUP_ALIGN((s), MALIGN_SIZE) ) :  \
		((size_t)(s)) )

/** �����ƥ��ꥵ�����򻻽Ф���
    @param[in] s ����󥯤Υ��ꥢ������
    @return ������Ƥ���ꥵ����
    @note s�ˤ�, chunk�إå���Υ桼���ΰ賫�ϰ��֤ޤǤ�Ĺ�� + �桼���ΰ�Ĺ�򥢥ɥ쥹�������ǥ饦��ɥ��åפ���Ĺ��
    ��Ϳ����(CALC_AREA_SIZE�λ�����). �ܥޥ����, s��Ĺ�����Ǽ��ǽ�ʥ���������ñ�̤�2�Ѥǻ��Ф���
 */
#define size2alloc(s)					\
	(index2size(size2index(s)))

/** ����󥯴������󤫤�桼���ΰ�Υ��ɥ쥹������
   @param[in] c ����󥯴�������Υ��ɥ쥹
   @return �桼���ΰ�Υ��ɥ쥹
 */
#define CHUNK2PTR(c)						\
	((void *)( (void *)(c) + CHUNK_MEM_OFFSET ))

/** �桼���ΰ���Ƭ���ɥ쥹�������󥯴�������Υ��ɥ쥹������
   @param[in] p �桼���ΰ���Ƭ���ɥ쥹
   @return ����󥯴�������Υ��ɥ쥹
 */
#define PTR2CHUNK(p)					\
	((chunk_t *)((void *)(p) - CHUNK_MEM_OFFSET))

/** �������ꥵ�����������󥯤Υ��ꥢ�������򻻽Ф���
    @param[in] s �������ꥵ����
    @return ����󥯤Υ��ꥢ������
    @note �ܥޥ����, �������򥢥ɥ쥹Ĺ�ǥ饦��ɥ��åפ������, �إå�Ĺ��û�����.�ºݤ˳�����Ƥ�����̤�,
    �ܥޥ�����ֵ��ͤ�size2alloc�ޥ����Ϳ�����������Ȥʤ�.
 */
#define CALC_AREA_SIZE(s)						\
	( ((size_t)roundup_size((s))) + CHUNK_MEM_OFFSET )

/** ����󥯤Υ��ꥢ���������������
    @param[in] c ����󥯴�������
    @return ����󥯤Υ��ꥢ������
 */
#define CHUNK_AREA_SIZE(c)			\
	((size_t)( ( (c)->size ) & ( ~( (size_t) (CHUNK_ATTR_MASK) ) ) ) )

/** ����󥯤�°���������Ф�
    @param[in] c ����󥯴�������
    @return ����󥯤�°������
 */
#define CHUNK_ATTR(c)					\
	( (c)->size  &  (size_t)(CHUNK_ATTR_MASK)  )

/** ����󥯤�°������򥯥ꥢ����
    @param[in] c ����󥯴�������
 */
#define CHUNK_CLR_ATTR(c)					\
	( ( (c)->size ) & ~( (size_t)(CHUNK_ATTR_MASK) )  )

/** ��������󥯤����ꤹ��
    @param[in] c ����󥯴�������
 */
#define CHUNK_MAKE_FREE(c)	do{					\
		(c)->size |= ((size_t)(CHUNK_FREE_BIT));		\
	}while(0)

/** ���Ѻѥ���󥯤����ꤹ��
    @param[in] c ����󥯴�������
 */
#define CHUNK_MAKE_USED(c)	do{					\
		(c)->size &= ~((size_t)(CHUNK_FREE_BIT));		\
	}while(0)

/** �������ȥ饢�꡼�ʤΥ���󥯤Ǥ��뤳�Ȥ��ǧ����
    @param[in] c ����󥯴�������
    @retval �� �������ȥ饢�꡼�ʤΥ���󥯤Ǥ���
    @retval �� �������ȥ饢�꡼�ʤΥ���󥯤Ǥ���
 */
#define CHUNK_IN_EXTRA_ARENA(c)						\
	( CHUNK_ATTR(c) & CHUNK_EXTRA_BIT )

/** �ᥤ�󥢥꡼�ʤΥ���󥯤Ǥ��뤳�Ȥ��ǧ����
    @param[in] c ����󥯴�������
    @retval �� �ҡ�����κǽ�����󥯤Ǥ���
    @retval �� �ҡ�����κǽ�����󥯤Ǥʤ�
    @note ����̤����(����Τ���˼���)
 */
#define CHUNK_IN_MAIN_HEAP(c)						\
	( !CHUNK_IN_EXTRA_ARENA(c) )

/** ����󥯤���ҡ����ΰ�����������Ф�
    @param[in] c ����󥯴�������
    @return ����󥯤ν�°����ҡ��פΥҡ��״�������Υ��ɥ쥹
 */
#define CHUNK2HEAP(c)				    \
	( refer_main_heap() )

/** �ҡ��פ���Ƭ����Υ��ե��åȰ��֤��ֵѤ���
    @param[in] c ����󥯴�������
    @return ����󥯤Υ��ե��åȰ���
 */
#define CHUNK_OFFSET_POS(c)			\
	( (size_t)( ( (size_t)(c) ) - ( (size_t)((CHUNK2HEAP((c)))->chunk)) ) )

/** �Хǥ��Τɤ���¦�˰��֤������󥯤Ǥ��뤫�ֵѤ���
    @param[in] c ����󥯴�������
    @retval CHUNK_BUDDY_ODD  ���¦
    @retval CHUNK_BUDDY_EVEN ����¦
 */
#define CHUNK_BUDDY_SIDE(c)			\
	( ( CHUNK_OFFSET_POS((c)) & CHUNK_AREA_SIZE((c)) ) ? (CHUNK_BUDDY_ODD) : (CHUNK_BUDDY_EVEN) )

/** �Хǥ�����󥯤Υ���󥯾�����ֵѤ���
    @param[in] c ����󥯴�������
    @return �Хǥ�����󥯤Υ���󥯾���ؤΥݥ���
 */
#define CHUNK_GET_BUDDY(c)				    \
	((chunk_t *)( (CHUNK_BUDDY_SIDE((c)) == CHUNK_BUDDY_ODD) ?	\
	     ( ((void *)(c)) - CHUNK_AREA_SIZE((c)) ) :			\
	     ( ((void *)(c)) + CHUNK_AREA_SIZE((c)) ) ) )

/** �Хǥ�����󥯤ζ���¦���ֵѤ���
    @param[in] c ����󥯴�������
    @return ����¦����󥯤Υ���󥯾���ؤΥݥ���
 */
#define CHUNK_GET_EVEN_SIDE(c)				    \
	((chunk_t *)( (CHUNK_BUDDY_SIDE((c)) == CHUNK_BUDDY_ODD) ?	\
	     ( ((void *)(c)) - CHUNK_AREA_SIZE((c)) ) :			\
	    ((void *)(c)) ) )

/** �����ΥХǥ����礭�������פ��뤳��(�ߤ����ߤ��ΥХǥ��ˤʤäƤ��뤳��)���ǧ����
    @param[in] c ����󥯴�������
    @retval �� �����ΥХǥ��Υ����������פ���
    @retval �� �����ΥХǥ��Υ����������פ��ʤ�
 */
#define CHUNK_BUDDY_MATCHED(c)				    \
	( CHUNK_AREA_SIZE(self) & CHUNK_AREA_SIZE(CHUNK_GET_BUDDY((c))) )

/** �ҡ�����κǽ�Υ���󥯤Ǥ��뤳�Ȥ��ǧ����
    @param[in] c ����󥯴�������
    @retval �� �ҡ�����κǽ�����󥯤Ǥ���
    @retval �� �ҡ�����κǽ�����󥯤Ǥʤ�
 */
#define CHUNK_IS_FIRST(c)						\
	(CHUNK_OFFSET_POS(c) == 0)

/** �ҡ�����κǽ�����󥯤Ǥ��뤳�Ȥ��ǧ����
    @param[in] c ����󥯴�������
    @retval �� �ҡ�����κǽ�����󥯤Ǥ���
    @retval �� �ҡ�����κǽ�����󥯤Ǥʤ�
 */
#define CHUNK_IS_LAST(c)						\
	( ( CHUNK_OFFSET_POS((c)) + CHUNK_AREA_SIZE((c)) ) == GET_HEAP_SIZE(CHUNK2HEAP((c))))

/** ����󥯤���������󥯤�Ĵ�٤�
    @param[in] c ����󥯴�������
    @retval �� ��������󥯤Ǥ���
    @retval �� ��������󥯤Ǥʤ�
 */
#define CHUNK_IS_FREE(c)						\
	( CHUNK_ATTR(c) & CHUNK_FREE_BIT )

/** ����󥯤������椫Ĵ�٤�
    @param[in] c ����󥯴�������
    @retval �� ���������󥯤Ǥ���
    @retval �� ���������󥯤Ǥʤ�
 */
#define CHUNK_IS_USED(c)						\
	(!CHUNK_IS_FREE(c))

/** ����󥯤Υ��ꥢ�����������ꤹ��
    @param[in] c ����󥯴�������
    @param[in] s ����󥯤Υ��ꥢ������
 */
#define CHUNK_SET_AREA_SIZE(c, s)	do{		\
		(c)->size = (s) | CHUNK_ATTR(c);	\
	}while(0)

/** ����󥯤Υ桼���ΰ襵�������������
    @param[in] c ����󥯴�������
    @retval  ����󥯤Υ桼���ΰ襵����
 */
#define CHUNK_MEM_SIZE(c)				\
	( CHUNK_AREA_SIZE(c)  - CHUNK_MEM_OFFSET )

/** ���Υ���󥯤򻲾Ȥ���
    @param[in] c ����󥯴�������
    @retval  ���Υ���󥯤������� ���Υ���󥯤δ�������Υ��ɥ쥹
    @retval  �ǽ�����󥯤ξ��     c �Υ��ɥ쥹
 */
#define CHUNK_NEXT(c)				\
	( CHUNK_IS_LAST(c) ?			\
	    (c) :							\
	    ( (chunk_t *) ( ( (void *)(c) ) + ( (size_t)(CHUNK_AREA_SIZE(c)) ) ) ) )

/** ���Υ���󥯤򻲾Ȥ���
    @param[in] c ����󥯴�������
    @retval  ���Υ���󥯤������� ���Υ���󥯤δ�������Υ��ɥ쥹
    @retval  �ǽ�����󥯤ξ��     c �Υ��ɥ쥹
 */
#define CHUNK_PREV(c)				\
	( CHUNK_IS_FIRST(c) ?						\
	    (c) :							\
	    ( (chunk_t *) ( ( (void *)(c) ) - (c)->prev_size ) ) )

/** ����󥯾������������
    @param[in] c ����󥯴�������
    @param[in] s ����󥯤Υ��ꥢ������
    @note����������󥯤ޤǤΥ�������0������, ���ꥢ������������ξ�, �����ꥹ�ȴ����Ρ��ɤ���������
 */
#define CHUNK_INIT(c, s) do {			\
		(c)->prev_size = 0;		\
		CHUNK_CLR_ATTR(c);		\
		CHUNK_MAKE_FREE(c);		\
		CHUNK_SET_AREA_SIZE(c, s);	\
		init_list_node(&((c)->free));	\
	} while(0)

/** ����󥯤�������ƥ�������2�Ѥ�·�����Ƥ��뤳�Ȥ��ǧ���뤿��Υޥ���
    @param[in] c ����󥯴�������
    @return �Ǿ�̥ӥåȰʳ��ΥӥåȤ�������Ф����ޥ�����
 */
#define CHUNK_ROUNDUP_MASK(c)			\
	(((size_t)( ( 1 << (find_msr_bit_in_size(CHUNK_AREA_SIZE(c)) - 1) ) - 1 ) ) )	

/** ����󥯥�������2�Ѥ�·�����Ƥ��뤳�Ȥ��ǧ����
    @param[in] c ����󥯴�������
    @retval �� ����󥯤�������ƥ�������2�Ѥ�·�����Ƥ���
    @retval �� ����󥯤�������ƥ�������2�Ѥ�·�����Ƥ��ʤ�
 */
#define CHUNK_IS_ROUNDUP(c)				    \
	( (find_msr_bit_in_size(CHUNK_AREA_SIZE(c)) < 2) ||	    \
	( !(CHUNK_AREA_SIZE(c) & (((size_t)(ALLOC_UNIT - 1)))) && \
	    !( CHUNK_AREA_SIZE(c) & CHUNK_ROUNDUP_MASK(c) ) ) )

/** �ΰ�Ĺ�������ñ�̤�2�Ѥ��ڤ�ͤ᤿�礭���򻻽Ф���
    @param[in] c ����󥯴�������
    @return �������������ñ�̤�2��ñ�̤ǳ����Ƥ����κ����ΰ�Ĺ
 */
#define CHUNK_TRANC_SIZE(c)			   \
	(find_msr_bit_in_size(CHUNK_AREA_SIZE((c)) < 2) ?	\
	(ALLOC_UNIT) :						\
	    (1 << (find_msr_bit_in_size(CHUNK_AREA_SIZE((c))) - 1)) )


int get_free_chunk(heap_t *, size_t , chunk_t **);
int free_chunk(chunk_t *);
void setup_heap_chunks(heap_t *, size_t );
#endif  /*   _KERN_CHUNK_H   */
