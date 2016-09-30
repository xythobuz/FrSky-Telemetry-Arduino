/*
 * FrSky Telemetry Display for Arduino
 *
 * Beeper handling utility
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
#include "beeper.h"
#include "options.h"
#include "led.h"

extern uint8_t ledBrightness;

uint8_t beeperState = BEEPER_STATE_OFF;
uint16_t currentBeepTime = 0;

void setBeeper(uint8_t state) {
    if (state > BEEPER_STATE_HIGH) {
        return;
    }

    beeperState = state;

    if (state == BEEPER_STATE_LOW) {
        currentBeepTime = BATTERY_LOW_WARN_ON;
    } else if (state == BEEPER_STATE_HIGH) {
        currentBeepTime = BATTERY_HIGH_WARN_ON;
    }
}

void beeperTask(void) {
    static unsigned long lastBeeperTime = 0;
    static uint8_t beepState = 0;

    if ((beeperState == BEEPER_STATE_OFF) && (beepState != 0)
            && ((millis() - lastBeeperTime) >= currentBeepTime)) {
        // in STATE_OFF, turn beeper off only after the last beep has finished
        noTone(BEEPER_OUTPUT);
        beepState = 0;
        lastBeeperTime = millis();
        setLED(0);
    } else if (beeperState == BEEPER_STATE_LOW) {
        // in STATE_LOW...
        if ((beepState == 0) && ((millis() - lastBeeperTime) >= BATTERY_LOW_WARN_OFF)) {
            // ...turn beeper on if it was off long enough
            tone(BEEPER_OUTPUT, BATTERY_LOW_FREQ);
            beepState = 1;
            lastBeeperTime = millis();
            setLED(ledBrightness);
        } else if ((beepState != 0) && ((millis() - lastBeeperTime) >= BATTERY_LOW_WARN_ON)) {
            // ...turn beeper off if it was on long enough
            noTone(BEEPER_OUTPUT);
            beepState = 0;
            lastBeeperTime = millis();
            setLED(0);
        }
    } else if (beeperState == BEEPER_STATE_HIGH) {
        // in STATE_HIGH...
        if ((beepState == 0) && ((millis() - lastBeeperTime) >= BATTERY_HIGH_WARN_OFF)) {
            // ...turn beeper on if it was off long enough
            tone(BEEPER_OUTPUT, BATTERY_HIGH_FREQ);
            beepState = 1;
            lastBeeperTime = millis();
            setLED(ledBrightness);
        } else if ((beepState != 0) && ((millis() - lastBeeperTime) >= BATTERY_HIGH_WARN_ON)) {
            // ...turn beeper off if it was on long enough
            noTone(BEEPER_OUTPUT);
            beepState = 0;
            lastBeeperTime = millis();
            setLED(0);
        }
    }
}

