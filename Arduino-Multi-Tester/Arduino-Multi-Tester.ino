// Arduino-Multi-Tester.ino / main.cpp
#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "menu.h"
#include "rpm_meter.h"
#include "battery_tester.h"
#include "tester_display.h"
#include "pwm_generator.h"
#include "pwm_reader.h" // PWM Reader Header Inclusion

// =====================================================
// OBJECT & GLOBAL INITIALIZATION
// =====================================================
BatteryTester battery(PWM_PIN, VOLT_PIN, CURR_PIN); 

// =====================================================
// BUZZER ALERTS
// =====================================================
// ১. সাধারণ শর্ট বিপ (বাটন ক্লিক)
void triggerBuzzer() {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(30);
    digitalWrite(BUZZER_PIN, LOW);
}

// ২. অটো কাট-অফ / টেস্ট কমপ্লিট এলার্ট (৪টি বিপিং)
void triggerCompletionAlert() {
    for (int i = 0; i < 4; i++) {
        digitalWrite(BUZZER_PIN, HIGH);
        delay(100);
        digitalWrite(BUZZER_PIN, LOW);
        delay(100);
    }
}

// ৩. ম্যানুয়াল স্টপ এলার্ট (১টি লম্বা বিপ)
void triggerStopAlert() {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(400);
    digitalWrite(BUZZER_PIN, LOW);
}

// =====================================================
// BUTTON RELEASE DEBOUNCE
// =====================================================
void waitForButtonRelease(byte pin) {
    while (digitalRead(pin) == LOW) {
        delay(10);
    }
    delay(50);
}

// =====================================================
// DISPLAY REFRESH HELPER
// =====================================================
void updateScreen() {
    if (getCurrentState() == STATE_MAIN_MENU) {
        showMainMenu();
    } else if (getCurrentState() == STATE_SUB_MENU) {
        showSubMenu(getSelectedMenuName());
    }
}

// =====================================================
// SETUP
// =====================================================
void setup() {
    pinMode(BUTTON_UP, INPUT_PULLUP);
    pinMode(BUTTON_DOWN, INPUT_PULLUP);
    pinMode(BUTTON_OK, INPUT_PULLUP);
    
    pinMode(BUZZER_PIN, OUTPUT);
    digitalWrite(BUZZER_PIN, LOW);

    if (!initDisplay()) {
        while (true) {}
    }

    initMenu();
    initRpmMeter();
    battery.begin();
    
    showSplash();
    showMainMenu();
}

// =====================================================
// LOOP
// =====================================================
void loop() {

    // -------------------------------------------------
    // ১. RUNNING STATE (যখন কোনো টুল সক্রিয় থাকবে)
    // -------------------------------------------------
    if (getCurrentState() == STATE_RUNNING) {
        
        // --- Index 0: Battery Capacity Tester ---
        if (getSelectedMenu() == 0) { 
            
            bool wasTesting = battery.getStatus();
            
            // ব্যাকএন্ড হিসাব ও সেন্সর রিডিং (With 1.178 Calibration Factor)
            battery.update(); 
            
            // অটো কাট-অফ চেক (টেস্ট অটো বন্ধ হলে বিপিং করবে)
            if (wasTesting && !battery.getStatus()) {
                triggerCompletionAlert(); 
            }

            // UI রেন্ডার (লাইভ সময় ও ফ্রিজ রেজাল্ট দেখাবে)
            renderBatteryUI(battery); 

            // --- ৩-সেকেন্ড লং প্রেস লজিক (OK Button Hold to Stop) ---
            if (digitalRead(BUTTON_OK) == LOW) {
                unsigned long pressStartTime = millis();
                bool stopTriggered = false;

                while (digitalRead(BUTTON_OK) == LOW) {
                    
                    // ৩ সেকেন্ড ধরে রাখলে টেস্ট বন্ধ হবে
                    if (battery.getStatus() && (millis() - pressStartTime >= 3000)) {
                        battery.stopTest(); // ডিসচার্জ বন্ধ ও সময় ফ্রিজ
                        triggerStopAlert(); // লম্বা বিপ
                        stopTriggered = true;
                        break;
                    }
                    delay(50);
                }

                // ৩ সেকেন্ড না হয়ে আলতো প্রেস হলে এবং টেস্ট বন্ধ থাকলে মেনুতে ব্যাক করবে
                if (!stopTriggered && !battery.getStatus()) {
                    triggerBuzzer();
                    setCurrentState(STATE_SUB_MENU);
                    updateScreen();
                }

                waitForButtonRelease(BUTTON_OK);
            }
        }
        // --- Index 1: RPM Meter ---
        else if (getSelectedMenu() == 1) { 
            updateRpmMeter();
            showRpmScreen(getLiveRpm(), getFinalRpm(), getRemainingTime(), isTestComplete());

            if (digitalRead(BUTTON_OK) == LOW) {
                triggerBuzzer();
                stopRpmMeter();
                setCurrentState(STATE_SUB_MENU);
                updateScreen();
                waitForButtonRelease(BUTTON_OK);
            }
        }
        // --- Index 2: PWM Generator ---
        else if (getSelectedMenu() == 2) {
            updatePWMGenerator(); // লাইভ স্ক্রিন এবং গ্রাফ রেন্ডার

            // UP Button: Increase Value
            if (digitalRead(BUTTON_UP) == LOW) {
                triggerBuzzer();
                handlePWMEncoderUp();
                waitForButtonRelease(BUTTON_UP);
            }

            // DOWN Button: Decrease Value
            if (digitalRead(BUTTON_DOWN) == LOW) {
                triggerBuzzer();
                handlePWMEncoderDown();
                waitForButtonRelease(BUTTON_DOWN);
            }

            // OK Button: Short Press -> Toggle Field / Long Press (2s) -> Exit
            if (digitalRead(BUTTON_OK) == LOW) {
                unsigned long pressStart = millis();
                bool isLongPress = false;

                while (digitalRead(BUTTON_OK) == LOW) {
                    if (millis() - pressStart >= 2000) {
                        isLongPress = true;
                        break;
                    }
                    delay(20);
                }

                if (isLongPress) {
                    stopPWMOutput();       // PWM জেনারেটর বন্ধ
                    triggerStopAlert();    // বিপিং এলার্ট
                    setCurrentState(STATE_SUB_MENU);
                    updateScreen();
                } else {
                    triggerBuzzer();
                    togglePWMEditMode();   // Duty এবং Freq এর মধ্যে টগল
                }

                waitForButtonRelease(BUTTON_OK);
            }
        }
        // --- Index 3: PWM Reader ---
        else if (getSelectedMenu() == 3) {
            updatePWMReader(); // লাইভ ফ্রিকোয়েন্সি, ডিউটি সাইকেল ও ওয়েভফর্ম রিড্র করবে

            // OK Button: প্রেস করলে রিডার বন্ধ করে সাব-মেনুতে ব্যাক করবে
            if (digitalRead(BUTTON_OK) == LOW) {
                triggerBuzzer();
                stopPWMReader(); // ইন্টারাপ্ট বিচ্ছিন্ন করবে
                setCurrentState(STATE_SUB_MENU);
                updateScreen();
                waitForButtonRelease(BUTTON_OK);
            }
        }
        return;
    }

    // -------------------------------------------------
    // ২. MENU NAVIGATION (UP / DOWN / OK)
    // -------------------------------------------------
    if (digitalRead(BUTTON_UP) == LOW) {
        triggerBuzzer();
        menuUp();
        updateScreen();
        waitForButtonRelease(BUTTON_UP);
    }

    if (digitalRead(BUTTON_DOWN) == LOW) {
        triggerBuzzer();
        menuDown();
        updateScreen();
        waitForButtonRelease(BUTTON_DOWN);
    }

    if (digitalRead(BUTTON_OK) == LOW) {
        triggerBuzzer();

        if (getCurrentState() == STATE_MAIN_MENU) {
            setCurrentState(STATE_SUB_MENU);
            updateScreen();
        } 
        else if (getCurrentState() == STATE_SUB_MENU) {
            if (getSubMenuSelection() == OPTION_START) {
                
                // Battery Tester Start (Index 0)
                if (getSelectedMenu() == 0) { 
                    battery.startTest();
                    setCurrentState(STATE_RUNNING);
                } 
                // RPM Meter Start (Index 1)
                else if (getSelectedMenu() == 1) { 
                    startRpmMeter();
                    setCurrentState(STATE_RUNNING);
                } 
                // PWM Generator Start (Index 2)
                else if (getSelectedMenu() == 2) {
                    initPWMGenerator();
                    setCurrentState(STATE_RUNNING);
                }
                // PWM Reader Start (Index 3)
                else if (getSelectedMenu() == 3) {
                    initPWMReader();
                    setCurrentState(STATE_RUNNING);
                }
                else {
                    showNotAvailable(getSelectedMenuName());
                    delay(1000);
                    updateScreen();
                }

            } else {
                setCurrentState(STATE_MAIN_MENU);
                updateScreen();
            }
        }

        waitForButtonRelease(BUTTON_OK);
    }
}