/**
 * settime_ao.c
 *
 * max hempe
 * june 22, 2013
 *
 * basis is:
 * menu_ao.c
 * franz schauer
 * june 20, 2013
 */


#include <LPC23xx.H>
#include <LCD.h>
#include <stdio.h>

#include "settime_ao.h"
#include "events.h"
#include "menu_ao.h"

// instance of SetTimeAO and opaque pointer
static SetTimeAO l_SetTimeAO;
QActive* const SetTimeAOBase = (QActive*)&l_SetTimeAO;

// event queue for SetTimeAO
QEvent const *l_SetTimeAOEvtQSto[SIZE_OF_EVENT_QUEUE];

// static events
static TimeSetEvt l_TimeSetEvt = {{TIME_SET_SIG}};

/**
 * constructor
 **/
void SetTimeAO_ctor(void)
{
	SetTimeAO* me = &l_SetTimeAO;
	QActive_ctor(&me->super, (QStateHandler)&SetTimeAO_initial);
	
	me->time.RTC_Min = 0;
	me->time.RTC_Hour = 0;
}

/**
 * initial transition
 **/
static QState SetTimeAO_initial(SetTimeAO *me, QEvent const *e)
{
	
	return Q_TRAN(&SetTimeAO_Idle);
}

/**
 * Idle state handler
 **/
static QState SetTimeAO_Idle(SetTimeAO *me, QEvent const *e)
{
	switch ( e->sig ) 
	{
		case Q_INIT_SIG: 
		{
			return Q_HANDLED();
		}
	      
		case Q_ENTRY_SIG: 
		{
	
			return Q_HANDLED();
		}
				
		case ENTER_SET_TIME_SIG:
		{
						
			return Q_TRAN(&SetTimeAO_Changing);
		}
			 	
		case Q_EXIT_SIG: 
		{
			return Q_HANDLED();
		} 	
	}
 
	return Q_SUPER(&QHsm_top);
}

/**
 * Changing state handler
 **/
static QState SetTimeAO_Changing(SetTimeAO *me, QEvent const *e)
{
	switch ( e->sig ) 
	{
		case Q_INIT_SIG: 
		{
			//GoTo State Change Hours
			return Q_TRAN(&SetTimeAO_ChangeHrs);
		}
	      
		case Q_ENTRY_SIG: 
		{
			//Subscribe Button & AD_Value
			QActive_subscribe(SetTimeAOBase, BUTTON_SHORTPRESS_SIG);
			QActive_subscribe(SetTimeAOBase, BUTTON_LONGPRESS_SIG);
			QActive_subscribe(SetTimeAOBase, AD_VALUE_SIG);

			return Q_HANDLED();
		}
		
		case BUTTON_LONGPRESS_SIG: 
		{	
			//Unsubscribe Button & AD_Value
			QActive_unsubscribe(SetTimeAOBase, BUTTON_SHORTPRESS_SIG);
			QActive_unsubscribe(SetTimeAOBase, BUTTON_LONGPRESS_SIG);
			QActive_unsubscribe(SetTimeAOBase, AD_VALUE_SIG);
			
			// long press > leave changing state and go back to Idle (Superstate)
			return Q_SUPER(&SetTimeAO_Idle);
		}
	 	
		case Q_EXIT_SIG: 
		{						
			//Send Event: EvtTimeSet
			l_TimeSetEvt.time.RTC_Min = l_SetTimeAO.time.RTC_Min;
			l_TimeSetEvt.time.RTC_Hour = l_SetTimeAO.time.RTC_Hour;
			//TODO: MenuAO not declared...
			QActive_postFIFO(MenuAOBase, (QEvent*)&l_TimeSetEvt);
			
			return Q_HANDLED();
		} 	
	}
 
return Q_SUPER(&SetTimeAO_Idle);
}

/**
 * ChangeHrs state handler
 **/
static QState SetTimeAO_ChangeHrs(SetTimeAO *me, QEvent const *e)
{
	//output to format lcd-output string
	static char* output;
	
	switch ( e->sig ) 
	{
		case Q_INIT_SIG: 
		{
			return Q_HANDLED();
		}
	      
		case Q_ENTRY_SIG: 
		{
			//Generate Formated String
			sprintf(output, "SetHour> %2d:%2d", l_SetTimeAO.time.RTC_Hour, l_SetTimeAO.time.RTC_Min);
			// display old value of Hours (2nd row of LCD)
			set_cursor(0, 1);
			lcd_print((unsigned char*)output);
			
			return Q_HANDLED();
		}
		
		case BUTTON_SHORTPRESS_SIG: 
		{
			// short press > ChangeHrs finished, goto ChangeMin
			return Q_TRAN(&SetTimeAO_ChangeMin);
		}
		
		case AD_VALUE_SIG:
		{
			// set Hours according to AD value
			
			// TODO calculate and save Hours
			// ADValueEvt const* evt = (ADValueEvt*)e;
			// uint16_t v = evt->value;
			uint16_t v = 100;
			
			// calculate hours:
			// - ad value is from 0..100
			// - devide by 4 -> range 0.....25
			// - if v higher 23 -> v auf 23 setzen -> range 0...23
			v /= 4;
			if ( v>23 )
				v = 23;
			
			l_SetTimeAO.time.RTC_Hour = (uint8_t)v;
			
			//Generate Formated String
			sprintf(output, "SetHour> %2d:%2d", l_SetTimeAO.time.RTC_Hour, l_SetTimeAO.time.RTC_Min);			
			// display changed Hours (2nd row of LCD)
			set_cursor(0, 1);
			lcd_print((unsigned char*)output);
			
			return Q_HANDLED();
		}
	 	
		case Q_EXIT_SIG: 
		{
			return Q_HANDLED();
		} 	
	}
	
	return Q_SUPER(&SetTimeAO_Changing);
}

/**
 * ChangeMin state handler
 **/
static QState SetTimeAO_ChangeMin(SetTimeAO *me, QEvent const *e)
{
	//output to format lcd-output
	static char* output;
	
	switch ( e->sig ) 
	{
		case Q_INIT_SIG: 
		{
			return Q_HANDLED();
		}
	      
		case Q_ENTRY_SIG: 
		{
			//Generate Formated String
			sprintf(output, "Set Min> %2d:%2d", l_SetTimeAO.time.RTC_Hour, l_SetTimeAO.time.RTC_Min);		
			// display old value of Min (2nd row of LCD)
			set_cursor(0, 1);
			lcd_print((unsigned char*)output);
			
			return Q_HANDLED();
		}
		
		case BUTTON_SHORTPRESS_SIG: 
		{
			//Change Min finished -> go back to Chang Hrs
			return Q_TRAN(&SetTimeAO_ChangeHrs);
		}
				
		case AD_VALUE_SIG:
		{
			// set Minutes according to AD value
			
			// TODO calculate and save Minutes
			// ADValueEvt const* evt = (ADValueEvt*)e;
			// uint16_t v = evt->value;
			uint16_t v = 100;
			
			// calculate Minutes:
			// - ad value is from 0..100
			// - multi by 10 and devide by 16 -> range 0.....62
			// - if v >= 60 -> v auf 59 setzen -> range 0...59
			v *= 10;
			v /= 16;
			if ( v>=60 )
				v = 59;
			
			l_SetTimeAO.time.RTC_Min = (uint8_t)v;
			
			//Generate Formated String
			sprintf(output, "Set Min> %2d:%2d", l_SetTimeAO.time.RTC_Hour, l_SetTimeAO.time.RTC_Min);			
			// display changed Minutes (2nd row of LCD)
			set_cursor(0, 1);
			lcd_print((unsigned char*)output);
			
			return Q_HANDLED();
		}
	 	
		case Q_EXIT_SIG: 
		{
			return Q_HANDLED();
		} 	
	}
 
	return Q_SUPER(&SetTimeAO_Changing);
}
