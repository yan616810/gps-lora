#ifndef __HW_SPI_H
#define __HW_SPI_H

#include "stm32f10x.h"

#define SPI_CS_PIN_CLR()  GPIO_ResetBits(GPIOA, GPIO_Pin_4)  // CS引脚拉低
#define SPI_CS_PIN_SET()  GPIO_SetBits(GPIOA, GPIO_Pin_4)

void hw_spi_init(void);
uint8_t hw_spi_transfer(uint8_t data);


#endif
