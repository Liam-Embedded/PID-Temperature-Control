#include "task.h"

//注册任务
Task_t TaskTable[TASK_MAX] = {
// 周期,  计时, 运行, 函数
    {500, 500,  0, Task_Led},
    {1000,   1000,   0, Task_Uart},
    {1000,  1000,  0, Task_DS18B20},
    {1000,  1000,  0, Task_Ctrl},
};

void Task_Init(void)
{
    HAL_UART_Receive_IT(&huart1,&rx_byte,1); //start uart rx
    HAL_TIM_Base_Start_IT(&htim1);           // 开启 TIM1 更新中断
}

//任务调度器
void Task_Run(void)
{
    unsigned char i;
    for(i=0; i<TASK_MAX; i++)
    {
        if(TaskTable[i].run == 1)
        {
            TaskTable[i].TaskFunc();        // 执行任务
            TaskTable[i].timer = TaskTable[i].period; // 重装周期
            TaskTable[i].run = 0;           // 清除标志
        }
    }
}

// 定时器中断中调用
void TimeSlice_IncTick(void)
{
    unsigned char i;
    for(i=0; i<TASK_MAX; i++)
    {
        if(TaskTable[i].timer > 0)
        {
            TaskTable[i].timer--;
            if(TaskTable[i].timer == 0)
            {
                TaskTable[i].run = 1; // 时间到，标记可运行
            }
        }
    }
}

//定时器中断回调
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM1)
    {
        TimeSlice_IncTick();
    }
}
