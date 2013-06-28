




/* Timer 1 for the QF Port to the LPC2300 

		MR0I 1 Interrupt on MR0: an interrupt is generated when MR0 matches  the value in the TC.
		
		MR0R 1 Reset on MR0: the TC will be reset if MR0 matches it. 

*/

#include "qp_port.h" 
#include "lpc23xx.h"
#include "push_button2.h"

static unsigned int count = 0;

 __irq void T1_IRQHandler(void ) {
	 
#ifdef XXX
			#ifdef QEP_TICK
 										/* ACK Timer1 int */
	 
	counter(1);	 
		
	if(count >= 100) {
		AD0CR |= 0x01000000;
		count = 0;
	}
	count++;

    QF_tick();
			#endif
#endif 

	
	#ifdef QF_TICK
#ifdef QK
 	__disable_irq(); 
  	  ++QK_intNest_; 
#endif  

	QF_INT_UNLOCK();
										/* ACK Timer1 int */
	counter(1);	 
		
	if(count >= 100) {
		AD0CR |= 0x01000000;
		count = 0;
	}
	count++;
	
	QF_tick();

	QF_INT_LOCK(); 
#ifdef QK 
	   --QK_intNest_;
	   if (   QK_intNest_ ==0) 
	   		QK_schedule_();
#endif
		 
#endif 
	
	
	


	

	T1IR        = 1;                      /* Clear interrupt flag               */

    VICVectAddr = 0;                      /* Acknowledge Interrupt              */
}

  	   

void Init_Timer1(void )	  {


 /* Enable and setup timer interrupt, start timer  
                           */
  T1MR0         = 11999;                       /* 1msec = 12000-1 at 12.0 MHz */
  T1MCR         = 3;                           /* Interrupt and Reset on MR0  */
  T1TCR         = 1;                           /* Timer0 Enable               */
  VICVectAddr5  = (unsigned long)T1_IRQHandler;/* Set Interrupt Vector        */
  VICVectPriority5  = 15;                           /* use it for Timer1 Priority  */
  VICIntEnable  = (1  << 5);                   /* Enable Timer0 Interrupt     */

  
}

