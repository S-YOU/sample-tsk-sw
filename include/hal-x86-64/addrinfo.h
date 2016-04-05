/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*   relevant routines                                 */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_ADDR_INFO_H)
#define  _HAL_ADDR_INFO_H 

#define MALIGN_SIZE       (sizeof(void *))       /*<  ����������ñ��  */

/** ���ꤵ�줿���饤����Ȥˤ��äƤ��ʤ����Ȥ��ǧ���뤿��Υޥ����ͤ򻻽Ф���
    @param[in] size ���饤����ȥ�����
    @return ���ꤵ�줿���饤����Ȥˤ��äƤ��ʤ����Ȥ��ǧ���뤿��Υޥ�����
 */
#define CALC_ALIGN_MASK(size)					\
	((size_t)((size) - 1))

/** ���ꤵ�줿���饤����Ȥˤ��äƤ��ʤ����Ȥ��ǧ����
    @param[in] addr ���ꥢ�ɥ쥹
    @param[in] size ���饤����ȥ�����
    @retval �� ���ꤵ�줿���ɥ쥹������Υ��饤����Ȥˤ��äƤ��ʤ�
    @retval �� ���ꤵ�줿���ɥ쥹������Υ��饤����Ȥˤ��äƤ���
 */
#define ADDR_NOT_ALIGNED(addr, size)				\
	( ( (size_t)(addr) ) & CALC_ALIGN_MASK((size)) )

/** ���ꤵ�줿���饤����Ȥ��ڤ�ͤ��
    @param[in] addr ���ꥢ�ɥ쥹
    @param[in] size ���饤����ȥ�����
    @return ����Υ��饤����ȶ����ˤ������ä����ɥ쥹���ڤ�ͤ᤿��(��Ƭ���ɥ쥹)
 */
#define TRUNCATE_ALIGN(addr, size)		\
	( ((size_t)(addr)) & ((size_t)(~CALC_ALIGN_MASK((size)) ) ) )

/** ���ꤵ�줿���饤����Ȥǥ饦��ɥ��åפ���
    @param[in] addr ���ꥢ�ɥ쥹
    @param[in] size ���饤����ȥ�����
    @return ����Υ��饤����Ȥǥ��ɥ쥹�ͤ�饦��ɥ��åפ�����
 */
#define ROUNDUP_ALIGN(addr, size)		\
	( TRUNCATE_ALIGN((((size_t)(addr)) + ((size_t)(size))), (size) ) )

#endif  /*  _KERN_ADDR_INFO_H   */

