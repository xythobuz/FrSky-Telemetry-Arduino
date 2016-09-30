/*
 * FrSky Telemetry Display for Arduino
 *
 * Pushbutton debouncing helper class
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
#include "debounce.h"
#include "options.h"

Debouncer::Debouncer(int p) : pin(p), currentState(0), lastState(0), lastTime(0) { }

int Debouncer::poll() {
    int ret = 0;
    int state = digitalRead(pin);

    if (state != lastState) {
        lastTime = millis();
    }

    if ((millis() - lastTime) > DEBOUNCE_DELAY) {
        if (state != currentState) {
            currentState = state;
            if (currentState == LOW) {
                ret = 1;
            }
        }
    }

    lastState = state;
    return ret;
}

