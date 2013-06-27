/**
 * rtc.h
 *
 * Dennis Wilfert
 * june 23, 2013
 */
 
#ifndef RTC_H_
#define RTC_H_

#define PREINT_RTC	0x016d  // RTC Clock prescaler integer PCLK
#define PREFRAC_RTC	0x1680  // RTC Clock prescaler fraction PCLK
						
typedef struct {
    unsigned char RTC_Min;     // Minutes
    unsigned char RTC_Hour;    // Hours
} RTCTime;

typedef enum {
	IMSEC	=	0x01,		// Seconds mask
	IMMIN	=	0x02,		// Minutes mask
	IMHOUR = 0x04,	// Hours mask
	IMDOM	= 0x08,		// Day of month mask
	IMDOW	= 0x10,		// Day of week mask
	IMDOY	= 0x20,		// Day of year mask
	IMMON	=	0x40,		// Month mask
	IMYEAR = 0x80		// Year mask
} RTCTimeMask;

typedef enum {
	AMRSEC = 0x01,  // Alarm seconds mask
	AMRMIN = 0x02,  // Alarm minutes mask
	AMRHOUR = 0x04, // Alarm hours mask
	AMRDOM = 0x08,  // Alarm day of month mask
	AMRDOW = 0x10,  // Alarm day of week mask
	AMRDOY = 0x20,  // Alarm day of year mask
	AMRMON = 0x40,  // Alarm month mask
	AMRYEAR = 0x80  // Alarm year mask
} RTCAlarmMask;

typedef enum {
	ILR_RTCCIF = 0x01,	// Counter increment interrupt mask
	ILR_RTCALF = 0x02		// Alarm interrupt mask
} RTCILRMask;

typedef enum {
	CCR_CLKEN =	0x01,		// Clock enable mask
	CCR_CTCRST = 0x02,	// Clock reset mask
	CCR_CLKSRC = 0x10		// Clock source mask
} RTCCCRMask;

void RTC_Init( void );
void RTC_Start( void );
void RTC_Stop( void );
void RTC_Reset( void );
void RTC_SetTime( RTCTime* time );
void RTC_SetAlarm( RTCTime* alarm );
void RTC_GetTime( RTCTime* time );
void RTC_GetAlarm( RTCTime* alarm );
void RTC_AlarmEnable( void );
void RTC_AlarmDisable( void );
#endif /* end RTC_H_ */
