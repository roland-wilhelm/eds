/**
 * pushbutton.c
 *
 * franz schauer
 * june 16, 2013
 */

#include <stdbool.h>
#include <LPC23xx.H>
#include "qf_pkg.h"

#include "pushbutton.h"
#include "events.h"


__irq void T1_IRQHandler (void);
__irq void P2_10_IRQHandler(void);

/**
 * Init Timer1
 */
void T1_init(unsigned int longPressTime_ms)
{
	// enable and setup timer interrupt
  T1MR0         		= (longPressTime_ms * 12000) - 1;	// 1msec = 12000-1 at 12.0 MHz
  T1MCR         		= 3;															// interrupt and Reset on MR0
  VICVectAddr5  		= (unsigned long)T1_IRQHandler;		// set IRQ handler
  VICVectPriority5  = 15;															// use it for Timer1 interrupt
  VICIntEnable  		= (1 << 5);												// enable Timer1 Interrupt
	
	// do not start Timer1 yet
}

void T1_start()
{
	T1TC  = 0;	// reset timer counter
	T1TCR = 1;  // start Timer1
}

void T1_stop()
{
	T1TCR = 0;  // stop Timer1
}

/**
 * Init P2.10
 */
void P2_10_init()
{
	/* Setup P2.10 for GPIO interrupt (rising and falling edge!) */
	PINSEL4 &= ( (0xffffffff^(1 << 20))^(1 << 21) );	// use P2.10 as GPIO pin
	FIO2DIR &= ( 0xffffffff^(1 << 10) );	// P2.10 is input
	
	IO2_INT_EN_R |= (1 << 10); // interrupt on rising edge
	IO2_INT_EN_F |= (1 << 10); // interrupt on falling edge
	
	VICVectPriority17 = 12;
	
	VICVectAddr17 = (unsigned long)P2_10_IRQHandler;
	VICIntEnable |= (1 << 17);
}

/**
 * Pushbutton init, uses Timer1 and P2_10
 */
void pushbutton_init(unsigned int longPressTime_ms)
{
	T1_init(longPressTime_ms);
	P2_10_init();
}

/**
 * Timer0 ISR: Executed if button is pressed long enough
 */
__irq void T1_IRQHandler (void) 
{
	// publish ButtonLongPressed
	static QEvent buttonLongEvt = {BUTTON_LONGPRESS_SIG, 0};
	
	QF_INT_LOCK();
	
	QF_publish(&buttonLongEvt);
	
	T1_stop();
	
	QF_INT_UNLOCK();
	
  T1IR        = 1;	// clear interrupt flag
  VICVectAddr = 0;	// acknowledge interrupt
}

/**
 * P2.10 ISR: Executed on falling/rising edges on P2.10
 */
__irq void P2_10_IRQHandler(void)
{	
	QF_INT_LOCK();
	
	if (IO2_INT_STAT_F & (1 << 10))
	{
		// button pressed
		
		// start timer, if button is pressed long enough a timer interrupt is triggered
		T1_start();
	}
	else // if (IO2_INT_STAT_F & (1 << 10))
	{
		// button released
		if (T1TCR == 1)
		{
			// publish ButtonShortPressed
			static QEvent buttonShortEvt = {BUTTON_SHORTPRESS_SIG, 0};
			QF_publish(&buttonShortEvt);
			
			// Timer0 still running, no long press yet
			T1_stop();
		}
		else
		{
			// ButtonLongPressed already dispatched in timer ISR to allow fast user feedback
		}
	}
	
	QF_INT_UNLOCK();
	
	IO2_INT_CLR |= (1 << 10); // clear interrupt flag for GPIO pin P2.10	
  VICVectAddr = 0; // acknowledge interrupt
}
