#ifndef __HW_IIC_H
#define __HW_IIC_H
#include <stdint.h>
#include "stm32f10x.h"                  // Device header

#define hw_iicx    2     //1表示开启iic1   2表示开启iic2

void hw_iic_init(void);
uint8_t hw_iic_write_byte(uint8_t addr,uint8_t reg,uint8_t data);
uint8_t hw_iic_write_byte_len(uint8_t addr,uint8_t reg,uint32_t len,uint8_t *data_buf);
uint8_t hw_iic_read_byte(uint8_t addr,uint8_t reg);
uint8_t hw_iic_read_byte_len(uint8_t addr,uint8_t reg,uint32_t len,uint8_t *data_buf);

uint8_t hw_iic_CheckEvent_timeout(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT);
// uint8_t hw_iic_search_devices(uint8_t addr_7bit);
#endif


