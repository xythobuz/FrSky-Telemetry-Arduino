/*
 * FrSky Telemetry Display for Arduino
 *
 * On Screen Menu
 * Copyright 2016 by Thomas Buck <xythobuz@xythobuz.de>
 *
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <xythobuz@xythobuz.de> wrote this file.  As long as you retain this notice
 * you can do whatever you want with this stuff. If we meet some day, and you
 * think this stuff is worth it, you can buy me a beer in return.   Thomas Buck
 * ----------------------------------------------------------------------------
 */
#ifndef __MENU_H__
#define __MENU_H__

#define MENU_NONE 0
#define MENU_NEXT 1
#define MENU_OK 2

void drawMenu(uint8_t input);

#endif

