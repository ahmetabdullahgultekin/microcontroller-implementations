// keypad.h
#ifndef KEYPAD_H
#define KEYPAD_H

#include "stm32l476xx.h"

void Keypad_Pin_Init(void);
char Keypad_Scan(void);

#endif // KEYPAD_H