#include "qp_port.h"                /* the port of the QEP event processor */
#include "blinky_ao.h"                                   /* board support package */							   
  
 #include <LPC23xx.H>                    /* LPC23xx definitions                */
  

static AD_Event ad;

extern QActive * const Blinky;

/* A/D IRQ: Executed when A/D Conversion is done                              */
__irq  void ADC_IRQHandler(void) {

 // AD_last = (AD0DR0 >> 6) & 0x3FF;      /* Read Conversion Result             */

 
   ad.new_conversion_value = (AD0DR0 >> 6) & 0x3FF;      /* Read Conversion Result             */
#ifdef XXX
	__disable_irq();
    QActive_postFIFO( Blinky, (QEvent *)&ad);    

 	__enable_irq();
	
#endif 
	
	
	
#ifdef QK
 	__disable_irq(); 
  	  ++QK_intNest_; 
#endif  

	QF_INT_UNLOCK();
	ad.new_conversion_value = (AD0DR0 >> 6) & 0x3FF;
	 QActive_postFIFO( Blinky, (QEvent *)&ad);


QF_INT_LOCK(); 
#ifdef QK 
	   --QK_intNest_;
	   if (   QK_intNest_ ==0) 
	   		QK_schedule_();
#endif
		 
	
	
	
  VICVectAddr = 0;                      /* Acknowledge Interrupt              */
}



  void init_ad( void) {
  
   ad.super.sig = 	 AD_READY_SIG;

  /* Power enable, Setup pin, enable and setup AD converter interrupt         */
  PCONP        |= (1 << 12);                   /* Enable power to AD block    */
  PINSEL1       = 0x4000;                      /* AD0.0 pin function select   */
  AD0INTEN      = (1 <<  0);                   /* CH0 enable interrupt        */
  AD0CR         = 0x00200301;                  /* Power up, PCLK/4, sel AD0.0 */
  VICVectAddr18 = (unsigned long)ADC_IRQHandler;/* Set Interrupt Vector       */
  VICVectCntl18 = 15;                          /* use it for ADC Interrupt    */
  VICIntEnable  = (1  << 18);                  /* Enable ADC Interrupt        */


  }
