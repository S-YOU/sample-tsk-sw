/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  �ӥå����                                                        */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/**  size_t����Ϳ����줿���������MSB(1�ȤʤäƤ���Ǿ�̥ӥåȤΰ�̣)�ΰ��֤򻻽Ф���
     @param[in] x ������
     @retval 0 x��0���ä�
     @retval MSB�ΰ���(�Ʋ��̥ӥåȤ�1�Ȥ����ֵ�)
     @note x��0�Υ��������θ����, �Ʋ��̥ӥåȤ�1�Ȥ���MSB���ֵѤ���
 */
int 
find_msr_bit_in_size(size_t x) {
	int i = 0;

	if (x == 0)
		goto out;

	for(i = (sizeof(x)*BIT_PER_BYTE - 1); i >= 0; --i) {
		/*  �Ǿ�̥ӥåȤ�����֤˥ӥåȤ�Ω�äƤ��뤫��������  */
		if (x & (1UL << i) ) /* 1�ˤʤäƤ���ӥåȤ򸡽�  */
			return i + 1;
	}

out:
	return 0;  /* x��0�Ǥ���  */
}


/**  size_t����Ϳ����줿���������LSB(1�ȤʤäƤ���ǲ��̥ӥåȤΰ�̣)�ΰ��֤򻻽Ф���
     @param[in] x ������
     @retval 0 x��0���ä�
     @retval LSB�ΰ���(�Ʋ��̥ӥåȤ�1�Ȥ����ֵ�)
     @note x��0�Υ��������θ����, �Ʋ��̥ӥåȤ�1�Ȥ���MSB���ֵѤ���
 */
int 
find_lsr_bit_in_size(size_t x) {
	int i = 0;

	if (x == 0)
		goto out;

	for(i = 0; i <= (sizeof(x)*BIT_PER_BYTE - 1); ++i) {
		/*  �ǲ��̥ӥåȤ�����֤˥ӥåȤ�Ω�äƤ��뤫��������  */
		if (x & (1UL << i) ) /* 1�ˤʤäƤ���ӥåȤ򸡽�  */
			return i + 1;
	}

out:
	return 0;  /* x��0�Ǥ���  */
}

/**  int����Ϳ����줿���������MSB(1�ȤʤäƤ���Ǿ�̥ӥåȤΰ�̣)�ΰ��֤򻻽Ф���
     @param[in] x ������
     @retval 0 x��0���ä�
     @retval MSB�ΰ���(�Ʋ��̥ӥåȤ�1�Ȥ����ֵ�)
     @note x��0�Υ��������θ����, �Ʋ��̥ӥåȤ�1�Ȥ���MSB���ֵѤ���
 */
int 
find_msr_bit(int x) {
	int i = 0;

	if (x == 0)
		goto out;

	for(i = (sizeof(x)*BIT_PER_BYTE - 1); i >= 0; --i) {
		/*  �Ǿ�̥ӥåȤ�����֤˥ӥåȤ�Ω�äƤ��뤫��������  */
		if (x & (1UL << i) ) /* 1�ˤʤäƤ���ӥåȤ򸡽�  */
			return i + 1;
	}

out:
	return 0;  /* x��0�Ǥ���  */
}

/**  int����Ϳ����줿���������LSB(1�ȤʤäƤ���ǲ��̥ӥåȤΰ�̣)�ΰ��֤򻻽Ф���
     @param[in] x ������
     @retval 0 x��0���ä�
     @retval LSB�ΰ���(�Ʋ��̥ӥåȤ�1�Ȥ����ֵ�)
     @note x��0�Υ��������θ����, �Ʋ��̥ӥåȤ�1�Ȥ���MSB���ֵѤ���
 */
int 
find_lsr_bit(int x) {
	int i = 0;

	if (x == 0)
		goto out;

	for(i = 0; i <= (sizeof(x)*BIT_PER_BYTE - 1); ++i) {
		/*  �ǲ��̥ӥåȤ�����֤˥ӥåȤ�Ω�äƤ��뤫��������  */
		if (x & (1UL << i) ) /* 1�ˤʤäƤ���ӥåȤ򸡽�  */
			return i + 1;
	}

out:
	return 0;  /* x��0�Ǥ���  */
}
