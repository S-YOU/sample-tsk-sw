/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  Processor Status Word����                                         */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_PSW_H)
#define  _HAL_PSW_H 
#include "param.h"

typedef uint64_t psw_t;  /*< �ץ������ơ��������  */

#if defined(CONFIG_HAL)

/** CPU�ؤγ����ߤ�̵���˵��Ĥ���
 */
#define __psw_enable_interrupt()     do{                     \
		__asm__ __volatile__ ("sti\n\t");	     \
    }while(0)
/** CPU�ؤγ����ߤ�̵���˶ػߤ���
 */
#define __psw_disable_interrupt()     do{                     \
		__asm__ __volatile__ ("cli\n\t");	      \
    }while(0)

/** �����ȥ��֥������¹Ԥ��Ƥ���̿����Ԥ���碌��
 */
#define barrier() do{							\
		__asm__ __volatile__ ("" :::"memory");			\
	}while(0)

/** PSW����¸����
    @param[in] psw PSW��¸�Ѥ��ѿ�
 */
#define __save_psw(psw)  do{				\
       __asm__ __volatile__ ("pushfq\n\t"               \
                             "popq %0\n\t"              \
                             "cli\n\t"                  \
	   :"=m"(psw));					\
       barrier();                                       \
    }while(0)

/** PSW����������
    @param[in] psw PSW��¸�Ѥ��ѿ�
 */
#define __restore_psw(psw) do{                          \
       barrier();                                       \
       __asm__ __volatile__ ("pushq %0\n\t"             \
			     "popfq\n\t"                \
	   ::"m"(psw));					\
   }while(0)
#else  /*  CONFIG_HAL  */
#define __psw_enable_interrupt()  do{}while(0)
#define __psw_disable_interrupt() do{}while(0)
#define barrier() do{}while(0)
#define __save_psw(psw) do{}while(0)
#define __restore_psw(psw) do{}while(0)
#endif  /*  CONFIG_HAL  */

void psw_disable_interrupt(psw_t *);
void psw_restore_interrupt(psw_t *);
#endif  /*  _KERN_PSW_H   */
