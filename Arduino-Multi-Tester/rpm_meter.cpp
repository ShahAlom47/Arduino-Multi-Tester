#include "rpm_meter.h"
#include "config.h"

static volatile uint16_t pulseCount = 0;
static uint32_t lastMillis = 0;

static uint16_t liveRpm = 0;
static uint16_t finalRpm = 0;

static uint32_t totalSamplesRpm = 0;
static uint8_t sampleCount = 0;

static uint8_t elapsedTime = 0;
static bool testFinished = false;

// =====================================================
// Interrupt Service Routine (ISR) with Debounce Filter
// =====================================================
void rpmPulseISR() {
    static uint32_t lastInterruptTime = 0;
    uint32_t interruptTime = micros();

    // ৫০০০ মাইক্রোসেকেন্ড (৫ মিলি-সেকেন্ড)-এর কম সময়ে কোনো পালস এলে তা নয়েজ ধরে ব্লক করবে
    if (interruptTime - lastInterruptTime > 5000) {
        pulseCount++;
        lastInterruptTime = interruptTime;
    }
}

void initRpmMeter() {
    pinMode(RPM_SENSOR_PIN, INPUT);
    pulseCount = 0;
    liveRpm = 0;
    finalRpm = 0;
    totalSamplesRpm = 0;
    sampleCount = 0;
    elapsedTime = 0;
    testFinished = false;
}

void startRpmMeter() {
    initRpmMeter();
    lastMillis = millis();
    attachInterrupt(digitalPinToInterrupt(RPM_SENSOR_PIN), rpmPulseISR, RISING);
}

void stopRpmMeter() {
    detachInterrupt(digitalPinToInterrupt(RPM_SENSOR_PIN));
    pulseCount = 0;
}

void updateRpmMeter() {
    if (testFinished) return;

    // প্রতি ১ সেকেন্ড (১০০০ms) পর পর প্রসেস হবে
    if (millis() - lastMillis >= 1000) {
        noInterrupts();
        uint16_t pulses = pulseCount;
        pulseCount = 0;
        interrupts();

        lastMillis = millis();
        elapsedTime++;

        // লাইভ ১ সেকেন্ডের RPM হিসাব
        liveRpm = pulses * 60;

        // ৩ থেকে ৮ নম্বর সেকেন্ড পর্যন্ত ডাটা জমা হবে (মাঝের মোট ৬ সেকেন্ড)
        if (elapsedTime >= 3 && elapsedTime <= 8) {
            totalSamplesRpm += liveRpm;
            sampleCount++;
        }

        // ১০ সেকেন্ড পূর্ণ হলে টেস্ট স্টপ হবে এবং বিইপ সাউন্ড বাজবে (৯ ও ১০ নম্বর সেকেন্ড স্কিপ)
        if (elapsedTime >= 10) {
            stopRpmMeter();
            testFinished = true;
            
            if (sampleCount > 0) {
                finalRpm = totalSamplesRpm / sampleCount; // ৬ সেকেন্ডের গড় RPM
            } else {
                finalRpm = 0;
            }

            // ===============================================
            // ৩ বার বিইপ সাউন্ড (Beep... Beep... Beeeeep!)
            // ===============================================
            digitalWrite(BUZZER_PIN, HIGH);
            delay(100);
            digitalWrite(BUZZER_PIN, LOW);
            delay(100);

            digitalWrite(BUZZER_PIN, HIGH);
            delay(100);
            digitalWrite(BUZZER_PIN, LOW);
            delay(100);

            digitalWrite(BUZZER_PIN, HIGH);
            delay(500); // লম্বা বিইপ
            digitalWrite(BUZZER_PIN, LOW);
        }
    }
}

uint16_t getLiveRpm() {
    return liveRpm;
}

uint16_t getFinalRpm() {
    return finalRpm;
}

uint8_t getRemainingTime() {
    if (elapsedTime >= 10) return 0;
    return 10 - elapsedTime;
}

bool isTestComplete() {
    return testFinished;
}