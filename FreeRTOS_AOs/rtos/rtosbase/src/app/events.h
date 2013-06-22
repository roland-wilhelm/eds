/**
 * events.h
 *
 * franz schauer
 * june 20, 2013
 */
 
#ifndef EVENTS_H_
#define EVENTS_H_

#include "qf_pkg.h"


#define SIZE_OF_EVENT_QUEUE 128
#define SIZE_OF_EVENT_POOL	128


/**
 * signals enumeration
 **/
enum CoffeeAlarmSignals 
{
	// published signals (starting at Q_USER_SIG)
  BUTTON_SHORTPRESS_SIG = Q_USER_SIG,
	BUTTON_LONGPRESS_SIG,
	AD_VALUE_SIG,
	
	// end of published signals
	MAX_PUB_SIG,
	
	// start pushed signals
	TIME_UPDATE_SIG,
	TIME_SET_SIG,
	ALARM_SIG,
	ENTER_SET_TIME_SIG,
	BREWSTRENGTH_SET_SIG,
	
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
	unsigned int hours;
	unsigned int min;
} TimeSetEvt;



#endif // EVENTS_H_
