#ifndef __DS18B20_H
#define __DS18B20_H

#include "main.h"

#define DS18B20_PORT  GPIOA
#define DS18B20_PIN   GPIO_PIN_0

#define OW_LOW()   HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_RESET)
#define OW_HIGH()  HAL_GPIO_WritePin(DS18B20_PORT, DS18B20_PIN, GPIO_PIN_SET)
#define OW_READ()  HAL_GPIO_ReadPin (DS18B20_PORT, DS18B20_PIN)

typedef enum {
    DS18B20_IDLE = 0,
    DS18B20_RESET,        // 拉低总线 >= 480us（实际用 1ms）
    DS18B20_WAIT_RESET,   // 释放总线，等待存在脉冲
    DS18B20_SEND_BYTE,    // 逐位发送 tx_byte（LSB first）
    DS18B20_WAIT_CONVERT, // 等待温度转换完成（750ms）
    DS18B20_READ_DATA,    // 读取 2 字节温度数据（16 位）
} DS18B20_State;

typedef struct {
    DS18B20_State state;
    uint8_t  step;      // 整体序列进度（0~6）
    uint32_t tick;      // 时间戳
    uint8_t  tx_byte;   // 待发字节
    uint8_t  tx_bit;    // 发送位计数
    uint8_t  buf[2];    // [0]=温度LSB, [1]=温度MSB
    uint8_t  bit_cnt;   // 读数据位计数（0~15）
    uint16_t    temperature;
    uint8_t  done;      // 1=数据就绪
} DS18B20_t;

extern DS18B20_t ds18b20;

void FSM_DS18B20(void);
void Task_DS18B20(void);
void Task_Cltrol(void);

#endif
