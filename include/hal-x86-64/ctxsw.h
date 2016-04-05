/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  context switch                                                    */
/*                                                                    */
/**********************************************************************/
#if !defined(_HAL_CTXSW_H)
#define _HAL_CTXSW_H

struct _thread;

/** X86-64�Υ����å��ڤ��ؤ�
    @param[in] prev �ڤ��ؤ����륹��åɤΥ����å����ɥ쥹���Ǽ���Ƥ���ݥ����ѿ��Υ��ɥ쥹
    @param[in] next �ڤ��ؤ��륹��åɤΥ����å����ɥ쥹���Ǽ���Ƥ���ݥ����ѿ��Υ��ɥ쥹
    @note PSW(Processor Status Word - X86-64�ξ��, RFLAGS��RIP����)��
          AMD64 ABI Draft 0.99.5(http://www.x86-64.org/documentation/abi.pdf)�ǵ��ꤵ�줿
          callee saved�쥸�����Τ��� x87 FPU �����ɰʳ��Υ쥸����������/��������
	  x86-64��callee saved�쥸����(AMD64 ABI Draft 0.99.5 Figure 3.4: Register Usage����)
 */
static inline void
hal_do_context_switch(void **prev, void **next) {

	__asm__ __volatile__(			
	"pushfq\n\t"	
	"pushq %%rbx\n\t"
	"pushq %%rbp\n\t"
	"pushq %%r12\n\t"
	"pushq %%r13\n\t"
	"pushq %%r14\n\t"
	"pushq %%r15\n\t"
	"pushq $1f\n\t"
	"movq %%rsp, (%%rcx)\n\t"
	"movq (%%rdx), %%rsp\n\t"
	"ret\n\t"
	"1:\n\t"
	"popq %%r15\n\t"
	"popq %%r14\n\t"
	"popq %%r13\n\t"
	"popq %%r12\n\t"
	"popq %%rbp\n\t"
	"popq %%rbx\n\t"
	"popfq\n\t"
	: /* no output */
	: "c"(prev), "d"(next) );
}
void __start_thread(void);
void hal_setup_thread_function(struct _thread *, void (*fn)(void *), void *);
#endif  /*  _HAL_CTXSW_H  */
