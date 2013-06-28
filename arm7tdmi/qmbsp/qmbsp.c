/*****************************************************************************
* Product: Board Support Package (BSP) for the QM examples
* Keil ARM MDK  2300 
* Dr. Ron Barker 
*****************************************************************************/
#include "qmbsp.h"
#include "LCD.h"

#include <stdio.h>
#include <stdlib.h>



/*BSP Init for the MC2300 */

void BSP_Init( ) {

	int i;  

  Init_Timer1( );
  init_serial();                               /* Init UART                   */
  uart_init_0 ( );
  lcd_init();
  lcd_clear();
  lcd_print ("GESCHISSE");
  set_cursor (0, 1);
  lcd_print ("EDS");
  for (i = 0; i < 10000; i++);       /* Wait for initial display           */


}


/*..........................................................................*/
void BSP_display(uint8_t timeout) {
    printf("[%3d] ", (int)timeout);
    fflush(stdout);
}


/*..........................................................................*/
void BSP_display_str(char *msg) {
     printf("%s", msg);

  
}
/*..........................................................................*/
void BSP_exit(void) {
    printf("Bye, Bye!");
    _sys_exit(0);
}

void BSP_delay ( int val ){

int i;
	for(i=0;i<val; i++);

}


#ifdef BOMB
void BSP_boom(void) {

	printf("boom!!!!\n");
	_sys_exit(0);
}
#endif

void  BSP_LCD_display_str( char * str, int line, int pos){

  set_cursor (line,pos);
  lcd_print ( (unsigned char const*) str);


};



#ifdef BSP_KEYBOARD


int _kbhit(void ) {

	int i;
	 i = 0xFF;
	 return(i);
}


int _getch()

{
	 FILE *f;
 return	 ( fgetc(  f) );
}
	

#endif 




/*..........................................................................*/
/* this function is used by the QP embedded systems-friendly assertions */
void Q_onAssert(char const * const file, int line) {
    printf("Assertion failed in %s, line %d", file, line);
    fflush(stdout);
    _sys_exit(-1);
}
/*..........................................................................*/
/* Application specific callbacks from the framework  */
void QF_onCleanup(void)	  {

	 ;

	 }

void QF_onIdle(void)	  {

	
	 ;

	 }
void QF_onStartup(void)	  {

	
	 ;

	 }
