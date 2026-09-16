// pwm_reader.h

#ifndef PWM_READER_H
#define PWM_READER_H

#include <Arduino.h>

// Function Prototypes
void initPWMReader();
void updatePWMReader();
void stopPWMReader();

// Sensor Data Getters
float getReadFrequency();
float getReadDutyCycle();

#endif // PWM_READER_H