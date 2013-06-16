/**
 * pushbutton.h
 *
 * franz schauer
 * june 16, 2013
 */
 
#ifndef PUSHBUTTON_H_
#define PUSHBUTTON_H_

//#define USE_EINT0

void pushbutton_init(unsigned int longPressTime_ms);

__irq void T1_IRQHandler (void);
__irq void P2_10_IRQHandler(void);


#endif // PUSHBUTTON_H_
