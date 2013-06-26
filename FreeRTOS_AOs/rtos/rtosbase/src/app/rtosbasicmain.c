/******************************************************************************/
/*  Harness main program for initialisation of MCB2300 Platform for:
* Date of the Last Update:  31.10.2011
* Dr. Ron Barker - Port to FreeRtos for ARM7TDMI - NXP 2378 - Keil 2300
* Example Program for Event Driven Seminar Munich Univeristiy	- DPP 
*	Dr. Ron Barker  
*	Keil 2300 - LPC 2378 
*	FreeRTOS	Dr. Ron Barker  
 */
/******************************************************************************/
                  
#include <stdio.h>
						
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "qp_port.h" 
#include "bsp.h"    

//#include "pushbutton.h"
#include "ad_ao.h"
#include "coffeemachine_ao.h"
#include "menu_ao.h"
#include "push_button2.h"
#include "settime_ao.h"

#define POOL_SIZE_TEST 16

// Event Queue AD converter
static const QEvent *adQueueSto[3];

// List for publish-subscribe
static QSubscrList subscrSto[MAX_PUB_SIG];

static AlarmEvt l_smlPoolSto[POOL_SIZE_TEST];
static TimeUpdateEvt l_medPoolSto[POOL_SIZE_TEST];

void qftick_task( void * pvParameters )
{
	portTickType xLastWakeTime;
	const portTickType xFrequency = 1;

  // Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();
  
	// Task code goes here.
	for( ;; ) {
        
			// Wait for the next cycle.
			vTaskDelayUntil(&xLastWakeTime, xFrequency);

			// increase timer 
      counter(1);
		
			QF_tick();   		//QM Port			
	
	}

}


int main (void) 
{
	xTaskHandle xHandle;
	
	// Hardware initialization
	BSP_Init();
	ad_converter_init();
	int0_init(); // init int0 as interrupt
	
	// QP/C framework initialization
	QF_init();
	
	QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));
	// QF_poolInit(l_medPoolSto, sizeof(l_medPoolSto), sizeof(l_medPoolSto[0]));
	
	QF_psInit(subscrSto, Q_DIM(subscrSto));

	xTaskCreate(qftick_task, "QFTICK" , 0x100 * 3, NULL , 1, &xHandle);
	
	/*	
	ad_ctor();
	CoffeeMachineAO_ctor();
	MenuAO_ctor();
	SetTimeAO_ctor();
	
	QActive_start((QActive *)&adAO, 1, adQueueSto, Q_DIM(adQueueSto), (void *)0, 0, (QEvent *)0);
	QActive_start(CoffeeMachineAOBase, 1, l_CoffeeMachineAOEvtQSto, Q_DIM(l_CoffeeMachineAOEvtQSto), (void*)0, 0, (QEvent*)0);
	QActive_start(MenuAOBase, 1, l_MenuAOEvtQSto, Q_DIM(l_MenuAOEvtQSto), (void*)0, 0, (QEvent*)0);
	QActive_start(SetTimeAOBase, 1, l_SetTimeAOEvtQSto, Q_DIM(l_SetTimeAOEvtQSto), (void*)0, 0, (QEvent*)0);	
	*/
	CoffeeMachineAO_ctor();
	QActive_start(CoffeeMachineAOBase, 1, l_CoffeeMachineAOEvtQSto, Q_DIM(l_CoffeeMachineAOEvtQSto), (void*)0, 0, (QEvent*)0);
	
	// vTaskStartScheduler();
	QF_run();
}




