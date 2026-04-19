#ifndef __CONTORLLER_H
#define __CONTORLLER_H

#include "main.h"
#include "ds18b20.h"

#define PWM_DUTY_MAX 999
#define PWM_DUTY_MIN  0

//#define BANG_BANG_CTRL  
//#define P_CTRL          
#define PI_CTRL         

typedef struct
{
  uint16_t target; //目标
  int16_t error;  //偏差
  
  uint16_t Kp ;    //比例增益 无量纲
  uint16_t Ti ;    //积分时间 单位：s
  uint16_t Ts ;    //采样时间 单位：s
  
  int32_t integral;  //积分累加和
  int32_t out;      //pid输出
  
}PID_TyptDef;

void Task_Ctrl(void);

void Bang_Bang_Ctrl(void);
void PI_Ctrl(void);

extern PID_TyptDef pid;

#endif