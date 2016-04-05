/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*                                                                    */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_SEGMENTS_H)
#define  _HAL_SEGMENTS_H 
#if !defined(ASM)
#include <stdint.h>

/*
 *  �������� �ǥ�������ץ�
 *  ����ƥ�R �������ƥ�ǥåɡ�����64 �ƥ��Υ������եȥ��������ǥ٥�åѡ�����������
 *  1-9�ڡ�������
 *  ����Ū��IA32�Υ������� �ǥ�������ץ���64�ӥåȳ�ĥ��, ͽ��ӥåȤ�long mode�ӥåȤ��ѹ�����Ƥ���.
 */
typedef struct  _segment_desc {
	uint64_t limit_low:16;      /*  �������ȡ���ߥå� 15:00                             */
	uint64_t base_low:24;       /*  �١��������ɥ쥹                                       */
	uint64_t type:5;            /*  ������                                                 */
	uint64_t dpl:2;             /*  �ǥ�������ץ��ø���٥�                               */
	uint64_t p:1;               /*  ��������¸�ߥӥå�                                   */
	uint64_t limit_high:4;      /*  �������ȡ���ߥå�(���)                             */
	uint64_t avl:1;             /*  OS�ǻ��Ѥ����ե������(̤����)                       */
	uint64_t long_mode:1;       /*  long mode�ӥå�                                        */
	uint64_t db:1;              /*  ���ڥ졼����󥵥��� 0=16�ӥå�, 1=32�ӥå�            */
	uint64_t g:1;               /*  γ��(����˥�ƥ�)�ӥå�  1=�ڡ���ñ��, 0=�Х���ñ��   */
	uint64_t base_high:8;       /*  �������ȥ١���(���)                                 */
}__attribute__  ((packed)) segment_desc_t;

typedef struct _descriptor_tbl{
	unsigned long dt_limit:16;
	unsigned long dt_base:64 __attribute__  ((packed)) ;
}__attribute__ ((packed)) descriptor_tbl_t;


typedef struct _x86_64_tss {
        uint32_t       tss_rsvd0;
        uint64_t       tss_rsp0 __attribute__  ((packed));      /* kernel stack pointer ring 0 */
        uint64_t       tss_rsp1 __attribute__  ((packed));      /* kernel stack pointer ring 1 */
        uint64_t       tss_rsp2 __attribute__  ((packed));      /* kernel stack pointer ring 2 */
        uint32_t       tss_rsvd1;
        uint32_t       tss_rsvd2;
        uint64_t       tss_ist1 __attribute__  ((packed));      /* Interrupt stack table 1 */
        uint64_t       tss_ist2 __attribute__  ((packed));      /* Interrupt stack table 2 */
        uint64_t       tss_ist3 __attribute__  ((packed));      /* Interrupt stack table 3 */
        uint64_t       tss_ist4 __attribute__  ((packed));      /* Interrupt stack table 4 */
        uint64_t       tss_ist5 __attribute__  ((packed));      /* Interrupt stack table 5 */
        uint64_t       tss_ist6 __attribute__  ((packed));      /* Interrupt stack table 6 */
        uint64_t       tss_ist7 __attribute__  ((packed));      /* Interrupt stack table 7 */
        uint32_t       tss_rsvd3;
        uint32_t       tss_rsvd4;
        uint16_t       tss_rsvd5;
        uint16_t       tss_iobase;     /* io bitmap offset */
}x86_64_tss_t;

/*
 * Gate descriptors (e.g. indirect descriptors, trap, interrupt etc. 128 bit)
 * Only interrupt and trap gates have gd_ist.
 */
typedef struct	gate_descriptor {
	uint64_t gd_looffset:16;	/* gate offset (lsb) */
	uint64_t gd_selector:16;	/* gate segment selector */
	uint64_t gd_ist:3;		/* IST table index */
	uint64_t gd_xx:5;		/* unused */
	uint64_t gd_type:5;		/* segment type */
	uint64_t gd_dpl:2;		/* segment descriptor priority level */
	uint64_t gd_p:1;		/* segment descriptor present */
	uint64_t gd_hioffset:48 __attribute__ ((packed));	/* gate offset (msb) */
	uint64_t sd_xx1:32;
} __attribute__ ((packed)) gate_descriptor_t;

void init_descriptor(void);
void lgdt(void *);
#endif   /* !defined(ASM)  */

#define	SD_T_NULL	 0	/* system null */
#define	SD_T_SYS286TSS	 1	/* system 286 TSS available */
#define	SD_T_SYSLDT	 2	/* system 64 bit local descriptor table */
#define	SD_T_SYS286BSY	 3	/* system 286 TSS busy */
#define	SD_T_SYS286CGT	 4	/* system 286 call gate */
#define	SD_T_SYSTASKGT	 5	/* system task gate */
#define	SD_T_SYS286IGT	 6	/* system 286 interrupt gate */
#define	SD_T_SYS286TGT	 7	/* system 286 trap gate */
#define	SD_T_SYSNULL2	 8	/* system null again */
#define SD_T_TSS         9      /* system Task State Segment */
#define	SD_T_SYSRSV3	10	/* system null again */
#define	SD_T_SYSBSY	11	/* system busy 64 bit TSS */
#define	SD_T_SYSCGT	12	/* system 64 bit call gate */
#define	SD_T_SYSRSV4	13	/* system null again */
#define	SD_T_SYSIGT	14	/* system 64 bit interrupt gate */
#define	SD_T_SYSTGT	15	/* system 64 bit trap gate */


#define	SD_T_MEMRO	16	/* memory read only */
#define	SD_T_MEMROA	17	/* memory read only accessed */
#define	SD_T_MEMRW	18	/* memory read write */
#define	SD_T_MEMRWA	19	/* memory read write accessed */
#define	SD_T_MEMROD	20	/* memory read only expand dwn limit */
#define	SD_T_MEMRODA	21	/* memory read only expand dwn limit accessed */
#define	SD_T_MEMRWD	22	/* memory read write expand dwn limit */
#define	SD_T_MEMRWDA	23	/* memory read write expand dwn limit accessed */
#define	SD_T_MEME	24	/* memory execute only */
#define	SD_T_MEMEA	25	/* memory execute only accessed */
#define	SD_T_MEMER	26	/* memory execute read */
#define SD_T_MRE        27      /* memory execute read accessed */

#define PL_KERN          0
#define PL_USER          3
/*
 * �������ȥǥ�������ץ��Υ��쥯��
 */
#define NULL_CS            0       /* Null �ǥ�������ץ� */
#define KERN_CS            1       /* Kernel Code Descriptor */
#define KERN_DS            2       /* Kernel Data Descriptor */
#define USER_CS            3       /* User 64 bit Code Descriptor */
#define USER_DS            4       /* User 64 bit Data Descriptor */
#define GPROC0_SEL         5       /* TSS(TSS for Kernel Trap) */
#define GPROC0_SEL_REST    6       /* TSS(TSS for Kernel Trap) */
#define GDT_NR             7       /* GDT�Υ���ȥ��  */
#define IDT_NR           256       /* IDT�Υ���ȥ�� */
#endif  /*  _HAL_SEGMENTS_H   */
