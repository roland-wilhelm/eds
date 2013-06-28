#include "push_button2.h"
#include <LPC23xx.H>
#include "events.h"
#include <stdio.h>

int counter(int incr){
	static unsigned int counter=0;

	if(incr!=0)
		counter+=incr;
	
	return counter;
}

void isr_handler_eint0(void) __irq // for external interrupt 0
{
	static unsigned int timestop=0, timestart=0;
	unsigned int diff=0;
	
	VICIntEnClr = (1 << 14); // Disable EINT0 in the VIC
	
	// Wenn nachfolgend auskommentiert, kein aufruf mehr von IRQ
	vPortEnterCritical ();
	
	if(EXTPOLAR == 0x00){ //falling edge
		timestart=counter(0);
		EXTPOLAR = 1; // next interrupt on falling edge
	}
	else{ //rising edge
		timestop=counter(0);
		EXTPOLAR = 0; // next interrupt on rising edge
		diff=timestop-timestart;
		if(diff >= BUTTON_PRESSED_LONG_TIME){ 	//500msec
			static QEvent buttonLongEvt = {BUTTON_LONGPRESS_SIG, 0};
			QF_publish(&buttonLongEvt);
		}
		else{
			// time < BUTTON_PRESSED_LONG_TIME --> BUTTON_PRESSES_SHORT
			static QEvent buttonShortEvt = {BUTTON_SHORTPRESS_SIG, 0};
			QF_publish(&buttonShortEvt);
			
		}
		
		timestop = 0;
		timestart = 0;
	}
	

	EXTINT = 0x01; // Clear the peripheral interrupt flag
	VICIntEnable = (1 << 14); // Enable EINT0 in the VIC
	VICVectAddr = 0; // Acknowledge Interrupt
	
	vPortExitCritical ();
}

void int0_init(void)
{
	// P2.10 as EINT0 interrupt    (push button on MCB2300)
	PINSEL4 |= 0x01 << 20;
	// EINT0 is edge-sensitive
	EXTMODE = 0x01;
	// assign our ISR handler function to this VIC address
	VICVectAddr14 = (unsigned) isr_handler_eint0;
	// Interrupt 1 rising edge - 0 is falling edge
	EXTPOLAR = 0x00;
	// P2.10 is input
	FIO2DIR &= ( 0xffffffff^(1 << 10) );
	// Clear the peripheral interrupt flag
	EXTINT = 0x01;
	// Priority
	VICVectPriority14 = 12;
	// Enable EINT0
	VICIntEnable = (1 << 14);
}
