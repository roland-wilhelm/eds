#ifndef AD_AO_H
#define AD_AO_H

#include "events.h"

#define START_AD_CONVERSION_INTERVAL 100

extern QActive* const adAO;

void ad_ctor(void);
int ad_converter_init(void);


#endif // AD_AO_H
