#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

// System States (ডিভাইসের বর্তমান অবস্থা ট্র্যাক করার জন্য)
enum SystemState {
    STATE_MAIN_MENU,   // মেইন মেনু স্ক্রিন
    STATE_SUB_MENU,    // [START] ও [BACK] অপশন স্ক্রিন
    STATE_RUNNING      // টেস্ট চলাকালীন স্ক্রিন
};

// Sub-Menu Options (ফিচারের ভেতরে নির্বাচন করার অপশন)
enum SubMenuOption {
    OPTION_START = 0,
    OPTION_BACK = 1
};

void initMenu();
void menuUp();
void menuDown();
byte getSelectedMenu();
const char* getSelectedMenuName();

// State Control Functions
SystemState getCurrentState();
void setCurrentState(SystemState state);
SubMenuOption getSubMenuSelection();
void toggleSubMenuSelection();

// Screen Drawing Functions
void showMainMenu();
void showSubMenu(const char* title);

#endif