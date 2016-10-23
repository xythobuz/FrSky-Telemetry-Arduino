/*
 * FrSky Telemetry Display for Arduino
 *
 * On Screen Menu
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */
#include <Arduino.h>
#include "menu.h"
#include "oled.h"
#include "options.h"
#include "config.h"
#include "logo.h"

#define MENU_STATE_NONE 0
#define MENU_STATE_MAIN 1
#define MENU_STATE_NOWARN 2
#define MENU_STATE_WARNING 3
#define MENU_STATE_ALARM 4
#define MENU_STATE_BRIGHT 5
#define MENU_STATES 6

extern int16_t noWarnVoltage[MODEL_COUNT];
extern int16_t warningVoltage[MODEL_COUNT];
extern int16_t alarmVoltage[MODEL_COUNT];
extern uint8_t ledBrightness;
extern uint8_t currentModel;

String voltageToString(int16_t voltage);

uint8_t drawMainMenu(uint8_t input) {
    static uint8_t index = 0;

    if (input == MENU_NEXT) {
        if (index < (MENU_STATES - 2)) {
            index++;
        } else {
            index = 0;
        }
    } else if (input == MENU_OK) {
        uint8_t r = index + 1;
        index = 0;
        return r;
    }

    clear_display();
    writeLine(1, "FrSky Telemetry");
    writeLine(2, "Configuration:");
    writeLine(3, ((index == 0) ? String("*") : String(" ")) + " No Warn Volt");
    writeLine(4, ((index == 1) ? String("*") : String(" ")) + " Warning Volt");
    writeLine(5, ((index == 2) ? String("*") : String(" ")) + " Alarm Volt");
    writeLine(6, ((index == 3) ? String("*") : String(" ")) + " LED Brightness");
    writeLine(7, ((index == 4) ? String("*") : String(" ")) + " Exit Menu");

    return 0;
}

uint8_t drawNoWarnMenu(uint8_t input) {
    static int16_t startValue = 0;

    if (input == MENU_NEXT) {
        if (noWarnVoltage[currentModel] < MENU_NOWARN_MAX) {
            noWarnVoltage[currentModel] += MENU_NOWARN_INC;
        } else {
            noWarnVoltage[currentModel] = MENU_NOWARN_MIN;
        }
    } else if (input == MENU_OK) {
        if (noWarnVoltage[currentModel] != startValue) {
            writeConfig();
        }
        return 1;
    } else if (input == MENU_NONE) {
        startValue = noWarnVoltage[currentModel];
    }

    clear_display();
    writeLine(1, "FrSky Telemetry");
    writeLine(2, "No-Warn Volt:");
    writeLine(5, voltageToString(noWarnVoltage[currentModel]));
    return 0;
}

uint8_t drawWarningMenu(uint8_t input) {
    static int16_t startValue = 0;

    if (input == MENU_NEXT) {
        if (warningVoltage[currentModel] < MENU_ALARM_MAX) {
            warningVoltage[currentModel] += MENU_ALARM_INC;
        } else {
            warningVoltage[currentModel] = MENU_ALARM_MIN;
        }
    } else if (input == MENU_OK) {
        if (warningVoltage[currentModel] != startValue) {
            writeConfig();
        }
        return 1;
    } else if (input == MENU_NONE) {
        startValue = warningVoltage[currentModel];
    }

    clear_display();
    writeLine(1, "FrSky Telemetry");
    writeLine(2, "Warning Volt:");
    writeLine(5, voltageToString(warningVoltage[currentModel]));
    return 0;
}

uint8_t drawAlarmMenu(uint8_t input) {
    static int16_t startValue = 0;

    if (input == MENU_NEXT) {
        if (alarmVoltage[currentModel] < MENU_ALARM_MAX) {
            alarmVoltage[currentModel] += MENU_ALARM_INC;
        } else {
            alarmVoltage[currentModel] = MENU_ALARM_MIN;
        }
    } else if (input == MENU_OK) {
        if (alarmVoltage[currentModel] != startValue) {
            writeConfig();
        }
        return 1;
    } else if (input == MENU_NONE) {
        startValue = alarmVoltage[currentModel];
    }

    clear_display();
    writeLine(1, "FrSky Telemetry");
    writeLine(2, "Alarm Volt:");
    writeLine(5, voltageToString(alarmVoltage[currentModel]));
    return 0;
}

uint8_t drawBrightMenu(uint8_t input) {
    static uint8_t startValue = 0;

    if (input == MENU_NEXT) {
        if (ledBrightness < MENU_BRIGHT_MAX) {
            ledBrightness += MENU_BRIGHT_INC;
        } else {
            ledBrightness = MENU_BRIGHT_MIN;
        }
    } else if (input == MENU_OK) {
        if (ledBrightness != startValue) {
            writeConfig();
        }
        return 1;
    } else if (input == MENU_NONE) {
        startValue = ledBrightness;
    }

    clear_display();
    writeLine(1, "FrSky Telemetry");
    writeLine(2, "LED Brightness:");
    writeLine(5, "1 / " + String(ledBrightness));
    return 0;
}

void drawMenu(uint8_t input) {
    static uint8_t state = MENU_STATE_NONE;

    if (state == MENU_STATE_NONE) {
        // Only allow 'ok' to enter menu from logo
        if (input == MENU_OK) {
            state = MENU_STATE_MAIN;
            input = MENU_NONE;
#if defined(MODEL_COUNT) && (MODEL_COUNT > 1)
        } else if (input == MENU_NEXT) {
            // Roll through models if enabled
            if (currentModel < (MODEL_COUNT - 1)) {
                currentModel++;
            } else {
                currentModel = 0;
            }

            writeLine(2, "Model " + String(currentModel + 1) + " active");
            if (modelName[currentModel].length() > 0) {
                writeLine(3, modelName[currentModel]);
            }
#endif // defined(MODEL_COUNT) && (MODEL_COUNT > 1)
        }
    }

    if (state == MENU_STATE_MAIN) {
        // Draw Main Menu
        uint8_t r = drawMainMenu(input);
        if ((r > 0) && (r < (MENU_STATES - 1))) {
            // Enter next state but discard current input
            state = r + 1;
            input = MENU_NONE;
        } else if (r > 0) {
            // Return to logo
            state = MENU_STATE_NONE;
            drawLogo(bootLogo);
        }
    }

    if (state == MENU_STATE_WARNING) {
        // Draw submenu
        if (drawWarningMenu(input)) {
            // Return to main menu but discard current input
            state = MENU_STATE_MAIN;
            drawMainMenu(MENU_NONE);
        }
    } else if (state == MENU_STATE_ALARM) {
        if (drawAlarmMenu(input)) {
            state = MENU_STATE_MAIN;
            drawMainMenu(MENU_NONE);
        }
    } else if (state == MENU_STATE_BRIGHT) {
        if (drawBrightMenu(input)) {
            state = MENU_STATE_MAIN;
            drawMainMenu(MENU_NONE);
        }
    } else if (state == MENU_STATE_NOWARN) {
        if (drawNoWarnMenu(input)) {
            state = MENU_STATE_MAIN;
            drawMainMenu(MENU_NONE);
        }
    }
}

