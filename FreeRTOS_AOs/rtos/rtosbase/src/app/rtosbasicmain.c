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


#define SIZE_OF_EVENT_QUEUE 32


// List for publish-subscribe
static QSubscrList subscrSto[MAX_PUB_SIG];


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
		
	
  // Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();
 
	// Task code goes here.
	for( ;; ) {
      
			static unsigned int count = 0;
		
			// Wait for the next cycle.
			vTaskDelayUntil(&xLastWakeTime, xFrequency);

			// increase timer 
      counter(xFrequency);
		
			//my_sleep(xFrequency);
		
			QF_tick();   		//QM Port
		
			if(count >= 10) {
				start_ad_conversion();
				count = 0;
			}
			count++;
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
	xTaskCreate(qftick_task, "QFTICK" , 0x100, NULL , 0, &xHandle);
	
	// construct active objects
	MenuAO_ctor();
	SetTimeAO_ctor();
	CoffeeMachineAO_ctor();

	// start active objects	
	QActive_start(MenuAOBase, 1, 0, SIZE_OF_EVENT_QUEUE, (void*)0, 0, (QEvent*)0);
 	QActive_start(SetTimeAOBase, 2, 0, SIZE_OF_EVENT_QUEUE, (void*)0, 0, (QEvent*)0);
 	QActive_start(CoffeeMachineAOBase, 3, 0, SIZE_OF_EVENT_QUEUE, (void*)0, 0, (QEvent*)0);
  	
  
	// run QF
	QF_run();	// calls vTaskStartScheduler
}




