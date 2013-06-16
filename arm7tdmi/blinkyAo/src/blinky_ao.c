#include <qp_port.h>  
#include <qmbsp.h>
#include <stdint.h>
#include <assert.h>
#include <LPC23xx.H>
#include <stdio.h>

#define assert(ignore) ((void)0)

#define DEBUG
#ifdef DEBUG
#define DBG(fmt, args...) printf("%s: %s:%d: " fmt "\n", __FILE__, __FUNCTION__, __LINE__, ## args);;
#else
#define DBG(fmt, args...)
#endif

typedef struct _TimerEvt {

	QTimeEvt super;
	
} TimerEvt;

typedef struct _DisplayEvt {
	
	QEvent super;
	uint16_t value_ad;
	
} DisplayEvt;

typedef struct _Blinky {
	
	QActive super;
	
} Blinky;

enum BlinkySignals {
	
	TIME_TICK_SIG = Q_USER_SIG,
	DISPLAYING_SIG,
	CONVERTING_SIG,
	MAX_SIG
	
};

static void Blinky_ctor(Blinky *me);
static QState Blinky_initial(Blinky *me, const QEvent *e);

static QState Blinky_idle(Blinky *me, const QEvent *e);
static QState Blinky_converting(Blinky *me, const QEvent *e);
static QState Blinky_displaying(Blinky *me, const QEvent *e);

static TimerEvt timerEvt;  
static Blinky blinky;

static const QEvent *blinkyQueueSto[3]; // Event Queue
static DisplayEvt smlPoolSto[10]; /* small-size pool */


/* Function that initializes LEDs                                             */
void LED_Init(void) {
  PINSEL10 = 0;                         /* Disable ETM interface, enable LEDs */
  FIO2DIR  = 0x000000FF;                /* P2.0..7 defined as Outputs         */
  FIO2MASK = 0x00000000;
}

/* Function that turns on requested LED                                       */
void LED_On (unsigned int num) {
  FIO2SET = (1 << num);
}

/* Function that turns off requested LED                                      */
void LED_Off (unsigned int num) {
  FIO2CLR = (1 << num);
}

/* Function that outputs value to LEDs                                        */
void LED_Out(unsigned int value) {
  FIO2CLR = 0xFF;                       /* Turn off all LEDs                  */
  FIO2SET = (value & 0xFF);             /* Turn on requested LEDs             */
}

void Blinky_ctor(Blinky *me) {
	
	assert(me);
	
	QActive_ctor(&me->super, (QStateHandler)&Blinky_initial);
	QTimeEvt_ctor(&timerEvt.super, TIME_TICK_SIG);
}

QState Blinky_initial(Blinky *me, const QEvent *e) {
	
	assert(me);
	
	(void)e;               /* avoid compiler warning about unused parameter */
    
	return Q_TRAN(&Blinky_idle);
}

QState Blinky_idle(Blinky *me, const QEvent *e) {
	
	assert(me);
	
	switch (e->sig) {
   
		case Q_INIT_SIG: {
			
			DBG("Blinky idle: INIT");
			
			return Q_TRAN(&Blinky_displaying);
		}

		case Q_ENTRY_SIG: {
			
			DBG("Blinky idle: ENTRY");			
			// Wenn Q_TRAN(&Blinky_converting) hier stattfindet, findet keineTransition statt --> Programm bleibt hier stehen.			
			return Q_HANDLED();
		}
		
		case Q_EXIT_SIG: {
   				
			DBG("Blinky idle: EXIT");
									
			return Q_HANDLED();
		}	  
				
	}
	 
	return Q_SUPER(&QHsm_top);	
	
}

QState Blinky_displaying(Blinky *me, const QEvent *e) {
	
	assert(me);
	
	switch (e->sig) {
   
		case Q_INIT_SIG: {
			
			DBG("Blinky displaying: INIT");
			
			return Q_TRAN(&Blinky_converting);
		}

		case Q_ENTRY_SIG: {
			
			DBG("Blinky displaying: ENTRY");
			
			return Q_HANDLED();
		}
		
		case Q_EXIT_SIG: {
   				
			DBG("Blinky displaying: EXIT");
			
			return Q_HANDLED();
		}	  
		
		case DISPLAYING_SIG: {
			
 			unsigned int n;
 			unsigned int v;
			
			DBG("Blinky displaying: DISPLAYING");
			
			// Enable/ disable LEDs			
 			v = (5*((DisplayEvt *)e)->value_ad) >> 9;                 /* Scale the Value                    */
 			for (n = 0; n < 8; n++)               									/* Show the Value                     */
 				if (n < v) LED_On (n);              /* Turn LED On                        */
 				else       LED_Off(n);              /* Turn LED Off                       */
			
			
      return Q_HANDLED();
		}
		
	}
	 
	return Q_SUPER(&Blinky_idle);
}

QState Blinky_converting(Blinky *me, const QEvent *e) {
	
	assert(me);
	
	switch (e->sig) {
   
		case Q_INIT_SIG: {
			
			DBG("Blinky converting: INIT");			
			
			return Q_HANDLED();
		}

		case Q_ENTRY_SIG: {
			
			DBG("Blinky converting: ENTRY");
			
			// Send TIME_TICK_SIG in defined intervals
			QTimeEvt_postEvery(&timerEvt.super, (QActive *)me, 100);
			
			return Q_HANDLED();
		}
		
		case Q_EXIT_SIG: {
   				
			DBG("Blinky converting: EXIT");
			
			QTimeEvt_disarm(&timerEvt.super);
			
			return Q_HANDLED();
		}	  

		case TIME_TICK_SIG: {
			
			DBG("Blinky converting: TIME_TICK");
			// Start AD
			AD0CR |= 0x01000000;                  /* Start A/D Conversion               */
			
      return Q_HANDLED();
		}		
			
	}
	 
	return Q_SUPER(&Blinky_displaying);
}

/* A/D IRQ: Executed when A/D Conversion is done                              */
__irq void ADC_IRQHandler(void) {

	static unsigned int value_old = 0;
	unsigned int value_cur = (AD0DR0 >> 6) & 0x3FF;      /* Read Conversion Result */
	
	if(value_old != value_cur) {
				
		DisplayEvt *dspEvt = Q_NEW(DisplayEvt, DISPLAYING_SIG);
// 		static DisplayEvt dspEvt;
// 		static QEvent dispEvt = {DISPLAYING_SIG, 0};
// 		dspEvt.super = dispEvt;
		dspEvt->value_ad = value_cur;
		value_old = value_cur;
		QActive_postFIFO((QActive *)&blinky, (QEvent *)dspEvt);
		
	}

  VICVectAddr = 0;                      /* Acknowledge Interrupt              */
}

int main ()	{


	BSP_Init();
	BSP_LCD_display_str("Base ActiveObj",0,0);	  
	BSP_LCD_display_str("EventDrivenSys",0,1);

	LED_Init();                           /* LED Initialization  */
	
	/* Power enable, Setup pin, enable and setup AD converter interrupt         */
  PCONP        |= (1 << 12);                   /* Enable power to AD block    */
  PINSEL1       = 0x4000;                      /* AD0.0 pin function select   */
  AD0INTEN      = (1 <<  0);                   /* CH0 enable interrupt        */
  AD0CR         = 0x00200301;                  /* Power up, PCLK/4, sel AD0.0 */
  VICVectAddr18 = (unsigned long)ADC_IRQHandler;/* Set Interrupt Vector       */
  VICVectCntl18 = 14;                          /* use it for ADC Interrupt    */
  VICIntEnable  = (1  << 18);                  /* Enable ADC Interrupt        */

	
	QF_init();    /* initialize the framework and the underlying RT kernel */

													
	Blinky_ctor(&blinky);	/* instantiate and start the active objects... */

 	QF_poolInit( (void*)&smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));
 	QActive_start((QActive *)&blinky, 1, blinkyQueueSto, Q_DIM(blinkyQueueSto),
 									(void *)0, 0, (QEvent *)0);


	QF_run();                                     /* run the QF application */

	return(0);
}
