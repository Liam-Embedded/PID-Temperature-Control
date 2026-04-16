#include "controller.h"

uint16_t temperature_target = 500; //精度0.1℃
int16_t temperature_error = 0;

uint16_t Kp = 10;
int16_t pwm_duty = 0;

#define PWM_DUTY_MAX 999
#define PWM_DUTY_MIN  0

//控制任务，1s执行一次
void Task_Ctrl(void)
{
   Bang_Bang_Ctrl();
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


//比例控制
void P_Ctrl(void)
{
   temperature_error = temperature_target - ds18b20.temperature; //计算偏差
   pwm_duty = Kp * temperature_error;  //计算PWM占空比
   
    if(pwm_duty > PWM_DUTY_MAX)  //限制在定时器的最大、最小占空比之内
   {
     pwm_duty = PWM_DUTY_MAX;
   }
   else if(pwm_duty < PWM_DUTY_MIN)
   {
     pwm_duty = PWM_DUTY_MIN;
   }
   
   __HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,pwm_duty); //更新占空比，控制开关管通断，进而控制功率输出 
  
}















