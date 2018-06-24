/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  �����ͥ륳�󥽡������                                            */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_KCONSOLE_H)
#define  _HAL_KCONSOLE_H 
#include "kern/list.h"

typedef struct _kconsole{
	list_t           link;      /*< ���󥽡���ꥹ��       */
	int                 x;      /*< ���ϥ�������X��ɸ����  */
	int                 y;      /*< ���ϥ�������Y��ɸ����  */
	void (*putchar)(int );      /*< ��ʸ������             */
	void (*cls)(void);          /*< ���̥��ꥢ             */
	void (*locate)(int , int);  /*< ���������ư           */
}kconsole_t;

typedef struct _kconsole_list{
	list_head_t head;
}kconsole_list_t;

#define KCONSOLE_INITILIZER(name)  {			\
	.link = {&((name).link), &((name).link)},	\
	.x = 0,				\
	.y = 0,				\
	.putchar = NULL,		\
	.cls = NULL,			\
	.locate = NULL			\
	}

#define KCONSOLE_LIST_INITILIZER(name) {	\
	.head = {(list_t *)&(name.head), (list_t *)&(name.head)} \
	 }

void kputchar(int );
void kcls(void);
void klocate(int, int);
int register_kconsole(kconsole_t *);
void unregister_kconsole(kconsole_t *);
void init_pseudo_console(void);
#endif  /*  _HAL_KCONSOLE_H   */
