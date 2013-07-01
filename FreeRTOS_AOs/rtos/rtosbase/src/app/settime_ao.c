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

#include "log.h"
#include "settime_ao.h"
#include "events.h"
#include "menu_ao.h"

// SetTimeAO active object
typedef struct SetTimeAOTag {
  QActive super;
	
	RTCTime time;
} SetTimeAO;

static uint16_t adc_temp;

// instance of SetTimeAO and opaque pointer
static SetTimeAO l_SetTimeAO;
QActive* const SetTimeAOBase = (QActive*)&l_SetTimeAO;


// static events
static TimeSetEvt l_TimeSetEvt = {{TIME_SET_SIG}};

// state handlers
static QState SetTimeAO_initial(SetTimeAO *me, QEvent const *e);
static QState SetTimeAO_Idle(SetTimeAO *me, QEvent const *e);
static QState SetTimeAO_Changing(SetTimeAO *me, QEvent const *e);
static QState SetTimeAO_ChangeHrs(SetTimeAO *me, QEvent const *e);
static QState SetTimeAO_ChangeMin(SetTimeAO *me, QEvent const *e);

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
			DBG("StateTime_Idle INIT");
			return Q_HANDLED();
		}
	      
		case Q_ENTRY_SIG: 
		{
			DBG("StateTime_Idle ENTRY");
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
			DBG("StateTime_Changing INIT");
			//GoTo State Change Hours
			return Q_TRAN(&SetTimeAO_ChangeHrs);
		}
	      
		case Q_ENTRY_SIG: 
		{
			DBG("StateTime_Changing ENTRY");
			//Subscribe Button & AD_Value
			QActive_subscribe(SetTimeAOBase, BUTTON_SHORTPRESS_SIG);
			QActive_subscribe(SetTimeAOBase, BUTTON_LONGPRESS_SIG);
			QActive_subscribe(SetTimeAOBase, AD_VALUE_SIG);
			QActive_subscribe(SetTimeAOBase, ALARM_SIG);

			return Q_HANDLED();
		}
		
		case BUTTON_LONGPRESS_SIG:
		{
			//Send Event: EvtTimeSet
			l_TimeSetEvt.time.RTC_Min = l_SetTimeAO.time.RTC_Min;
			l_TimeSetEvt.time.RTC_Hour = l_SetTimeAO.time.RTC_Hour;
			QActive_postFIFO(MenuAOBase, (QEvent*)&l_TimeSetEvt);
			
			//Unsubscribe Button & AD_Value
			QActive_unsubscribe(SetTimeAOBase, BUTTON_SHORTPRESS_SIG);
			QActive_unsubscribe(SetTimeAOBase, BUTTON_LONGPRESS_SIG);
			QActive_unsubscribe(SetTimeAOBase, AD_VALUE_SIG);
			QActive_unsubscribe(SetTimeAOBase, ALARM_SIG);
			
			// long press > leave changing state and go back to Idle
			return Q_TRAN(&SetTimeAO_Idle);
		}
		
		case ALARM_SIG:
		{
			//Unsubscribe Button & AD_Value
			QActive_unsubscribe(SetTimeAOBase, BUTTON_SHORTPRESS_SIG);
			QActive_unsubscribe(SetTimeAOBase, BUTTON_LONGPRESS_SIG);
			QActive_unsubscribe(SetTimeAOBase, AD_VALUE_SIG);
			QActive_unsubscribe(SetTimeAOBase, ALARM_SIG);
			
			// alarm > leave changing state and go back to Idle
			return Q_TRAN(&SetTimeAO_Idle);
		}
	 	
		case Q_EXIT_SIG: 
		{
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
	//Pointer um ADC Event zu casten
	AdValueChangedEvt *evt;
	
	//output to format lcd-output string
	static char output[17];
	
	switch ( e->sig ) 
	{
		case Q_INIT_SIG: 
		{
			DBG("StateTime_ChangeHrs INIT");
			return Q_HANDLED();
		}
	      
		case Q_ENTRY_SIG: 
		{
			DBG("StateTime_ChangeHrs ENTRY");
			// display Status (2nd row of LCD)
			set_cursor(0, 1);
			lcd_print((unsigned char*)"SetHour> ");
			
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
			
			// calculate and save Hours
			// ADValueEvt const* evt = (ADValueEvt*)e;
			// uint16_t v = evt->value;
			evt =  (AdValueChangedEvt*)e;
			adc_temp = evt->value;
			
			
			// calculate hours:
			// - ad value is from 0..100
			// - devide by 4 -> range 0.....25
			// - if v higher 23 -> v auf 23 setzen -> range 0...23
			adc_temp /= 4;
			if ( adc_temp>23 )
				adc_temp = 23;
			
			l_SetTimeAO.time.RTC_Hour = (uint8_t)adc_temp;
			
			//Generate Formated String
			sprintf(output, "SetHour> %02d:%02d", l_SetTimeAO.time.RTC_Hour, l_SetTimeAO.time.RTC_Min);			
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
	//Pointer um ADC Event zu casten
	AdValueChangedEvt *evt;
	
	
	//output to format lcd-output
	static char output[17];
	
	switch ( e->sig ) 
	{
		case Q_INIT_SIG: 
		{
			DBG("StateTime_ChangeMin INIT");
			return Q_HANDLED();
		}
	      
		case Q_ENTRY_SIG: 
		{		
			DBG("StateTime_ChangeMin ENTRY");
			// display Status (2nd row of LCD)
			set_cursor(0, 1);
			lcd_print((unsigned char*)"Set Min> ");
			
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
			
			// calculate and save Minutes
			// ADValueEvt const* evt = (ADValueEvt*)e;
			// uint16_t v = evt->value;
			evt =  (AdValueChangedEvt*)e;
			adc_temp = evt->value;
			
			// calculate Minutes:
			// - ad value is from 0..100
			// - multi by 10 and devide by 16 -> range 0.....62
			// - if v >= 60 -> v auf 59 setzen -> range 0...59
			adc_temp *= 10;
			adc_temp /= 16;
			if ( adc_temp>=60 )
				adc_temp = 59;
			
			l_SetTimeAO.time.RTC_Min = (uint8_t)adc_temp;
			
			//Generate Formated String
			sprintf(output, "Set Min> %02d:%02d", l_SetTimeAO.time.RTC_Hour, l_SetTimeAO.time.RTC_Min);			
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
