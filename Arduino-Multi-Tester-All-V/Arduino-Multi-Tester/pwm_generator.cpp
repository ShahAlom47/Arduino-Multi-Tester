// pwm_generator.cpp

#include "pwm_generator.h"
#include "config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

// Default values
static uint16_t currentFreq = 1000; // 1kHz (1000 Hz)
static byte currentDuty = 50;       // 50% Duty Cycle
static PWMEditMode currentEditMode = EDIT_DUTY;

// =====================================================
// HARDWARE TIMER1 SETUP (Pin D9)
// =====================================================
static void applyPWMHardware(uint16_t freq, byte duty) {
    if (duty == 0) {
        pinMode(PWM_GEN_PIN, OUTPUT);
        digitalWrite(PWM_GEN_PIN, LOW);
        return;
    }
    if (duty >= 100) {
        pinMode(PWM_GEN_PIN, OUTPUT);
        digitalWrite(PWM_GEN_PIN, HIGH);
        return;
    }

    pinMode(PWM_GEN_PIN, OUTPUT);
    
    // Set Fast PWM mode 14 (ICR1 as TOP)
    TCCR1A = _BV(COM1A1) | _BV(WGM11);
    
    // Set Prescaler to 8 (16MHz / 8 = 2MHz timer tick)
    TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS11); 

    // Calculate TOP value for ICR1 based on Frequency
    uint32_t topValue = (16000000UL / (8UL * freq)) - 1;
    ICR1 = topValue;

    // Calculate OCR1A for Duty Cycle Match
    OCR1A = (topValue * duty) / 100;
}


// =====================================================
// INITIALIZATION & STOP
// =====================================================
void initPWMGenerator() {
    currentFreq = 1000;
    currentDuty = 50;
    currentEditMode = EDIT_DUTY;
    applyPWMHardware(currentFreq, currentDuty);
}

void stopPWMOutput() {
    TCCR1A = 0; // Disable Timer1 PWM
    TCCR1B = 0;
    digitalWrite(PWM_GEN_PIN, LOW);
}


// =====================================================
// BUTTON & ENCODER HANDLING
// =====================================================
void togglePWMEditMode() {
    if (currentEditMode == EDIT_DUTY) {
        currentEditMode = EDIT_FREQ;
    } else {
        currentEditMode = EDIT_DUTY;
    }
}

void handlePWMEncoderUp() {
    if (currentEditMode == EDIT_DUTY) {
        if (currentDuty < 100) currentDuty++;
    } else {
        // Frequency Step Logic
        if (currentFreq < 100) currentFreq += 1;
        else if (currentFreq < 1000) currentFreq += 50;
        else if (currentFreq < 20000) currentFreq += 500;
    }
    applyPWMHardware(currentFreq, currentDuty);
}

void handlePWMEncoderDown() {
    if (currentEditMode == EDIT_DUTY) {
        if (currentDuty > 0) currentDuty--;
    } else {
        // Frequency Step Logic
        if (currentFreq > 500 && currentFreq <= 20000) currentFreq -= 500;
        else if (currentFreq > 100 && currentFreq <= 500) currentFreq -= 50;
        else if (currentFreq > 1) currentFreq -= 1;
    }
    applyPWMHardware(currentFreq, currentDuty);
}


// =====================================================
// UI / OLED DISPLAY UPDATE WITH WAVEFORM GRAPH
// =====================================================
void updatePWMGenerator() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    
    // Header Title
    display.setTextSize(1);
    display.setCursor(25, 0);
    display.print(F("PWM GENERATOR"));
    display.drawLine(0, 10, SCREEN_WIDTH - 1, 10, SSD1306_WHITE);

    // Display Frequency
    display.setCursor(2, 16);
    if (currentEditMode == EDIT_FREQ) display.print(F("> "));
    else display.print(F("  "));
    display.print(F("Freq: "));
    display.print(currentFreq);
    display.print(F("Hz"));

    // Display Duty Cycle
    display.setCursor(2, 28);
    if (currentEditMode == EDIT_DUTY) display.print(F("> "));
    else display.print(F("  "));
    display.print(F("Duty: "));
    display.print(currentDuty);
    display.print(F("%"));

    // Divider for Graph Box
    display.drawLine(0, 40, SCREEN_WIDTH - 1, 40, SSD1306_WHITE);

    // =====================================================
    // DRAW WAVEFORM GRAPH (Y: 42 to 62, Height: 20px)
    // =====================================================
    int yHigh = 43; // Top line of square wave
    int yLow  = 61; // Bottom line of square wave
    int graphWidth = 120;
    int startX = 4;

    // Calculate High Time pixel width based on Duty Cycle
    int periodPx = 30; // Pixel length per full PWM cycle (4 cycles rendered)
    int highPx = (periodPx * currentDuty) / 100;

    for (int x = startX; x < (startX + graphWidth); x += periodPx) {
        int cycleEnd = x + periodPx;
        if (cycleEnd > SCREEN_WIDTH - 4) cycleEnd = SCREEN_WIDTH - 4;

        if (currentDuty == 0) {
            // Flat Low Line
            display.drawFastHLine(x, yLow, cycleEnd - x, SSD1306_WHITE);
        } 
        else if (currentDuty >= 100) {
            // Flat High Line
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