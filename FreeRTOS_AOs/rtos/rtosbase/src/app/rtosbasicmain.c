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

void my_sleep(portTickType ticks_sleep) {
	
	portTickType start = 0;
	
	start = xTaskGetTickCount();	
	for(;;) {
		
		if( (xTaskGetTickCount() - start) <= ticks_sleep ) {
			
			taskYIELD();
		}
		else {
			
			break;
		}		
		
	}
	
}

void qftick_task( void * pvParameters )
{
	portTickType xLastWakeTime;
	
	// 1 Tick = 1,25 ms
	// 100 Ticks = 125 ms
	const portTickType xFrequency = 10;
	static unsigned int timestop = 0, timestart = 0;
	
	
  // Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();
  timestart = counter(0);
	// Task code goes here.
	for( ;; ) {
      
			static count = 0;
		
			// Wait for the next cycle.
			vTaskDelayUntil(&xLastWakeTime, xFrequency);

			// increase timer 
      timestop = counter(xFrequency);
		
			//my_sleep(xFrequency);
		
			QF_tick();   		//QM Port
		
// 			if(count >= 100) {
// 				start_ad_conversion();
// 				count = 0;
// 			}
// 			count++;
	}
}


int main (void) 
{
	xTaskHandle xHandle;
	
			
	// Hardware initialization
	BSP_Init();
	ad_converter_init();
	int0_init(); // init int0 as interrupt for pushbutton
	
	// QP/C framework initialization
	QF_init();
	QF_psInit(subscrSto, Q_DIM(subscrSto));


	// create tick task
	xTaskCreate(qftick_task, "QFTICK" , 0x100 * 3, NULL , 1, &xHandle);
	
	// construct active objects
	
	SetTimeAO_ctor();
	MenuAO_ctor();
	CoffeeMachineAO_ctor();

	
	// construct active objects	
	QActive_start(MenuAOBase, 1, l_MenuAOEvtQSto, Q_DIM(l_MenuAOEvtQSto), (void*)0, 0, (QEvent*)0);
//  	QActive_start(SetTimeAOBase, 2, l_SetTimeAOEvtQSto, Q_DIM(l_SetTimeAOEvtQSto), (void*)0, 0, (QEvent*)0);
//  	QActive_start(CoffeeMachineAOBase, 3, l_CoffeeMachineAOEvtQSto, Q_DIM(l_CoffeeMachineAOEvtQSto), (void*)0, 0, (QEvent*)0);
  	
  

	vTaskStartScheduler();
 

	
	// run QF
	//QF_run();	// calls vTaskStartScheduler
}




