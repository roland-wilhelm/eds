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
#include "push_button2.h"
#include "menu_ao.h"
#include "settime_ao.h"
#include "ad_ao.h"

// Event Queue AD converter
static const QEvent *adQueueSto[3];

// List for publish-subscribe
static QSubscrList subscrSto[MAX_PUB_SIG];



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
	BSP_Init();
	
	// Initialize the publish-subscribe mechanism
	QF_psInit(subscrSto, Q_DIM(subscrSto));
	
	// init pushbutton, 1000ms for long press
	// pushbutton_init(1000);
	
	// init int0 as interrupt
	int0_init();

	xTaskCreate(qftick_task, "QFTICK" , 0x100 * 3, NULL , 1, &xHandle);
	
	// TODO: initialise and start QF framework
	
	// construct and start MenuAO
	MenuAO_ctor();
	QActive_start(MenuAOBase, 1, l_MenuAOEvtQSto, Q_DIM(l_MenuAOEvtQSto), (void*)0, 0, (QEvent*)0);	

	// construct and start SetTimeAO
	SetTimeAO_ctor();
	QActive_start(SetTimeAOBase, 1, l_SetTimeAOEvtQSto, Q_DIM(l_SetTimeAOEvtQSto), (void*)0, 0, (QEvent*)0);	

	
	// AD converter CTOR an start
	ad_converter_init();
	ad_ctor();
	QActive_start((QActive *)&adAO, 1, adQueueSto, Q_DIM(adQueueSto),
 									(void *)0, 0, (QEvent *)0);
									
	
	vTaskStartScheduler();
}




