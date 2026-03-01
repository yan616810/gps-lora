#ifndef __DMA_H
#define __DMA_H
#include "stm32f10x.h"
#include "GPS.h"

/*使用DMA2+UART4接收*/
#define __DMA2_UART4_RX_DMA__
/*USART接收GPS数据波特率宏定义*/
#define USART1_BaudRate_bps (uint32_t)9600  // USART1波特率设置
#define UART4_BaudRate_bps (uint32_t)115200  // UART4波特率设置，130.208ms/帧

/*DMA1 + USART1*/
void DMA1_usart1_to_db_HW_Init(GPS_db_t *db);                      //硬件初始化函数，配置USART1+DMA1的双缓冲接收
void DMA1_Channel5_IRQHandler_usart1_rxFULL_callback(GPS_t *gps);  //DMA1通道5接收满中断处理函数
void USART1_IRQHandler_IDLE_callback(GPS_t *gps);                  //USART1空闲中断处理函数
/*DMA2 + UART4*/
void DMA2_uart4_to_db_HW_Init(GPS_db_t *db);                      //硬件初始化函数，配置UART4+DMA2的双缓冲接收
void DMA2_Channel3_IRQHandler_uart4_rxFULL_callback(GPS_t *gps);  //DMA2通道3接收满中断处理函数
void UART4_IRQHandler_IDLE_callback(GPS_t *gps);                  //UART4空闲中断处理函数

#endif // !__DMA_H
