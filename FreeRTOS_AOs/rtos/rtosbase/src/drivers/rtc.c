/**
 * rtc.c
 *
 * Dennis Wilfert
 * june 23, 2013
 */
 
#include "LPC23xx.H"                        /* LPC23xx/24xx definitions */
#include "../app/events.h"
#include "../app/menu_ao.h"
#include "../app/coffeemachine_ao.h"
#include "rtc.h"


void RTCHandler(void) __irq;

// static events
static AlarmEvt l_AlarmEvt = {{ALARM_SIG}};
static TimeUpdateEvt l_TimeUpdateEvt = {{TIME_UPDATE_SIG}};

/* Initialization of the RTC
 *
 * Enables RTC clock, enables interrupt, disables RTC clock
 */
void RTC_Init( void )
{
  RTC_PREINT = PREINT_RTC; // RTC clock integer divider
  RTC_PREFRAC = PREFRAC_RTC; // RTC clock fraction divider
  PCONP |= 0x200; // Enable power for RTC
  RTC_AMR = 0;	// Alarm mask register
  RTC_CIIR = 0;	// Counter increment interrupt register
  RTC_CCR = 0;	// Clock control register (disable)
  RTC_ILR = 0xFF; // Clear RTC interrupt register
  RTC_CISS = 0;	// SubSecondInterrupt disable
	
  VICVectAddr13  		= (unsigned long)RTCHandler;		// set IRQ handler
  VICVectPriority13  = 10;		// use it for RTC interrupt
  VICIntEnable  		= (1 << 13);	// enable RTC Interrupt
}

/* Starts RTC clock
 *
 */
void RTC_Start( void ) 
{
  RTC_CCR |= CCR_CLKEN;	// Clock enable
  RTC_ILR = ILR_RTCCIF;	// Clear counter increment interrupt interrupt
  RTC_AMR = 0xFF;	// Alarm mask register (disabled)	
  RTC_CIIR = 1 << 1;	// We want a clock interrupt every minute	
}

/* Stops RTC clock
 *
 */
void RTC_Stop( void )
{   
  RTC_CCR &= ~CCR_CLKEN; // Clock disable
} 

/* Resets RTC clock
 *
 */
void RTC_Reset( void )
{   
  RTC_CCR |= CCR_CTCRST;
}

/* Sets RTC time
 * (Only hours and minutes)
 */
void RTC_SetTime( RTCTime* time ) 
{
  RTC_MIN = time->RTC_Min;
  RTC_HOUR = time->RTC_Hour;   
}

/* Sets RTC alarm
 * (Only hours and minutes)
 */
void RTC_SetAlarm( RTCTime* alarm ) 
{   
  RTC_ALMIN = alarm->RTC_Min;
  RTC_ALHOUR = alarm->RTC_Hour;   
}

/* Returns RTC time 
 * (Only hours and minutes)
 */
void RTC_GetTime( RTCTime* time ) 
{
  time->RTC_Min = RTC_MIN;
  time->RTC_Hour = RTC_HOUR;
}

/* Returns RTC alarm time 
 * (Only hours and minutes)
 */
void RTC_GetAlarm( RTCTime* alarm ) 
{
  alarm->RTC_Min = RTC_ALMIN;
  alarm->RTC_Hour = RTC_ALHOUR;
}

/* Enables RTC alarm
 * (Only hours and minutes are compared)
 */
void RTC_AlarmEnable( void ) 
{
  // Disable all alarm compares except minute and hour    
  RTC_AMR = (AMRSEC|AMRDOM|AMRDOW|AMRDOY|AMRMON|AMRYEAR);
  return;
}

/* Disables RTC alarm
 */
void RTC_AlarmDisable( void ) 
{
  // Disable all alarm compares except minute and hour    
  RTC_AMR = (AMRSEC|AMRDOM|AMRMIN|AMRHOUR|AMRDOW|AMRDOY|AMRMON|AMRYEAR);
  return;
}

/* RTC irq handler
 * (Interrupts for change of minutes and for alarm are handled
 */
void RTCHandler(void) __irq 
{  
	
	QF_INT_LOCK();
	
	// Counter increment interrupt
	if((RTC_ILR & ILR_RTCCIF) == 1) {
		l_TimeUpdateEvt.time.RTC_Hour = RTC_HOUR;
		l_TimeUpdateEvt.time.RTC_Min = RTC_MIN;
		// Menu gets the time update
		QActive_postFIFO(MenuAOBase, (QEvent*)&l_TimeUpdateEvt);
		RTC_ILR |= ILR_RTCCIF;		// clear interrupt flag		
	}
	// Alarm interrupt
	if((RTC_ILR & ILR_RTCALF) == 1) {
		QActive_postFIFO(CoffeeMachineAOBase, (QEvent*)&l_AlarmEvt);
		RTC_ILR |= ILR_RTCALF;		// clear interrupt flag		
	}
	
	QF_INT_UNLOCK();
	
	RTC_ILR = 0;
  VICVectAddr = 0;	// acknowledge interrupt
}
