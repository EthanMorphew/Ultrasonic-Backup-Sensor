//Ultra Sonic
#pragma once
#include<Arduino.h>
#define TRIGGER PB4
#define ECHO PB5

void sonicSensorInit();
int getSonicRangeCM();
ISR(PCINT0_vect);