#include "battery_tester.h"

// Buzzer পিন সহ কনস্ট্রাক্টর
BatteryTester::BatteryTester(uint8_t pwm, uint8_t volt, uint8_t curr, uint8_t buzzer) {
    pwmPin = pwm;
    voltPin = volt;
    currPin = curr;
    buzzerPin = buzzer;
    
    currentVoltage = 0.0;
    currentAmps = 0.0;
    capacitymAh = 0.0;
    energyWh = 0.0;
    isTesting = false;
    warningBeepDone = false;
    buzzerActive = false;
    buzzerStartTime = 0;
    
    startTime = 0;
    lastTime = 0;
    totalTestDuration = 0;
}

void BatteryTester::begin() {
    pinMode(pwmPin, OUTPUT);
    analogWrite(pwmPin, 0);
    pinMode(voltPin, INPUT);
    pinMode(currPin, INPUT);
    
    if (buzzerPin != 255) {
        pinMode(buzzerPin, OUTPUT);
        digitalWrite(buzzerPin, LOW);
    }
}

void BatteryTester::startTest() {
    isTesting = true;
    warningBeepDone = false; // নতুন টেস্ট শুরু হলে ওয়ার্নিং ফ্ল্যাগ রিসেট
    buzzerActive = false;
    
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
    
    // টেস্ট বন্ধ হলে বিজার অফ করে দেওয়া
    if (buzzerPin != 255) {
        digitalWrite(buzzerPin, LOW);
        buzzerActive = false;
    }
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
    
    // 2. Convert to Pin Voltage (৪.৬৫V রেফারেন্স অনুযায়ী)
    float rawMeasuredVoltage = (rawV * 4.65) / 1023.0;
    
    // Zener & Divider Hardware Correction
    currentVoltage = rawMeasuredVoltage * CALIBRATION_FACTOR;

    // Current Measurement via Shunt Resistor
    float vShunt = (rawI * 4.65) / 1023.0;
    
    if (isTesting) {
       currentAmps = vShunt / 5.4; // Load Resistor + MOSFET Resistance Correction
    } else {
        currentAmps = 0.0;
    }
    
    // -------------------------------------------------------------
    // 3. Warning & Cut-off Logic Update
    // -------------------------------------------------------------
    if (isTesting) {
        // A) Absolute Cut-off Logic (২.৭০V বা তার নিচে নামলে অটো অফ)
        if (currentVoltage <= CUTOFF_VOLTAGE && currentVoltage > 0.5) {
            stopTest();
            return;
        }

        // B) 3.0V Warning Beep Logic (Non-blocking 3 Sec Beep)
        if (currentVoltage <= WARNING_VOLTAGE && currentVoltage > CUTOFF_VOLTAGE && !warningBeepDone) {
            if (buzzerPin != 255) {
                digitalWrite(buzzerPin, HIGH);
                buzzerStartTime = millis();
                buzzerActive = true;
            }
            warningBeepDone = true; // ফ্ল্যাগ ট্রু করা হলো যেন একবারই বাজবে
        }
    }

    // Non-blocking 3 Sec Buzzer Timer Handler
    if (buzzerActive && (millis() - buzzerStartTime >= 3000)) {
        if (buzzerPin != 255) {
            digitalWrite(buzzerPin, LOW);
        }
        buzzerActive = false;
    }

    // -------------------------------------------------------------
    // 4. Accumulate mAh & Wh (Non-blocking integration)
    // -------------------------------------------------------------
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
        return totalTestDuration; // টেস্ট বন্ধ হলে ফ্রিজ রেজাল্ট
    }
}