#include "ad_ao.h"
#include "log.h"
#include "qf.h"

typedef struct _TimerEvt {

	QTimeEvt super;
	
} TimerEvt;

typedef struct _Ad {
	
	QActive super;
	
} Ad;

static QState ad_initial(Ad *me, const QEvent *e);
static QState ad_idle(Ad *me, const QEvent *e);
static QState ad_converting(Ad *me, const QEvent *e);


static TimerEvt timerEvt;  
static Ad ad;

const QActive *adAO = (QActive *)&ad;

void ad_ctor(void) {
		
	Ad *me = &ad;
	QActive_ctor(&me->super, (QStateHandler)&ad_initial);
	QTimeEvt_ctor(&timerEvt.super, START_AD_SIG);
}

QState ad_initial(Ad *me, const QEvent *e) {
	
	(void)e;               /* avoid compiler warning about unused parameter */
    
	return Q_TRAN(&ad_idle);
}

QState ad_idle(Ad *me, const QEvent *e) {
	
	switch (e->sig) {
   
		case Q_INIT_SIG: {
			
			DBG("AD idle: INIT");
			
			return Q_TRAN(&ad_converting);
		}

		case Q_ENTRY_SIG: {
			
			DBG("AD idle: ENTRY");			
			
			return Q_HANDLED();
		}
		
		case Q_EXIT_SIG: {
   				
			DBG("AD idle: EXIT");
									
			return Q_HANDLED();
		}	  
				
	}
	 
	return Q_SUPER(&QHsm_top);	
	
}

QState ad_converting(Ad *me, const QEvent *e) {
	
	switch (e->sig) {
   
		case Q_INIT_SIG: {
			
			DBG("AD converting: INIT");			
			
			return Q_HANDLED();
		}

		case Q_ENTRY_SIG: {
			
			DBG("AD converting: ENTRY");
			
			// Send TIME_TICK_SIG in defined intervals
			QTimeEvt_postEvery(&timerEvt.super, (QActive *)me, START_AD_CONVERSION_INTERVAL);
			
			return Q_HANDLED();
		}
		
		case Q_EXIT_SIG: {
   				
			DBG("AD converting: EXIT");
			
			QTimeEvt_disarm(&timerEvt.super);
			
			return Q_HANDLED();
		}	  

		case START_AD_SIG: {
			
			DBG("AD converting: TIME_TICK");
			// Start AD
			AD0CR |= 0x01000000;                  /* Start A/D Conversion               */
			
      return Q_HANDLED();
		}		
			
	}
	 
	return Q_SUPER(&ad_idle);
}

/* A/D IRQ: Executed when A/D Conversion is done                              */
__irq void ADC_IRQHandler(void) {

	static unsigned int value_old = 0;
	unsigned int value_cur = (AD0DR0 >> 6) & 0x3FF;      /* Read Conversion Result */
	
	// Result converting from 0 to 100
	
	if(value_old != value_cur) {
				
 		static AdValueChangedEvt adValueEvt;
 		static const QEvent adQEvt = {AD_VALUE_SIG, 0};
 		adValueEvt.super = adQEvt;
		adValueEvt.value = value_cur;
		value_old = value_cur;
		QF_publish((QEvent *)&adValueEvt);
		
	}

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

