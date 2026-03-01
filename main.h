#ifndef __MAIN_H
#define __MAIN_H
#include "stm32f10x.h"
#include "GPS.h"
#include "u8g2.h"

extern GPS_t gps;          // 全局 GPS 实例

extern u8g2_t u8g2;
extern char u8g2_buf[20];


#endif // !__MAIN_H