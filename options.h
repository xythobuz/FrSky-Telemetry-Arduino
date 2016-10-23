/*
 * FrSky Telemetry Display for Arduino
 *
 * Configuration options
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */
#ifndef __OPTIONS_H__
#define __OPTIONS_H__

// Hardware configuration
#define LED_OUTPUT 3
#define BEEPER_OUTPUT 4
#define S1_INPUT 8
#define S2_INPUT 9

// FrSky Telemetry Baudrate
#define BAUDRATE 9600

// Default LED PWM Timer values
#define LED_PWM 15
#define TIMER_FREQUENCY 50

// Number of models that can be selected
#define MODEL_COUNT 2

// Names have to fit screen (15 chars)
const static String modelName[MODEL_COUNT] = {
    "Micro Copter",
    "Flying Wing"
};

// How to calculate battery voltage from analog value (a1 or a2)
// Voltage is stored with factor 100, so 430 -> 4.30V
const static uint8_t batteryAnalogs[MODEL_COUNT] = {
    2, 2
};
const static int16_t batteryValuesMin[MODEL_COUNT] = {
    231, 169
};
const static int16_t batteryVoltagesMin[MODEL_COUNT] = {
    388, 900
};
const static int16_t batteryValuesMax[MODEL_COUNT] = {
    250, 239
};
const static int16_t batteryVoltagesMax[MODEL_COUNT] = {
    417, 1260
};

// How often to refresh display at most (in ms)
#define DISPLAY_MAX_UPDATE_RATE 10

// How long until logo is shown when no data is received (in ms)
#define DISPLAY_REVERT_LOGO_TIME 2500
#define DISPLAY_SHOW_INFO_SCREEN 1500

// When to sound the voltage alarm. Don't warn below batteryMinWarnLevel.
const static int16_t batteryMinWarnLevel[MODEL_COUNT] = {
    100, 500
};
const static int16_t batteryLowWarnLevel[MODEL_COUNT] = {
    327, 1050
};
const static int16_t batteryHighWarnLevel[MODEL_COUNT] = {
    322, 1030
};

// Time for beeps for different alarm levels (in ms)
#define BATTERY_LOW_WARN_OFF 250
#define BATTERY_LOW_WARN_ON 100
#define BATTERY_HIGH_WARN_OFF 100
#define BATTERY_HIGH_WARN_ON 200

// Low pass filter for analog voltages (higher = more filtering)
#define BATTERY_FILTER_BETA 2

// Frequency for different buzzer sounds (in hz)
#define INIT_FREQ 440
#define BATTERY_LOW_FREQ 500
#define BATTERY_HIGH_FREQ 1500

// Software debounce time for switches (in ms)
#define DEBOUNCE_DELAY 50

// How much you're able to change the alarm/warn voltage in the menu
#define MENU_ALARM_INC 1
#define MENU_ALARM_MIN 315
#define MENU_ALARM_MAX 335
#define MENU_NOWARN_INC 10
#define MENU_NOWARN_MIN 100
#define MENU_NOWARN_MAX 300

#define MENU_BRIGHT_INC 1
#define MENU_BRIGHT_MIN 1
#define MENU_BRIGHT_MAX 25

#endif

