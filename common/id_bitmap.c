/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  ID�ӥåȥޥå���� s                                              */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** ID�ӥåȥޥå���ζ���ID������
    @param[in] idmap ID�ޥåפΥ��ɥ쥹
    @param[in] idp ��������ID���ֵѤ����ΰ�
    @retval 0  ���ｪλ
    @retval ENOENT ���Ѳ�ǽ��ID���ʤ�
 */
int
get_new_id(id_bitmap_t *idmap, obj_id_t *idp) {
	int i,j;
	psw_t psw;

	psw_disable_interrupt(&psw);
	for(i = 0; i < BITMAP_ARRAY_NR; ++i) {
		for(j = 0; j < sizeof(bitmap_ent_t); ++j) {
			if (!(idmap->bitmap[i] & ( 1UL << j ) )) {
				idmap->bitmap[i] |= ((bitmap_ent_t)( 1 << j ));
				*idp = j + i * sizeof(bitmap_ent_t);
				return 0;
			}
		}
	}
	psw_restore_interrupt(&psw);

	return ENOENT;
}

/** ID�ӥåȥޥåפ�ID���ֵѤ���
    @param[in] idmap ID�ޥåפΥ��ɥ쥹
    @param[in] id    �ֵѤ���ID
 */
void
put_id(id_bitmap_t *idmap, obj_id_t id) {
	int idx, offset;
	psw_t psw;

	psw_disable_interrupt(&psw);
	idx = id / BITMAP_ARRAY_NR;
	offset = id % sizeof(bitmap_ent_t);
	idmap->bitmap[idx] &= ~((bitmap_ent_t)( 1UL << offset ));
	psw_restore_interrupt(&psw);

	return;
}

/** ID�ӥåȥޥåפ�ID��ͽ�󤹤�
    @param[in] idmap ID�ޥåפΥ��ɥ쥹
    @param[in] id    ͽ�󤹤�ID
    @retval 0     �����ͽ�󤷤�
    @retval EBUSY ���˻��Ѥ���Ƥ���
 */
int
reserve_id(id_bitmap_t *idmap, obj_id_t id) {
	int rc;
	int idx, offset;
	psw_t psw;

	psw_disable_interrupt(&psw);
	idx = id / BITMAP_ARRAY_NR;
	offset = id % sizeof(bitmap_ent_t);
	if (idmap->bitmap[idx] & ((bitmap_ent_t)( 1UL << offset ))) {
		rc = EBUSY;
		goto out;
	}
	idmap->bitmap[idx] |= ((bitmap_ent_t)( 1UL << offset ));
	rc = 0;
out:
	psw_restore_interrupt(&psw);

	return rc;
}

/** ID�ӥåȥޥåפν����
    @param[in] idmap ID�ӥåȥޥåפΥ��ɥ쥹
 */
void
init_idmap(id_bitmap_t *idmap){
	psw_t psw;

	psw_disable_interrupt(&psw);
	memset(idmap, 0, sizeof(id_bitmap_t));
	psw_restore_interrupt(&psw);

	return;
}
