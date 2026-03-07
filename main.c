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
#include "LCD.h"
//单色屏幕u8g2图形库
#include "u8g2.h"
#include "u8g2_monochrome_display.h"
#include <string.h> //memset
//GPS
#include "GPS.h"
//bmp280
#include "bmp280.h"
//qmc6309
#include "lib/WMM_Tiny/Core/Inc/wmm.h"

volatile uint8_t key_cnt=10;
volatile uint16_t second_cnt=500;
volatile uint8_t bmp280_cnt=100;//bmp280读取计数器，达到一定值后读取一次bmp280数据
/*u8g2*/
u8g2_t u8g2;
char u8g2_buf[20];
/*GPS*/
GPS_t gps={0};          // 全局 GPS 实例
uint8_t earth_flag=0;//是否以全球缩略图的形式显示实时坐标 1:文本形式 0:全球缩略图形式
/*bmp280*/
BMP280_t bmp280={0};          // 全局 BMP280 实例
float fake_sea_level_pressure = 103019.0f; // 相对标准大气压，单位是Pa


/**
 * @brief 定时器6用于任务调度，周期为1ms
 * 
 */
void timer6_init(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6,ENABLE);
    TIM_InternalClockConfig(TIM6);
    {
        TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

        TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);

        TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
        TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
        TIM_TimeBaseInitStruct.TIM_Period=1000-1;//1ms
        TIM_TimeBaseInitStruct.TIM_Prescaler=(72-1);
        TIM_TimeBaseInit(TIM6,&TIM_TimeBaseInitStruct);
    }

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    {
        NVIC_InitTypeDef NVIC_InitStruct;
        NVIC_InitStruct.NVIC_IRQChannel = TIM6_IRQn;
        NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 15;
        NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
        NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStruct);
    }
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);

    TIM_Cmd(TIM6,ENABLE);
}

void TIM6_IRQHandler (void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET)
    {
        if(key_cnt<10)key_cnt++;
        if(second_cnt<500)second_cnt++;
		if(bmp280_cnt<100)bmp280_cnt++;
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}
/**
 * @brief 按键任务，检测到按键事件后执行相应的操作
 * 
 */
void key_task(void)
{
    if(key_value == 'L')
    {
        printf("Long press detected!\r\n");
        earth_flag = (earth_flag == 0) ? 1 : 0; // 切换GPS显示模式
        if (gps.lwgps_handle.is_valid) 
		{
            // printf("[--YLAD--]\r\n");
			if(earth_flag)//以文本形式显示实时坐标
			{
				u8g2_SetFont(&u8g2,u8g2_font_courB08_tr);  //w=7  h=10
				u8g2_SetFontPosTop(&u8g2);
				u8g2_SetFontMode(&u8g2,0);  //显示字体的背景，不透明
				u8g2_SetDrawColor(&u8g2,1);
				// 可选：解析结果输出
    			// if (lwgps_handle.is_valid) {
    			    // printf("Lat: %.6f, Lon: %.6f, Alt: %.4f\r\n",
    			    //     latitude, longitude, altitude);
    			// }
				u8g2_ClearBuffer(&u8g2);
				memset(u8g2_buf, 0, sizeof(u8g2_buf));
				// sprintf(u8g2_buf,"[>Lon:%03.7f<]",lwgps_handle.latitude);
				int32_t var_int = (int32_t)(gps.lwgps_handle.latitude * 1000000);//小数点后6位可以精确到​约0.11 米
				int32_t var_decimal_temp = var_int%1000000;//取出小数点后6位
				uint32_t var_decimal = var_decimal_temp<0 ? -var_decimal_temp : var_decimal_temp;
				sprintf(u8g2_buf,"[Lat:%+d.%06lu]",(int16_t)(var_int/1000000),var_decimal);
				u8g2_SetDrawColor(&u8g2,0);
				u8g2_DrawBox(&u8g2,0*7,0*10,18*7,10);
				u8g2_SetDrawColor(&u8g2,1);
				u8g2_DrawStr(&u8g2,0*7,0*10,u8g2_buf);

				memset(u8g2_buf, 0, sizeof(u8g2_buf));
				// sprintf(u8g2_buf,"[>Lon:%03.7f<]",lwgps_handle.longitude);
				var_int = (int32_t)(gps.lwgps_handle.longitude * 1000000);//小数点后6位可以让低纬度地区精确到​约0.11 米，高纬度地区更精确
				var_decimal_temp = var_int%1000000;//取出小数点后6位
				var_decimal = var_decimal_temp<0 ? -var_decimal_temp : var_decimal_temp;
				sprintf(u8g2_buf,"[Lon:%+d.%06lu]",(int16_t)(var_int/1000000),var_decimal);
				u8g2_SetDrawColor(&u8g2,0);
				u8g2_DrawBox(&u8g2,0*7,1*10,18*7,10);
				u8g2_SetDrawColor(&u8g2,1);
				u8g2_DrawStr(&u8g2,0*7,1*10,u8g2_buf);

				memset(u8g2_buf, 0, sizeof(u8g2_buf));
				// sprintf(u8g2_buf,"[>Alt:%03.5f<]", lwgps_handle.altitude);
				var_int = (int32_t)(gps.lwgps_handle.altitude * 100);//小数点后2位可以精确到厘米
				var_decimal_temp = var_int%100;//取出小数点后2位
				var_decimal = var_decimal_temp<0 ? -var_decimal_temp : var_decimal_temp;
				sprintf(u8g2_buf,"[Alt:%+d.%02lu]",(int16_t)(var_int/100),var_decimal);
				u8g2_SetDrawColor(&u8g2,0);
				u8g2_DrawBox(&u8g2,0*7,2*10,18*7,10);
				u8g2_SetDrawColor(&u8g2,1);
				u8g2_DrawStr(&u8g2,0*7,2*10,u8g2_buf);

				memset(u8g2_buf, 0, sizeof(u8g2_buf));
				var_int = (int32_t)(gps.lwgps_handle.variation * 100);//小数点后2位
				var_decimal_temp = var_int%100;//取出小数点后2位
				var_decimal = var_decimal_temp<0 ? -var_decimal_temp : var_decimal_temp;
				sprintf(u8g2_buf,"[Mag:%+d.%02lu]",(int16_t)var_int/100,var_decimal);
				u8g2_SetDrawColor(&u8g2,0);
				u8g2_DrawBox(&u8g2,0*7,3*10,18*7,10);
				u8g2_SetDrawColor(&u8g2,1);
				u8g2_DrawStr(&u8g2,0*7,3*10,u8g2_buf);

				// u8g2_SendBuffer(&u8g2);
			}
			else{//全球缩略图
				u8g2_ClearBuffer(&u8g2);
				u8g2_oled_draw_earth(&u8g2);//在全幅缓冲区内绘制全球缩略图
				u8g2_oled_draw_earth_pixel_VHxvLine(&u8g2,gps.lwgps_handle.latitude,gps.lwgps_handle.longitude);//在全球缩略图上绘制实时经纬度坐标点
				// u8g2_SendBuffer(&u8g2);
			}
				
		}
		else
		{
			// u8g2_ClearBuffer(&u8g2);
			memset(u8g2_buf, 0, sizeof(u8g2_buf));
			sprintf(u8g2_buf,"[>GPS No Data<]");
			u8g2_SetDrawColor(&u8g2,1);
			u8g2_DrawBox(&u8g2,3*7,27,13*7,15);
			u8g2_SetDrawColor(&u8g2,0);
			u8g2_DrawStr(&u8g2,3*7,3*10,u8g2_buf);
			// u8g2_SendBuffer(&u8g2);
		}

    }
    else if(key_value == 'D')
    {
        printf("Double press detected!\r\n");
        oled_image_hongzhong(85);
    }
    else if(key_value == 2)
    {
        printf("Key 2 pressed!\r\n");
        oled_image_leige(85);

        // LCD_ShowSnow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
    }
    else if(key_value == 3)
    {
        printf("Key 3 pressed!\r\n");
        oled_image_binbin();

        // u8g2_ClearBuffer(&u8g2);
        // u8g2_oled_draw_earth(&u8g2);
        // u8g2_SendBuffer(&u8g2);

        // LCD_DrawLine(0,0,200,200,0x0000);//画一条黑色斜线
        // LCD_DrawRect(50,50,150,150,0xf800);//画一个红色矩形
    }
    else if(key_value == 4)
    {
        printf("Key 4 pressed!\r\n");
        oled_image_jinxin(85);
		
        // LCD_FillRect(0,50,85,150,0xc88c);
        // LCD_FillRect(50,100,120,200,0x57f6);

		fake_sea_level_pressure = bmp280.Pressure_ture;//将当前的气压读数作为相对标准大气压，这样可以得到相对于当前环境的高度变化，适合手持设备使用
    }
    key_value=0;
}

void task_proc(void)
{
    if(key_cnt==10)
    {
        key_cnt=0;
		get_key();
		if(key_value != 0)
		{
			key_task();
		}
    }
    if(second_cnt==500)
    {
		second_cnt=0;
		GPS_lwgps_parser_lwrb(&gps);
        if (gps.lwgps_handle.is_valid) 
		{
			if(earth_flag)//以文本形式显示实时坐标
			{
				u8g2_SetFont(&u8g2,u8g2_font_courB08_tr);  //w=7  h=10
				u8g2_SetFontPosTop(&u8g2);
				u8g2_SetFontMode(&u8g2,0);  //显示字体的背景，不透明
				u8g2_SetDrawColor(&u8g2,1);

				// 可选：解析结果输出
    			// if (lwgps_handle.is_valid) {
    			    // printf("Lat: %.6f, Lon: %.6f, Alt: %.4f\r\n",
    			    //     latitude, longitude, altitude);
    			// }
				u8g2_ClearBuffer(&u8g2);
				memset(u8g2_buf, 0, sizeof(u8g2_buf));
				// sprintf(u8g2_buf,"[>Lon:%03.7f<]",lwgps_handle.latitude);
				int32_t var_int = (int32_t)(gps.lwgps_handle.latitude * 1000000);//小数点后6位可以精确到​约0.11 米
				int32_t var_decimal_temp = var_int%1000000;//取出小数点后6位
				uint32_t var_decimal = var_decimal_temp<0 ? -var_decimal_temp : var_decimal_temp;
				sprintf(u8g2_buf,"[Lat:%+d.%06lu]",(int16_t)gps.lwgps_handle.latitude,var_decimal);
				u8g2_SetDrawColor(&u8g2,0);
				u8g2_DrawBox(&u8g2,0*7,0*10,18*7,10);
				u8g2_SetDrawColor(&u8g2,1);
				u8g2_DrawStr(&u8g2,0*7,0*10,u8g2_buf);

				memset(u8g2_buf, 0, sizeof(u8g2_buf));
				// sprintf(u8g2_buf,"[>Lon:%03.7f<]",lwgps_handle.longitude);
				var_int = (int32_t)(gps.lwgps_handle.longitude * 1000000);//小数点后6位可以让低纬度地区精确到​约0.11 米，高纬度地区更精确
				var_decimal_temp = var_int%1000000;//取出小数点后6位
				var_decimal = var_decimal_temp<0 ? -var_decimal_temp : var_decimal_temp;
				sprintf(u8g2_buf,"[Lon:%+d.%06lu]",(int16_t)gps.lwgps_handle.longitude,var_decimal);
				u8g2_SetDrawColor(&u8g2,0);
				u8g2_DrawBox(&u8g2,0*7,1*10,18*7,10);
				u8g2_SetDrawColor(&u8g2,1);
				u8g2_DrawStr(&u8g2,0*7,1*10,u8g2_buf);

				memset(u8g2_buf, 0, sizeof(u8g2_buf));
				// sprintf(u8g2_buf,"[>Alt:%03.5f<]", lwgps_handle.altitude);
				var_int = (int32_t)(gps.lwgps_handle.altitude * 100);//小数点后2位可以精确到厘米
				var_decimal_temp = var_int%100;//取出小数点后2位
				var_decimal = var_decimal_temp<0 ? -var_decimal_temp : var_decimal_temp;
				sprintf(u8g2_buf,"[Alt:%+d.%02lu]",(int16_t)gps.lwgps_handle.altitude,var_decimal);
				u8g2_SetDrawColor(&u8g2,0);
				u8g2_DrawBox(&u8g2,0*7,2*10,18*7,10);
				u8g2_SetDrawColor(&u8g2,1);
				u8g2_DrawStr(&u8g2,0*7,2*10,u8g2_buf);

				// memset(u8g2_buf, 0, sizeof(u8g2_buf));
				// var_int = (int32_t)(gps.lwgps_handle.variation * 100);//小数点后2位
				// var_decimal_temp = var_int%100;//取出小数点后2位
				// var_decimal = var_decimal_temp<0 ? -var_decimal_temp : var_decimal_temp;
				// sprintf(u8g2_buf,"[Mag:%+d.%02lu]",(int16_t)var_int/100,var_decimal);
				// u8g2_SetDrawColor(&u8g2,0);
				// u8g2_DrawBox(&u8g2,0*7,3*10,18*7,10);
				// u8g2_SetDrawColor(&u8g2,1);
				// u8g2_DrawStr(&u8g2,0*7,3*10,u8g2_buf);

				float Date_WMM = wmm_get_date(gps.lwgps_handle.year % 100, gps.lwgps_handle.month, gps.lwgps_handle.date);
				float Magnetic_variation;
				E0000(gps.lwgps_handle.latitude, gps.lwgps_handle.longitude, Date_WMM, &Magnetic_variation);
				int32_t Magnetic_variation_frac_part = (int32_t)((Magnetic_variation - (int32_t)Magnetic_variation) * 100);
				if (Magnetic_variation_frac_part < 0) Magnetic_variation_frac_part = -Magnetic_variation_frac_part;
				memset(u8g2_buf, 0, sizeof(u8g2_buf));
				sprintf(u8g2_buf,"[Mag:%+d.%02u]",(int16_t)Magnetic_variation,(uint16_t)Magnetic_variation_frac_part);
				u8g2_SetDrawColor(&u8g2,0);
				u8g2_DrawBox(&u8g2,0*7,3*10,18*7,10);
				u8g2_SetDrawColor(&u8g2,1);
				u8g2_DrawStr(&u8g2,0*7,3*10,u8g2_buf);


				// u8g2_SendBuffer(&u8g2);
			}
			else{//全球缩略图
				u8g2_ClearBuffer(&u8g2);
				u8g2_oled_draw_earth(&u8g2);//在全幅缓冲区内绘制全球缩略图
				u8g2_oled_draw_earth_pixel_VHxvLine(&u8g2,gps.lwgps_handle.latitude,gps.lwgps_handle.longitude);//在全球缩略图上绘制实时经纬度坐标点
				// u8g2_SendBuffer(&u8g2);
			}
				
		}
		else
		{
			// u8g2_ClearBuffer(&u8g2);
			memset(u8g2_buf, 0, sizeof(u8g2_buf));
			sprintf(u8g2_buf,"[>GPS No Data<]");
			u8g2_SetDrawColor(&u8g2,1);
			u8g2_DrawBox(&u8g2,3*7,27,13*7,15);
			u8g2_SetDrawColor(&u8g2,0);
			u8g2_DrawStr(&u8g2,3*7,3*10,u8g2_buf);
			// u8g2_SendBuffer(&u8g2);
		}

    }
	if(bmp280_cnt==100)
	{
		bmp280_cnt=0;
		if(BMP280_Get_PressureTemperature_ADC(&bmp280) == 0)
		{
			BMP280_Get_Temperature_ture_int32(&bmp280);
			BMP280_Get_Pressure_ture_int32(&bmp280);

			// int32_t temp_fixed = bmp280.Temperature_ture;
			// int32_t int_part = temp_fixed / 100;
			// int32_t frac_part = temp_fixed - (int_part * 100);   // 避免 % 的实现差异
			// if (frac_part < 0) frac_part = -frac_part;
			char temp_sign = (bmp280.Temperature_ture >= 0) ? '+' : '-';
			uint32_t temp_fixed = (bmp280.Temperature_ture >= 0) ? (uint32_t)(bmp280.Temperature_ture) : (uint32_t)(-bmp280.Temperature_ture);
			uint16_t temp_int_part = temp_fixed / 100;
			uint16_t temp_frac_part = temp_fixed % 100;

			// int32_t altitude = 100*calculate_altitude(bmp280.Pressure_ture, fake_sea_level_pressure);//单位是厘米，输出值5123表示51.23米
			// int32_t altitude_int_part = altitude / 100;
			// int32_t altitude_frac_part = altitude - (altitude_int_part * 100);   // 避免 % 的实现差异
			// if (altitude_frac_part < 0) altitude_frac_part = -altitude_frac_part;
			int32_t altitude = 100*calculate_altitude(bmp280.Pressure_ture, fake_sea_level_pressure);//单位是厘米，输出值5123表示51.23米
			char altitude_sign = (altitude >= 0) ? '+' : '-';
			uint32_t altitude_fixed = (altitude >= 0) ? (uint32_t)(altitude) : (uint32_t)(-altitude);
			uint16_t altitude_int_part = altitude_fixed / 100;
			uint16_t altitude_frac_part = altitude_fixed % 100;
			//输出到串口
			printf("BMP280 Read Success! Temperature: %c%u.%02u C, Pressure: %lu Pa, Altitude: %c%u.%02u m\r\n",
					temp_sign,
			    	temp_int_part,
			    	temp_frac_part,
			    	bmp280.Pressure_ture,
			    	altitude_sign,
			    	altitude_int_part,
			    	altitude_frac_part
			);
			//显示在OLED上
			memset(u8g2_buf, 0, sizeof(u8g2_buf));
			sprintf(u8g2_buf,"[R-H:%c%u.%02um]", altitude_sign, altitude_int_part, altitude_frac_part);//相对高度，最大相对高度65535m
			u8g2_SetDrawColor(&u8g2,0);
			u8g2_DrawBox(&u8g2,0*7,4*10,18*7,10);
			u8g2_SetDrawColor(&u8g2,1);
			u8g2_DrawStr(&u8g2,0*7,4*10,u8g2_buf);
			u8g2_SendBuffer(&u8g2);
		}
		else printf("BMP280 Read error!\r\n");
	}
}

int main(void)
{
/*POWER-EN Configure PC13开机*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure={
        .GPIO_Pin = GPIO_Pin_13,
        .GPIO_Mode = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_2MHz
    };
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET); // Set PC13 high to turn on the power
/*usart1连接到CH340初始化*/
    usart1_init();
    printf("USART1 initialized successfully!\r\n");
/*按键初始化*/
    key_init();
/*软件IIC初始化，搜索挂载的iic设备数*/
    IIC_InitPins_or_ChangePins(RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_6,RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_7);   
    IIC_Set_speed(5);
    IIC_Search_all_devices_printf_example();
    IIC_Set_speed(1);
/*OLED显示初始化*/
    // OLED_Init();
    // oled_image_yanhui();
/*LCD显示初始化*/
    // LCD_Init_All();
    // LCD_Clear(BLACK);
/*LWGPS*/
	GPS_init(&gps);               //初始化一个GPS所依赖的软硬件环境
	/*执行到这里是DMA已经可以自动从uart4接收数据并自动拷贝到LWRB的环形缓冲区中*/
	printf("GPS init success!\r\n");
/*qmc6309*/
	wmm_init();
	// if(gps.lwgps_handle.is_valid)
	// {
	// 	float Date_WMM = wmm_get_date(gps.lwgps_handle.year % 100, gps.lwgps_handle.month, gps.lwgps_handle.date);
	// 	float Magnetic_variation;
	// 	E0000(gps.lwgps_handle.latitude, gps.lwgps_handle.longitude, Date_WMM, &Magnetic_variation);

	// 	int32_t Magnetic_variation_frac_part = (int32_t)((Magnetic_variation - (int32_t)Magnetic_variation) * 100);
	// 	if (Magnetic_variation_frac_part < 0) Magnetic_variation_frac_part = -Magnetic_variation_frac_part;
	// 	printf("WMM magnetic declination calculated successfully! Declination: %+d.%02d degrees\r\n", (int32_t)Magnetic_variation, Magnetic_variation_frac_part);
	// }
	// else{//使用主控内部RTC日期

	// }
/*bmp280*/
	if(BMP280_Init(&bmp280,BMP280_HANDHELD_DEVICE_LOW_POWER,0x77,IIC_Read_Len,IIC_Write_Len,NULL))
	{
		printf("BMP280 init failed!\r\n");
		while(1);
	}
	else
	{

		printf("BMP280 init success!\r\n");
	}
/*u8g2单色屏初始化*/
	u8g2_oled_init(&u8g2);
	u8g2_oled_play_Animation(&u8g2);
    
	u8g2_SetFont(&u8g2,u8g2_font_courB08_tr);  //w=7  h=10
	u8g2_SetFontPosTop(&u8g2);
	u8g2_SetFontMode(&u8g2,0);  //显示字体的背景，不透明
	u8g2_SetDrawColor(&u8g2,1);
	u8g2_ClearDisplay(&u8g2);
    
    // u8g2_oled_draw_earth(&u8g2);

	// u8g2_SendBuffer(&u8g2);
/*定时器6初始化，周期1ms，用于任务调度*/    
    timer6_init();
    for(;;)
    {
        rx_data_proc();
        task_proc();
    }
    // GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET); // Set PC13 Low
}

//用函数指针替代
void rx_data_procCallback(uint16_t cmd_num)//Starting from 0
{
	switch(cmd_num)
	{
		case 0://cmd1
			printf("rx_buff=cmd1 -> Ctrl_1\r\n");
			break;
		case 1://cmd2
			printf("rx_buff=cmd2 -> Ctrl_2\r\n");
			break;
		case 2://cmd3
			printf("rx_buff=cmd3 -> Ctrl_3\r\n");
			break;
		case 3://help
			printf("rx_buff=help -> I will help you!\r\n");
			break;
		case 4://YLAD
			printf("\
[===============YLAD=============]\r\n\
[================================]\r\n\
[*****欢迎来到无人机租赁公司*******]\r\n\
[1.关于公司。。。                 ]\r\n\
[2.立刻注册免费租一架试玩10min!    ]\r\n\
[3.更多功能敬请期待！             ]\r\n\
");
			break;
		default:break;
	}
}