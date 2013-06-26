/**
 * settime_ao.h
 * max hempe
 * june 22, 2013
 *
 * basis is:
 * menu_ao.h
 * franz schauer
 * june 20, 2013
 */

 
#ifndef SETTIME_AO_H_
#define SETTIME_AO_H_

#include "qf_pkg.h"
#include "../drivers/rtc.h"
#include "events.h"

// make opaque pointers available externally
extern QActive* const SetTimeAOBase;
extern QEvent const *l_SetTimeAOEvtQSto[SIZE_OF_EVENT_QUEUE];

// SetTimeAO active object
typedef struct SetTimeAOTag {
  QActive super;
	
	RTCTime time;
} SetTimeAO;

// constructor
void SetTimeAO_ctor(void);

// state handlers
static QState SetTimeAO_initial(SetTimeAO *me, QEvent const *e);
static QState SetTimeAO_Idle(SetTimeAO *me, QEvent const *e);
static QState SetTimeAO_Changing(SetTimeAO *me, QEvent const *e);
static QState SetTimeAO_ChangeHrs(SetTimeAO *me, QEvent const *e);
static QState SetTimeAO_ChangeMin(SetTimeAO *me, QEvent const *e);

#endif // SETTIME_AO_H_
