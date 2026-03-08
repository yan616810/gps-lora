#ifndef __MAIN_H
#define __MAIN_H
#include "stm32f10x.h"
//软件延时
#include "Delay.h"
//OLED
#include "OLED.h"
#include "iic.h"
//USART
#include "USART.h"
#include <stdio.h>
#include <math.h>
//key
#include "key.h"
//LCD
// #include "LCD.h"
//u8g2
#include "u8g2.h"
#include "u8g2_monochrome_display.h"
#include <string.h> //memset
#include "UI/UI_GPS.h"
//GPS
#include "GPS.h"
#include <math.h>
//bmp280
#include "bmp280.h"
//qmc6309
#include "lib/WMM_Tiny/Core/Inc/wmm.h"


/*GPS*/
extern GPS_t gps;           // 全局 GPS 实例
extern uint8_t earth_flag;  //是否以全球缩略图的形式显示实时坐标 1:文本形式 0:全球缩略图形式
/*u8g2*/
extern u8g2_t u8g2;
extern char u8g2_buf[20];
/*bmp280*/
extern BMP280_t bmp280;                // 全局 BMP280 实例
extern float fake_sea_level_pressure;  // 相对标准大气压，单位是Pa


#endif // !__MAIN_H