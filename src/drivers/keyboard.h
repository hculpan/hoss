#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "../cpu/types.h"

void init_keyboard();

char waitForAscii();

unsigned char waitForScancode();

unsigned char isCtrlPressed();

unsigned char isAltPressed();

unsigned char isShiftPressed();

unsigned char isCapsLockOn();

#endif