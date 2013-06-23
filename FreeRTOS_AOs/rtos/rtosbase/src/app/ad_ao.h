#ifndef AD_AO_H
#define AD_AO_H



typedef struct _TimerEvt {

	QTimeEvt super;
	
} TimerEvt;

typedef struct _AdValueChangedEvt {
	
	QEvent super;
	uint16_t value;
	
} AdValueChangedEvt;

typedef struct _Ad {
	
	QActive super;
	
} Ad;

enum AdSignals {
	
	AD_VALUE_CHANGED_SIG = Q_USER_SIG,
	MAX_PUB_SIG,
	TIME_TICK_SIG,
	CONVERTING_SIG,
	MAX_SIG
	
};

extern const QActive *adAO;

int init_adAo();

#endif // AD_AO_H