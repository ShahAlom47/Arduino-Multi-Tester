#ifndef TESTER_DISPLAY_H
#define TESTER_DISPLAY_H

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "battery_tester.h"

// display.h ফাইল থেকে প্রজেক্টের আসল ডিসপ্লে অবজেক্ট লিংক করার জন্য
extern Adafruit_SSD1306 display; 

// গ্লোবাল ফাংশন প্রোটোটাইপ
void renderBatteryUI(BatteryTester &tester);

#endif