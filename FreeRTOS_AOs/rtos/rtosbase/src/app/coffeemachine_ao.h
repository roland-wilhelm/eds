/**
 * An active object representation with two states:
 * - Idle
 * - Brewing
 *
 * The following transitions are possible:
 *
 *                 Alarm (RTC)
 *       +------+ -------------> +---------+
 *  o--> | Idle |                | Brewing |
 *       +------+ <------------- +---------+
 *                  ShortPress
 */
 
#ifndef COFFEE_MACHINE_H_
#define COFFEE_MACHINE_H_

#include "qf_pkg.h"
#include "events.h"

// make opaque pointers available externally
extern QActive* const CoffeeMachineAOBase;
extern QEvent const *l_CoffeeMachineAOEvtQSto[SIZE_OF_EVENT_QUEUE];

// CoffeeMachineAO active object
typedef struct CoffeeMachineAOTag {
 QActive super;
} CoffeeMachineAO;

// constructor
void CoffeeMachineAO_ctor(void);
 
#endif // COFFEE_MACHINE_H_
