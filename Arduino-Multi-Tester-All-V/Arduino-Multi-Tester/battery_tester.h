#ifndef BATTERY_TESTER_H
#define BATTERY_TESTER_H

#include <Arduino.h>

class BatteryTester {
private:
    uint8_t pwmPin;
    uint8_t voltPin;
    uint8_t currPin;
    uint8_t buzzerPin;
    
    float currentVoltage;
    float currentAmps;
    float capacitymAh;
    float energyWh;
    
    bool isTesting;
    bool warningBeepDone;            // ৩.০V বিজার একবার বাজার ট্র্যাক
    bool buzzerActive;               // বিজার বর্তমানে অন আছে কি না
    unsigned long buzzerStartTime;   // বিজার অন হওয়ার সময়
    
    unsigned long startTime;         // টেস্ট শুরুর সময়
    unsigned long lastTime;          // ব্যাকএন্ড হিসাবের সময়
    unsigned long totalTestDuration; // টেস্ট বন্ধ হলে মোট সময় (সেকেন্ডে)
    
    // Hardware Protection & Safety Constants
    const float CALIBRATION_FACTOR = 1.180; // Zener & Divider Loss Multiplier
    const float WARNING_VOLTAGE = 3.00;     // ৩.০০V ওয়ার্নিং বিজার থ্রেশহোল্ড
    const float CUTOFF_VOLTAGE = 2.70;      // ২.৭০V অটো কাট-অফ থ্রেশহোল্ড

public:
    BatteryTester(uint8_t pwm, uint8_t volt, uint8_t curr, uint8_t buzzer = 255);
    void begin();
    void startTest();
    void stopTest();
    void update();
    
    // Getters
    float getVoltage() { return currentVoltage; }
    float getCurrentmA() { return currentAmps * 1000.0; }
    float getCapacitymAh() { return capacitymAh; }
    float getEnergyWh() { return energyWh; }
    bool getStatus() { return isTesting; }
    uint32_t getElapsedTimeSeconds();
};

#endif