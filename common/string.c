/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  string operations                                                 */
/*                                                                    */
/**********************************************************************/

#include "kern/kernel.h"

/** �������ꤵ�줿�ͤ�����
    @param[in] b   �񤭹����оݥ������Ƭ���ɥ쥹
    @param[in] c   �񤭹�����
    @param[in] len �񤭹����о��ΰ�Υ�����
    @return �񤭹����оݥ������Ƭ���ɥ쥹
 */
void *
memset(void *b, int c, size_t len) {
	size_t i;
	caddr_t m = b;

	for(i = 0; i < len; ++i) {
		*m++ = (char )c;
	}
	
	return b;
}

/** ��������Ƥ򥳥ԡ�����
    @param[in] dst   ���ԡ���������Ƭ���ɥ쥹
    @param[in] src   ���ԡ����������Ƭ���ɥ쥹
    @param[in] len   ���ԡ����륵����
    @return ���ԡ���������Ƭ���ɥ쥹
    @note src��dst���ΰ�˽Ťʤ꤬������ϥ��ԡ����������Ԥ��ʤ�
 */
void *
memcpy(void *dst, const void *src, size_t len){
	caddr_t end  = (caddr_t)(src + len);
	caddr_t dstp = (caddr_t)dst, srcp = (caddr_t)src;

	do {
		*dstp++ = *srcp++;
	}while (srcp != end);

	return dst;
}

/** ��������Ƥ򥳥ԡ�����
    @param[in] dst   ���ԡ���������Ƭ���ɥ쥹
    @param[in] src   ���ԡ����������Ƭ���ɥ쥹
    @param[in] len   ���ԡ����륵����
    @return ���ԡ���������Ƭ���ɥ쥹
 */
void *
memmove(void *dst, const void *src, size_t len){
	caddr_t end;
	caddr_t dstp = (caddr_t)dst, srcp = (caddr_t)src;

	if (dst < src) {
		end = (caddr_t)( src + len );

		do {
			*dstp++ = *srcp++;
		}while (srcp != end);

	} else {
		srcp = (caddr_t)(src + len);
		dstp = (caddr_t)(dst + len);
		end = (caddr_t)src;

		do{
			*--dstp = *--srcp;
		}while (srcp != end);
	}

	return dst;
}

/** ʸ����Ĺ���ֵѤ���
    @param[in] str ʸ����
    @retval str��Ĺ��
 */
size_t
strlen(const char *str) {
	const char *cp = str;
	size_t len;

	for(len = 0; *cp != '\0'; ++cp, ++len);

	return len;
}
