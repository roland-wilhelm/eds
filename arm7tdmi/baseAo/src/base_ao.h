


#ifndef base_ao_h
#define base_ao_h



#include "qp_port.h"                /* the port of the QEP event processor */


#include <stdlib.h>   
/* Active object class -----------------------------------------------------*/



typedef struct BaseTimEvtTag {

	QTimeEvt  super;
	uint8_t   clk_ctr;

} BaseTimEvt;

typedef struct BasePrintEvtTag {
    
	QEvent  super;
	
} BasePrintEvt;


typedef struct BaseAOTag {
    
	QActive super;
	
} BaseAO;

    

enum Base_AO_Signals {                                         /* all signals for the Base Active  state machine*/
    
	SER_DISPLAY_SIG = Q_USER_SIG,
	BASE_TIME_SIG
   
};





#endif
