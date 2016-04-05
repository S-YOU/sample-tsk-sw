/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  id bitmap operations                                              */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_ID_BITMAP_H)
#define _KERN_ID_BITMAP_H
#include <errno.h>
#include "kern/kern_types.h"

typedef uint64_t bitmap_ent_t;  /*< �ӥåȥޥå�1����ȥ�  */
#define BITMAP_ARRAY_NR (CONFIG_ID_BITMAP_UNIT/(sizeof(bitmap_ent_t)*BIT_PER_BYTE)) /*< �ӥåȥޥå���������ǿ�  */

/** ID�ӥåȥޥå�
 */
typedef struct _bitmap{
	bitmap_ent_t bitmap[BITMAP_ARRAY_NR];  /*< id�ӥåȥޥå�����  */
}id_bitmap_t;

/** ID�ӥåȥޥå׽������
 */
#define ID_BITMAP_INITIALIZER {			\
	.bitmap = {0,}				\
	 }

void init_idmap(id_bitmap_t *);
int get_new_id(id_bitmap_t *, obj_id_t *);
void put_id(id_bitmap_t *, obj_id_t );
int reserve_id(id_bitmap_t *, obj_id_t );
#endif  /*  _KERN_ID_BITMAP_H  */
