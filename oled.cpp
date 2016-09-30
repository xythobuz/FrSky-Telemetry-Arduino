/*
 * FrSky Telemetry Display for Arduino
 *
 * OLED low level methods
 * Modified from the OLED library in the HobbyKing shop comments:
 * http://www.hobbyking.com/hobbyking/store/uploads/121771368X1172162X54.zip
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */
#include <Arduino.h>
#include "i2c.h"
#include "oled.h"
#include "font.h"

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

void drawLogo(const uint8_t *logo) {
    for (uint8_t k = 0; k < 8; k++) {
        setXY(k, 0);
        for (uint8_t i = 0; i < 128; i++) {
            i2c_OLED_send_byte(pgm_read_byte(&logo[i + (k * 128)]));
        }
    }
}

void clear_display(void) {
  for (uint8_t k = 0; k < 8; k++) {    
    setXY(k, 0);
    for (uint8_t i = 0; i < 128; i++) {
      i2c_OLED_send_byte(0);
    }
  }
}

void setXY(uint8_t row, uint8_t col) {
  i2c_OLED_send_cmd(0xb0 + row); // set page address
  i2c_OLED_send_cmd(0x00 + (8 * col & 0x0f)); // set low col address
  i2c_OLED_send_cmd(0x10 + ((8 * col >> 4) & 0x0f)); // set high col address
}

void sendStr(const char *string) {
  while (*string) {
    for (uint8_t i = 0; i < 8; i++) {
      i2c_OLED_send_byte(pgm_read_byte(&font[*string - 0x20][i]));
    }
    *string++;
  }
}

void i2c_OLED_send_cmd(uint8_t command) {
  TWBR = ((F_CPU / 400000L) - 16) / 2; // change the I2C clock rate
  i2c_writeReg(OLED_address, 0x80, (uint8_t)command);
}

void i2c_OLED_send_byte(uint8_t val) {
  TWBR = ((F_CPU / 400000L) - 16) / 2; // change the I2C clock rate
  i2c_writeReg(OLED_address, 0x40, (uint8_t)val);
}

void  i2c_OLED_init(void) {
  i2c_OLED_send_cmd(0xae); // display off
  i2c_OLED_send_cmd(0x2e); // deactivate scrolling
  i2c_OLED_send_cmd(0xa4); // Set all pixels Off
  delay(50);
  i2c_OLED_send_cmd(0x20); // Set Memory Addressing Mode
  i2c_OLED_send_cmd(0x02); // Set Memory Addressing Mode to Page addressing mode(RESET)
  i2c_OLED_send_cmd(0xa1); // colum address 127 mapped to SEG0 (POR)
  i2c_OLED_send_cmd(0xC8); // Scan from Left to Right
  i2c_OLED_send_cmd(0xa6); // Set WHITE chars on BLACK backround
  i2c_OLED_send_cmd(0x81); // 81 Setup CONTRAST CONTROL, following byte is the contrast Value
  i2c_OLED_send_cmd(0xff); // af contrast value between 1 ( == dull) to 255 ( == bright)
  delay(20);
  i2c_OLED_send_cmd(0xaf); // display on
  delay(20);
}

