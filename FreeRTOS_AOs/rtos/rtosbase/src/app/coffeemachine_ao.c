#include "../includes/led.h"

#include "coffeemachine_ao.h"
#include "events.h"
#include "../drivers/rtc.h"
#include "log.h"

#define ALL_LEDS_OFF 0
#define TOTAL_NUMBER_OF_LEDS 8
#define BREW_STRENGTHS 3
#define LEDS_PER_BREW_STRENGH (TOTAL_NUMBER_OF_LEDS / BREW_STRENGTHS)

// instance of CoffeeMachineAO and opaque pointer
static CoffeeMachineAO l_CoffeeMachineAO;
QActive* const CoffeeMachineAOBase = (QActive*)&l_CoffeeMachineAO;

// prototypes
static QState CoffeeMachineAO_initial(CoffeeMachineAO *me, QEvent const *e);
static QState CoffeeMachineAO_Idle(CoffeeMachineAO *me, QEvent const *e);
static QState CoffeeMachineAO_Brewing(CoffeeMachineAO *me, QEvent const *e);

/**
 * constructor
	*/
void CoffeeMachineAO_ctor(void)
{
	 CoffeeMachineAO* me = &l_CoffeeMachineAO;
	 QActive_ctor(&me->super, (QStateHandler)&CoffeeMachineAO_initial);
}

/**
* initial transition
**/
static QState CoffeeMachineAO_initial(CoffeeMachineAO *me, QEvent const *e)
{
	QActive_subscribe((QActive *)me, ALARM_SIG);
	
	return Q_TRAN(&CoffeeMachineAO_Idle);
}

/**
 * Idle state handler
 */
static QState CoffeeMachineAO_Idle(CoffeeMachineAO *me, QEvent const *e)
{
	 switch ( e->sig )
	 {
		 case Q_INIT_SIG:
		 {
			 DBG("CoffeeMachine Idle: INIT");
			 return Q_HANDLED();
		 }
		 
		 case Q_ENTRY_SIG:
		 {
			 DBG("CoffeeMachine Idle: ENTRY");
			 LED_Out(ALL_LEDS_OFF);
			 return Q_HANDLED();
		 }
		 
		 case ALARM_SIG:
		 {
			 DBG("CoffeeMachine Idle: ALARM");
			 return Q_TRAN(&CoffeeMachineAO_Brewing);
		 }
		 
		 case BREWSTRENGTH_SET_SIG:
		 {
			 int brewStrength;
			 int LEDsToFlash;
			 int i;
			 
			 DBG("CoffeeMachine Idle: BREW STRENGH SET");
			 
			 brewStrength = ((BrewStrengthSetEvt const *)e)->brewStrength; // exptected value: 0, 1 or 2 (weak, medium or strong)
			 brewStrength++; // make sure that value is between 1 and 3
			 LEDsToFlash = brewStrength * LEDS_PER_BREW_STRENGH;
			 
			 LED_Out(ALL_LEDS_OFF);
			 
			 for(i = 0; i < LEDsToFlash; i++)
				LED_On(i);
			
			 return Q_HANDLED();
		 }
		 
		 case Q_EXIT_SIG:
		 {
			 DBG("CoffeeMachine Idle: EXIT");
			 return Q_HANDLED();
		 }
	 }
	 
	 return Q_SUPER(&QHsm_top);
}

/**
 * Brewing state handler
 */
static QState CoffeeMachineAO_Brewing(CoffeeMachineAO *me, QEvent const *e)
{
	 switch ( e->sig )
	 {
		 case Q_INIT_SIG:
		 {
			 DBG("CoffeeMachine Brewing: INIT");
			 return Q_HANDLED();
		 }
		 
		 case Q_ENTRY_SIG:
		 {
			 DBG("CoffeeMachine Brewing: ENTRY");
			 QActive_subscribe((QActive *)me, BUTTON_SHORTPRESS_SIG);
			 return Q_HANDLED();
		 }
		 
		 case BUTTON_SHORTPRESS_SIG:
		 {
			 DBG("CoffeeMachine Brewing: BUTTON SHORT PRESS");
			 return Q_TRAN(&CoffeeMachineAO_Idle);
		 }
		 
		 case Q_EXIT_SIG:
		 {
			 DBG("CoffeeMachine Brewing: EXIT");
			 QActive_unsubscribe((QActive *)me, BUTTON_SHORTPRESS_SIG);
			 return Q_HANDLED();
		 }
	 }
	 
	 return Q_SUPER(&QHsm_top);
}
