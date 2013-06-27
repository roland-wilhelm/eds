/**
 * led.h
 *
 * franz schauer
 * june 20, 2013
 */
 
#ifndef LED_H_
#define LED_H_
                   
/* Function that initializes LEDs                                             */
void LED_Init(void);

/* Function that turns on requested LED                                       */
void LED_On (unsigned int num);

/* Function that turns off requested LED                                      */
void LED_Off (unsigned int num);

/* Function that outputs value to LEDs                                        */
void LED_Out(unsigned int value);

#endif // LED_H_	
