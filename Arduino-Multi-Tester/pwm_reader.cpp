// pwm_reader.cpp

#include "pwm_reader.h"
#include "config.h"
#include "menu.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

// Dynamic Signal Variables (Volatile for ISR)
static volatile unsigned long riseTime = 0;
static volatile unsigned long fallTime = 0;
static volatile unsigned long highPeriod = 0;
static volatile unsigned long totalPeriod = 0;
static volatile bool newSignalReady = false;

// Processed Data
static float measuredFreq = 0.0;
static float measuredDuty = 0.0;
static unsigned long lastSignalCheck = 0;

// =====================================================
// INTERRUPT SERVICE ROUTINE (ISR)
// =====================================================
void pwmInputISR() {
    unsigned long now = micros();
    
    if (digitalRead(PWM_READ_PIN) == HIGH) {
        // Rising Edge Detected
        totalPeriod = now - riseTime;
        riseTime = now;
        newSignalReady = true;
    } else {
        // Falling Edge Detected
        fallTime = now;
        highPeriod = fallTime - riseTime;
    }
}

// =====================================================
// INITIALIZATION & STOP
// =====================================================
void initPWMReader() {
    pinMode(PWM_READ_PIN, INPUT);
    riseTime = 0;
    fallTime = 0;
    highPeriod = 0;
    totalPeriod = 0;
    measuredFreq = 0.0;
    measuredDuty = 0.0;
    
    // Attach Hardware Interrupt on Pin D2 (INT0)
    attachInterrupt(digitalPinToInterrupt(PWM_READ_PIN), pwmInputISR, CHANGE);
}

void stopPWMReader() {
    detachInterrupt(digitalPinToInterrupt(PWM_READ_PIN));
}

// =====================================================
// GETTERS
// =====================================================
float getReadFrequency() { return measuredFreq; }
float getReadDutyCycle() { return measuredDuty; }

// =====================================================
// UI & DATA UPDATE WITH WAVEFORM GRAPH
// =====================================================
void updatePWMReader() {
    // 1. Frequency and Duty Calculation Logic
    if (newSignalReady && totalPeriod > 0) {
        newSignalReady = false;
        lastSignalCheck = millis();

        // Calculate Frequency (1,000,000 us / totalPeriod us)
        measuredFreq = 1000000.0 / (float)totalPeriod;

        // Calculate Duty Cycle (highPeriod / totalPeriod * 100)
        measuredDuty = ((float)highPeriod / (float)totalPeriod) * 100.0;
        
        if (measuredDuty > 100.0) measuredDuty = 100.0;
        if (measuredDuty < 0.0) measuredDuty = 0.0;
    }

    // Timeout Check: No signal detected within 1 second
    if (millis() - lastSignalCheck > 1000) {
        if (digitalRead(PWM_READ_PIN) == HIGH) {
            measuredFreq = 0.0;
            measuredDuty = 100.0; // Constant High 5V
        } else {
            measuredFreq = 0.0;
            measuredDuty = 0.0;   // Constant Low GND
        }
    }

    // 2. Render Screen UI
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Header Title
    display.setTextSize(1);
    display.setCursor(28, 0);
    display.print(F("PWM READER"));
    display.drawLine(0, 10, SCREEN_WIDTH - 1, 10, SSD1306_WHITE);

    // Frequency Display
    display.setCursor(2, 16);
    display.print(F("Freq: "));
    if (measuredFreq >= 1000.0) {
        display.print(measuredFreq / 1000.0, 2);
        display.print(F("kHz"));
    } else {
        display.print((int)measuredFreq);
        display.print(F("Hz"));
    }

    // Duty Cycle Display
    display.setCursor(2, 28);
    display.print(F("Duty: "));
    display.print(measuredDuty, 1);
    display.print(F("%"));

    // Divider for Graph Box
    display.drawLine(0, 40, SCREEN_WIDTH - 1, 40, SSD1306_WHITE);

    // =====================================================
    // DRAW INPUT WAVEFORM GRAPH (Y: 42 to 62, Height: 20px)
    // =====================================================
    int yHigh = 43; // Top line of square wave
    int yLow  = 61; // Bottom line of square wave
    int graphWidth = 120;
    int startX = 4;

    // Calculate High Time pixel width based on Measured Duty Cycle
    int periodPx = 30; // Pixel length per full PWM cycle
    int highPx = (periodPx * (int)measuredDuty) / 100;

    for (int x = startX; x < (startX + graphWidth); x += periodPx) {
        int cycleEnd = x + periodPx;
        if (cycleEnd > SCREEN_WIDTH - 4) cycleEnd = SCREEN_WIDTH - 4;

        if (measuredDuty <= 0.0) {
            // Flat Low Line (0% Duty / 0V Input)
            display.drawFastHLine(x, yLow, cycleEnd - x, SSD1306_WHITE);
        } 
        else if (measuredDuty >= 100.0) {
            // Flat High Line (100% Duty / 5V Input)
            display.drawFastHLine(x, yHigh, cycleEnd - x, SSD1306_WHITE);
        } 
        else {
            int highEnd = x + highPx;
            if (highEnd > cycleEnd) highEnd = cycleEnd;

            // 1. High pulse horizontal line
            display.drawFastHLine(x, yHigh, highEnd - x, SSD1306_WHITE);
            // 2. Falling Edge vertical line
            display.drawFastVLine(highEnd, yHigh, yLow - yHigh + 1, SSD1306_WHITE);
            // 3. Low pulse horizontal line
            display.drawFastHLine(highEnd, yLow, cycleEnd - highEnd, SSD1306_WHITE);
            // 4. Rising Edge vertical line
            if (cycleEnd < SCREEN_WIDTH - 4) {
                display.drawFastVLine(cycleEnd, yHigh, yLow - yHigh + 1, SSD1306_WHITE);
            }
        }
    }

    display.display();
}