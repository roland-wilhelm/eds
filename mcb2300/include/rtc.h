//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Header file <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
#ifndef SERVICE_H_
#define SERVICE_H_

typedef struct{
unsigned char seconds;
unsigned char minutes;
unsigned char hours;
unsigned char day_of_week;
unsigned char day_of_month;
unsigned char month;
unsigned short int year;
unsigned short int day_of_year;
}RTC_T;

extern volatile unsigned char rtc_update;

void  Read_RTC(RTC_T* rtc);
void  Write_RTC(RTC_T* new_rtc);

#endif
