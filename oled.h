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
#ifndef __OLED_H__
#define __OLED_H__

void clear_display(void);
void setXY(uint8_t row, uint8_t col);
void sendStr(const char *string);
void writeLine(int l, String s);
void drawLogo(const uint8_t *logo);

void i2c_OLED_init(void);
void i2c_OLED_send_cmd(uint8_t command);
void i2c_OLED_send_byte(uint8_t val);

#endif

