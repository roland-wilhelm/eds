/*****************************************************************************
* Product: Orthogonal Component state pattern example
* Date of the Last Update:  Oct 21, 2011
* Ported to Keil MCB 2300 - NXP 2378 Arm/TDMI STANDALONE
*
*                    Q u a n t u m     L e a P s
*                    ---------------------------
*                    innovating embedded systems
*
* Copyright (C) 2002-2009 Quantum Leaps, LLC. All rights reserved.
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
#include "qp_port.h"
#include "qmbsp.h"
#include "alarm.h"
#include "clock.h"

#include <stdio.h>

// Q_DEFINE_THIS_FILE		Qassert not used

/* FSM state-handler functions */
static QState Alarm_initial(Alarm *me, QEvent const *e);
static QState Alarm_off    (Alarm *me, QEvent const *e);
static QState Alarm_on     (Alarm *me, QEvent const *e);

/*..........................................................................*/
void Alarm_ctor(Alarm *me) {
    QFsm_ctor(&me->super, (QStateHandler)&Alarm_initial);
}

/* FSM definition ----------------------------------------------------------*/
QState Alarm_initial(Alarm *me, QEvent const *e) {
    (void)e;               /* avoid compiler warning about unused parameter */
    me->alarm_time = 12*60;
    return Q_TRAN(&Alarm_off);
}
/*..........................................................................*/
QState Alarm_off(Alarm *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
             /* while in the off state, the alarm is kept in decimal format */
            me->alarm_time = (me->alarm_time/60)*100 + me->alarm_time%60;
            printf("*** Alarm OFF %02ld:%02ld\n",
                   me->alarm_time/100, me->alarm_time%100);
            fflush(stdout);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
                      /* upon exit, the alarm is converted to binary format */
            me->alarm_time = (me->alarm_time/100)*60 + me->alarm_time%100;
            return Q_HANDLED();
        }
        case ALARM_ON_SIG: {
                                                         /* alarm in range? */
            if ((me->alarm_time / 100 < 24) && (me->alarm_time % 100 < 60)) {
                return Q_TRAN(&Alarm_on);
            }
            else {      /* alarm out of range -- clear and don't transition */
                me->alarm_time = 0;
                printf("*** Alarm reset %02ld:%02ld\n",
                       me->alarm_time/100, me->alarm_time%100);
                return Q_HANDLED();
            }
        }
        case ALARM_SET_SIG: {
                      /* while setting, the alarm is kept in decimal format */
            me->alarm_time = (10 * me->alarm_time
                              + ((SetEvt const *)e)->digit) % 10000;
            printf("*** Alarm SET %02ld:%02ld\n",
                   me->alarm_time/100, me->alarm_time%100);
            fflush(stdout);
            return Q_HANDLED();
        }
    }
    return Q_IGNORED();
}
/*..........................................................................*/
QState Alarm_on(Alarm *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            printf("*** Alarm ON %02ld:%02ld\n",
                   me->alarm_time/60, me->alarm_time%60);
            fflush(stdout);
            return Q_HANDLED();
        }
        case ALARM_SET_SIG: {
            printf("*** Cannot set Alarm when it is ON\n");
            fflush(stdout);
            return Q_HANDLED();
        }
        case ALARM_OFF_SIG: {
            return Q_TRAN(&Alarm_off);
        }
        case TIME_SIG: {
            if (((TimeEvt *)e)->current_time == me->alarm_time) {
                printf("ALARM!!!\n");
                       /* asynchronously post the event to the container AO */
                QActive_postFIFO(APP_alarmClock, Q_NEW(QEvent, ALARM_SIG));
            }
            return Q_HANDLED();
        }
    }
    return Q_IGNORED();
}
