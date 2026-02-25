#ifndef __HW_IIC_H
#define __HW_IIC_H
#include <stdint.h>
#include "stm32f10x.h"                  // Device header


#define HW_IIC_TIMEOUT_US   1000u   /* micro‑seconds; adjust for your clock speed */


void hw_iic_init(I2C_TypeDef *I2Cx);
uint8_t hw_iic_write_byte(I2C_TypeDef *I2Cx,uint8_t addr,uint8_t reg,uint8_t data);
uint8_t hw_iic_write_byte_len(I2C_TypeDef *I2Cx,uint8_t addr,uint8_t reg,uint32_t len,uint8_t *data_buf);
uint8_t hw_iic_read_byte(I2C_TypeDef *I2Cx,uint8_t addr,uint8_t reg);
uint8_t hw_iic_read_byte_len(I2C_TypeDef *I2Cx,uint8_t addr,uint8_t reg,uint32_t len,uint8_t *data_buf);

uint8_t hw_iic_CheckEvent_timeout(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT);
uint8_t hw_iic_wait_not_busy(I2C_TypeDef *I2Cx);


/* recover an I2C peripheral and its GPIO pins when the bus is locked
   (SCL/SDA stuck high/low).  Defined in hw_iic.c. */
void hw_iic_bus_recover(I2C_TypeDef *I2Cx);


// uint8_t hw_iic_search_devices(uint8_t addr_7bit);
#endif


