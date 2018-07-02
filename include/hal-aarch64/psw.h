/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Processor Status Word                                             */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_PSW_H)
#define  _HAL_PSW_H 

#include <kern/freestanding.h>

#include <kern/param.h>
#include <hal/aarch64.h>

typedef uint64_t psw_t;  /*< �ץ������ơ��������  */

#if defined(CONFIG_HAL)

/** CPU�ؤγ����ߤ�̵���˵��Ĥ���
 */
#define psw_enable_interrupt()     do{				\
      __asm__ __volatile__ ("msr daifclr, %0\n\t" : : "i"(AARCH64_DAIF_IRQ)); \
    }while(0)
/** CPU�ؤγ����ߤ�̵���˶ػߤ���
 */
#define psw_disable_interrupt()     do{				\
	__asm__ __volatile__ ("msr daifset, %0\n\t" : : "i"(AARCH64_DAIF_IRQ)); \
    }while(0)


/** PSW����¸����
    @param[in] psw PSW��¸�Ѥ��ѿ�
 */
#define __save_psw(psw)  do{				    \
     __asm__ __volatile__ ("mrs %0, daif\n\t":"=r"(psw));   \
    }while(0)

/** PSW����������
    @param[in] psw PSW��¸�Ѥ��ѿ�
 */
#define __restore_psw(psw) do{                          \
       __asm__ __volatile__ ("msr daif, %0\n\t"         \
	   ::"r"(psw));					\
   }while(0)
#else  /*  CONFIG_HAL  */
#define psw_enable_interrupt()  do{}while(0)
#define psw_disable_interrupt() do{}while(0)
#define barrier() do{}while(0)
#define __save_psw(psw) do{}while(0)
#define __restore_psw(psw) do{}while(0)
#endif  /*  CONFIG_HAL  */

void psw_disable_and_save_interrupt(psw_t *);
void psw_restore_interrupt(psw_t *);
#endif  /*  _KERN_PSW_H   */
