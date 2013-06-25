/**
 * coffeemachine_ao.h
 *
 * benedikt kleinmeier
 * june 23, 2013
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

// state handlers
static QState CoffeeMachineAO_initial(CoffeeMachineAO *me, QEvent const *e);
static QState CoffeeMachineAO_Idle(CoffeeMachineAO *me, QEvent const *e);
static QState CoffeeMachineAO_Brewing(CoffeeMachineAO *me, QEvent const *e);
 
#endif // COFFEE_MACHINE_H_
