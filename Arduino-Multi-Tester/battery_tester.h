#ifndef BATTERY_TESTER_H
#define BATTERY_TESTER_H

#include <Arduino.h>

class BatteryTester {
private:
    uint8_t pwmPin;
    uint8_t voltPin;
    uint8_t currPin;
    
    float currentVoltage;
    float currentAmps;
    float capacitymAh;
    float energyWh;
    
    bool isTesting;
    unsigned long startTime;         // টেস্ট শুরুর সময়
    unsigned long lastTime;          // ব্যাকএন্ড হিসাবের সময়
    unsigned long totalTestDuration; // টেস্ট বন্ধ হলে মোট সময় (সেকেন্ডে)
    
    // Hardware Protection & Safety Constants
    const float CALIBRATION_FACTOR = 1.180; // Zener & Divider Loss Multiplier
    const float CUTOFF_VOLTAGE = 3.00;      // 3.7V Li-ion Cut-off (3.0 Volts)

public:
    BatteryTester(uint8_t pwm, uint8_t volt, uint8_t curr);
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
    uint32_t getElapsedTimeSeconds(); // টাইমার ও ফ্রিজ রেজাল্টের জন্য ফাংশন
};

#endif