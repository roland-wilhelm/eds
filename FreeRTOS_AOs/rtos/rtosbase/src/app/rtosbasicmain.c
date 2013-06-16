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
#include "LCD.h"                    





void qftick_task( void * pvParameters )
{
	portTickType xLastWakeTime;
//int i;
	const portTickType xFrequency = 1000;

  // Initialise the xLastWakeTime variable with the current time.
	xLastWakeTime = xTaskGetTickCount();
  
	// Task code goes here.
	for( ;; )
		{
        
			// Wait for the next cycle.
			vTaskDelayUntil( &xLastWakeTime, xFrequency );

			// Perform action here.
      
			QF_tick();   		//QM Port
			
//    while ( i--) ;  	// busy loop to slow output to terminal
// 			printf("basic freertos task\n");
// 												// Task code goes here.
// 		i=0xffffff;
		}
}




int main (void) {

   xTaskHandle xHandle;

  
 
	 BSP_Init();

	 xTaskCreate( qftick_task, "QFTICK" , 0x100 * 3, NULL ,1, &xHandle);

 
     vTaskStartScheduler();



}




