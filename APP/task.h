#ifndef __TASK_H
#define __TASK_H

#include "main.h"

#include "uart.h"
#include "led.h"
#include "ds18b20.h"
#include "controller.h"

// 任务数量
#define TASK_MAX  4

// 任务结构体
typedef struct {
    unsigned int  period;   // 执行周期 ms
    unsigned int  timer;    // 倒计时
    unsigned char run;      // 运行标志
    void (*TaskFunc)(void); // 任务函数
} Task_t;


void TimeSlice_IncTick(void);
void Task_Init(void);    // 应用初始化
void Task_Run(void);     // 主循环运行



#endif