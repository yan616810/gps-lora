#ifndef __LCD_H
#define __LCD_H

#include "stm32f10x.h"
#include <stdint.h>

#define LCD_WIDTH  240 // 屏幕宽度
#define LCD_HEIGHT 320 // 屏幕高度

//==================================================================
///*画笔颜色*/
//==================================================================
#define WHITE       0xFFFF
#define BLACK      	0x0000	  
#define BLUE       	0x001F 
#define BRED        0XF81F
#define GRED 		0XFFE0
#define GBLUE		0X07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define GREEN       0x07E0
#define CYAN        0x7FFF
#define YELLOW      0xFFE0
#define BROWN 		0XBC40 //棕色
#define BRRED 		0XFC07 //棕红色
#define GRAY  		0X8430 //灰色
#define GRAY25      0xADB5
//GUI颜色
#define DARKBLUE      	0X01CF	//深蓝色
#define LIGHTBLUE      	0X7D7C	//浅蓝色  
#define GRAYBLUE       	0X5458 //灰蓝色
//以上三色为PANEL的颜色 
#define LIGHTGREEN     	0X841F //浅绿色
//#define LIGHTGRAY     0XEF5B //浅灰色(PANNEL)
#define LGRAY 			0XC618 //浅灰色(PANNEL),窗体背景色
#define LGRAYBLUE      	0XA651 //浅灰蓝色(中间层颜色)
#define LBBLUE          0X2B12 //浅棕蓝色(选择条目的反色)


// 控制引脚操作宏（需根据实际硬件连接修改）
#define LCD_RES_SET()   GPIO_SetBits(GPIOB, GPIO_Pin_1)
#define LCD_RES_CLR()   GPIO_ResetBits(GPIOB, GPIO_Pin_1)
#define LCD_DC_SET()    GPIO_SetBits(GPIOC, GPIO_Pin_5)
#define LCD_DC_CLR()    GPIO_ResetBits(GPIOC, GPIO_Pin_5)
#define LCD_CS1_SET()   GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define LCD_CS1_CLR()   GPIO_ResetBits(GPIOA, GPIO_Pin_4)

#define LCD_CLK_SET()   GPIO_SetBits(GPIOB, GPIO_Pin_10)
#define LCD_CLK_CLR()   GPIO_ResetBits(GPIOB, GPIO_Pin_10)
#define LCD_MOSI_SET()  GPIO_SetBits(GPIOB, GPIO_Pin_11)
#define LCD_MOSI_CLR()  GPIO_ResetBits(GPIOB, GPIO_Pin_11)

void LCD_PIN_Init(void);
void LCD_PIN_Init_HW_SPI(void);
void LCD_Reset(void);
void LCD_IC_Init(void);
void LCD_Init_All(void);
void LCD_WriteCommand(uint8_t cmd);
void LCD_WriteData(uint8_t data);
void LCD_WriteData16(uint16_t data);
void LCD_WriteData24(uint32_t data);
void LCD_SetAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);

void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color);
void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color);
void LCD_DrawChar(uint16_t x, uint16_t y, char chr, uint16_t color, uint16_t bgcolor);
void LCD_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bgcolor);

void LCD_SetTextColor(uint16_t color);
void LCD_SetBackColor(uint16_t color);
void LCD_Clear(uint16_t color);
void LCD_ShowSnow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
#endif

