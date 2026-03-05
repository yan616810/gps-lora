#ifndef __GPS_H
#define __GPS_H
#include "stm32f10x.h"                  // Device header
#include "lwrb.h"
#include "lwgps.h"

/*双缓冲中每个缓冲区的大小*/
#define DMA_BUF_SIZE (uint32_t)1024*2  // 适当增大双缓冲区空间大小，每个缓冲区最好大于单帧报文的长度1.5KB

/*DMA双缓冲区结构体定义*/
typedef struct GPS_db{
    uint8_t double_buffering1[DMA_BUF_SIZE];  // 双缓冲区buf1
    uint8_t double_buffering2[DMA_BUF_SIZE];  // 双缓冲区buf2
    uint8_t double_buffering_index;           // 指示当前正在使用的缓冲区，0: buf1, 1: buf2
}GPS_db_t;

/*环形缓冲区结构体定义*/
typedef struct GPS_rb{
    uint8_t ring_buffer_data[DMA_BUF_SIZE*2];  //一帧数据1.5KB多，动态调整为4K
    lwrb_t  ring_buffer;                       // GPS数据解析使用的环形缓冲区实例
}GPS_rb_t;

/*GPS模块主结构体定义*/
typedef struct GPS{//标签(tag)命名空间
    GPS_db_t db;            // 双缓冲区结构体
    GPS_rb_t rb;            // 环形缓冲区结构体
    lwgps_t  lwgps_handle;  // GPS软件解析库的主结构体实例
    /* interrupt callbacks registered by GPS_init
       这两个函数将在中断服务例程中被调用，传入完整的GPS_t结构体指针
       这样可以将中断处理逻辑与具体的GPS实例绑定，适用于多模块或测试场景。
    */
    void (*dma_full_irq)(struct GPS *gps);//当双缓冲每个缓冲区大小可以容纳单帧报文时，DMA转运完成满中断处理函数几乎使用不到，这个函数存在的意义是，以后减少双缓冲大小后依然可以稳健地运行；
    void (*uart_idle_irq)(struct GPS *gps);
    /* hardware initialization callback
       给GPS模块配置USART/DMA之类的底层硬件，传入双缓冲区结构体
    */
    void (*hw_init)(struct GPS_db *db);
}GPS_t;//_t后缀表示这是一个类型定义表示普通命名空间，方便在代码中直接使用GPS_t来声明变量，而不需要每次都写struct GPS。



void GPS_init(GPS_t *gps);
void GPS_lwgps_parser_lwrb(GPS_t *gps);//500ms解析一次

#endif // !__GPS_H
