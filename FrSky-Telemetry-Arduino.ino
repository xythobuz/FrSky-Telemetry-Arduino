/*
 * FrSky Telemetry Display for Arduino
 *
 * Main Logic
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */
#include "i2c.h"
#include "oled.h"
#include "frsky.h"
#include "logo.h"
#include "config.h"
#include "beeper.h"
#include "options.h"
#include "debounce.h"
#include "led.h"
#include "menu.h"

FrSky frsky(&Serial);
uint8_t showingLogo = 0;
uint8_t redrawScreen = 0;
int16_t rssiRx = 0;
int16_t rssiTx = 0;
int16_t voltageBattery = 0;
uint8_t analog1 = 0;
uint8_t analog2 = 0;
String userDataString = "";
int16_t noWarnVoltage = BATTERY_MIN_WARN_LEVEL;
int16_t warningVoltage = BATTERY_LOW_WARN_LEVEL;
int16_t alarmVoltage = BATTERY_HIGH_WARN_LEVEL;
uint8_t ledBrightness = LED_PWM;

// Print battery voltage with dot (-402 -> -4.02V)
String voltageToString(int16_t voltage) {
    String volt = String(abs(voltage) / 100);
    String fract = String(abs(voltage) % 100);
    String s;
    if (voltage < 0) {
        s += '-';
    }
    s += volt + ".";
    for (int i = 0; i < (2 - fract.length()); i++) {
        s += '0';
    }
    s += fract + "V";
    return s;
}

void drawInfoScreen(void) {
    writeLine(0, "FrSky Telemetry");
    writeLine(1, "Version: " + String(versionString));
    writeLine(2, "Patch Level: " + String(PATCH_LEVEL_STRING));
    writeLine(3, "by xythobuz.de");
    writeLine(4, "Warning Volt:");
    writeLine(5, voltageToString(warningVoltage));
    writeLine(6, "Alarm Volt:");
    writeLine(7, voltageToString(alarmVoltage));
}

void dataHandler(uint8_t a1, uint8_t a2, uint8_t q1, uint8_t q2) {
    redrawScreen = 1;

    rssiRx = map(q1, 0, 255, 0, 100);
    rssiTx = map(q2, 0, 255, 0, 100);
    voltageBattery = map(BATTERY_ANALOG, BATTERY_VALUE_MIN, BATTERY_VALUE_MAX,
            BATTERY_VOLTAGE_MIN, BATTERY_VOLTAGE_MAX);
    analog1 = a1;
    analog2 = a2;
}

void alarmThresholdHandler(FrSky::AlarmThreshold alarm) {
}

void userDataHandler(const uint8_t* buf, uint8_t len) {
    redrawScreen = 1;

    String s;
    for (int i = 0; i < len; i++) {
        s += buf[i];
    }
    userDataString = s;
}

void setup(void) {
    delay(200);

    pinMode(BEEPER_OUTPUT, OUTPUT);
    pinMode(LED_OUTPUT, OUTPUT);
    pinMode(S1_INPUT, INPUT);
    pinMode(S2_INPUT, INPUT);
    digitalWrite(S1_INPUT, HIGH);
    digitalWrite(S2_INPUT, HIGH);
    tone(BEEPER_OUTPUT, INIT_FREQ);

    initLED();
    setLED(ledBrightness);

    Serial.begin(BAUDRATE);
    i2c_init();
    i2c_OLED_init();

    clear_display();
    delay(50);
    i2c_OLED_send_cmd(0x20);
    i2c_OLED_send_cmd(0x02);
    i2c_OLED_send_cmd(0xA6);

    readConfig();
    drawInfoScreen();
    noTone(BEEPER_OUTPUT);
    setLED(0);
    delay(DISPLAY_SHOW_INFO_SCREEN);

    drawLogo(bootLogo);
    showingLogo = 1;

    frsky.setDataHandler(&dataHandler);
    frsky.setAlarmThresholdHandler(&alarmThresholdHandler);
    frsky.setUserDataHandler(&userDataHandler);

    setLED(ledBrightness);
    tone(BEEPER_OUTPUT, INIT_FREQ);
    delay(100);
    noTone(BEEPER_OUTPUT);
    setLED(0);
}

void loop(void) {
    frsky.poll();
    beeperTask();

    if (!showingLogo) {
        // Enable battery alarm beeper as required
        if (voltageBattery > noWarnVoltage) {
            if (voltageBattery > warningVoltage) {
                setBeeper(BEEPER_STATE_OFF);
            } else if (voltageBattery > alarmVoltage) {
                setBeeper(BEEPER_STATE_LOW);
            } else {
                setBeeper(BEEPER_STATE_HIGH);
            }
        } else {
            setBeeper(BEEPER_STATE_OFF);
        }
    }

    static unsigned long lastTime = 0;
    if (redrawScreen && ((millis() - lastTime) > DISPLAY_MAX_UPDATE_RATE)) {
        redrawScreen = 0;
        lastTime = millis();

        // Clear the display removing the logo on first received packet
        if (showingLogo) {
            showingLogo = 0;
            clear_display();
        }

        writeLine(0, "RSSI Rx: " + String(rssiRx) + "%");
        writeLine(1, "RSSI Tx: " + String(rssiTx) + "%");
        writeLine(2, "A1 Value: " + String(analog1));
        writeLine(3, "A2 Value: " + String(analog2));
        writeLine(4, "User Data:");
        writeLine(5, userDataString);
        writeLine(6, "Battery Volt:");
        writeLine(7, voltageToString(voltageBattery));
    } else if ((!redrawScreen) && ((millis() - lastTime) > DISPLAY_REVERT_LOGO_TIME) && (!showingLogo)) {
        // Show the logo again if nothing has been received for a while
        drawLogo(bootLogo);
        showingLogo = 2;

        // Beep twice when we lost the connection
        setBeeper(BEEPER_STATE_LOW);
        lastTime = millis();
    } else if ((showingLogo == 2) && ((millis() - lastTime) > BATTERY_LOW_WARN_ON)) {
        showingLogo = 3;
        setBeeper(BEEPER_STATE_HIGH);
    } else if ((showingLogo == 3) && ((millis() - lastTime) > BATTERY_HIGH_WARN_ON)) {
        // Turn beeper off again after losing connection
        setBeeper(BEEPER_STATE_OFF);
        showingLogo = 1;
    } else if (showingLogo && (!redrawScreen)) {
        // Only handle menu inputs when we're in 'idle' mode...
        static Debouncer s1(S1_INPUT);
        if (s1.poll()) {
            drawMenu(MENU_NEXT);
        }

        static Debouncer s2(S2_INPUT);
        if (s2.poll()) {
            drawMenu(MENU_OK);
        }
    }
}

