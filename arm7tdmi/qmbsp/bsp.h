
/*****************************************************************************
* Product: Board Support Package (BSP) for the QM examples
* Keil ARM MDK  2300 
* Dr. Ron Barker 
*****************************************************************************/

#ifndef bsp_h
#define bsp_h

             /* Exact-width types. WG14/N843 C99 Standard, Section 7.18.1.1 */
#include <stdint.h>

/*  bsp function references for the mcb 2300 */

void delay( int time );



int _kbhit(void);
int _getch(void);



#define _exit(x)  _sys_exit(x)

void BSP_display(uint8_t timeout);
void BSP_boom(void);




#endif                                                             /* bsp_h */
