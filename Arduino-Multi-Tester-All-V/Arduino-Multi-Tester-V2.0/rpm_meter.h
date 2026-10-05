#ifndef RPM_METER_H
#define RPM_METER_H

#include <Arduino.h>

void initRpmMeter();
void startRpmMeter();
void stopRpmMeter();
void updateRpmMeter();

uint16_t getLiveRpm();
uint16_t getFinalRpm();
uint8_t getRemainingTime();
bool isTestComplete();

#endif