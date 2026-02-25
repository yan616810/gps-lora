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
//key
#include "key.h"
//LCD
#include "LCD.h"
/*单色屏幕u8g2图形库*/
#include "u8g2.h"
#include "u8g2_monochrome_display.h"
#include <string.h> //memset


volatile uint8_t key_cnt=10;
/*u8g2*/
u8g2_t u8g2;
char u8g2_buf[20];


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
        oled_image_meinv(117);

        // LCD_Clear(BLUE);
        // Delay_s(1);
        // LCD_Clear(BRED);
        // Delay_s(1);
        // LCD_Clear(GRED);
        // Delay_s(1);
        // LCD_Clear(GBLUE);
        // Delay_s(1);
        // LCD_Clear(RED);
        // Delay_s(1);
        // LCD_Clear(MAGENTA);
        // Delay_s(1);
        // LCD_Clear(GREEN);
        // Delay_s(1);
        // LCD_Clear(CYAN);
        // Delay_s(1);
        // LCD_Clear(YELLOW);
        // Delay_s(1);
        // LCD_Clear(BROWN);
        // Delay_s(1);
        // LCD_Clear(BRRED);
        // Delay_s(1);
        // LCD_Clear(GRAY);
        // Delay_s(1);
        // LCD_Clear(GRAY25);
        // Delay_s(1);
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

        LCD_ShowSnow(0,0,LCD_WIDTH-1,LCD_HEIGHT-1);
    }
    else if(key_value == 3)
    {
        printf("Key 3 pressed!\r\n");
        oled_image_binbin();

        // u8g2_ClearBuffer(&u8g2);
        // u8g2_oled_draw_earth(&u8g2);
        // u8g2_SendBuffer(&u8g2);

        LCD_DrawLine(0,0,200,200,0x0000);//画一条黑色斜线
        LCD_DrawRect(50,50,150,150,0xf800);//画一个红色矩形
    }
    else if(key_value == 4)
    {
        printf("Key 4 pressed!\r\n");
        oled_image_jinxin(85);

        LCD_FillRect(0,50,85,150,0xc88c);
        LCD_FillRect(50,100,120,200,0x57f6);
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
}

int main(void)
{
/*POWER-EN Configure PC13开机*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_WriteBit(GPIOC, GPIO_Pin_13, Bit_SET); // Set PC13 high to turn on the power
/*按键初始化*/
    key_init();
/*usart1连接到CH340初始化*/
    usart1_init();
    printf("USART1 initialized successfully!\r\n");
/*软件IIC初始化，搜索挂载的iic设备数*/
    IIC_InitPins_or_ChangePins(RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_6,RCC_APB2Periph_GPIOB,GPIOB,GPIO_Pin_7);   
    IIC_Set_speed(5);
    IIC_Search_all_devices_printf_example();
    IIC_Set_speed(1);
/*OLED显示初始化*/
    // OLED_Init();
    // oled_image_yanhui();
/*LCD显示初始化*/
    LCD_Init_All();
    LCD_Clear(BLACK);
/*u8g2单色屏初始化*/
	u8g2_oled_init(&u8g2);
	u8g2_oled_play_Animation(&u8g2);
    
	u8g2_SetFont(&u8g2,u8g2_font_courB08_tr);//w=7  h=10
	u8g2_SetFontPosTop(&u8g2);
	u8g2_SetFontMode(&u8g2,0);//显示字体的背景，不透明
	u8g2_SetDrawColor(&u8g2,1);
	u8g2_ClearDisplay(&u8g2);
    
    u8g2_oled_draw_earth(&u8g2);

	// u8g2_DrawStr(&u8g2,0,0*10,"X=");
	// u8g2_DrawStr(&u8g2,0,1*10,"Y=");
	// u8g2_DrawStr(&u8g2,0,2*10,"Z=");
	// u8g2_DrawStr(&u8g2,0,3*10,"Gyro");
	// u8g2_DrawStr(&u8g2,9*7,0*10,"X=");
	// u8g2_DrawStr(&u8g2,9*7,1*10,"Y=");
	// u8g2_DrawStr(&u8g2,9*7,2*10,"Z=");
	// u8g2_DrawStr(&u8g2,9*7,3*10,"Acce");
	// u8g2_DrawStr(&u8g2,0,5*10,"Tem->");

	u8g2_SendBuffer(&u8g2);
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