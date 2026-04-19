#include "controller.h"

uint16_t temperature_target = 500; //目标
int16_t temperature_error = 0;  //偏差


PID_TyptDef pid =
{
    .target = 500,   
    .error = 0,
    
    .Kp = 10,       //比例增益 无量纲
    .Ti = 10,       //积分时间 单位：s
    .Ts = 1,        //采样时间 单位：s
    
    .integral = 0,  //积分累加和
    .out = 0,       ////pid输出
};

//控制任务，1s执行一次
void Task_Ctrl(void)
{
   PI_Ctrl();
}

//比例-积分控制
void PI_Ctrl(void)
{
   pid.error = pid.target - ds18b20.temperature; // 步骤1：偏差 = 目标 -测量
   
   pid.integral += pid.error;                    //步骤2：积分项累加         
   
    pid.out = (pid.Kp * pid.error) + ( pid.Kp * pid.Ts * pid.integral / pid.Ti);  //步骤3：计算比例、积分输出
  
   if(pid.out >= PWM_DUTY_MAX)                   //步骤4：限制pid输出在定时器占空比有效范围内
   {
     pid.out = PWM_DUTY_MAX;
   }
   else if(pid.out <= PWM_DUTY_MIN)
   {
     pid.out = PWM_DUTY_MIN;
   }
   else
   {
     pid.integral += pid.error;                //步骤5：抗积分饱和
   }
   
   __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,pid.out); //步骤6：更新占空比，PWM输出控制开关管，进而控制功率输出 
}


//bang-bang控制
void Bang_Bang_Ctrl(void)
{
  temperature_error = temperature_target - ds18b20.temperature; //计算偏差
  
  if(temperature_error >= 10)
  {
      HAL_GPIO_WritePin(PTC_GPIO_Port, PTC_Pin, GPIO_PIN_SET); //开
  }
  else if(temperature_error < -10)
  {
     HAL_GPIO_WritePin(PTC_GPIO_Port, PTC_Pin, GPIO_PIN_RESET); //关
  }
  else
  {}
}














