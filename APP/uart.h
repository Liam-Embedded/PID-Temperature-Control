#ifndef __UART_H
#define __UART_H

#define RX_BUF_MAX 10
#define TX_BUF_MAX 12

#include "main.h"
#include "ds18b20.h"
#include "controller.h"

//0xAA +Len + data1 + data2 +...+ data5 + 0x55
typedef enum {
    UART_WAIT_HEAD,
    UART_GET_LEN,
    UART_GET_DATA,
    UART_GET_TAIL,
}UART_ParseState;

typedef enum {
    UART_RX_READY,
    UART_RX_PROCESS,
    UART_TX_READY,
    UART_TX_PROCESS,
}UART_CommState;

void UART_ParseByte(uint8_t byte);
void UART_ProcessFrame(void);
void UART_Communication(void);
void UART_GetTxDdta(void);
void Task_Uart(void);   // 串口 20ms

extern uint8_t rx_byte;


#endif