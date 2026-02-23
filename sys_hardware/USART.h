#ifndef __USART_H
#define __USART_H
#include <stdint.h>


//选择使用哪个USART,usart1和usart2所挂载的总线不同
#define usartx     1     //1表示开启usart1   2表示开启usart2

//接收缓存区大小(x-1个字节)
#define usart_rx_buff_size 32
#define usart_rx_baud      115200
#define TIM3CLK            72000000

//使用AC6编译器时，不支持__weak
#define USE_std_C11    //开启C11，使用AC6编译器，LWRB使用C11


#if (usartx == 1)
void usart1_init(void);
void usart1_send_Char(uint8_t character);
void usart1_send_Hex(uint8_t *Arry,uint32_t len);
void usart1_send_str(char *str);
#elif (usartx ==2)
void usart2_init(void);
void usart2_send_Char(uint8_t character);
void usart2_send_Hex(uint8_t *Arry,uint32_t len);
void usart2_send_str(char *str);
#endif

void rx_data_proc(void);





//void test(void);

#endif // !__USART_H

