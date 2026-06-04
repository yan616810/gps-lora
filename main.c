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
volatile uint8_t lora_cnt=100;//LoRa数据转换，用于OLED显示位置共享界面所需要的各个节点的方位和距离

/*u8g2*/
u8g2_t u8g2;
char u8g2_buf[25];
/*GPS*/
GPS_t    gps           = {0};  // 全局 GPS 实例
/*bmp280*/
BMP280_t bmp280        = {0};        // 全局 BMP280 实例
/*qmc6309*/
QMC6309_t qmc6309={0};
/*RTC*/
Type_Struct_Timezone_and_UTCxTime Struct_RTC={0};//作用：1.用于首次初始RTC；2.用于读取RTC时间戳并保存转换得到的已修正月份/年份偏移的细分时间；
/*LoRa*/
LoRa_t lora = {0}; // 全局 LoRa 实例，初始化为0


int8_t ui_root=0;//家
uint8_t ui_switch_window_flag=0;//0:关闭切换界面 1:弹出切换界面
/*LoRa界面及其子界面*/
int8_t ui_lora=0;//LoRa位置全览图界面内的子界面，0是默认的全览图，1是显示具体节点信息,2显示该节点的导航界面
uint8_t lora_ui_last_node_display_2_flag=0;//0表示不触发显示节点详细信息；1表示传给LORA_UI触发函数内部切换一个节点详细信息节点来显示
uint8_t lora_ui_next_node_display_2_flag=0;//0表示不触发显示节点详细信息


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
		if(lora_cnt<100)lora_cnt++;
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
    }
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
    if(GPS_cnt==100)
    {
		GPS_cnt=0;
		GPS_lwgps_parser_lwrb(&gps);
		UI_GPS_display_earth_data_proc();//函数内判断秒输出是否变化，来决定是否将gps结构体中的经纬度转换成OLED可显示的格式；转换频率不需要太高，GPS报文输出频率才1HZ；

		// printf("fix: %d, sats_in_use: %d, num_sats_in_view: %d\r\n",
		// 	gps.lwgps_handle.fix,
		// 	gps.lwgps_handle.sats_in_use,
		// 	gps.lwgps_handle.sats_in_view);
    }
	if(bmp280_cnt==200)//5HZ
	{
		bmp280_cnt=0;
		if(BMP280_Get_PressureTemperature_ADC(&bmp280) == 0)
		{
			BMP280_Get_Temperature_ture_int32(&bmp280);
			BMP280_Get_Pressure_ture_int32(&bmp280);
			UI_BPM280_data_proc();//温度
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
	if(RTC_cnt==50)//放在OLED_cnt上面，保证刚上电时，显示的时间是从RTC读出来的，而不是初始时的默认的结构体参数UTC+8 2004-08-01 18:23:45
	{
		RTC_cnt=0;
		GPS_RTC_time_task(&Struct_RTC);
		// printf("Timestamp = %lu\r\n", RTC_read_Timestamp());
	}
	if(OLED_cnt==30)
	{
		OLED_cnt=0;
		UI_OLED_display(&u8g2, ui_root);
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
	LoRa_init(9600, 9600);
    // usart1_init();
    printf("USART1 initialized successfully!\r\n");
/*LoRa*/
	lora.LoRa_node_online_flag[2]=1;//模拟测试，默认节点2在线
	lora.node[2].LoRa_id=2;
	lora.node[2].latitude=36726339;
	lora.node[2].longitude=115529936;
	lora.node[2].speed=100;
	lora.node[2].pressure_pa=101325;

	lora.LoRa_node_online_flag[5]=1;//模拟测试，默认节点5在线
	lora.node[5].LoRa_id=5;
	lora.node[5].latitude=36720339;
	lora.node[5].longitude=115536936;
	lora.node[5].speed=40;
	lora.node[5].pressure_pa=91325;
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
	uint16_t error_code;
	if((error_code = BMP280_Init(&bmp280,BMP280_HANDHELD_DEVICE_LOW_POWER,0x77,IIC_Read_Len,IIC_Write_Len,NULL)))
	{
		printf("BMP280 initialized failed!\r\n");
		u8g2_ClearBuffer(&u8g2);
		sprintf(u8g2_buf, "BMP280 Not Init-%d",error_code);
		u8g2_DrawStr(&u8g2,0,0,u8g2_buf);
		
		// u8g2_SetFont(&u8g2,u8g2_font_boutique_bitmap_9x9_t_all);
		u8g2_SetFont(&u8g2,u8g2_font_helvB08_tr);
    	u8g2_SetFontPosBaseline(&u8g2);
    	u8g2_SetFontMode(&u8g2,0);  //显示字体的背景，不透明
    	u8g2_SetDrawColor(&u8g2,1);
		
		u8g2_DrawBox(&u8g2,0,29,128,15);
		u8g2_SetDrawColor(&u8g2,0);
		// u8g2_DrawStr(&u8g2,3*7,3*10,":(按任意键关机:(");
		u8g2_DrawStr(&u8g2,7,40,"Press any key to OFF:(");
		u8g2_SendBuffer(&u8g2);
		while(1)//按任意键关机
		{
			if((GPIOB->IDR & GPIO_Pin_15)==0) GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
			else if((GPIOB->IDR & GPIO_Pin_14)==0) GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
			else if((GPIOB->IDR & GPIO_Pin_13)==0) GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
		}
	}
	else printf("BMP280 initialized successfully!\r\n");
/*qmc6309*/
	wmm_init();//磁偏角库初始化	
	if((error_code = QMC6309_Init(&qmc6309, IIC_Read_Len, IIC_Write_Len, NULL)))//自测试会延时共170ms
	{
		printf("QMC6309 initialized failed! error code: %d\r\n", error_code);
		u8g2_ClearBuffer(&u8g2);
		sprintf(u8g2_buf, "QMC6309 Not Init-%d",error_code);
		u8g2_DrawStr(&u8g2,0,0,u8g2_buf);

		u8g2_SetFont(&u8g2,u8g2_font_helvB08_tr);
    	u8g2_SetFontPosBaseline(&u8g2);
    	u8g2_SetFontMode(&u8g2,0);  //显示字体的背景，不透明
    	u8g2_SetDrawColor(&u8g2,1);
		
		u8g2_DrawBox(&u8g2,0,29,128,15);
		u8g2_SetDrawColor(&u8g2,0);
		u8g2_DrawStr(&u8g2,7,40,"Press any key to OFF:(");

		u8g2_SendBuffer(&u8g2);
		while(1)//按任意键关机
		{
			if((GPIOB->IDR & GPIO_Pin_15)==0) GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
			else if((GPIOB->IDR & GPIO_Pin_14)==0) GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
			else if((GPIOB->IDR & GPIO_Pin_13)==0) GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_RESET);
		}
	}
	else printf("QMC6309 initialized successfully!\r\n");

/*定时器6初始化，周期1ms，用于任务调度*/    
    timer6_init();
    for(;;)
    {
        // rx_data_proc();
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