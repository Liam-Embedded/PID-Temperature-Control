#ifndef __LED_H
#define __LED_H

#include "main.h"

typedef enum {
    LED_ON,
    LED_OFF
}LED_State;

void Task_Led(void);    // LED 500ms

#endif