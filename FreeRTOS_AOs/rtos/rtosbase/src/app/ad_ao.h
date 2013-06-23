#ifndef AD_AO_H
#define AD_AO_H

#include "events.h"


// AD_VALUE_CHANGED_SIG, published Signal each time ad value changed
typedef struct _AdValueChangedEvt {
	
	QEvent super;
	uint16_t value;
	
} AdValueChangedEvt;


extern const QActive *adAO;

void ad_ctor(void);
int ad_converter_init(void);


#endif // AD_AO_H
