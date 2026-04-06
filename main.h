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
#include "UI/UI.h"
//GPS
#include "GPS.h"
#include <math.h>
//bmp280
#include "bmp280.h"
//qmc6309
#include "lib/WMM_Tiny/Core/Inc/wmm.h"
#include "qmc6309.h"
//RTC
#include "RTC.h"

/*u8g2*/
extern u8g2_t u8g2;
extern char u8g2_buf[25];
/*GPS*/
extern GPS_t gps;               // 全局 GPS 实例
extern uint8_t earth_flag;      //是否以全球缩略图的形式显示实时坐标 1:文本形式 0:全球缩略图形式
/*bmp280*/
extern BMP280_t bmp280;                // 全局 BMP280 实例
extern float fake_sea_level_pressure;  //相对标准大气压，单位是Pa
extern char altitude_sign;             //相对高度符号，默认正号
extern uint16_t altitude_int_part;     //bmp280推算出的相对高度整数部分，单位是米
extern uint16_t altitude_frac_part;    //bmp280推算出的相对高度小数部分，单位是0.1米
extern char *temp_sign;                //温度符号，默认正号不显示
extern uint16_t temp_labs_int;         //温度整数部分，单位是摄氏度
extern uint16_t temp_labs_frac;        //温度小数部分，单位是0.01摄氏度
/*qmc6309*/
extern QMC6309_t qmc6309;
/*RTC*/
extern Type_Struct_Timezone_and_UTCxTime Struct_RTC;


extern int8_t ui_root;//家
extern uint8_t ui_switch_window_flag;

#endif // !__MAIN_H