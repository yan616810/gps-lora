#ifndef __DMA_H
#define __DMA_H

#include "lwgps.h"

/**
 * @brief 双缓冲中一个缓冲区的大小宏定义
 * 
 */
#define DMA_BUF_SIZE (uint32_t)128         // 适当增大双缓冲区空间大小，每个缓冲区最好大于单帧报文的长度
/**
 * @brief USART接收GPS数据波特率宏定义
 * 
 */
#define USART1_BaudRate_bps (uint32_t)9600  // USART波特率设置
#define UART4_BaudRate_bps (uint32_t)115200  // UART4波特率设置

void GPS_Init_all_module(void);               //初始化一个环形缓冲区 + 配置usart1的DMA接收 + 配置DMA1的通道5 + 双缓冲机制
void GPS_Parser_lwrb(lwgps_t* lwgps_handle);  //GPS解析任务

/*usart1*/
void USART1_IRQHandler_IDLE_callback(void);                  //USART1空闲中断处理函数（DMA方式，处理未满缓冲区数据）
void DMA1_Channel5_IRQHandler_usart1_rxFULL_callback(void);  //DMA1通道5接收满中断处理函数，处理USART1接收数据到双缓冲区
/*uart4*/
void UART4_IRQHandler_IDLE_callback(void);
void DMA2_Channel3_IRQHandler_uart4_rxFULL_callback(void);

#endif // !__DMA_H
