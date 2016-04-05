/**********************************************************************/
/*  Tiny -- The Inferior operating system Nucleus Yeah!!              */
/*  Copyright 2001 Takeharu KATO                                      */
/*                                                                    */
/*  printf relevant routines                                          */
/*                                                                    */
/**********************************************************************/
#if !defined(_KERN_PRINTF_H)
#define  _KERN_PRINTF_H
#include <stdarg.h>

#define PRFBUFLEN        (128)  /*< �����Хåե�Ĺ                    */
int kprintf(const char *, ...);
#endif  /*  _KERN_PRINTF_H   */
