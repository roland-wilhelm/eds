/**
 * events.h
 *
 * franz schauer
 * june 20, 2013
 */
 
#ifndef EVENTS_H_
#define EVENTS_H_

#include "qf_pkg.h"
#include "../drivers/rtc.h"


#define SIZE_OF_EVENT_QUEUE 128
#define SIZE_OF_EVENT_POOL	128


/**
 * signals enumeration
 **/
enum CoffeeAlarmSignals 
{
	// published signals (starting at Q_USER_SIG)
  // push_button2: published at button short pressed
	BUTTON_SHORTPRESS_SIG = Q_USER_SIG,
	// push_button2: published at button long pressed
	BUTTON_LONGPRESS_SIG,
	// ad_ao: published Signal each time ad value changed
	AD_VALUE_SIG,
	
	// end of published signals
	MAX_PUB_SIG,
	
	// start pushed signals
	TIME_UPDATE_SIG,
	TIME_SET_SIG,
	ALARM_SIG,
	ENTER_SET_TIME_SIG,
	BREWSTRENGTH_SET_SIG,
	
	// ad_ao: start ad conversion
	START_AD_SIG,
	
	// end of pushed signals
	MAX_SIG
};

/**
 * BrewStrength event
 **/
typedef enum 
{
	Weak			=0,
	Medium		=1,
	Strong		=2,
	Unchanged	=3
} BrewStrength;

typedef struct 
{
  QEvent super;
	BrewStrength brewStrength;
} BrewStrengthSetEvt;

/**
 * EnterSetTime event
 **/
typedef struct 
{
  QEvent super;
} EnterSetTimeEvt;

/**
 * TimeSet event
**/
typedef struct 
{
  QEvent super;
	RTCTime time;
} TimeSetEvt;

/**
 * TimeUpdate event
**/
typedef struct 
{
  QEvent super;
	RTCTime time;
} TimeUpdateEvt;

/**
 * Alarm event
**/
typedef struct 
{
  QEvent super;
} AlarmEvt;

/**
 * ADValueChanged event (published each time ad value changed)
 */
typedef struct _AdValueChangedEvt {
	
	QEvent super;
	uint16_t value;
	
} AdValueChangedEvt;

#endif // EVENTS_H_
