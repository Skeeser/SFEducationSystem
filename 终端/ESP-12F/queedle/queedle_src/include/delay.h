#ifndef _DELAY_
#define _DELAY_

#include <Arduino.h>

void driver_delay_us(unsigned int xus);
void driver_delay_xms(unsigned long xms);
void DELAY_S(unsigned int delaytime);

#endif