#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "config.h"
#include "display.h"

// =====================================================
// OLED OBJECT
// =====================================================

Adafruit_SSD1306 display(
  SCREEN_WIDTH,
  SCREEN_HEIGHT,
  &Wire,
  OLED_RESET
);

// =====================================================
// DISPLAY INITIALIZATION
// =====================================================

bool initDisplay() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
    return false;
  }

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.display();

  return true;
}

// =====================================================
// SPLASH SCREEN
// =====================================================

void showSplash() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  int16_t x1, y1;
  uint16_t w, h;

  // TITLE
  const char* title = DEVICE_NAME;
  display.setTextSize(1);
  display.getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 12);
  display.println(title);

  // BY
  const char* byText = "by";
  display.getTextBounds(byText, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 34);
  display.println(byText);

  // CREATOR
  const char* creator = CREATOR_NAME;
  display.getTextBounds(creator, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 47);
  display.println(creator);

  display.display();
  delay(2500);
}

// =====================================================
// NOT AVAILABLE SCREEN
// =====================================================

void showNotAvailable(const char* title) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  int16_t x1, y1;
  uint16_t w, h;

  // MODULE NAME
  display.getTextBounds(title, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 5);
  display.println(title);

  // MESSAGE 1
  const char* msg1 = "Currently";
  display.getTextBounds(msg1, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 25);
  display.println(msg1);

  // MESSAGE 2
  const char* msg2 = "Not Available";
  display.getTextBounds(msg2, 0, 0, &x1, &y1, &w, &h);
  display.setCursor((SCREEN_WIDTH - w) / 2, 38);
  display.println(msg2);

  display.display();
}

// =====================================================
// CAPACITY TEST LIVE DISPLAY SCREEN
// =====================================================

void showCapacityScreen(float voltage, float current, uint16_t mah, uint32_t seconds) {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);

  // Header
  display.setCursor(0, 0);
  display.print(F("CAPACITY TEST"));
  display.drawLine(0, 9, SCREEN_WIDTH - 1, 9, SSD1306_WHITE);

  // Voltage
  display.setCursor(0, 15);
  display.print(F("Volt: "));
  display.print(voltage, 2);
  display.print(F(" V"));

  // Current
  display.setCursor(0, 27);
  display.print(F("Curr: "));
  display.print(current, 0);
  display.print(F(" mA"));

  // Capacity (mAh)
  display.setCursor(0, 39);
  display.print(F("Cap : "));
  display.print(mah);
  display.print(F(" mAh"));

  // Time elapsed (HH:MM:SS or MM:SS)
  uint16_t hrs = seconds / 3600;
  uint16_t mins = (seconds % 3600) / 60;
  uint8_t secs = seconds % 60;

  display.setCursor(0, 51);
  display.print(F("Time: "));
  if (hrs > 0) {
    if (hrs < 10) display.print(F("0"));
    display.print(hrs);
    display.print(F(":"));
  }
  if (mins < 10) display.print(F("0"));
  display.print(mins);
  display.print(F(":"));
  if (secs < 10) display.print(F("0"));
  display.print(secs);

  display.display();
}

// =====================================================
// RPM METER DISPLAY SCREEN
// =====================================================

void showRpmScreen(uint16_t liveRpm, uint16_t finalRpm, uint8_t remainingTime, bool isComplete) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Header
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(F("RPM METER (10s Test)"));
    display.drawLine(0, 9, SCREEN_WIDTH - 1, 9, SSD1306_WHITE);

    if (!isComplete) {
        // টেস্ট চলাকালীন দেখাবে
        display.setTextSize(1);
        display.setCursor(0, 16);
        display.print(F("Time Left: "));
        display.print(remainingTime);
        display.print(F("s"));

        display.setCursor(0, 28);
        display.print(F("Live RPM :"));

        display.setTextSize(2);
        display.setCursor(10, 42);
        display.print(liveRpm);

    } else {
        // ১০ সেকেন্ড শেষ হলে ফাইনাল রেজাল্ট দেখাবে
        display.setTextSize(1);
        display.setCursor(0, 16);
        display.print(F("TEST COMPLETED!"));

        display.setCursor(0, 28);
        display.print(F("Avg RPM (Mid 6s):"));

        display.setTextSize(2);
        display.setCursor(10, 42);
        display.print(finalRpm);
    }

    display.display();
}