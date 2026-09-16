#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>

// Display initialization
bool initDisplay();

// Screens
void showSplash();
void showNotAvailable(const char* title);

// Battery & Capacity Screen Helplines (Wh সহ আপডেট করা হলো)
void showCapacityScreen(float voltage, float current, uint16_t mah, float wh, uint32_t seconds);

// RPM Display Screen (৪টি প্যারামিটার সহ)
void showRpmScreen(uint16_t liveRpm, uint16_t finalRpm, uint8_t remainingTime, bool isComplete);

#endif