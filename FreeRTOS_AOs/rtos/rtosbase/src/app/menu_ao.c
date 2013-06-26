/**
 * menu_ao.c
 *
 * franz schauer
 * june 20, 2013
 */

#include <stdbool.h>
#include <LPC23xx.H>
#include <LCD.h>
#include <stdio.h>

#include "menu_ao.h"
#include "settime_ao.h"
#include "coffeemachine_ao.h"
#include "../drivers/rtc.h"
#include "events.h"

// MenuAO active object
typedef struct MenuAOTag {
  QActive super;
	
	BrewStrength brewStrength;
	bool waitingForSetTime;
} MenuAO;

// instance of MenuAO and opaque pointer
static MenuAO l_MenuAO;
QActive* const MenuAOBase = (QActive*)&l_MenuAO;

// event queue for MenuAO
QEvent const *l_MenuAOEvtQSto[SIZE_OF_EVENT_QUEUE];

// static events
static BrewStrengthSetEvt l_BrewStrengthSetEvt = {{BREWSTRENGTH_SET_SIG}};
static EnterSetTimeEvt l_EnterSetTimeEvt = {{ENTER_SET_TIME_SIG}};

// static variables
static char output[17];
static RTCTime rtcTime;

// state handlers
static QState MenuAO_initial(MenuAO *me, QEvent const *e);
static QState MenuAO_ClockMenu(MenuAO *me, QEvent const *e);
static QState MenuAO_BrewStrengthMenu(MenuAO *me, QEvent const *e);
static QState MenuAO_ChangeBrewStrength(MenuAO *me, QEvent const *e);
static QState MenuAO_AlarmMenu(MenuAO *me, QEvent const *e);

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
	
	// Initialize RTC
	RTC_Init();
	RTC_Start();
	
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
			// get current time
			RTC_GetTime(&rtcTime);
			sprintf(output, "1: Clock %2d:%2d", rtcTime.RTC_Hour, rtcTime.RTC_Min);
			
			// display clock menu (1st row of LCD)
			lcd_clear();
			set_cursor(0, 0);
			lcd_print((unsigned char*)output);
			
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
			TimeSetEvt* evt = (TimeSetEvt*)e;
			me->waitingForSetTime = false;
			
			// display new time (1st row of LCD)
			sprintf(output, "1: Clock %2d:%2d", evt->time.RTC_Hour, evt->time.RTC_Min);
			lcd_clear();
			set_cursor(0, 0);
			lcd_print((unsigned char*)output);
			
			// save time at RTC
			RTC_Stop();
			RTC_SetTime(&evt->time);
			RTC_Start();
			
			return Q_HANDLED();
		}
		
		case TIME_UPDATE_SIG:
		{
			// display updated time
			TimeUpdateEvt* evt = (TimeUpdateEvt*)e;
			sprintf(output, "1: Clock %2d:%2d", evt->time.RTC_Hour, evt->time.RTC_Min);
			
			set_cursor(0, 0);
			lcd_print((unsigned char*)output);
			
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
			sprintf(output, "2: Strength %d", me->brewStrength);
			
			// TODO display brew strength menu (1st row of LCD)
			lcd_clear();
			set_cursor(0, 0);
			lcd_print((unsigned char*)output);
			
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
	static BrewStrength tmpBrewStrength;
	
	switch ( e->sig ) 
	{
		case Q_INIT_SIG: 
		{
			return Q_HANDLED();
		}
	      
		case Q_ENTRY_SIG: 
		{		
			// reset brew strength
			tmpBrewStrength = Unchanged;
			
			// display change brew strength (2nd row of LCD)
			sprintf(output, "SetStrength> %d", me->brewStrength);
			set_cursor(0, 1);
			lcd_print((unsigned char*)output);
			
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
			
			// calculate brew strenght:
			// - ad value is from 0..100
			// - multiplying by 2 and dividing by 100 scales between 0..2
			// - adding 50 for correct rounding (integer-division)
			
			AdValueChangedEvt* evt = (AdValueChangedEvt*)e;
			tmpBrewStrength = (BrewStrength)(((evt->value * 2 + 50) / 100));
					
			// display change brew strength (2nd row of LCD)
			sprintf(output, "SetStrength> %d", tmpBrewStrength);
			set_cursor(0, 1);
			lcd_print((unsigned char*)output);
			
			return Q_HANDLED();
		}
	 	
		case Q_EXIT_SIG: 
		{
			if (tmpBrewStrength != Unchanged)
			{
				// brew strength has been changed
				me->brewStrength = tmpBrewStrength;
				
				// TODO maybe better use dynamic event, but should be okay here!
				l_BrewStrengthSetEvt.brewStrength = tmpBrewStrength;
				
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
			// get current alarm
			RTC_GetAlarm(&rtcTime);
			sprintf(output, "3: Alarm %2d:%2d", rtcTime.RTC_Hour, rtcTime.RTC_Min);
			
			// display alarm menu (1st row of LCD)
			lcd_clear();
			set_cursor(0, 0);
			lcd_print((unsigned char*)output);
			
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
			TimeSetEvt* evt = (TimeSetEvt*)e;
			me->waitingForSetTime = false;
			
			// save and arm time at RTC
			RTC_AlarmDisable();
			RTC_SetAlarm(&evt->time);
			RTC_AlarmEnable();
			
			// display new alarm
			sprintf(output, "3: Alarm %2d:%2d", evt->time.RTC_Hour, evt->time.RTC_Min);

			lcd_clear();
			set_cursor(0, 0);
			lcd_print((unsigned char*)output);
			
			return Q_HANDLED();
		}
	 	
		case Q_EXIT_SIG: 
		{
		} 	
	}
 
	return Q_SUPER(&QHsm_top);
}
