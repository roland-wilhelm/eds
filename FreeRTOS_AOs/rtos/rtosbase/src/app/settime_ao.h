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
#include "events.h"

// make opaque pointers available externally
extern QActive* const SetTimeAOBase;
extern QEvent const *l_SetTimeAOEvtQSto[SIZE_OF_EVENT_QUEUE];

// constructor
void SetTimeAO_ctor(void);

#endif // SETTIME_AO_H_
