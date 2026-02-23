#ifndef __IIC_H_
#define __IIC_H_
#include "stdint.h"
#include "stm32f10x.h"
/*****************用户可修改的宏定义*****************/
//0：最快，理论引脚电平切换无延时     1：可调速，默认速度是500kHZ，用IIC_Set_speed()函数自由调节iic通信速度
#define is_use_delay 1

//0：只遍历一次,先缓存在主栈，遍历完成再复制到堆空间中       1：遍历2次所有设备地址，第一次遍历得到设备数量，第二次遍历搜索到1个设备地址就存一个数据到堆中
#define is_use_low_memory_IIC_Search_all_devices 0
/*************************************************/

#if (is_use_delay == 1)
void IIC_Set_speed(uint32_t us);
#endif
void IIC_InitPins_or_ChangePins(uint32_t SCL_CHANGE_RCC_APB2Periph,GPIO_TypeDef *SCL_change_port,uint16_t SCL_change_pin,
                                uint32_t SDA_CHANGE_RCC_APB2Periph,GPIO_TypeDef *SDA_change_port,uint16_t SDA_change_pin);
void IIC_Start(void);				
void IIC_Stop(void);	  			

void IIC_Send_8bit(u8 txd);			
u8 IIC_Read_8bit(unsigned char ack);

u8 IIC_Wait_Ack(void); 				
void IIC_Ack(void);					
void IIC_NAck(void);

u8 IIC_Write_Byte(u8 addr,u8 reg,u8 data);
u8 IIC_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf);
u8 IIC_Read_Byte(u8 addr,u8 reg);
u8 IIC_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf);

uint8_t IIC_Search_all_devices(uint8_t **devices_addr_info_heap_ptr);
void IIC_Search_all_devices_printf_example(void);
#endif 
