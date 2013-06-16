/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <stdio.h>
#include <rt_misc.h>

#pragma import(__use_no_semihosting_swi)


extern int  send_char(int ch);  /* in serial.c */
extern int  getkey0(void);


struct __FILE { int handle; /* Add whatever you need here */ };

FILE __stdout;


int fputc(int ch, FILE *f) {
 
 if ( ch == 0x0a	)
	send_char(0x0d);

	return (send_char(ch));
}


int getch(void) {

return (getkey0()) ;

}

int fgetc( FILE *f) {
						    
	return(getkey0());	  /* in serial.c */
}


#ifdef xxx
int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}
#endif

void _ttywrch(int ch) {

  send_char(ch);
  


}


void _sys_exit(int return_code) {
label:  goto label;  /* endless loop */
}
