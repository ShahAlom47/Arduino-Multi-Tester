// pwm_generator.h

#ifndef PWM_GENERATOR_H
#define PWM_GENERATOR_H

#include <Arduino.h>

// PWM Editing Options
enum PWMEditMode {
  EDIT_DUTY = 0,
  EDIT_FREQ = 1
};

// Function Prototypes
void initPWMGenerator();
void updatePWMGenerator();
void handlePWMEncoderUp();
void handlePWMEncoderDown();
void togglePWMEditMode();
void stopPWMOutput();

#endif // PWM_GENERATOR_H