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
#include "main.h"

volatile uint8_t key_cnt=10;
volatile uint16_t GPS_cnt=100;
volatile uint8_t bmp280_cnt=200;//bmp280读取计数器，达到一定值后读取一次bmp280数据
volatile uint8_t qmc6309_cnt=20;//qmc6309读取计数器，达到一定值后读取一次qmc6309数据
volatile uint8_t OLED_cnt=30;//帧率33HZ，传感器永远按自己的节奏采集，OLED 永远按自己的节奏刷新；采集和显示完全解耦
volatile uint8_t RTC_cnt=50;//GPS同步RTC；从RTC计数器中的时间戳得到年月日等；

/*u8g2*/
u8g2_t u8g2;
char u8g2_buf[25];
/*GPS*/
GPS_t    gps           = {0};  // 全局 GPS 实例
uint8_t  earth_flag    = 1;    //是否以全球缩略图的形式显示实时坐标 1:文本形式 0:全球缩略图形式
/*bmp280*/
BMP280_t bmp280                  = {0};        // 全局 BMP280 实例
float    fake_sea_level_pressure = 103019.0f;  //相对标准大气压，单位是Pa
char     altitude_sign           = '+';        //bmp280推算出的相对高度符号，默认正号
uint16_t altitude_int_part       = 0;          //bmp280推算出的相对高度整数部分，单位是米
uint16_t altitude_frac_part      = 0;          //bmp280推算出的相对高度小数部分，单位是0.1米
char     *temp_sign              = "";         //温度符号，默认正号不显示
uint16_t temp_labs_int           = 0;          //温度整数部分，单位是摄氏度
uint16_t temp_labs_frac          = 0;          //温度小数部分，单位是0.01摄氏度
/*qmc6309*/
QMC6309_t qmc6309={0};
/*RTC*/
Type_Struct_Timezone_and_UTCxTime Struct_RTC={0};//作用：1.用于首次初始RTC；2.用于读取RTC时间戳并保存转换得到的已修正月份/年份偏移的细分时间；


uint8_t ui_root=0;//家


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
        if(GPS_cnt<100)GPS_cnt++;
		if(bmp280_cnt<200)bmp280_cnt++;
		if(qmc6309_cnt<20)qmc6309_cnt++;
		if(OLED_cnt<30)OLED_cnt++;
		if(RTC_cnt<50)RTC_cnt++;
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
}
// /**
//  * @brief 按键任务，检测到按键事件后执行相应的操作
//  * 
//  */
// void key_task(void)
// {
//     if(key_value == 'L')
//     {
//         printf("Long press detected!\r\n");
// 		switch (ui_root)
// 		{
// 			case 0://家界面
// 				ui_root=1;//切换到GPS文本界面
// 				break;
// 			case 1://GPS文本界面
// 				ui_root=0;//切换到家界面
// 				break;
// 			default:
// 				break;
// 		}
//     }
//     else if(key_value == 'D')
//     {
//         printf("Double press detected!\r\n");
//         // oled_image_hongzhong(85);
// 		switch (ui_root)
// 		{
// 			case 0://家界面
// 				break;
// 			case 1://GPS界面
// 				break;
// 			default:
// 				break;
// 		}
//     }
//     else if(key_value == 2)
//     {
//         printf("Key 2 pressed!\r\n");
//         // oled_image_leige(85);
//         // LCD_ShowSnow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
// 		switch (ui_root)
// 		{
// 			case 0://家界面
// 				break;
// 			case 1://GPS界面
// 				// earth_flag = (earth_flag == 0) ? 1 : 0; // 切换GPS显示模式
// 				earth_flag = 0; // 切换GPS显示模式
// 				break;
// 			default:
// 				break;
// 		}
//     }
//     else if(key_value == 3)
//     {
//         printf("Key 3 pressed!\r\n");
//         // oled_image_binbin();
//         // LCD_DrawLine(0,0,200,200,0x0000);//画一条黑色斜线
//         // LCD_DrawRect(50,50,150,150,0xf800);//画一个红色矩形
// 		switch (ui_root)
// 		{
// 			case 0://家界面
// 				break;
// 			case 1://GPS界面
// 				fake_sea_level_pressure = bmp280.Pressure_ture;//将当前的气压读数作为相对标准大气压，这样可以得到相对于当前环境的高度变化，适合手持设备使用
// 				break;
// 			default:
// 				break;
// 		}
//     }
//     else if(key_value == 4)
//     {
//         printf("Key 4 pressed!\r\n");
//         // oled_image_jinxin(85);
//         // LCD_FillRect(0,50,85,150,0xc88c);
//         // LCD_FillRect(50,100,120,200,0x57f6);

// 		switch (ui_root)
// 		{
// 			case 0://家界面
// 				break;
// 			case 1://GPS界面
// 				earth_flag = 1; // 切换GPS显示模式
// 				break;
// 			default:
// 				break;
// 		}
//     }
//     key_value=0;
// }

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
    if(GPS_cnt==100)
    {
		GPS_cnt=0;
		GPS_lwgps_parser_lwrb(&gps);
		UI_GPS_display_earth_data_proc();//函数内判断秒输出是否变化，来决定是否将gps结构体中的经纬度转换成OLED可显示的格式；转换频率不需要太高，GPS报文输出频率才1HZ；

		printf("fix: %d, sats_in_use: %d, num_sats_in_view: %d\r\n",
			gps.lwgps_handle.fix,
			gps.lwgps_handle.sats_in_use,
			gps.lwgps_handle.sats_in_view);
    }
	if(bmp280_cnt==200)//5HZ
	{
		bmp280_cnt=0;
		if(BMP280_Get_PressureTemperature_ADC(&bmp280) == 0)
		{
			BMP280_Get_Temperature_ture_int32(&bmp280);
			BMP280_Get_Pressure_ture_int32(&bmp280);
		//温度只显示负号
    		temp_sign=(bmp280.Temperature_ture<0) ? "-" : "";//要用字符串
    		uint16_t temp_labs=(bmp280.Temperature_ture<0) ? -bmp280.Temperature_ture : bmp280.Temperature_ture;
			temp_labs += 5;//四舍五入，单位是0.01摄氏度
    		temp_labs_int = temp_labs/100;
			temp_labs_frac = (temp_labs%100)/10;//四舍五入保留小数点后一位
		//有符号整数int32_t转符号整数部分和小数部分
			// char temp_sign = (bmp280.Temperature_ture >= 0) ? '+' : '-';
			// uint32_t temp_labs = (bmp280.Temperature_ture >= 0) ? (uint32_t)(bmp280.Temperature_ture) : (uint32_t)(-bmp280.Temperature_ture);
			// uint16_t temp_int_part = temp_labs / 100;
			// uint16_t temp_frac_part = temp_labs % 100;
		//相对高度差，浮点float转符号整数部分和小数部分
			float altitude = calculate_altitude(bmp280.Pressure_ture, fake_sea_level_pressure);
			altitude_sign = (altitude >= 0) ? '+' : '-';
			float abs_var = fabsf(altitude);
			uint32_t temp = abs_var * 10.0f + 0.5f;//四舍五入保留小数点后两位
			altitude_int_part = temp / 10;//整数部分
			altitude_frac_part = temp % 10;//小数部分
		}
		else printf("BMP280 Read error!\r\n");
	}
	if(qmc6309_cnt==20)//50HZ
	{
		qmc6309_cnt=0;
		uint16_t error_code;
		if((error_code = QMC6309_Get_Magnetic(&qmc6309)) == 0)
		{
			QMC6309_CalibMagnetic(&qmc6309);
			QMC6309_Get_heading(&qmc6309);
			printf("qmc6309 -> x=%+d y=%+d z=%+d  heading: %d\r\n",qmc6309.x, qmc6309.y, qmc6309.z, qmc6309.heading);
		}
		else printf("QMC6309 Read error! Error code: %d\r\n", error_code);
	}
	if(RTC_cnt==50)//放在OLED_cnt其那面，保证刚向的上电时，显示的时间是从RTC读出来的，而不是初始时的默认的结构体参数UTC+8 2004-08-01 18:23:45
	{
		RTC_cnt=0;
		GPS_RTC_time_task(&Struct_RTC);
		printf("Timestamp = %lu\r\n", RTC_read_Timestamp());
	}
	if(OLED_cnt==30)
	{
		OLED_cnt=0;
		UI_OLED_display(&u8g2);
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
/*RTC*/
	RTC_Filling_DataStruct(&Struct_RTC, +800, 2004, 8, 1, 18, 23, 45, 0);//初始时间设置为UTC+8 2004-08-01 18:23:45，后续会根据GPS时间来驯服RTC时间戳
	RTC_init(&Struct_RTC);//首先读取BKP_DR1的标志位，如果标志位存在，证明备份域始终没掉电，RTC之前初始过了；否则执行RTC初始化，将Struct_RTC内的细分日期转换成时间戳，写入RTC计数器；同时会设置BKP_DR1的标志位标记防止之后复位重复初始设置RTC分频和计数寄存器，并且将Struct_RTC内的初始时区信息保存到BKP_DR2寄存器中，供后续使用
	printf("RTC initialized successfully!\r\n");
/*OLED显示初始化*/
    // OLED_Init();
    // oled_image_yanhui();
/*LCD显示初始化*/
    // LCD_Init_All();
    // LCD_Clear(BLACK);
/*LWGPS*/
	GPS_init(&gps);               //初始化一个GPS所依赖的软硬件环境
	printf("GPS initialized successfully!\r\n");/*执行到这里是DMA已经可以自动从uart4接收数据并自动拷贝到LWRB的环形缓冲区中*/
/*软件IIC初始化，搜索挂载的iic设备数*/
	IIC_InitPins_or_ChangePins(RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_6,RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_7);
	printf("SW_IIC initialized successfully!\r\n");
	Delay_ms(100);//等待外设上电稳定,这一版软件，不等上电稳定，根本无法得到iic设备的ACK回应；bmp280-2ms；qmc6309-1ms;MAX30102-1ms
    IIC_Set_speed(10);
    IIC_Search_all_devices_printf_example();
    IIC_Set_speed(1);
/*u8g2单色屏初始化*/
	u8g2_oled_init(&u8g2);
	printf("OLED_u8g2 initialized successfully!\r\n");
	// u8g2_oled_play_Animation(&u8g2);
    
	u8g2_SetFont(&u8g2,u8g2_font_courB08_tr);  //w=7  h=10
	u8g2_SetFontPosTop(&u8g2);
	u8g2_SetFontMode(&u8g2,0);  //显示字体的背景，不透明
	u8g2_SetDrawColor(&u8g2,1);

	u8g2_ClearDisplay(&u8g2);
/*bmp280*/
	if(BMP280_Init(&bmp280,BMP280_HANDHELD_DEVICE_LOW_POWER,0x77,IIC_Read_Len,IIC_Write_Len,NULL))
	{
		printf("BMP280 initialized failed!\r\n");
		u8g2_ClearBuffer(&u8g2);
		u8g2_DrawStr(&u8g2,0,0,"BMP280 Not Init");
		u8g2_SendBuffer(&u8g2);
		while(1);
	}
	else
	{

		printf("BMP280 initialized successfully!\r\n");
	}
/*qmc6309*/
	wmm_init();//磁偏角库初始化
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
	uint16_t error_code;
	if((error_code = QMC6309_Init(&qmc6309, IIC_Read_Len, IIC_Write_Len, NULL)))//自测试会延时共170ms
	{
		printf("QMC6309 initialized failed! error code: %d\r\n", error_code);
		u8g2_ClearBuffer(&u8g2);
		sprintf(u8g2_buf, "QMC6309 Not Init-%d",error_code);
		u8g2_DrawStr(&u8g2,0,0,u8g2_buf);
		u8g2_SendBuffer(&u8g2);
		while(1);
	}
	else
	{

		printf("QMC6309 initialized successfully!\r\n");
	}
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