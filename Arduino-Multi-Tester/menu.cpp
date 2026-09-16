//manu.cpp
#include "menu.h"
#include "config.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

extern Adafruit_SSD1306 display;

const char* menuItems[MENU_COUNT] = {
    "Battery Capacity",
    "RPM Meter",
    "PWM Generator",
    "PWM Reader"
};

static byte selectedMenu = 0;
static SystemState currentState = STATE_MAIN_MENU;
static SubMenuOption subMenuSelection = OPTION_START;

void initMenu() {
    selectedMenu = 0;
    currentState = STATE_MAIN_MENU;
    subMenuSelection = OPTION_START;
}

void menuUp() {
    if (currentState == STATE_MAIN_MENU) {
        if (selectedMenu == 0) {
            selectedMenu = MENU_COUNT - 1;
        } else {
            selectedMenu--;
        }
    } else if (currentState == STATE_SUB_MENU) {
        toggleSubMenuSelection();
    }
}

void menuDown() {
    if (currentState == STATE_MAIN_MENU) {
        selectedMenu++;
        if (selectedMenu >= MENU_COUNT) {
            selectedMenu = 0;
        }
    } else if (currentState == STATE_SUB_MENU) {
        toggleSubMenuSelection();
    }
}

byte getSelectedMenu() {
    return selectedMenu;
}

const char* getSelectedMenuName() {
    return menuItems[selectedMenu];
}

SystemState getCurrentState() {
    return currentState;
}

void setCurrentState(SystemState state) {
    currentState = state;
}

SubMenuOption getSubMenuSelection() {
    return subMenuSelection;
}

void toggleSubMenuSelection() {
    if (subMenuSelection == OPTION_START) {
        subMenuSelection = OPTION_BACK;
    } else {
        subMenuSelection = OPTION_START;
    }
}

// =====================================================
// SHOW MAIN MENU (WITH SCROLL SUPPORT)
// =====================================================
void showMainMenu() {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    // Header Title
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(DEVICE_NAME, 0, 0, &x1, &y1, &w, &h);
    display.setCursor((SCREEN_WIDTH - w) / 2, 0);
    display.println(DEVICE_NAME);

    // Divider Line
    display.drawLine(0, 9, SCREEN_WIDTH - 1, 9, SSD1306_WHITE);

    // Displaying 4 items at a time
    byte topItem = 0;
    if (selectedMenu >= 4) {
        topItem = selectedMenu - 3;
    }

    for (byte i = 0; i < 4 && (topItem + i) < MENU_COUNT; i++) {
        byte index = topItem + i;
        int y = 13 + (i * 12);

        if (index == selectedMenu) {
            display.setCursor(2, y);
            display.print(F(">"));
        }

        display.setCursor(12, y);
        display.println(menuItems[index]);
    }

    display.display();
}

// =====================================================
// SHOW SUB MENU ([START] / [BACK])
// =====================================================
void showSubMenu(const char* title) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);

    // Header Title
    display.setCursor(0, 0);
    display.println(title);
    display.drawLine(0, 10, SCREEN_WIDTH - 1, 10, SSD1306_WHITE);

    // Options Layout
    int startY = 25;
    
    // START Option Selection
    if (subMenuSelection == OPTION_START) {
        display.setCursor(10, startY);
        display.print(F("> [ START ]"));
    } else {
        display.setCursor(22, startY);
        display.print(F("  START  "));
    }

    // BACK Option Selection
    if (subMenuSelection == OPTION_BACK) {
        display.setCursor(10, startY + 16);
        display.print(F("> [ BACK ]"));
    } else {
        display.setCursor(22, startY + 16);
        display.print(F("  BACK  "));
    }

    display.display();
}