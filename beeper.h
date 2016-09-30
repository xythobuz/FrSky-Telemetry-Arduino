/*
 * FrSky Telemetry Display for Arduino
 *
 * Beeper handling utility
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */
#ifndef __BEEPER_H__
#define __BEEPER_H__

#define BEEPER_STATE_OFF 0
#define BEEPER_STATE_LOW 1
#define BEEPER_STATE_HIGH 2

void setBeeper(uint8_t state);
void beeperTask(void);

#endif

