// config.h

#ifndef CONFIG_H
#define CONFIG_H

// =====================================================
// OLED CONFIGURATION
// =====================================================

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDRESS 0x3C


// =====================================================
// BUTTON CONFIGURATION (D2 & D3 Free for Interrupts)
// =====================================================

#define BUTTON_UP    6   // D6
#define BUTTON_DOWN  8   // D8 (D3 ফ্রি করার জন্য D8-এ আনা হলো)
#define BUTTON_OK    4   // D4


// =====================================================
// BUZZER & SENSOR CONFIGURATION
// =====================================================

#define BUZZER_PIN     5
#define RPM_SENSOR_PIN 2 // Hardware Interrupt 0 (D2)


// =====================================================
// BATTERY TESTER CONFIGURATION
// =====================================================

#define PWM_PIN  10  // D10 -> MOSFET Gate Control (D7 ফ্রি করার জন্য D10-এ নেওয়া হলো)
#define VOLT_PIN A0  // A0 -> Battery Voltage Sensing
#define CURR_PIN A1  // A1 -> Current Sensing (across 5 ohm load)


// =====================================================
// PWM GENERATOR & READER CONFIGURATION
// =====================================================

#define PWM_GEN_PIN  9 // D9 -> PWM Output (Hardware Timer1)
#define PWM_READ_PIN 3 // Hardware Interrupt 1 (D3) (RPM-এর সাথে সংঘাত এড়াতে D3-তে আনা হলো)


// =====================================================
// DEVICE INFORMATION
// =====================================================

#define DEVICE_NAME "MULTI TESTER"
#define CREATOR_NAME "Shah Alom"
#define FIRMWARE_VERSION "1.0"


// =====================================================
// MENU
// =====================================================

#define MENU_COUNT 4

#endif // CONFIG_H