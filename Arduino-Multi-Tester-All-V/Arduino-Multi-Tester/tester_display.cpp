#include "tester_display.h"

void renderBatteryUI(BatteryTester &tester) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // ১. হেডার টেক্সট (টেস্ট চলাকালীন এবং টেস্ট বন্ধ হলে পরিবর্তন হবে)
    display.setTextSize(1);
    display.setCursor(0, 0);
    if(tester.getStatus()) {
        display.print(F(" [ TESTING BATTERY ]"));
    } else {
        display.print(F(" [ TEST COMPLETED ]"));
    }
    display.drawLine(0, 10, 127, 10, SSD1306_WHITE);

    // ২. লাইভ ভোল্টেজ এবং কারেন্ট
    display.setCursor(0, 15);
    display.print(F("V: "));
    display.print(tester.getVoltage(), 2);
    display.print(F("V  "));

    display.print(F("I: "));
    display.print(tester.getCurrentmA(), 0);
    display.print(F("mA"));

    // ৩. ক্যাপাসিটি (mAh) বড় ফন্টে
    display.setCursor(0, 27);
    display.print(F("Cap: "));
    display.setTextSize(2);
    display.print((uint16_t)tester.getCapacitymAh());
    display.setTextSize(1);
    display.print(F(" mAh"));

    // ৪. এনার্জি (Wh)
    display.setCursor(0, 46);
    display.print(F("Wh : "));
    display.print(tester.getEnergyWh(), 2);
    display.print(F(" Wh"));

    // ৫. অতিক্রান্ত সময় (লাইভ ঘড়ি এবং ফ্রিজ টাইম)
    uint32_t totalSec = tester.getElapsedTimeSeconds();
    uint8_t hrs = totalSec / 3600;
    uint8_t mins = (totalSec % 3600) / 60;
    uint8_t secs = totalSec % 60;

    display.setCursor(0, 56);
    display.print(F("Time: "));
    if (hrs < 10) display.print(F("0"));
    display.print(hrs);
    display.print(F(":"));
    if (mins < 10) display.print(F("0"));
    display.print(mins);
    display.print(F(":"));
    if (secs < 10) display.print(F("0"));
    display.print(secs);

    display.display();
}