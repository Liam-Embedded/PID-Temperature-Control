#include "led.h"

void Task_Led(void)
{
    static LED_State led_state = LED_OFF;
    
    switch(led_state)
    {
        case LED_OFF:
         HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
         led_state = LED_ON;
        break;

        case LED_ON:
         HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
         led_state = LED_OFF;
        break;  

        default:
        led_state = LED_OFF;
        break;  
    }
}



