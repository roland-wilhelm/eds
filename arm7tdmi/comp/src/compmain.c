/******************************************************************************/
/*  Harness main program for initialisation of MCB2300 Platform for:
    FSM Generic Event Process Quantum  Implementation Example Bomb4
	Example Program for Event Driven Seminar Munich Univeristiy
	Dr. Ron Barker  
	Keil 2300 - LPC 2378 
	Standalone - no RTOS
 */
/******************************************************************************/
                  
#include <stdio.h>
#include <LPC23xx.H>                     /* LPC23xx definitions                */ 
#include "LCD.h"                        /* Graphic LCD function prototypes    */
#include "bsp.h"


extern void comp_main (void);

int main (void) {
  int i;
  

  Init_Timer1( );
  init_serial();                               /* Init UART                   */
  uart_init_0 ( );
  lcd_init();
  lcd_clear();
  lcd_print ("HSM AO Compare");
  set_cursor (0, 1);
  lcd_print ("EventDrivenSystem");

  for (i = 0; i < 10000; i++);       /* Wait for initial display           */
 

  comp_main();

 
 

}

