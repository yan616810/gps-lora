#ifndef __LORA_H
#define __LORA_H

#include <stdint.h>
#include "stm32f10x.h"

/**
 * @brief Initialize USART1 (PC side) and USART2 (LoRa side) for transparent forwarding.
 * @param baud1 USART1 baud rate
 * @param baud2 USART2 baud rate
 */
void LoRa_init(uint32_t baud1, uint32_t baud2);

void UART1_IRQHandler_RXNE_callback(void);
void UART2_IRQHandler_RXNE_callback(void); 

#endif // __LORA_H
