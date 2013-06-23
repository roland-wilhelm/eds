/**
 * coffeemachine_ao.c
 *
 * benedikt kleinmeier
 * june 23, 2013
 */
 
 #include "../includes/led.h"
 
 #include "coffeemachine_ao.h"
 #include "events.h"
 
 // instance of CoffeeMachineAO and opaque pointer
 static CoffeeMachineAO l_CoffeeMachineAO;
 QActive* const CoffeeMachineAO = (QActive*)&l_CoffeeMachineAO;
 
 // event queue for CoffeeMachineAO
 QEvent const *l_CoffeeMachineAOEvtQSto[SIZE_OF_EVENT_QUEUE];
 
 /**
  * constructor
	*/
 void CoffeeMachineAO_ctor(void)
 {
	 CoffeeMachine* me = &l_CoffeeMachineAO;
	 QActive_ctor(&me->super, (QStateHandler)&CoffeeMachineAO_initial);
 }
 
 /**
 * initial transition
 **/
 static QState CoffeeMachineAO_initial(CoffeeMachineAO *me, QEvent const *e)
 {
	 Q_TRAN(&CoffeeMachineAO_Idle);
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
			 return Q_HANDLED();
		 }
		 
		 case Q_ENTRY_SIG:
		 {
			 // TODO clear all LEDs
			 return Q_HANDLED();
		 }
		 
		 case ALARM_SIG:
		 {
			 // TODO show brew strength on LEDs
			 return Q_TRAN(&CoffeeMachineAO_Brewing);
		 }
		 
		 case Q_EXIT_SIG:
		 {
			 return Q_HANDLED();
		 }
	 }
	 
	 return Q_SUPER(&QHsm_top)
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
			 return Q_HANDLED();
		 }
		 
		 case Q_ENTRY_SIG:
		 {
			 QActive_subscribe(CoffeeMachineAO, BUTTON_SHORTPRESS_SIG);
		 }
		 
		 case BUTTON_SHORTPRESS_SIG:
		 {
			 return Q_TRAN(&CoffeeMachineAO_Idle);
		 }
		 
		 case Q_EXIT:
		 {
			 QActive_unsubscribe(CoffeeMachineAO, BUTTON_SHORTPRESS_SIG);
		 }
	 }
	 
	 return Q_SUPER(&QHsm_top);
 }
 