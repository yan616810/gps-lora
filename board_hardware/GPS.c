#include "GPS.h"
#include "DMA_USART.h"
#include "USART.h"
/**
 * @brief 初始化GPS模块
 * 
 * @param gps 指向GPS_t结构体的指针
 */
void GPS_init(GPS_t *gps)
{
/*中断回调函数注册到 GPS 实例*/
    gps->dma_full_irq  = DMA2_Channel3_IRQHandler_uart4_rxFULL_callback;  // 设置DMA传输完成中断回调函数指针
    gps->uart_idle_irq = UART4_IRQHandler_IDLE_callback;                  // 设置UART空闲中断回调函数指针
/*软件*/
    lwrb_init (&gps->rb.ring_buffer, gps->rb.ring_buffer_data, sizeof(gps->rb.ring_buffer_data));  // 初始化GPS所依赖的的软件：初始化stm32内的一个大小合适的环形缓冲区
    lwgps_init(&gps->lwgps_handle);                                                                // 初始化GPS软件解析库的主结构体
/*硬件*/
    gps->db.double_buffering_index = 0;                      // 设置DMA硬件初始时使用双缓冲区的哪个缓冲区，0: buf1, 1: buf2
    gps->hw_init                   = DMA2_uart4_to_db_HW_Init;  // 设置硬件初始化函数指针
    if (gps->hw_init != NULL) {
        gps->hw_init(&gps->db);  // 调用硬件初始化函数，传入双缓冲区结构体指针
    }
}

/**
 * @brief GPS解析任务，解析器流式解析环形缓冲区的数据
 * 
 * @param gps 指向GPS_t结构体的指针;
 * @note 作用：将从环形缓冲区解析得到的GPS模块输出的所有的参数，存储到GPS_t实例的lwgps_handle成员中！
 */
void GPS_lwgps_parser_lwrb(GPS_t *gps)
{
    /* 解析任务依赖于 GPS 实例中的环形缓冲区和 lwgps 句柄 */
    uint8_t parser_temp_buf[DMA_BUF_SIZE]; // 建议temp_buf大小可以容纳单帧GPS报文；此处使用任务栈而不是结构体内单独缓冲，高效利用SRAM，但是要注意栈区空间大小，避免栈溢出；
    size_t len;
    /*尝试从环形缓冲区读取数据*/
    while ((len = lwrb_read(&gps->rb.ring_buffer, parser_temp_buf, sizeof(parser_temp_buf))) > 0) {
        /*送入LWGPS解析*/
        lwgps_process(&gps->lwgps_handle, parser_temp_buf, len);//lwgps_process()本身是流式解析，处理速度很快。
        // usart1_send_Hex((uint8_t *)parser_temp_buf, len);
    }
}

