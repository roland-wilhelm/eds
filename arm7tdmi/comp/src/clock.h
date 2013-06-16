/*****************************************************************************
* Product: Orthogonal Component state pattern example
* Last Updated for Version: 4.0.00
* Date of the Last Update:  Apr 07, 2008
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2008 Quantum Leaps, LLC. All rights reserved.
*
* This software may be distributed and modified under the terms of the GNU
* General Public License version 2 (GPL) as published by the Free Software
* Foundation and appearing in the file GPL.TXT included in the packaging of
* this file. Please note that GPL Section 2[b] requires that all works based
* on this software must also be made publicly available under the terms of
* the GPL ("Copyleft").
*
* Alternatively, this software may be distributed and modified under the
* terms of Quantum Leaps commercial licenses, which expressly supersede
* the GPL and are specifically designed for licensees interested in
* retaining the proprietary status of their code.
*
* Contact information:
* Quantum Leaps Web site:  http://www.quantum-leaps.com
* e-mail:                  info@quantum-leaps.com
*****************************************************************************/
#ifndef clock_h
#define clock_h

enum AlarmClockSignals {
    TICK_SIG = Q_USER_SIG,                               /* time tick event */
    ALARM_SET_SIG,                                         /* set the alarm */
    ALARM_ON_SIG,                                      /* turn the alarm on */
    ALARM_OFF_SIG,                                    /* turn the alarm off */
    ALARM_SIG,  /* alarm event from Alarm component to AlarmClock container */
    CLOCK_12H_SIG,                             /* set the clock in 12H mode */
    CLOCK_24H_SIG,                             /* set the clock in 24H mode */
    TIME_SIG,           /* time event sent to Alarm (contains current time) */
    TERMINATE_SIG                              /* terminate the application */
};
/*..........................................................................*/
typedef struct SetEvtTag {
    QEvent super;                                     /* derive from QEvent */
    uint8_t digit;
} SetEvt;

typedef struct TimeEvtTag {
    QEvent super;                                     /* derive from QEvent */
    uint32_t current_time;
} TimeEvt;

extern QActive *APP_alarmClock;       /* AlarmClock container active object */

#endif                                                           /* clock_h */
