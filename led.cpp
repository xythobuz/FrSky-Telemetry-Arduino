/*
 * FrSky Telemetry Display for Arduino
 *
 * LED PWM
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
#include "led.h"
#include "options.h"

// Using TimerOne out of lazyness to allow simple fast LED PWM on any pin
// http://playground.arduino.cc/Code/Timer1
#include <TimerOne.h>

volatile uint8_t ledValue = 0;

void setLED(uint8_t v) {
    ledValue = v;
}

void timerInterrupt(void) {
    static uint8_t count = 0;
    static uint8_t ledState = 0;
    if (ledState || (ledValue == 0)) {
        ledState = 0;
    } else {
        count++;
        if (count >= ledValue) {
            count = 0;
            ledState ^= 1;
        }
    }
    digitalWrite(LED_OUTPUT, ledState ? HIGH : LOW);
}

void initLED(void) {
    Timer1.initialize();
    Timer1.attachInterrupt(timerInterrupt, TIMER_FREQUENCY);
}

