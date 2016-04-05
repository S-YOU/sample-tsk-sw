/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  low level screen manipulation                                     */
/*                                                                    */
/*                                                                    */
/**********************************************************************/
#include <stddef.h>
#include <stdint.h>
#include "kern/kernel.h"

#define COLUMNS			(80)  /* columns.  */
#define LINES			(25)  /* lines.  */
#define ATTRIBUTE		 (7)  /* attribute  */

#define VRAM_ADDR               (0xB8000) /* VRAM Addresss  */
#define MC6845_INDEX_REG 	 (0x03D4)  /* VRAM Controler(M6845) Index Register */
#define MC6845_DATA_REG  	 (0x03D5)  /* VRAM Controler(M6845) Data Register */

static int xpos; /*  X position.  */
static int ypos; /*  Y position.  */
static volatile unsigned char *video; /* Pointer to the video memory.  */

static kconsole_t raw_console = KCONSOLE_INITILIZER(raw_console);

static void
raw_locate(int x,int y) {
	unsigned short vram_pos = (y * COLUMNS) + x;
	psw_t psw;

	psw_disable_interrupt(&psw);	

	xpos = x; /*  ������, ���������å��򤷤ʤ��Ȥޤ���  */
	ypos = y;

	/* ���������ư  */

	out_port_byte (MC6845_INDEX_REG, 0x0F); /* set low-order address of cursor */
	out_port_byte (MC6845_DATA_REG, (unsigned char)(vram_pos & 0xFF)); 
	out_port_byte (MC6845_INDEX_REG, 0x0E); /* set high-order address of cursor */
	out_port_byte (MC6845_DATA_REG, (unsigned char)((vram_pos >> 8) & 0xFF));

	psw_restore_interrupt(&psw);	
}

static void
raw_do_scroll(void) {
	unsigned short col=0;
	int cx,cy;
	psw_t psw;

	video = (unsigned char *) VRAM_ADDR;

	psw_disable_interrupt(&psw);
	for(cx = 0; cx < COLUMNS; ++cx) {
		for(cy = 0; cy < LINES-1; ++cy) {
			/*  �Ǵ���1�Ԥ������ɽ��  */
			*(video + (cx + cy * COLUMNS) * 2)  = *(video + (cx + (cy+1) * COLUMNS) * 2) ;
			*(video + (cx + cy * COLUMNS) * 2 + 1) = *(video + (cx + (cy + 1) * COLUMNS) * 2 + 1) ;
	
		}
	}

	for(cx = 0; cx < COLUMNS; ++cx) { /*  �ԥ��ꥢ  */
		raw_locate(cx,LINES-1);
		*(video + (cx + (LINES-1) * COLUMNS) * 2) =  0x0;
		*(video + (cx + (LINES-1) * COLUMNS) * 2 + 1) = ATTRIBUTE;
	}

	psw_restore_interrupt(&psw);

	/* ���������ư  */
	raw_locate(0,LINES-1);
}


/* 
 *  ���̾õ�ؿ�
 */
static void
raw_cls (void) {
	int i;
	psw_t psw;

	video = (unsigned char *) VRAM_ADDR;

	psw_disable_interrupt(&psw);
	for (i = 0; i < COLUMNS * LINES * 2; i++)
		*(video + i) = 0; 	/*  �֥�󥯤�񤭹���  */

	psw_restore_interrupt(&psw);

	raw_locate(0,0); /*  �ݥ��������᤹  */

	return;
}

/* Put the character C on the screen.  */
static void
raw_putchar (int c) {
	psw_t psw;

	video = (unsigned char *) VRAM_ADDR;

	psw_disable_interrupt(&psw);

	if (c == '\n' || c == '\r'){
	newline:
		xpos = 0;
		ypos++;
		if (ypos == LINES)
			raw_do_scroll();
		psw_restore_interrupt(&psw);
		return;
	}

	*(video + (xpos + ypos * COLUMNS) * 2) = c & 0xFF;
	*(video + (xpos + ypos * COLUMNS) * 2 + 1) = ATTRIBUTE;

	xpos++;
	if (xpos >= COLUMNS)
		goto newline;

	psw_restore_interrupt(&psw);

	raw_locate(xpos,ypos);
}

void
init_raw_console(void) {

	raw_console.putchar = raw_putchar;
	raw_console.cls = raw_cls;
	raw_console.locate = raw_locate;
	register_kconsole(&raw_console);
}
