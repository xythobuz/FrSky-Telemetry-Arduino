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

#include "frsky.h"

//#define DEBUG_OUTPUT Serial

FrSky::FrSky(Stream* s) : serial(s), dataHandler(0),
        alarmThresholdHandler(0), userDataHandler(0), bufferIndex(0) {
    for (uint8_t i = 0; i < userDataSize; i++) {
        userData[i] = 0;
    }

    for (uint8_t i = 0; i < bufferSize; i++) {
        buffer[i] = 0;
    }
}

void FrSky::poll() {
    if ((!serial) || (!serial->available())) {
        return;
    }

    uint8_t c = serial->read();
    if (c == delimiter) {
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.print("Got delimiter at ");
        DEBUG_OUTPUT.println(bufferIndex);
#endif
        if (bufferIndex < (minPacketSize - 1)) {
#ifdef DEBUG_OUTPUT
            DEBUG_OUTPUT.print("Reset to 0: ");
            DEBUG_OUTPUT.print(bufferIndex);
            DEBUG_OUTPUT.print(" ! <= ");
            DEBUG_OUTPUT.println(minPacketSize - 1);
#endif
            bufferIndex = 0;
        }
        if (bufferIndex >= bufferSize) {
#ifdef DEBUG_OUTPUT
            DEBUG_OUTPUT.print("too large: ");
            DEBUG_OUTPUT.print(bufferIndex);
            DEBUG_OUTPUT.print(" / ");
            DEBUG_OUTPUT.println(bufferSize);
#endif
            bufferIndex = bufferSize - 1;
        }
        buffer[bufferIndex++] = c;
        if (bufferIndex >= minPacketSize) {
#ifdef DEBUG_OUTPUT
            DEBUG_OUTPUT.println("Handling...");
#endif
            handleMessage();
            bufferIndex = 0;
        }
    } else if ((bufferIndex > 0) && (bufferIndex < bufferSize)) {
        buffer[bufferIndex++] = c;
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.print("Got ");
        DEBUG_OUTPUT.print(c);
        DEBUG_OUTPUT.print(" at ");
        DEBUG_OUTPUT.println(bufferIndex - 1);
#endif
    } else {
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.print("Invalid: ");
        DEBUG_OUTPUT.print(bufferIndex);
        DEBUG_OUTPUT.print(" / ");
        DEBUG_OUTPUT.println(bufferSize);
#endif
    }
}

void FrSky::pollAlarms() {
    serial->write(delimiter);
    writeEscaped(idGetAlarms);
    for (uint8_t i = 0; i < 8; i++) {
        writeEscaped(0);
    }
    serial->write(delimiter);
}

void FrSky::setAlarm(AlarmThreshold alarm) {
    uint8_t id = (alarm.id == analog1_1) ? idAlarm0
            : ((alarm.id == analog1_2) ? idAlarm1
            : ((alarm.id == analog2_1) ? idAlarm2 : idAlarm3));
    serial->write(delimiter);
    writeEscaped(id);
    writeEscaped(alarm.value);
    writeEscaped(alarm.dir);
    writeEscaped(alarm.level);
    for (uint8_t i = 0; i < 5; i++) {
        writeEscaped(0);
    }
    serial->write(delimiter);
}

void FrSky::writeEscaped(uint8_t v) {
    if ((v == delimiter) || (v == escape)) {
        v ^= key;
        serial->write(escape);
    }
    serial->write(v);
}

void FrSky::handleMessage() {
#ifdef DEBUG_OUTPUT
    DEBUG_OUTPUT.println("FrSky::handleMessage()");
    for (uint8_t i = 0; i < bufferIndex; i++) {
        DEBUG_OUTPUT.print(buffer[i], HEX);
        DEBUG_OUTPUT.print(" ");
    }
    DEBUG_OUTPUT.println();
#endif

    if ((buffer[0] != delimiter) || (buffer[bufferIndex - 1] != delimiter)) {
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.println("invalid packet begin/end!");
#endif
        return;
    }

    // Fix escaped bytes
    for (uint8_t i = 0; i < (bufferIndex - 1); i++) {
        if (buffer[i] == escape) {
            buffer[i] = buffer[i + 1] ^ key;
            for (uint8_t j = i + 1; j < (bufferIndex - 1); j++) {
                buffer[j] = buffer[j + 1];
            }
            bufferIndex--;
        }
    }

    if (buffer[1] == idVoltageQuality) {
        if (dataHandler) {
            dataHandler(buffer[2], buffer[3], buffer[4], buffer[5]);
        }
    } else if (buffer[1] == idUserData) {
        uint8_t len = buffer[2];
        if (len > userDataSize) {
            len = userDataSize;
        }
        for (uint8_t i = 0; i < len; i++) {
            userData[i] = buffer[i + 4];
        }
        if ((len > 0) && (userDataHandler)) {
            userDataHandler(userData, len);
        }
    } else if ((buffer[1] == idAlarm0) || (buffer[1] == idAlarm1)
            || (buffer[1] == idAlarm2) || (buffer[1] == idAlarm3)) {
        AnalogValue v = (buffer[1] == idAlarm0) ? analog1_1
                : ((buffer[1] == idAlarm1) ? analog1_2
                : ((buffer[1] == idAlarm2) ? analog2_1 : analog2_2));
        if (alarmThresholdHandler) {
            alarmThresholdHandler(AlarmThreshold(v, (GreaterLessThan)buffer[3],
                    (AlarmLevel)buffer[4], buffer[2]));
        }
    } else {
#ifdef DEBUG_OUTPUT
        DEBUG_OUTPUT.print("Unexpected ID: ");
        DEBUG_OUTPUT.println(buffer[1], HEX);
#endif
    }
}

