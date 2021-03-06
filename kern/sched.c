/* -*- mode: c; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2014 Takeharu KATO                                      */
/*                                                                    */
/*  Thread scheduler                                                  */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

/** 実行可能なスレッドを選択する
    @retval NULL スケジュール可能なスレッドがいない(レディーキューが空)
    @retval 次に動作させるスレッドのスレッド管理情報
 */
static thread_t *
select_next_thread(void) {
	psw_t psw;
	thread_t *next;

	psw_disable_and_save_interrupt(&psw);
	next = rdq_find_runnable_thread();

	/* 実行可能なスレッドがなければ, アイドルスレッドを選択  */	
	if (next == NULL)
		next = idle_refer_idle_thread(); 

	psw_restore_interrupt(&psw);

	return next;
}

/** スレッドをスケジュール可能にする
    @param[in] thr スレッド管理情報
 */
void
sched_set_ready(thread_t *thr) {
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);
	rdq_add_thread(thr);  /* レディーキューに追加する  */
	psw_restore_interrupt(&psw);
}

/** レディーキューを回転する
 */
void
sched_rotate_queue(void) {
	psw_t psw;

	psw_disable_and_save_interrupt(&psw);
	rdq_rotate_queue();
	psw_restore_interrupt(&psw);	
}

/** スケジューラ本体
 */
void
sched_schedule(void) {
	psw_t psw;
	thread_t *prev, *next;
#if defined(CONFIG_HAL)
	thread_info_t *ti;
#endif  /* CONFIG_HAL */

	psw_disable_and_save_interrupt(&psw);

#if defined(CONFIG_HAL)
	ti = get_current_thread_info();
	ti_set_preempt_active(ti);  /* Set preempt-active flag. */

	if ( ( ti_refer_preempt_count(ti) > 0 ) ||
	    ( ti_refer_irq_count(ti) > 0 ) ) {

                /*  Need to delay dispatch */
		ti_set_delay_dispatch(ti);
		goto out;
	}
#endif  /* CONFIG_HAL */

	prev = current;
	next = select_next_thread(); 
	if (prev == next) {/* ディスパッチする必要なし  */
		goto out;
	}
	current = next;
	thr_thread_switch(prev, next);

	current->status = THR_TSTATE_RUN ;
out:
	psw_restore_interrupt(&psw);	

#if defined(CONFIG_HAL)
	ti_clr_preempt_active(ti);  /* Clear preempt-active flag. */
#endif  /* CONFIG_HAL */
}

#if defined(CONFIG_HAL)
void
sched_delay_disptach(void) {
	thread_info_t *ti;

	ti = get_current_thread_info();
	
	/*
	 * Delay dispatching
	 */
	if  ( ( ti_refer_preempt_count(ti) > 0 ) ||
	    ( ti_refer_irq_count(ti) > 0 ) )
		return; /* Dispatching is NOT allowed */
	
	if ( ti_check_preempt_active(ti) )
		return;
	
	if ( !ti_check_need_dispatch(ti) ) 
		return;

	ti_clr_delay_dispatch(ti); /* Clear a delay scheduling request */
	sched_schedule(); 

	return;
}
#endif  /* CONFIG_HAL */

/** スケジューラの初期化
 */
void
sched_init(void) {

	rdq_init_ready_queue();
}
