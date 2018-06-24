/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  list relevant routines                                            */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_LIST_H)
#define _KERN_LIST_H

#include "kern/kern_types.h"

/** �ꥹ�ȥ���ȥ�
 */
typedef struct _list{
	struct _list *prev;       /*<  �����ΰ�ؤΥݥ���  */
	struct _list *next;       /*<  �����ΰ�ؤΥݥ���  */
}list_t;

/** �ꥹ�ȥإå�
 */
typedef struct _list_head{
	struct _list *prev;       /*<  �����ΰ�ؤΥݥ���  */
	struct _list *next;       /*<  �����ΰ�ؤΥݥ���  */
}list_head_t;

/** �ꥹ�Ȥ��鹽¤�ΤؤΥݥ��󥿤�����
    @param[in] p �ꥹ�ȥ��ФΥ��ɥ쥹
    @param[in] t ��¤�Τη�
    @param[in] m t�ι�¤����Υꥹ�ȥ��ФΥ���̾
 */
#define CONTAINER_OF(p, t, m)			\
	((t *)(((void *)(p)) - ((void *)(&(((t *)(0))->m)))))

void list_rotate(struct _list_head *);
int list_is_empty(struct _list_head *);
list_t *list_ref_top(struct _list_head *);
list_t *list_get_top(struct _list_head *);
void list_add_top(struct _list_head *, list_t *);
void list_add(struct _list_head *, list_t *);
void list_del(list_t *);

void init_list_node(list_t *);
void init_list_head(list_head_t *);

static inline int
list_is_linked(list_t *node) {
	return !( (node->prev == node) && (node->next == node) );
}


/** �ꥹ��õ��
    @param[in] itr    ���ƥ졼���˻��Ѥ���list_t���Υݥ����ѿ�̾
    @param[in] refp   �ꥹ�ȥإå���ޤ๽¤�ΤؤΥݥ����ѿ�̾
    @param[in] member refp��Υꥹ�ȥإåɥ���̾
 */
#define list_for_each(itr, refp, member)					\
  for((itr) = list_ref_top(&((refp)->member)); (itr) != (list_t *)&((refp)->member); (itr) = (itr)->next) 

#endif  /* _KERN_LIST_H  */
