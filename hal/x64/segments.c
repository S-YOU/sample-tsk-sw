/* -*- mode: C; coding:utf-8 -*- */
/**********************************************************************/
/*  OS kernel sample                                                  */
/*  Copyright 2018 Takeharu KATO                                      */
/*                                                                    */
/*  Segment routines                                                  */
/*                                                                    */
/**********************************************************************/

#include <kern/kernel.h>

#include <hal/kmap.h>
#include <hal/segments.h>
#include <hal/exception.h>

//#define DEBUG_SHOW_GLOBAL_DESCRIPTOR

#if defined(DEBUG_SHOW_GLOBAL_DESCRIPTOR)
static void
show_gdt_descriptor_entry(uint16_t sel, gdt_descriptor *gdtp) {
	uintptr_t addr;
	uintptr_t limit;

	kassert(gdtp != NULL);
	kassert(gdtp->avl == 0);

	addr = gdtp->base0 | ( (uintptr_t)gdtp->base1 ) << 16 | ( (uintptr_t)gdtp->base2 ) << 24;
	limit = gdtp->limit0 | ( (uintptr_t)gdtp->limit1 ) << 16;
	
	kprintf("%d : addr=%p limit=0x%lx %s %s %s %s %s %s %s %s %s\n", 
	    sel, addr, limit, 
	    ( gdtp->access ) ? ("accessed") : ("Not accessed"),
	    ( gdtp->rw ) ? ("Read/Write") : ("ReadOnly"),
	    ( gdtp->dc ) ? ("Conformed segment") : (""),
	    ( gdtp->exec ) ? ("Executable") : ("Not Executable"),
	    ( gdtp->dpl == X86_DESC_DPL_KERNEL ) ? ("Kernel") : ("User"),
	    ( gdtp->present ) ? ("Present") : ("NotPresent"),
	    ( gdtp->mode ) ? ("Long Mode") : ("16/32bit mode"),
	    ( gdtp->size ) ? ("32bit size") : ("16bit/64bit size"),
	    ( gdtp->gran ) ? ("page granularity") : ("byte granularity"));
}

static void
show_gdt_descriptor(gdt_descriptor *gdt_base) {
	int i;

	for(i = 0; i < (GDT_TSS64_SEL + 1); ++i) {

		show_gdt_descriptor_entry(i << 3, &gdt_base[i]);
	}
}
#else
static void
show_gdt_descriptor(gdt_descriptor __attribute__ ((unused)) *gdt_base) {
}
#endif  /*  DEBUG_SHOW_GLOBAL_DESCRIPTOR  */

static void
load_global_segment(void *p, size_t size) {
	region_descriptor rd;

	rd.rd_limit = size - 1;
	rd.rd_base = (uint64_t)p;
	lgdtr(&rd, GDT_KERN_CODE64, GDT_KERN_DATA64);
}

static void
init_tss_descriptor(gdt_descriptor *gdt_ent, uintptr_t addr, uintptr_t limit) {
	gdt_descriptor *gdtp;
	uint64_t *high_addr;

	gdtp = gdt_ent;

	kassert(gdtp != NULL);

	gdtp->base0 = addr & 0xffff;
	gdtp->base1 = ( addr >> 16 ) & 0xff;
	gdtp->base2 = ( addr >> 24 ) & 0xff;
	
	gdtp->limit0 = limit & 0xffff;
	gdtp->limit1 = ( limit >> 16 ) & 0xf;

	gdtp->access = 1;
	gdtp->rw = X86_DESC_RDONLY;
	gdtp->dc = 0;
	gdtp->exec = X86_DESC_EXEC;
	gdtp->resv0 = 0;
	gdtp->dpl = X86_DESC_DPL_USER;
	gdtp->present = 1;
	gdtp->avl = 0;
	gdtp->mode = 0;
	gdtp->size = 0;
	gdtp->gran = 0;
	
	high_addr = (uint64_t *)( gdt_ent + 1 );
	*high_addr = ( (addr >> 32) & 0xffffffff );

	return;
}

static void
init_gdt_descriptor_table_entry(gdt_descriptor *gdtp, uintptr_t addr, uintptr_t limit,
    unsigned rw, unsigned exec, unsigned dpl, unsigned mode, 
    unsigned size, unsigned granularity) {

	kassert(gdtp != NULL);

	gdtp->base0 = addr & 0xffff;
	gdtp->base1 = ( addr >> 16 ) & 0xff;
	gdtp->base2 = ( addr >> 24 ) & 0xff;
	
	gdtp->limit0 = limit & 0xffff;
	gdtp->limit1 = ( limit >> 16 ) & 0xf;

	gdtp->access = 0;
	gdtp->rw = rw;
	gdtp->dc = 0;

		gdtp->mode = mode;
	if (exec) {

		gdtp->exec = 1;
	} else {

		gdtp->exec = 0;
	}

	gdtp->resv0 = 1;
	gdtp->dpl = dpl;
	gdtp->present = 1;
	gdtp->avl = 0;

	if ( gdtp->mode ) {

		gdtp->size = 0;
	} else {

		gdtp->size = size;
	}

	gdtp->gran = granularity;
}

void
x64_init_segments(void *gdtp, tss64 **tssp){
	uint64_t                *gdt;
	uint64_t                addr;

	memset(gdtp, 0, sizeof(PAGE_SIZE));
	gdt = (uint64_t *)gdtp;
	
	addr = (uint64_t)( gdtp + X64_SEGMENT_CPUINFO_OFFSET );
	((tss64 *)addr)->iomap = sizeof(tss64);

	gdt[GDT_NULL1_SEL] = (uint64_t)0;
	gdt[GDT_NULL2_SEL] = (uint64_t)0;

	init_gdt_descriptor_table_entry((gdt_descriptor *)&gdt[GDT_KERN_CODE32_SEL], 
	    0, 0xffffffff, X86_DESC_RDWR, X86_DESC_EXEC, X86_DESC_DPL_KERNEL, 
	    X86_DESC_32BIT_MODE, X86_DESC_32BIT_SEG, X86_DESC_PAGE_SIZE);

	init_gdt_descriptor_table_entry((gdt_descriptor *)&gdt[GDT_KERN_DATA32_SEL], 
	    0, 0xffffffff, X86_DESC_RDWR, X86_DESC_NONEXEC, X86_DESC_DPL_KERNEL, 
	    X86_DESC_32BIT_MODE, X86_DESC_32BIT_SEG, X86_DESC_PAGE_SIZE);

	init_gdt_descriptor_table_entry((gdt_descriptor *)&gdt[GDT_KERN_CODE64_SEL], 
	    0, 0xffffffff, X86_DESC_RDWR, X86_DESC_EXEC, X86_DESC_DPL_KERNEL, 
	    X86_DESC_64BIT_MODE, X86_DESC_64BIT_SEG, X86_DESC_PAGE_SIZE);

	init_gdt_descriptor_table_entry((gdt_descriptor *)&gdt[GDT_KERN_DATA64_SEL], 
	    0, 0xffffffff, X86_DESC_RDWR, X86_DESC_NONEXEC, X86_DESC_DPL_KERNEL, 
	    X86_DESC_64BIT_MODE, X86_DESC_64BIT_SEG, X86_DESC_PAGE_SIZE);

	init_gdt_descriptor_table_entry((gdt_descriptor *)&gdt[GDT_USER_CODE64_SEL], 
	    0, 0xffffffff, X86_DESC_RDWR, X86_DESC_EXEC, X86_DESC_DPL_USER, 
	    X86_DESC_64BIT_MODE, X86_DESC_64BIT_SEG, X86_DESC_PAGE_SIZE);

	init_gdt_descriptor_table_entry((gdt_descriptor *)&gdt[GDT_USER_DATA64_SEL], 
	    0, 0xffffffff, X86_DESC_RDWR, X86_DESC_NONEXEC, X86_DESC_DPL_USER, 
	    X86_DESC_64BIT_MODE, X86_DESC_64BIT_SEG, X86_DESC_PAGE_SIZE);

	init_tss_descriptor((gdt_descriptor *)&gdt[GDT_TSS64_SEL], addr, sizeof(tss64) - 1);
	
	load_global_segment( (void *)gdt, (GDT_TSS64_SEL + 2) * sizeof(uint64_t));

	ltr(GDT_TSS64);

	*tssp = (tss64 *)addr;

	show_gdt_descriptor((gdt_descriptor *)&gdt[0]);
}

