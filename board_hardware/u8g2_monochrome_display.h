#ifndef __U8G2_MONOCHROME_DISPLAY_H
#define __U8G2_MONOCHROME_DISPLAY_H
#include <stdint.h>
#include "u8g2.h"
//初始化软件iic引脚，只有在配置u8g2使用软件iic是才会使用的到
#define u8g2_iic_sw_scl_RCC_APB2Periph_GPIOx    RCC_APB2Periph_GPIOB
#define u8g2_iic_sw_scl_port                    GPIOB 
#define u8g2_iic_sw_scl_pin                     GPIO_Pin_10
#define u8g2_iic_sw_sda_RCC_APB2Periph_GPIOx    RCC_APB2Periph_GPIOB
#define u8g2_iic_sw_sda_port                    GPIOB 
#define u8g2_iic_sw_sda_pin                     GPIO_Pin_11

//给某个单色屏幕启动一个u8g2实例
void u8g2_oled_init(u8g2_t *u8g2);

//必要回调1:端口底层回调
uint8_t u8x8_gpio_and_delay_for_sw_iic(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t u8x8_gpio_and_delay_for_hw_iic(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
//必要回调2:通信基础设施回调，如软件iic协议实现，或硬件iic配置实现通信协议
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);


//u8g2的使用效果
void u8g2_oled_play_Animation(u8g2_t *u8g2);
void u8g2_oled_draw_earth(u8g2_t *u8g2);
void Latlon2pixel(uint16_t *x, uint16_t *y, float lat, float lon);
void u8g2_oled_draw_earth_pixel_VHxvLine(u8g2_t *u8g2,float lat, float lon);

#endif // !__u8g2_monochrome_display_H

