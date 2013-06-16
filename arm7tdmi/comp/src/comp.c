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

//Q_DEFINE_THIS_FILE				Qasert not used 

/*..........................................................................*/
typedef struct AlarmClockTag {              /* the AlarmClock active object */

    QActive super;                                   /* derive from QActive */

    uint32_t current_time;                   /* the current time in seconds */
    Alarm alarm;                              /* Alarm orthogonal component */
    QTimeEvt timeEvt;        /* time event generator (generates time ticks) */
} AlarmClock;

void AlarmClock_ctor(AlarmClock *me);                       /* default ctor */
                                          /* hierarchical state machine ... */
static QState AlarmClock_initial    (AlarmClock *me, QEvent const *e);
static QState AlarmClock_timekeeping(AlarmClock *me, QEvent const *e);
static QState AlarmClock_mode12hr   (AlarmClock *me, QEvent const *e);
static QState AlarmClock_mode24hr   (AlarmClock *me, QEvent const *e);
static QState AlarmClock_final      (AlarmClock *me, QEvent const *e);

/*..........................................................................*/
void AlarmClock_ctor(AlarmClock *me) {                      /* default ctor */
    QActive_ctor(&me->super, (QStateHandler)&AlarmClock_initial);
    Alarm_ctor(&me->alarm);                /* orthogonal component ctor */
    QTimeEvt_ctor(&me->timeEvt, TICK_SIG);       /* private time event ctor */
}
/* HSM definition ----------------------------------------------------------*/
QState AlarmClock_initial(AlarmClock *me, QEvent const *e) {
    (void)e;               /* avoid compiler warning about unused parameter */
    me->current_time = 0;
    Alarm_init(&me->alarm);      /* the initial transition in the component */
    return Q_TRAN(&AlarmClock_timekeeping);
}
/*..........................................................................*/
QState AlarmClock_final(AlarmClock *me, QEvent const *e) {
    (void)me;          /* avoid the compiler warning about unused parameter */
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            printf("-> final\nBye!Bye!\n");
            QF_stop();                         /* terminate the application */
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState AlarmClock_timekeeping(AlarmClock *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
                                           /* periodic timeout every second */
            QTimeEvt_postEvery(&me->timeEvt,
                              (QActive *)me, BSP_TICKS_PER_SEC);
            return Q_HANDLED();
        }
        case Q_EXIT_SIG: {
            QTimeEvt_disarm(&me->timeEvt);
            return Q_HANDLED();
        }
        case Q_INIT_SIG: {
            return Q_TRAN(&AlarmClock_mode24hr);
        }
        case CLOCK_12H_SIG: {
            return Q_TRAN(&AlarmClock_mode12hr);
        }
        case CLOCK_24H_SIG: {
            return Q_TRAN(&AlarmClock_mode24hr);
        }
        case ALARM_SIG: {
            printf("Wake up!!!\n");
            return Q_HANDLED();
        }
        case ALARM_SET_SIG:
        case ALARM_ON_SIG:
        case ALARM_OFF_SIG: {
                      /* synchronously dispatch to the orthogonal component */
            Alarm_dispatch(&me->alarm, e);
            return Q_HANDLED();
        }
        case TERMINATE_SIG: {
            return Q_TRAN(&AlarmClock_final);
        }
    }
    return Q_SUPER(&QHsm_top);
}
/*..........................................................................*/
QState AlarmClock_mode24hr(AlarmClock *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            printf("*** 24-hour mode\n");
            return Q_HANDLED();
        }
        case TICK_SIG: {
            TimeEvt pe;    /* temporary synchronous event for the component */

            if (++me->current_time == 24*60) {  /* roll over in 24-hr mode? */
                me->current_time = 0;
            }
            printf("%02ld:%02ld\n",
                   me->current_time/60, me->current_time%60);
            ((QEvent *)&pe)->sig = TIME_SIG;
            pe.current_time = me->current_time;
                      /* synchronously dispatch to the orthogonal component */
            Alarm_dispatch(&me->alarm, (QEvent *)&pe);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&AlarmClock_timekeeping);
}
/*..........................................................................*/
QState AlarmClock_mode12hr(AlarmClock *me, QEvent const *e) {
    switch (e->sig) {
        case Q_ENTRY_SIG: {
            printf("*** 12-hour mode\n");
            return Q_HANDLED();
        }
        case TICK_SIG: {
            TimeEvt pe;    /* temporary synchronous event for the component */
            uint32_t h;                  /* temporary variable to hold hour */

            if (++me->current_time == 12*60) {  /* roll over in 12-hr mode? */
                me->current_time = 0;
            }
            h = me->current_time/60;
            printf("%02ld:%02ld %s\n", (h % 12) ? (h % 12) : 12,
                   me->current_time % 60, (h / 12) ? "PM" : "AM");
            ((QEvent *)&pe)->sig = TIME_SIG;
            pe.current_time = me->current_time;

                      /* synchronously dispatch to the orthogonal component */
            Alarm_dispatch(&me->alarm, (QEvent *)&pe);
            return Q_HANDLED();
        }
    }
    return Q_SUPER(&AlarmClock_timekeeping);
}

/* test harness ============================================================*/

/* Local-scope objects -----------------------------------------------------*/
static AlarmClock l_alarmClock;            /* the AlarmClock active object  */
static QEvent const *l_alarmClockQSto[10];  /* queue storage for AlarmClock */
static TimeEvt l_smlPoolSto[10];       /* storage for small-size event pool */

/* Global-scope objects (opaque pointer to the AlarmClock container) -------*/
QActive *APP_alarmClock= (QActive *)&l_alarmClock;/*AlarmClock container AO */

/*..........................................................................*/
int main() {
   
   BSP_Init();
    printf("Orthogonal Component pattern\nQEP version: %s\nQF  version: %s\n"
           "Press 'o' to turn the Alarm ON\n"
           "Press 'f' to turn the Alarm OFF\n"
           "Press '0'..'9' to set the Alarm time\n"
           "Press 'a' to set the Clock in 12-hour mode\n"
           "Press 'b' to set the Clock in 24-hour mode\n"
           "Press ESC to quit...\n",
           QEP_getVersion(), QF_getVersion());

   

    QF_init();    /* initialize the framework and the underlying RT kernel */

    

                                               /* initialize event pools... */

    QF_poolInit(l_smlPoolSto, sizeof(l_smlPoolSto), sizeof(l_smlPoolSto[0]));

                             /* instantiate and start the active objects... */
    AlarmClock_ctor(&l_alarmClock);
    QActive_start((QActive *)&l_alarmClock, 1,
                  l_alarmClockQSto, Q_DIM(l_alarmClockQSto),
                  (void *)0, 0, (QEvent *)0);

    QF_run();                                     /* run the QF application */

    return 0;
}
/*..........................................................................*/
void BSP_onKeyboardInput(uint8_t key) {
    switch (key) {
        case 0x6f: {                                  /* 'o': Alarm on event? */
            QActive_postFIFO((QActive *)&l_alarmClock,
                             Q_NEW(QEvent, ALARM_ON_SIG));
            break;
        }
        case 0x66: {                                 /* 'f': Alarm off event? */
            QActive_postFIFO((QActive *)&l_alarmClock,
                             Q_NEW(QEvent, ALARM_OFF_SIG));
            break;
        }
        case  0x32:                                                     /* '1' */
        case  0x33:                                                     /* '2' */
        case  0x34:                                                     /* '3' */
        case  0x35:                                                     /* '4' */
        case  0x36:                                                     /* '5' */
        case  0x37:                                                     /* '6' */
        case  0x38:                                                     /* '7' */
        case  0x39:                                                     /* '8' */
        case  0x31:{                                                   /* '9' */
            SetEvt *e = Q_NEW(SetEvt, ALARM_SET_SIG);
            e->digit = (uint8_t)(key - 1);
            QActive_postFIFO((QActive *)&l_alarmClock, (QEvent *)e);
            break;
        }
        case 0x4f: {                                                   /* '0' */
            SetEvt *e = Q_NEW(SetEvt, ALARM_SET_SIG);
            e->digit = 0;
            QActive_postFIFO((QActive *)&l_alarmClock, (QEvent *)e);
            break;
        }
        case 0x61: {                                 /* 'a': Clock 12H event? */
            QActive_postFIFO((QActive *)&l_alarmClock,
                             Q_NEW(QEvent, CLOCK_12H_SIG));
            break;
        }
        case 0x62: {                                 /* 'b': Clock 24H event? */
            QActive_postFIFO((QActive *)&l_alarmClock,
                             Q_NEW(QEvent, CLOCK_24H_SIG));
            break;
        }
        case 0x1b: {                                         /* ESC pressed? */
            QActive_postFIFO((QActive *)&l_alarmClock,
                             Q_NEW(QEvent, TERMINATE_SIG));
            break;
        }
    }
}
