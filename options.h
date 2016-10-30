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

// GPIO configuration
#define LED_OUTPUT 3
#define BEEPER_OUTPUT 4
#define S1_INPUT 8
#define S2_INPUT 9

// FrSky Telemetry Baudrate
#define BAUDRATE 9600

// Default LED PWM Timer values
#define LED_PWM 15
#define TIMER_FREQUENCY 50

// How often to refresh display at most (in ms)
#define DISPLAY_MAX_UPDATE_RATE 10

// How long until logo is shown when no data is received (in ms)
#define DISPLAY_REVERT_LOGO_TIME 2500

// How long the boot splash screen is shown
#define DISPLAY_SHOW_INFO_SCREEN 1500

// ---------------------------------------------------------
// !!! Change all these when adding/removing a new model !!!
// ---------------------------------------------------------

// Number of models that can be selected
#define MODEL_COUNT 2

// Model Names, have to fit screen (15 chars)
const static String modelName[MODEL_COUNT] = {
    "Micro Copter",
    "Flying Wing"
};

// Which analog port (1 or 2) to use
const static uint8_t batteryAnalogs[MODEL_COUNT] = {
    2,
    2
};

// Analog Sample Min Value
const static int16_t batteryValuesMin[MODEL_COUNT] = {
    231,
    169
};

// Battery Voltage when Min Value has been reached
// Voltage is stored with factor 100, so 388 -> 3.88V
const static int16_t batteryVoltagesMin[MODEL_COUNT] = {
    388,
    900
};

// Analog Sample Max Value
const static int16_t batteryValuesMax[MODEL_COUNT] = {
    250,
    239
};

// Battery Voltage when Max Value has been reached
const static int16_t batteryVoltagesMax[MODEL_COUNT] = {
    417,
    1260
};

// Voltage below which no warning will be given
const static int16_t batteryMinWarnLevel[MODEL_COUNT] = {
    100,
    500
};

// Voltage for warning
const static int16_t batteryLowWarnLevel[MODEL_COUNT] = {
    327,
    1054
};

// Voltage for alarm
const static int16_t batteryHighWarnLevel[MODEL_COUNT] = {
    322,
    1033
};

// How much you're able to change the alarm/warn voltage in the menu
const static int16_t menuOptionIncrease[MODEL_COUNT] = {
    1,
    3
};

// Minimum alarm voltage in the menu
const static int16_t menuOptionMinimum[MODEL_COUNT] = {
    315,
    1009
};

// Maximum alarm voltage in the menu
const static int16_t menuOptionMaximum[MODEL_COUNT] = {
    335,
    1075
};

// ---------------------------------------
// !!! Model specific options end here !!!
// ---------------------------------------

#define MENU_NOWARN_INC 10
#define MENU_NOWARN_MIN 100
#define MENU_NOWARN_MAX 500

#define MENU_BRIGHT_INC 1
#define MENU_BRIGHT_MIN 1
#define MENU_BRIGHT_MAX 25

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

#endif

