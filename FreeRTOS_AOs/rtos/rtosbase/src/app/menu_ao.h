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

// constructor
void MenuAO_ctor(void);

#endif // MENU_AO_H_
