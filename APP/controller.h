#ifndef __CONTORLLER_H
#define __CONTORLLER_H

#include "main.h"
#include "ds18b20.h"

void Task_Ctrl(void);
void P_Ctrl(void);
void Bang_Bang_Ctrl(void);

extern uint16_t temperature_target;
extern int16_t pwm_duty;

#endif