#include "uart.h"

static uint8_t rx_buf[RX_BUF_MAX] = {0};
static uint8_t tx_buf[TX_BUF_MAX] = {0};
static uint8_t rx_index = 0;
static uint8_t data_len = 0;
static uint8_t message_rx_finish = 0;
static uint8_t message_tx_finish = 0;

uint8_t rx_byte = 0;
uint8_t command = 1;

void UART_ProcessRxFrame(void)
{
}

void UART_GetTxFrame(void)
{
    tx_buf[0] = 0xAA;
    tx_buf[1] = ds18b20.temperature & 0xFF;
    tx_buf[2] = (ds18b20.temperature >> 8) & 0xFF;
    tx_buf[3] = temperature_target & 0xFF;
    tx_buf[4] = (temperature_target >> 8) & 0xFF;
    tx_buf[5] = pwm_duty & 0xFF;
    tx_buf[6] = (pwm_duty >> 8) & 0xFF;
    tx_buf[7] = 0x55;
}

void Task_Uart(void)
{
    static UART_CommState comm_state = UART_TX_READY;
    
    switch(comm_state)
    {
        case UART_RX_READY:
            if(message_rx_finish == 1)
            {
               message_rx_finish = 0;
               comm_state = UART_RX_PROCESS;
            }
        break;

        case UART_RX_PROCESS:
            UART_ProcessRxFrame();
            comm_state = UART_TX_READY; 
        break;

        case UART_TX_READY:
            UART_GetTxFrame();
            HAL_UART_Transmit_IT(&huart1,tx_buf,8);
            comm_state = UART_TX_PROCESS;
        break;

        case UART_TX_PROCESS:
            if(message_tx_finish == 1)
            {
                message_tx_finish = 0;
                comm_state = UART_TX_READY;
            }
        break;

        default:
            comm_state = UART_TX_READY;
        break;
    }
}

void UART_ParseByte(uint8_t byte)
{
    static UART_ParseState parse_state = UART_WAIT_HEAD;
    
    switch(parse_state)
    {
        case UART_WAIT_HEAD:
            if(byte == 0xAA)
            {
                rx_index = 0;
                rx_buf[rx_index++] = byte;
                parse_state = UART_GET_LEN;
            }
        break;

        case UART_GET_LEN:
            data_len = byte;
            rx_buf[rx_index++] = byte;
            parse_state = UART_GET_DATA;
        break;


        case UART_GET_DATA:
            rx_buf[rx_index++] = byte;
            if(rx_index >= data_len + 2)
            {
                parse_state = UART_GET_TAIL;
            }
        break;

        case UART_GET_TAIL:
            rx_buf[rx_index++] = byte;
            
            if(byte == 0x55)
            {
                message_rx_finish = 1;
            }
            parse_state = UART_WAIT_HEAD;
        break;

        default:
            parse_state = UART_WAIT_HEAD;
        break;
    }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
        UART_ParseByte(rx_byte);
        HAL_UART_Receive_IT(&huart1,&rx_byte,1);
    }
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    if(huart->Instance == USART1)
    {
       message_tx_finish = 1;
    }
}


