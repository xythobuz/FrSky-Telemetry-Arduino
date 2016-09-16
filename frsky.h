/*
 * FrSky Telemetry Protocol Host implementation.
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * Based on the FrSky Telemetry Protocol documentation:
 * http://www.frsky-rc.com/download/down.php?id=128
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, version 2.
 */

#ifndef __FRSKY_H__
#define __FRSKY_H__

#include <Arduino.h>

class FrSky {
  public:
    enum AnalogValue {
        analog1_1 = 0,
        analog1_2 = 1,
        analog2_1 = 2,
        analog2_2 = 3
    };

    enum GreaterLessThan {
        less = 0,
        greater = 1
    };

    enum AlarmLevel {
        disable = 0,
        yellow = 1,
        orange = 2,
        red = 3
    };

    struct AlarmThreshold {
        AlarmThreshold(AnalogValue i, GreaterLessThan d, AlarmLevel l, uint8_t v)
                : id(i), dir(d), level(l), value(v) { }

        AnalogValue id;
        GreaterLessThan dir;
        AlarmLevel level;
        uint8_t value;
    };

    typedef void (*DataHandler)(uint8_t a1, uint8_t a2, uint8_t q1, uint8_t q2);
    typedef void (*AlarmThresholdHandler)(AlarmThreshold alarm);
    typedef void (*UserDataHandler)(const uint8_t* buf, uint8_t len);

    // -------------------------------------------------------------------------------

    FrSky(Stream* s);

    void poll();
    void pollAlarms();
    void setAlarm(AlarmThreshold alarm);

    void setDataHandler(DataHandler h) { dataHandler = h; }
    void setAlarmThresholdHandler(AlarmThresholdHandler h) { alarmThresholdHandler = h; }
    void setUserDataHandler(UserDataHandler h) { userDataHandler = h; }

  private:
    void handleMessage();
    void writeEscaped(uint8_t v);

    const static uint8_t bufferSize = 19;
    const static uint8_t userDataSize = 6;
    const static uint8_t minPacketSize = 11;
    const static uint8_t delimiter = 0x7E;
    const static uint8_t escape = 0x7D;
    const static uint8_t key = 0x20;
    const static uint8_t alarms = 4;
    const static uint8_t idVoltageQuality = 0xFE;
    const static uint8_t idUserData = 0xFD;
    const static uint8_t idAlarm0 = 0xFC;
    const static uint8_t idAlarm1 = 0xFB;
    const static uint8_t idAlarm2 = 0xFA;
    const static uint8_t idAlarm3 = 0xF9;
    const static uint8_t idGetAlarms = 0xF8;

    Stream* serial;
    DataHandler dataHandler;
    AlarmThresholdHandler alarmThresholdHandler;
    UserDataHandler userDataHandler;

    uint8_t userData[userDataSize];
    uint8_t buffer[bufferSize];
    uint8_t bufferIndex;
};

#endif // __FRSKY_H__

