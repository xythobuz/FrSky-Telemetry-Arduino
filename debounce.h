/*
 * FrSky Telemetry Display for Arduino
 *
 * Pushbutton debouncing helper class
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */
#ifndef __DEBOUNCE_H__
#define __DEBOUNCE_H__

class Debouncer {
  public:
    Debouncer(int p);
    int poll();

  private:
    int pin;
    int currentState;
    int lastState;
    unsigned long lastTime;
};

#endif

