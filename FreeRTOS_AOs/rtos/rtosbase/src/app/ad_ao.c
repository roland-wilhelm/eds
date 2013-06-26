#include "events.h"

#include "ad_ao.h"
#include "log.h"
#include "qf.h"


/* A/D IRQ: Executed when A/D Conversion is done                              */
__irq void ADC_IRQHandler(void) {

	static unsigned int value_old = 0;
	unsigned int value_cur = (AD0DR0 >> 6) & 0x3FF;      /* Read Conversion Result */
	
	VICIntEnClr  = (1  << 18);                  /* Disable ADC Interrupt        */
	
	value_cur = value_cur / 10.2;
	
	// Result converting from 0 to 100
	printf("ADC %d\n\r", value_cur);
	if(value_old != value_cur) {
				
		
 		static AdValueChangedEvt adValueEvt;
 		static const QEvent adQEvt = {AD_VALUE_SIG, 0};
 		adValueEvt.super = adQEvt;
		adValueEvt.value = value_cur;
		value_old = value_cur;
		//printf("ADC value sent %d\n\r", value_old);
		QF_publish((QEvent *)&adValueEvt);
		
	}

	EXTINT = 0x01; 												// Clear the peripheral interrupt flag
	VICIntEnable  = (1  << 18);           /* Enable ADC Interrupt        */
  VICVectAddr = 0;                      /* Acknowledge Interrupt              */
}

int ad_converter_init() {
	
	/* Power enable, Setup pin, enable and setup AD converter interrupt         */
  PCONP        |= (1 << 12);                   /* Enable power to AD block    */
  PINSEL1       = 0x4000;                      /* AD0.0 pin function select   */
  AD0INTEN      = (1 <<  0);                   /* CH0 enable interrupt        */
  AD0CR         = 0x00200301;                  /* Power up, PCLK/4, sel AD0.0 */
  VICVectAddr18 = (unsigned long)ADC_IRQHandler;/* Set Interrupt Vector       */
  VICVectCntl18 = 14;                          /* use it for ADC Interrupt    */
  VICIntEnable  = (1  << 18);                  /* Enable ADC Interrupt        */
	
	return 0;
}

int start_ad_conversion() {
	
	DBG("AD start converting.");
	// Start A/D Conversion
	AD0CR |= 0x01000000;
	
	return 0;
}


