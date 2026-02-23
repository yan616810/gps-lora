/**
 * @file main.c
 * @author YLAD (yhlad0801@gmail.com)
 * @brief GPS+LoRa handheld device
 * @version 0.1
 * @date 2026-01-18
 * 
 * @copyright Copyright (c) 2026
 * 
 */

#include "stm32f10x.h"
#include "Delay.h"
#include "OLED.h"
#include "iic.h"
int main(void)
{
    /* Add your application code here */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); // Enable clock for GPIOA
    
        GPIO_InitTypeDef GPIO_InitStructure;
        
        // // POWER-EN Configure PB4 as input
        // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
        // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        // GPIO_Init(GPIOB, &GPIO_InitStructure);
        // GPIO_WriteBit(GPIOB, GPIO_Pin_4, Bit_RESET); // Set PB4 Low

        // // key1 Configure PA0 as output
        // GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
        // GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        // GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
        // GPIO_Init(GPIOA, &GPIO_InitStructure);
        // GPIO_WriteBit(GPIOA, GPIO_Pin_0, Bit_SET); // Set PA0 High-Z

        // POWER-EN Configure PC13
        GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
        GPIO_Init(GPIOC, &GPIO_InitStructure);
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET); // Set PC13


        IIC_InitPins_or_ChangePins(RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_10,RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_11);
        OLED_Init();
        OLED_ShowString(1, 1, "Hello, World!");
        Delay_s(2);
        oled_image_binbin();
        Delay_s(60);
        GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET); // Set PC13 Low

    
    for(;;);
}