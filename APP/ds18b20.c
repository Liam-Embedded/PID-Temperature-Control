#include "ds18b20.h"

DS18B20_t ds18b20;
uint32_t now;

static void delay_us(uint32_t us)
{
    uint32_t start  = DWT->CYCCNT;
    uint32_t cycles = us * (SystemCoreClock / 1000000);
    while ((DWT->CYCCNT - start) < cycles);
}

void FSM_DS18B20(void)
{
    DS18B20_t *h = &ds18b20;
    now = HAL_GetTick();

    switch (h->state)
    {
        case DS18B20_IDLE:
            break;

        // 拉低总线，计时
        case DS18B20_RESET:
            OW_LOW();
            h->tick  = now;
            h->state = DS18B20_WAIT_RESET;
            break;

        // 释放总线，等待存在脉冲结束后再发命令
        case DS18B20_WAIT_RESET:
            if (now - h->tick >= 1) {
                OW_HIGH();
                delay_us(480); // 等待 DS18B20 存在脉冲完全结束（最长 ~300us）
                h->tx_byte = 0xCC;
                h->tx_bit  = 0;
                h->step++;
                h->state   = DS18B20_SEND_BYTE;
            }
            break;

        // 逐位（LSB first）发送 tx_byte，每次调用发一位
        case DS18B20_SEND_BYTE:
            if (h->tx_bit < 8) {
                OW_LOW();
                delay_us(2);
                if (h->tx_byte & (1 << h->tx_bit)) OW_HIGH();
                delay_us(60);
                OW_HIGH();
                delay_us(2);
                h->tx_bit++;
            } else {
                // 8位发完，按进度决定下一步
                // step: 1→发完首个0xCC  2→发完0x44  4→发完第二个0xCC  5→发完0xBE
                switch (h->step) {
                    case 1: // 首个0xCC已发 → 发0x44启动转换
                        h->tx_byte = 0x44; h->tx_bit = 0; h->step = 2;
                        break;
                    case 2: // 0x44已发 → 等待750ms转换
                        h->tick = now; h->step = 3;
                        h->state = DS18B20_WAIT_CONVERT;
                        break;
                    case 4: // 第二个0xCC已发 → 发0xBE读暂存器
                        h->tx_byte = 0xBE; h->tx_bit = 0; h->step = 5;
                        break;
                    case 5: // 0xBE已发 → 读温度数据
                        h->buf[0] = h->buf[1] = 0;
                        h->bit_cnt = 0; h->step = 6;
                        h->state = DS18B20_READ_DATA;
                        break;
                }
            }
            break;

        // 等待 DS18B20 温度转换完成（12位精度750ms）
        // step此时=3，复位后 WAIT_RESET 会 step++ → 4
        case DS18B20_WAIT_CONVERT:
            if (now - h->tick >= 750)
                h->state = DS18B20_RESET;
            break;

        // 逐位读取温度 LSB+MSB（共16位），每次调用读一位
        case DS18B20_READ_DATA:
            if (h->bit_cnt < 16) {
                OW_LOW();
                delay_us(2);
                OW_HIGH();
                delay_us(5);
                if (OW_READ())
                    h->buf[h->bit_cnt >> 3] |= (1 << (h->bit_cnt & 7));
                delay_us(50);
                h->bit_cnt++;
            } else {
                h->temperature = (uint16_t)(((h->buf[1] << 8) | h->buf[0]) * 0.0625f * 10);
                h->done  = 1;
                h->step  = 0;
                h->state = DS18B20_IDLE;
            }
            break;
    }
}

void Task_DS18B20(void)
{
    if (ds18b20.state == DS18B20_IDLE) {
        ds18b20.done  = 0;
        ds18b20.state = DS18B20_RESET;
    }
}
