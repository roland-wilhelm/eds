/**
 * menu_ao.h
 *
 * franz schauer
 * june 20, 2013
 */
 
#ifndef MENU_AO_H_
#define MENU_AO_H_

#include <stdbool.h>
#include "qf_pkg.h"

#include "events.h"

// make opaque pointers available externally
extern QActive* const MenuAOBase;
extern QEvent const *l_MenuAOEvtQSto[SIZE_OF_EVENT_QUEUE];

// MenuAO active object
typedef struct MenuAOTag {
  QActive super;
	
	BrewStrength brewStrength;
	bool waitingForSetTime;
} MenuAO;

// constructor
void MenuAO_ctor(void);

// state handlers
static QState MenuAO_initial(MenuAO *me, QEvent const *e);
static QState MenuAO_ClockMenu(MenuAO *me, QEvent const *e);
static QState MenuAO_BrewStrengthMenu(MenuAO *me, QEvent const *e);
static QState MenuAO_ChangeBrewStrength(MenuAO *me, QEvent const *e);
static QState MenuAO_AlarmMenu(MenuAO *me, QEvent const *e);

#endif // MENU_AO_H_
