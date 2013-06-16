/******************************************************************************/
/* SERIAL.C: Low Level Serial Routines                                        */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005-2006 Keil Software. All rights reserved.                */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/


#define BAUD15  // use 155200

#include <LPC23xx.H>                     /* LPC23xx definitions               */
#include "qp_port.h" 
#ifdef QM_FREERTOS
 #include "FreeRTOS.h"
#endif 
#define UART0                          /* Use UART 0 for printf             */
void BSP_onKeyboardInput(int buf);
static int keystroke;
static int buf;

/* If UART 0 is used for printf                                               */
#ifdef UART0
  #define UxFDR  U0FDR
  #define UxLCR  U0LCR
  #define UxDLL  U0DLL
  #define UxDLM  U0DLM
  #define UxLSR  U0LSR
  #define UxTHR  U0THR
  #define UxRBR  U0RBR
/* If UART 1 is used for printf                                               */
#elif defined(UART1)
  #define UxFDR  U1FDR
  #define UxLCR  U1LCR
  #define UxDLL  U1DLL
  #define UxDLM  U1DLM
  #define UxLSR  U1LSR
  #define UxTHR  U1THR
  #define UxRBR  U1RBR
#endif




void init_serial (void)  {               /* Initialize Serial Interface       */
  #ifdef UART0
    PINSEL0 |= 0x00000050;               /* Enable TxD0 and RxD0              */
  #elif defined (UART1)
    PINSEL0 |= 0x40000000;               /* Enable TxD1                       */
    PINSEL1 |= 0x00000001;               /* Enable RxD1                       */
  #endif
  UxFDR    = 0;                          /* Fractional divider not used       */
  UxLCR    = 0x83;                       /* 8 bits, no Parity, 1 Stop bit     */
  UxDLL    = 78;                         /* 9600 Baud Rate @ 12.0 MHZ PCLK    */
  UxDLM    = 0;                          /* High divisor latch = 0            */
  UxLCR    = 0x03;                       /* DLAB = 0                          */
}


/* Implementation of putchar (also used by printf function to output data)    */
int sendchar (int ch)  {                 /* Write character to Serial Port    */

	if ( ch == 0 ) return (-1);
  while (!(UxLSR & 0x20));

  return (UxTHR = ch);
}


int getkey (void)  {                     /* Read character from Serial Port   */

  while (!(UxLSR & 0x01));
 

  return (UxRBR);
}


int getkey0 ( void ) {
	int tmp;
	tmp = buf;
	buf=0;
	return( tmp );

}

int get_char(){

	 while (!(U0LSR & 0x01));
	 return (U0RBR);
 }

int send_char( int ch) {

   while (!(U0LSR & 0x20));

     return ( U0THR = ch );

}

__irq  void uart_isr0(void ){
	
 	
	int ch;
#ifdef QK
 
  	  ++QK_intNest_; 
#endif  

	
	ch=get_char();	
	buf = ch;
	keystroke =ch;
#ifdef QM_FREERTOS
  vPortEnterCritical ();
#endif 

#ifdef QM_ActiveObject
  

	BSP_onKeyboardInput(buf);
#endif


// echo 	send_char(ch1); 
	#ifdef QK 
	   --QK_intNest_;
	   if (   QK_intNest_ ==0) 
	   		QK_schedule_();
#endif
	VICVectAddr = 0;                      /* Acknowledge Interrupt              */
#ifdef QM_FREERTOS
  vPortExitCritical ();
#endif
}



void uart_init_0 ( void ) {


	    PINSEL0 |= 0x00000050;               /* Enable TxD0 and RxD0              */
#ifdef BAUD15
	U0LCR = 0x83; /* 8 bits, no Parity, 1 Stop bit*/ 
	U0DLL = 3; /* for 12MHz PCLK Clock 115200 */ 
	U0FDR = 0x67; /* Fractional Divider */ 
	U0LCR = 0x03; /* DLAB = 0 */
	U0DLM    = 0;
#else

			U0FDR    = 0;                          /* Fractional divider not used       */
  		U0LCR    = 0x83;  // 0x83;                       /* 8 bits, no Parity, 1 Stop bit     */
  		U0DLL    = 78;                         /* 9600 Baud Rate @ 12.0 MHZ PCLK    */
 	    U0DLM    = 0;                          /* High divisor latch = 0            */
  		U0LCR    = 0x03;                       /* DLAB = 0                          */
#endif
			U0IER    = 0x01;
			U0FCR    = 0x02;
			U0FCR    = 0x81;	 
	
	
	


	
	
			VICVectAddr6  = (unsigned long)uart_isr0;			/* Set Interrupt Vector        */
  		VICVectCntl6  = 12;                           /* use it for UART Priority  */
  		VICIntEnable  = (1  << 6);                   /* Enable UART= Interrupt     */

}

int kbhit(void) {

	if (keystroke )	{
		keystroke=0;
		return(1);
		}
	else
		return(0);
}
		


