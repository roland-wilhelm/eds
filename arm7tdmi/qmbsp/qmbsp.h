
/*****************************************************************************
* Product: Board Support Package (BSP) for the QM examples
* Keil ARM MDK  2300 
* Dr. Ron Barker 
*****************************************************************************/

#ifndef  qmbsp_h
#define qmbsp_h

             /* Exact-width types. WG14/N843 C99 Standard, Section 7.18.1.1 */
#include <stdint.h>

	   /* defines for AOs */

# define BSP_TICKS_PER_SEC		 12000000/100

/*  bsp function references for the mcb 2300 */

void    Init_Timer1(void );
void    init_serial(void );                               /* Init UART                   */
void    uart_init_0(void );
void    _sys_exit(int);

#define _exit(x)  _sys_exit(x)

/* bsp function references for standalone port of QM FSM 7 HSM */

void    delay( int time );

int  _kbhit(void);
int   kbhit(void);
int  _getch(void);
int   getch(void);
void _sys_exit(int);




void BSP_display(uint8_t timeout);
void BSP_boom(void);
void BSP_LCD_display_str(char *msg, int pos, int line);
void BSP_display_str(char *msg);
void BSP_exit(void);	 
void BSP_Init(void);
void BSP_delay(int delay);



#endif                                                             /* bsp_h */
