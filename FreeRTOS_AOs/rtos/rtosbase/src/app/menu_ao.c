/**
 * menu_ao.c
 *
 * franz schauer
 * june 20, 2013
 */

#include <stdbool.h>
#include <LPC23xx.H>
#include <LCD.h>

#include "menu_ao.h"
#include "settime_ao.h"
#include "coffeemachine_ao.h"
#include "events.h"

// instance of MenuAO and opaque pointer
static MenuAO l_MenuAO;
QActive* const MenuAOBase = (QActive*)&l_MenuAO;

// event queue for MenuAO
QEvent const *l_MenuAOEvtQSto[SIZE_OF_EVENT_QUEUE];

// static events
static BrewStrengthSetEvt l_BrewStrengthSetEvt = {{BREWSTRENGTH_SET_SIG}};
static EnterSetTimeEvt l_EnterSetTimeEvt = {{ENTER_SET_TIME_SIG}};

/**
 * constructor
 **/
void MenuAO_ctor(void)
{
	MenuAO* me = &l_MenuAO;
	QActive_ctor(&me->super, (QStateHandler)&MenuAO_initial);
	
	me->brewStrength = Medium;
	me->waitingForSetTime = false;
}

/**
 * initial transition
 **/
static QState MenuAO_initial(MenuAO *me, QEvent const *e)
{
	// TODO subscribe to published events
	QActive_subscribe(MenuAOBase, BUTTON_SHORTPRESS_SIG);
	QActive_subscribe(MenuAOBase, BUTTON_LONGPRESS_SIG);
	QActive_subscribe(MenuAOBase, AD_VALUE_SIG);
	
	return Q_TRAN(&MenuAO_ClockMenu);
}

/**
 * ClockMenu state handler
 **/
static QState MenuAO_ClockMenu(MenuAO *me, QEvent const *e)
{
	switch ( e->sig ) 
	{
		case Q_INIT_SIG: 
		{
			return Q_HANDLED();
		}
	      
		case Q_ENTRY_SIG: 
		{
			// TODO display clock menu (1st row of LCD)
			lcd_clear();
			set_cursor(0, 0);
			lcd_print("1: Clock XX:XX");
			
			return Q_HANDLED();
		}
		
		case BUTTON_SHORTPRESS_SIG: 
		{
			// short press > proceed to next submenu
			if (!me->waitingForSetTime)
				// guard condition: only if not waiting for set time
				return Q_TRAN(&MenuAO_BrewStrengthMenu);
			else
				return Q_HANDLED();
		}
		
		case BUTTON_LONGPRESS_SIG: 
		{
			// long press > send change time request and wait for response
			me->waitingForSetTime = true;
			
			// send ENTER_SET_TIME_SIG to SetTimeAO
			QActive_postFIFO(SetTimeAOBase, (QEvent*)&l_EnterSetTimeEvt);
			
			return Q_HANDLED();
		}
		
		case TIME_SET_SIG:
		{
			me->waitingForSetTime = false;
			
			// TODO display new time (1st row of LCD)
			lcd_clear();
			set_cursor(0, 0);
			lcd_print("1: Clock XX:XX");
			
			// TODO save time at RTC
			
			return Q_HANDLED();
		}
		
		case TIME_UPDATE_SIG:
		{
			// TODO display updated time
			set_cursor(0, 0);
			lcd_print("1: Clock XX:XX");
			
			return Q_HANDLED();
		}
	 	
		case Q_EXIT_SIG: 
		{
		} 	
	}
 
	return Q_SUPER(&QHsm_top);
}

/**
 * BrewStrengthMenu state handler
 **/
static QState MenuAO_BrewStrengthMenu(MenuAO *me, QEvent const *e)
{
	switch ( e->sig ) 
	{
		case Q_INIT_SIG: 
		{
			return Q_HANDLED();
		}
	      
		case Q_ENTRY_SIG: 
		{
			// TODO display brew strength menu (1st row of LCD)
			lcd_clear();
			set_cursor(0, 0);
			lcd_print("2: Strength XXXX");
			
			return Q_HANDLED();
		}
		
		case BUTTON_SHORTPRESS_SIG: 
		{
			// short press > proceed to next submenu
			return Q_TRAN(&MenuAO_AlarmMenu);
		}
		
		case BUTTON_LONGPRESS_SIG: 
		{	
			return Q_TRAN(&MenuAO_ChangeBrewStrength);
		}
	 	
		case Q_EXIT_SIG: 
		{
		} 	
	}
 
	return Q_SUPER(&QHsm_top);
}

/**
 * ChangeBrewStrength state handler
 **/
static QState MenuAO_ChangeBrewStrength(MenuAO *me, QEvent const *e)
{
	static BrewStrength brewStrength;
	
	switch ( e->sig ) 
	{
		case Q_INIT_SIG: 
		{
			return Q_HANDLED();
		}
	      
		case Q_ENTRY_SIG: 
		{
			// reset brew strength
			brewStrength = Unchanged;
			
			// TODO display change brew strength (2nd row of LCD)
			set_cursor(0, 1);
			lcd_print("> XXXX");
			
			return Q_HANDLED();
		}
		
		case BUTTON_SHORTPRESS_SIG: 
		{
			// short press > block proceeding to next submenu in parent-state
			return Q_HANDLED();
		}
		
		case BUTTON_LONGPRESS_SIG: 
		{	
			// long press > leave changing state and go back to submenu
			return Q_TRAN(&MenuAO_BrewStrengthMenu);
		}
		
		case AD_VALUE_SIG:
		{
			// set brew strength according to AD value
			
			// TODO calculate and save brew strength
			// ADValueEvt const* evt = (ADValueEvt*)e;
			// short v = evt->value;
			short v = 100;
			
			// calculate brew strenght:
			// - ad value is from 0..100
			// - multiplying by 2 and dividing by 100 scales between 0..2
			// - adding 50 for correct rounding (integer-division)
			
			brewStrength = (BrewStrength)(((v * 2 + 50) / 100));
			
			// TODO display change brew strength (2nd row of LCD)
			set_cursor(0, 1);
			lcd_print("> XXXX");
			
			return Q_HANDLED();
		}
	 	
		case Q_EXIT_SIG: 
		{
			if (brewStrength != Unchanged)
			{
				// brew strength has been changed
				me->brewStrength = brewStrength;
				
				// TODO maybe better use dynamic event, but should be okay here!
				l_BrewStrengthSetEvt.brewStrength = brewStrength;
				
				// send BREWSTRENGTH_SET_SIG to CoffeeMachineAO
				QActive_postFIFO(CoffeeMachineAOBase, (QEvent*)&l_BrewStrengthSetEvt);
			}
		} 	
	}
	
	return Q_SUPER(&MenuAO_BrewStrengthMenu);
}

/**
 * AlarmMenu state handler
 **/
static QState MenuAO_AlarmMenu(MenuAO *me, QEvent const *e)
{
	switch ( e->sig ) 
	{
		case Q_INIT_SIG: 
		{
			return Q_HANDLED();
		}
	      
		case Q_ENTRY_SIG: 
		{
			// TODO display alarm menu (1st row of LCD)
			lcd_clear();
			set_cursor(0, 0);
			lcd_print("3: Alarm XX:XX");
			
			return Q_HANDLED();
		}
		
		case BUTTON_SHORTPRESS_SIG: 
		{
			// short press > proceed to next submenu
			if (!me->waitingForSetTime)
				// guard condition: only if not waiting for set time
				return Q_TRAN(&MenuAO_BrewStrengthMenu);
			else
				return Q_HANDLED();
		}
		
		case BUTTON_LONGPRESS_SIG: 
		{ 
			// long press > send change time request and wait for response
			me->waitingForSetTime = true;
			
			// send ENTER_SET_TIME_SIG to SetTimeAO
			QActive_postFIFO(SetTimeAOBase, (QEvent*)&l_EnterSetTimeEvt);
			
			return Q_HANDLED();
		}
		
		case TIME_SET_SIG:
		{
			me->waitingForSetTime = false;
			
			// TODO display new alarm
			lcd_clear();
			set_cursor(0, 0);
			lcd_print("3: Alarm XX:XX");
			
			// TODO arm alarm clock at RTC
			
			return Q_HANDLED();
		}
	 	
		case Q_EXIT_SIG: 
		{
		} 	
	}
 
	return Q_SUPER(&QHsm_top);
}
