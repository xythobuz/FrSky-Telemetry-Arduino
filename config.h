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

#define CONFIG_STRING_LENGTH 6
const char versionString[CONFIG_STRING_LENGTH] = "1.1.0";

struct ConfigData {
    int16_t warningVoltage, alarmVoltage;
    char versionString[CONFIG_STRING_LENGTH];
};

#define CONFIG_DATA_LENGTH (sizeof(ConfigData))

void readConfig(void);
void writeConfig(void);

#endif

