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
//LoRa
#include "LoRa.h"

/*u8g2*/
extern u8g2_t u8g2;
extern char u8g2_buf[25];
/*GPS*/
extern GPS_t gps;               // 全局 GPS 实例
/*bmp280*/
extern BMP280_t bmp280;                // 全局 BMP280 实例
/*qmc6309*/
extern QMC6309_t qmc6309;
/*RTC*/
extern Type_Struct_Timezone_and_UTCxTime Struct_RTC;


extern int8_t ui_root;//家
extern uint8_t ui_switch_window_flag;
extern int8_t ui_lora;//LoRa位置全览图界面内的子界面，0是默认的全览图，1是显示具体节点信息
extern uint8_t lora_ui_last_node_display_2_flag;
extern uint8_t lora_ui_next_node_display_2_flag;

#endif // !__MAIN_H