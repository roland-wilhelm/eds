            
#include <stdio.h>
#include "qp_port.h" 
#include "qmbsp.h"    
#include <LPC23xx.H>
#include "ad_ao.h"
#include "coffeemachine_ao.h"
#include "menu_ao.h"
#include "push_button2.h"
#include "settime_ao.h"



// List for publish-subscribe
static QSubscrList subscrSto[MAX_PUB_SIG];

// event queue for MenuAO
static QEvent const *l_MenuAOEvtQSto[3];
// event queue for CoffeeMachineAO
static QEvent const *l_CoffeeMachineAOEvtQSto[3];
// event queue for SetTimeAO
static QEvent const *l_SetTimeAOEvtQSto[3];



int main (void) 
{
				
	// Hardware initialization
	BSP_Init();
	ad_converter_init();
	int0_init(); // init int0 as interrupt for pushbutton
	
	// QP/C framework initialization
	QF_init();
	QF_psInit(subscrSto, Q_DIM(subscrSto));
	
	MenuAO_ctor();
	SetTimeAO_ctor();
	CoffeeMachineAO_ctor();
	
	
	
	// construct active objects	
	QActive_start(MenuAOBase, 1, l_MenuAOEvtQSto, Q_DIM(l_MenuAOEvtQSto), (void*)0, 0, (QEvent*)0);
	QActive_start(SetTimeAOBase, 2, l_SetTimeAOEvtQSto, Q_DIM(l_SetTimeAOEvtQSto), (void*)0, 0, (QEvent*)0);
	QActive_start(CoffeeMachineAOBase, 3, l_CoffeeMachineAOEvtQSto, Q_DIM(l_CoffeeMachineAOEvtQSto), (void*)0, 0, (QEvent*)0);
	
	
	QF_run();
}




