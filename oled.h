
#ifndef __OLED_H__

void clear_display(void);
void setXY(uint8_t row, uint8_t col);
void sendStr(const char *string);
void drawLogo(const uint8_t *logo);

void i2c_OLED_init(void);
void i2c_OLED_send_cmd(uint8_t command);
void i2c_OLED_send_byte(uint8_t val);

#endif

