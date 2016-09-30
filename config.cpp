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
#include <Arduino.h>
#include <EEPROM.h>
#include "config.h"

extern int16_t warningVoltage;
extern int16_t alarmVoltage;

void readConfig(void) {
    ConfigData d;
    uint8_t *buffer = (uint8_t *)((void *)&d);
    uint8_t checksum = 0;
    for (int i = 0; i < CONFIG_DATA_LENGTH; i++) {
        buffer[i] = EEPROM.read(i);
        checksum ^= buffer[i];
    }

    uint8_t storedSum = EEPROM.read(CONFIG_DATA_LENGTH);
    if (storedSum == checksum) {
        uint8_t match = 1;
        for (int i = 0; i < CONFIG_STRING_LENGTH; i++) {
            if (d.versionString[i] != versionString[i]) {
                match = 0;
                break;
            }
        }

        if (match) {
            warningVoltage = d.warningVoltage;
            alarmVoltage = d.alarmVoltage;
        }
    }
}

void writeConfig(void) {
    ConfigData d;
    d.warningVoltage = warningVoltage;
    d.alarmVoltage = alarmVoltage;
    for (int i = 0; i < CONFIG_STRING_LENGTH; i++) {
        d.versionString[i] = versionString[i];
    }

    uint8_t checksum = 0;
    const uint8_t *buffer = (const uint8_t *)((const void *)&d);
    for (int i = 0; i < CONFIG_DATA_LENGTH; i++) {
        EEPROM.write(i, buffer[i]);
        checksum ^= buffer[i];
    }

    EEPROM.write(CONFIG_DATA_LENGTH, checksum);
}

