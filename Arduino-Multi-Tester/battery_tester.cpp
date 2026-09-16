#include "battery_tester.h"

BatteryTester::BatteryTester(uint8_t pwm, uint8_t volt, uint8_t curr) {
    pwmPin = pwm;
    voltPin = volt;
    currPin = curr;
    
    currentVoltage = 0.0;
    currentAmps = 0.0;
    capacitymAh = 0.0;
    energyWh = 0.0;
    isTesting = false;
    startTime = 0;
    lastTime = 0;
    totalTestDuration = 0;
}

void BatteryTester::begin() {
    pinMode(pwmPin, OUTPUT);
    analogWrite(pwmPin, 0);
    pinMode(voltPin, INPUT);
    pinMode(currPin, INPUT);
}

void BatteryTester::startTest() {
    isTesting = true;
    capacitymAh = 0.0;
    energyWh = 0.0;
    startTime = millis();
    lastTime = millis();
    totalTestDuration = 0;
    analogWrite(pwmPin, 255); // Enable Discharge PWM
}

void BatteryTester::stopTest() {
    if (isTesting) {
        totalTestDuration = (millis() - startTime) / 1000; // টেস্টের মোট সময় সেভ
    }
    isTesting = false;
    analogWrite(pwmPin, 0); // Cutoff Gate
    currentAmps = 0.0;
}

void BatteryTester::update() {
    // 1. Read Raw Sensor Data with Averaging (Noise Reduction)
    long sumV = 0;
    long sumI = 0;
    for (int i = 0; i < 10; i++) {
        sumV += analogRead(voltPin);
        sumI += analogRead(currPin);
    }
    float rawV = sumV / 10.0;
    float rawI = sumI / 10.0;
    
    // 2. Convert to Pin Voltage (৪.৬৫V রেফারেন্স অনুযায়ী)
    float rawMeasuredVoltage = (rawV * 4.65) / 1023.0;
    
    // Zener & Divider Hardware Correction (CALIBRATION_FACTOR = 1.178)
    currentVoltage = rawMeasuredVoltage * CALIBRATION_FACTOR;

    // Current Measurement via Shunt Resistor
    float vShunt = (rawI * 4.65) / 1023.0;
    
    if (isTesting) {
       currentAmps = vShunt / 5.4; // Corrected for Load Resistor Tolerance + MOSFET Resistance
    } else {
        currentAmps = 0.0;
    }
    
    // 3. Safety Cut-off Logic (৩.০V-এর নিচে নামলে অটো অফ)
    // 0.5V-এর কম হলে ধরে নেওয়া হবে কোনো ব্যাটারি লাগানো নেই
    if (isTesting && currentVoltage <= CUTOFF_VOLTAGE && currentVoltage > 0.5) {
        stopTest();
        return;
    }

    // 4. Accumulate mAh & Wh (Non-blocking integration)
    if (isTesting) {
        unsigned long currentTime = millis();
        float dt = (currentTime - lastTime) / 3600000.0; // Convert ms to Hours
        
        capacitymAh += (currentAmps * 1000.0) * dt;
        energyWh += (currentVoltage * currentAmps) * dt;
        
        lastTime = currentTime;
    }
}

// মোট অতিক্রান্ত সময় (সেকেন্ডে) রিটার্ন করার ফাংশন
uint32_t BatteryTester::getElapsedTimeSeconds() {
    if (isTesting) {
        return (millis() - startTime) / 1000; // লাইভ স্টপওয়াচ
    } else {
        return totalTestDuration; // টেস্ট বন্ধ হলে অতিক্রান্ত সময় ফ্রিজ থাকবে
    }
}