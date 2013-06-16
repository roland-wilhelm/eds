/******************************************************************************/
/*  Harness main program for initialisation of MCB2300 Platform for:
    BASE 
	Example Program for Event Driven Seminar Munich Univeristiy
	Dr. Ron Barker  
	Keil 2300 - LPC 2378 
	Standalone - no RTOS
 */
/******************************************************************************/
                  
#include <stdio.h>
#include <LPC23xx.H>                     /* LPC23xx definitions                */ 
#include "LCD.h"                        /* Graphic LCD function prototypes    */
#include "qmbsp.h"




int main (void) {
	 int i, count;
	 i=0;
	 count =50;
     BSP_Init();
	 BSP_LCD_display_str("BASE  MCB 2300",0,0);	  
	 BSP_LCD_display_str("EventDrivenSys",0,1);
     while(i++ < count) {

		BSP_display_str("EDS BASE \n");

		delay(100);
		}
	
	 _exit(0);



}

