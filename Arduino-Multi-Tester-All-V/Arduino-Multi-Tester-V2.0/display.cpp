#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "display.h"
#include "config.h"

// OLED Display Object
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool initDisplay() {
    if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        return false;
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.display();
    return true;
}

void showSplash() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(15, 10);
    display.print(F("MULTI-TESTER"));
    display.setCursor(25, 25);
    display.print(F("PROJECT"));
    display.display();
    delay(1500);
}

void showNotAvailable(const char* title) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print(title);
    display.setCursor(0, 20);
    display.print(F("Not Implemented!"));
    display.display();
}

void showCapacityScreen(float volts, float amps, uint16_t mah, float wh, uint32_t seconds) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    
    // Line 1: Voltage and Current
    display.setCursor(0, 0);
    display.print(volts, 2);
    display.print(F("V  "));
    display.print(amps, 2);
    display.print(F("A"));

    // Line 2: mAh and Wh
    display.setCursor(0, 12);
    display.print(mah);
    display.print(F("mAh "));
    display.print(wh, 2);
    display.print(F("Wh"));

    // Line 3: Elapsed Time (MM:SS)
    display.setCursor(0, 24);
    uint16_t mins = seconds / 60;
    uint8_t secs = seconds % 60;
    if (mins < 10) display.print(F("0"));
    display.print(mins);
    display.print(F(":"));
    if (secs < 10) display.print(F("0"));
    display.print(secs);

    display.display();
}

void showRpmScreen(uint16_t liveRpm, uint16_t finalRpm, uint8_t remainingSecs, bool isComplete) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    display.setCursor(0, 0);
    display.print(F("RPM METER"));

    if (!isComplete) {
        display.setCursor(0, 12);
        display.print(F("RPM: "));
        display.print(liveRpm);

        display.setCursor(0, 24);
        display.print(F("Time: "));
        display.print(remainingSecs);
        display.print(F("s"));
    } else {
        display.setCursor(0, 12);
        display.print(F("FINAL RPM:"));
        display.setCursor(0, 24);
        display.print(finalRpm);
    }

    display.display();
}