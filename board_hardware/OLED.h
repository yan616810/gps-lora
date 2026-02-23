#ifndef __OLED_H
#define __OLED_H
#include "stdint.h"

#define OLED_device_addr 0x3c //7 bit addr -> 0x78

#define OLED_Command_register_addr 0x00
#define OLED_Data_register_addr    0x40

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

void oled_all_open(void);
void oled_all_open2(void);
void oled_chinese(uint8_t Line,uint8_t Colum,uint8_t chinese_sernum);
void oled_image_leige(uint8_t X_hight);
void oled_image_binbin(void);
void oled_image_jinxin(uint8_t X_hight);
void oled_image_meinv(uint8_t X_hight);
void oled_image_hongzhong(uint8_t X_hight);
void oled_image_zongyao(uint8_t X_hight);
void oled_compress_image_zongyao(uint8_t X_hight);
void oled_image_yanhui(void);
void oled_compress_image_yanhui(uint8_t X_hight);
#endif
