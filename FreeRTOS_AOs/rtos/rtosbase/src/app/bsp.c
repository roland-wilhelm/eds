	   
#include "qp_port.h"                /* the port of the QEP event processor */
#include "bsp.h"
#include "LCD.h"
#include "../includes/led.h"
#include <LPC23xx.H> 
#include <stdio.h>
#include <stdlib.h>   
#include "ad_ao.h"
#include "push_button2.h"

/* MCB2300 board specific initalisation, display and other go here */

void BSP_Init(void )  {

  int i;
  

	init_eth();

  uart_init_0();
	LED_Init();
  lcd_init();
  lcd_clear();
  lcd_print ("GESCHISSE");
  set_cursor (0, 1);
  lcd_print ("EDS");
  for (i = 0; i < 10000; i++);       /* Wait for initial display           */

}

void BSP_Start_AD (void) {

  /* test for IPC on AO - move to new state later */
   AD0CR |= 0x01000000;                  /* Start A/D Conversion               */
	//  start_ad();

}


void BSP_Display_Serial(short AD_value) {

 printf ("AD value = 0x%03x\n", AD_value);

}


void BSP_Display_LCD(int pos, int row, int value) {

   Disp_Bargraph( pos, row, value);
}


/**  user supplied qep framework call backs go here */

	 /* this function is used by the QP embedded systems-friendly assertions */
void Q_onAssert(char const * const file, int line) {
    printf("Assertion failed in %s, line %d", file, line);
//    fflush(stdout);
    _sys_exit(-1);
}


void QF_onCleanup(void)	  {

	 ;

	 }
	

void QF_onIdle(void)	  {

	
	 ;

	 }
void QF_onStartup(void)	  {

	;

}
