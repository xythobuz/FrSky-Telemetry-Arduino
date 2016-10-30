/*
 * FrSky Telemetry Display for Arduino
 *
 * EEPROM Config Storage
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "options.h"

#define CONFIG_STRING_LENGTH 6
const char versionString[CONFIG_STRING_LENGTH] = "1.2.0";

// Shown on splash screen but not stored in EEPROM
#define PATCH_LEVEL_STRING "02"

struct ConfigData {
    uint8_t modelCount;
    int16_t noWarnVoltage[MODEL_COUNT];
    int16_t warningVoltage[MODEL_COUNT];
    int16_t alarmVoltage[MODEL_COUNT];
    uint8_t ledBrightness;
    char versionString[CONFIG_STRING_LENGTH];
};

#define CONFIG_DATA_LENGTH (sizeof(ConfigData))

// Return 0 on ok, 1 on checksum error, 2 on version error, 3 on model count error
uint8_t readConfig(void);

void writeConfig(void);

#endif

