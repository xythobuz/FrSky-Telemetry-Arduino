#include "i2c.h"
#include "oled.h"
#include "frsky.h"
#include "logo.h"

FrSky frsky(&Serial);

void dataHandler(uint8_t a1, uint8_t a2, uint8_t q1, uint8_t q2);
void alarmThresholdHandler(FrSky::AlarmThreshold alarm);
void userDataHandler(const uint8_t* buf, uint8_t len);

uint8_t showingLogo = 0;
uint8_t ledState = 0;
uint8_t redrawScreen = 0;
int16_t rssiRx = 0;
int16_t rssiTx = 0;
int16_t voltageBattery = 0;
uint8_t analog1 = 0;
uint8_t analog2 = 0;

void setup(void) {
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  Serial.begin(9600);
  i2c_init();
  i2c_OLED_init();

  clear_display();
  delay(50);
  i2c_OLED_send_cmd(0x20);
  i2c_OLED_send_cmd(0x02);
  i2c_OLED_send_cmd(0xa6);

  drawLogo(bootLogo);
  showingLogo = 1;

  frsky.setDataHandler(&dataHandler);
  frsky.setAlarmThresholdHandler(&alarmThresholdHandler);
  frsky.setUserDataHandler(&userDataHandler);

  digitalWrite(13, LOW);
}

void dataHandler(uint8_t a1, uint8_t a2, uint8_t q1, uint8_t q2) {
    ledState ^= 1;
    digitalWrite(13, ledState ? HIGH : LOW);
    redrawScreen = 1;

    rssiRx = map(q1, 0, 255, 0, 100);
    rssiTx = map(q2, 0, 255, 0, 100);
    voltageBattery = map(a2, 207, 254, 350, 430);
    analog1 = a1;
    analog2 = a2;
}

void alarmThresholdHandler(FrSky::AlarmThreshold alarm) {
    ledState ^= 1;
    digitalWrite(13, ledState ? HIGH : LOW);
}

void userDataHandler(const uint8_t* buf, uint8_t len) {
    ledState ^= 1;
    digitalWrite(13, ledState ? HIGH : LOW);
}

void writeLine(int l, String s) {
    setXY(l, 0);
    sendStr(s.c_str());
}

void loop(void) {
    frsky.poll();

    static unsigned long lastTime = 0;
    if (redrawScreen && ((millis() - lastTime) > 100)) {
        redrawScreen = 0;
        lastTime = millis();

        // Clear the display removing the bootLogo on first received packet
        if (showingLogo) {
            showingLogo = 0;
            clear_display();
        }

        writeLine(0, "RSSI Rx: " + String(rssiRx) + "%");
        writeLine(1, "RSSI Tx: " + String(rssiTx) + "%");
        writeLine(3, "A1 Value: " + String(analog1));
        writeLine(4, "A2 Value: " + String(analog2));
        writeLine(6, "Battery Volt:");
        writeLine(7, String(voltageBattery / 100) + "."  + String(voltageBattery % 100) + "V");
    }
}

