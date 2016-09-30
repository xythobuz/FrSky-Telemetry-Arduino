/*
 * FrSky Telemetry Display for Arduino
 *
 * Main Logic
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */
#include "i2c.h"
#include "oled.h"
#include "frsky.h"
#include "logo.h"
#include "config.h"

// Using TimerOne out of lazyness to allow simple fast LED PWM on any pin
// http://playground.arduino.cc/Code/Timer1
#include <TimerOne.h>

// Hardware configuration
#define LED_OUTPUT 3
#define BEEPER_OUTPUT 4
#define S1_INPUT 8
#define S2_INPUT 9
#define BAUDRATE 9600
#define LED_PWM 25
#define TIMER_FREQUENCY 50

// How to calculate battery voltage from analog value (a1 or a2)
// Voltage is stored with factor 100, so 430 -> 4.30V
#define BATTERY_ANALOG a2
#define BATTERY_VALUE_MIN 207
#define BATTERY_VALUE_MAX 254
#define BATTERY_VOLTAGE_MIN 350
#define BATTERY_VOLTAGE_MAX 430

// How often to refresh display (in ms)
#define DISPLAY_MAX_UPDATE_RATE 100

// How long until logo is shown when no data is received (in ms)
#define DISPLAY_REVERT_LOGO_TIME 2500
#define DISPLAY_SHOW_INFO_SCREEN 1500

// When to sound the voltage alarm. Don't warn below MIN_WARN_LEVEL.
#define BATTERY_MIN_WARN_LEVEL 100
#define BATTERY_LOW_WARN_LEVEL 327
#define BATTERY_HIGH_WARN_LEVEL 318

// Time for beeps for different alarm levels (in ms)
#define BATTERY_LOW_WARN_OFF 250
#define BATTERY_LOW_WARN_ON 100
#define BATTERY_HIGH_WARN_OFF 100
#define BATTERY_HIGH_WARN_ON 200

// Frequency for different buzzer sounds (in hz)
#define INIT_FREQ 440
#define BATTERY_LOW_FREQ 500
#define BATTERY_HIGH_FREQ 1500

// Software debounce time for switches (in ms)
#define DEBOUNCE_DELAY 50

// How much you're able to change the alarm/warn voltage in the menu
#define MENU_ALARM_INC 1
#define MENU_ALARM_MIN 310
#define MENU_ALARM_MAX 335

#define BEEPER_STATE_OFF 0
#define BEEPER_STATE_LOW 1
#define BEEPER_STATE_HIGH 2

#define MENU_NONE 0
#define MENU_NEXT 1
#define MENU_OK 2

#define MENU_STATE_NONE 0
#define MENU_STATE_MAIN 1
#define MENU_STATE_WARNING 2
#define MENU_STATE_ALARM 3

FrSky frsky(&Serial);
uint8_t showingLogo = 0;
uint8_t redrawScreen = 0;
int16_t rssiRx = 0;
int16_t rssiTx = 0;
int16_t voltageBattery = 0;
uint8_t analog1 = 0;
uint8_t analog2 = 0;
String userDataString = "";
uint8_t beeperState = BEEPER_STATE_OFF;
uint16_t currentBeepTime = 0;
volatile uint8_t ledValue = 0;
int16_t warningVoltage = BATTERY_LOW_WARN_LEVEL;
int16_t alarmVoltage = BATTERY_HIGH_WARN_LEVEL;

void setLED(uint8_t v) {
    ledValue = v;
}

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
            setLED(LED_PWM);
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
            setLED(LED_PWM);
        } else if ((beepState != 0) && ((millis() - lastBeeperTime) >= BATTERY_HIGH_WARN_ON)) {
            // ...turn beeper off if it was on long enough
            noTone(BEEPER_OUTPUT);
            beepState = 0;
            lastBeeperTime = millis();
            setLED(0);
        }
    }
}

uint8_t drawMainMenu(uint8_t input) {
    static uint8_t index = 0;

    if (input == MENU_NEXT) {
        if (index == 0) {
            index = 1;
        } else if (index == 1) {
            index = 2;
        } else {
            index = 0;
        }
    } else if (input == MENU_OK) {
        uint8_t r = index + 1;
        index = 0;
        return r;
    }

    clear_display();
    writeLine(1, "FrSky Telemetry");
    writeLine(2, "Configuration:");
    writeLine(4, ((index == 0) ? String("*") : String(" ")) + " Warning Volt");
    writeLine(5, ((index == 1) ? String("*") : String(" ")) + " Alarm Volt");
    writeLine(6, ((index == 2) ? String("*") : String(" ")) + " Exit Menu");

    return 0;
}

uint8_t drawWarningMenu(uint8_t input) {
    static int16_t startValue = 0;

    if (input == MENU_NEXT) {
        if (warningVoltage < MENU_ALARM_MAX) {
            warningVoltage += MENU_ALARM_INC;
        } else {
            warningVoltage = MENU_ALARM_MIN;
        }
    } else if (input == MENU_OK) {
        if (warningVoltage != startValue) {
            writeConfig();
        }
        return 1;
    } else if (input == MENU_NONE) {
        startValue = warningVoltage;
    }

    clear_display();
    writeLine(1, "FrSky Telemetry");
    writeLine(2, "Warning Volt:");
    writeLine(5, voltageToString(warningVoltage));
    return 0;
}

uint8_t drawAlarmMenu(uint8_t input) {
    static int16_t startValue = 0;

    if (input == MENU_NEXT) {
        if (alarmVoltage < MENU_ALARM_MAX) {
            alarmVoltage += MENU_ALARM_INC;
        } else {
            alarmVoltage = MENU_ALARM_MIN;
        }
    } else if (input == MENU_OK) {
        if (alarmVoltage != startValue) {
            writeConfig();
        }
        return 1;
    } else if (input == MENU_NONE) {
        startValue = alarmVoltage;
    }

    clear_display();
    writeLine(1, "FrSky Telemetry");
    writeLine(2, "Alarm Volt:");
    writeLine(5, voltageToString(alarmVoltage));
    return 0;
}

void drawMenu(uint8_t input) {
    static uint8_t state = MENU_STATE_NONE;

    if (state == MENU_STATE_NONE) {
        if (input == MENU_OK) {
            state = MENU_STATE_MAIN;
            input = MENU_NONE;
        }
    }

    if (state == MENU_STATE_MAIN) {
        uint8_t r = drawMainMenu(input);
        if ((r > 0) && (r < 3)) {
            state = (r == 1) ? MENU_STATE_WARNING : MENU_STATE_ALARM;
            input = MENU_NONE;
        } else if (r > 0) {
            state = MENU_STATE_NONE;
            drawLogo(bootLogo);
        }
    }

    if (state == MENU_STATE_WARNING) {
        if (drawWarningMenu(input)) {
            state = MENU_STATE_MAIN;
            drawMainMenu(MENU_NONE);
        }
    } else if (state == MENU_STATE_ALARM) {
        if (drawAlarmMenu(input)) {
            state = MENU_STATE_MAIN;
            drawMainMenu(MENU_NONE);
        }
    }
}

void writeLine(int l, String s) {
    setXY(l, 0);
    sendStr(s.c_str());

    // Fill rest of line with whitespace so there are no wrong stale characters
    String whitespace;
    for (int i = s.length(); i < 16; i++) {
        whitespace += ' ';
    }
    sendStr(whitespace.c_str());
}

// Print battery voltage with dot (-402 -> -4.02V)
String voltageToString(int16_t voltage) {
    String volt = String(abs(voltage) / 100);
    String fract = String(abs(voltage) % 100);
    String s;
    if (voltage < 0) {
        s += '-';
    }
    s += volt + ".";
    for (int i = 0; i < (2 - fract.length()); i++) {
        s += '0';
    }
    s += fract + "V";
    return s;
}

void drawInfoScreen(void) {
    writeLine(0, "FrSky Telemetry");
    writeLine(1, "Version: " + String(versionString));
    writeLine(2, "by xythobuz");
    writeLine(3, "Warning Volt:");
    writeLine(4, voltageToString(warningVoltage));
    writeLine(5, "Alarm Volt:");
    writeLine(6, voltageToString(alarmVoltage));
}

void dataHandler(uint8_t a1, uint8_t a2, uint8_t q1, uint8_t q2) {
    redrawScreen = 1;

    rssiRx = map(q1, 0, 255, 0, 100);
    rssiTx = map(q2, 0, 255, 0, 100);
    voltageBattery = map(BATTERY_ANALOG, BATTERY_VALUE_MIN, BATTERY_VALUE_MAX,
            BATTERY_VOLTAGE_MIN, BATTERY_VOLTAGE_MAX);
    analog1 = a1;
    analog2 = a2;
}

void alarmThresholdHandler(FrSky::AlarmThreshold alarm) {
}

void userDataHandler(const uint8_t* buf, uint8_t len) {
    redrawScreen = 1;

    String s;
    for (int i = 0; i < len; i++) {
        s += buf[i];
    }
    userDataString = s;
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

void setup(void) {
    delay(200);

    pinMode(BEEPER_OUTPUT, OUTPUT);
    pinMode(LED_OUTPUT, OUTPUT);
    pinMode(S1_INPUT, INPUT);
    pinMode(S2_INPUT, INPUT);
    digitalWrite(S1_INPUT, HIGH);
    digitalWrite(S2_INPUT, HIGH);
    tone(BEEPER_OUTPUT, INIT_FREQ);

    Timer1.initialize();
    Timer1.attachInterrupt(timerInterrupt, TIMER_FREQUENCY);
    setLED(LED_PWM);

    Serial.begin(BAUDRATE);
    i2c_init();
    i2c_OLED_init();

    clear_display();
    delay(50);
    i2c_OLED_send_cmd(0x20);
    i2c_OLED_send_cmd(0x02);
    i2c_OLED_send_cmd(0xA6);

    readConfig();
    drawInfoScreen();
    noTone(BEEPER_OUTPUT);
    setLED(0);
    delay(DISPLAY_SHOW_INFO_SCREEN);

    drawLogo(bootLogo);
    showingLogo = 1;

    frsky.setDataHandler(&dataHandler);
    frsky.setAlarmThresholdHandler(&alarmThresholdHandler);
    frsky.setUserDataHandler(&userDataHandler);

    setLED(LED_PWM);
    tone(BEEPER_OUTPUT, INIT_FREQ);
    delay(100);
    noTone(BEEPER_OUTPUT);
    setLED(0);
}

class Debouncer {
  public:
    Debouncer(int p) : pin(p), currentState(0), lastState(0), lastTime(0) { }

    int poll() {
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

  private:
    int pin;
    int currentState;
    int lastState;
    unsigned long lastTime;
};

void loop(void) {
    frsky.poll();
    beeperTask();

    if (!showingLogo) {
        // Enable battery alarm beeper as required
        if (voltageBattery > BATTERY_MIN_WARN_LEVEL) {
            if (voltageBattery > warningVoltage) {
                setBeeper(BEEPER_STATE_OFF);
            } else if (voltageBattery > alarmVoltage) {
                setBeeper(BEEPER_STATE_LOW);
            } else {
                setBeeper(BEEPER_STATE_HIGH);
            }
        } else {
            setBeeper(BEEPER_STATE_OFF);
        }
    }

    static unsigned long lastTime = 0;
    if (redrawScreen && ((millis() - lastTime) > DISPLAY_MAX_UPDATE_RATE)) {
        redrawScreen = 0;
        lastTime = millis();

        // Clear the display removing the logo on first received packet
        if (showingLogo) {
            showingLogo = 0;
            clear_display();
        }

        writeLine(0, "RSSI Rx: " + String(rssiRx) + "%");
        writeLine(1, "RSSI Tx: " + String(rssiTx) + "%");
        writeLine(2, "A1 Value: " + String(analog1));
        writeLine(3, "A2 Value: " + String(analog2));
        writeLine(4, "User Data:");
        writeLine(5, userDataString);
        writeLine(6, "Battery Volt:");
        writeLine(7, voltageToString(voltageBattery));
    } else if ((!redrawScreen) && ((millis() - lastTime) > DISPLAY_REVERT_LOGO_TIME) && (!showingLogo)) {
        // Show the logo again if nothing has been received for a while
        drawLogo(bootLogo);
        showingLogo = 2;

        // Beep once when we lost the connection
        setBeeper(BEEPER_STATE_HIGH);
        lastTime = millis();
    } else if ((showingLogo == 2) && ((millis() - lastTime) > BATTERY_HIGH_WARN_ON)) {
        // Turn beeper off again after losing connection
        setBeeper(BEEPER_STATE_OFF);
        showingLogo = 1;
    } else if (showingLogo && (!redrawScreen)) {
        // Only handle menu inputs when we're in 'idle' mode...
        static Debouncer s1(S1_INPUT);
        if (s1.poll()) {
            drawMenu(MENU_NEXT);
        }

        static Debouncer s2(S2_INPUT);
        if (s2.poll()) {
            drawMenu(MENU_OK);
        }
    }
}

