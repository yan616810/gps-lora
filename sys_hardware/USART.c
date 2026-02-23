/*要求：用到TIM3计时器，用于超时检测;
1.115200，72MHZ，115200最慢1ms检测一次缓冲区，根据波特率与缓冲区大小决定，最好直接放在main循环中，加快串口接收数据处理速度;
2.通过宏可调整缓冲区大小
3.添加命令的话直接在指针数组中添加即可
4.默认115200，改变波特率后别忘记修改TIM3的超时检测值;2025/5/15经过思考细节，做了2倍处理，不用手动调整了;
5.回调函数使用见函数简介

使用方法：
1.usart1_init初始化usart1和TIM3
2.重写回调函数
*/
#include "USART.h"
#include "stm32f10x.h"
#include <string.h>//strlen
#include <stdio.h>//微库
#include <stddef.h>//size_t类型


#define psc                ((TIM3CLK/1000000)-1)
#define cnt                ((10000000/usart_rx_baud)-1)


#ifdef USE_std_C11
extern void rx_data_procCallback(uint16_t cmd_num);//回调函数，处理接收到的命令
#endif

//如何去重？
const char *rx_command[]={"cmd1","cmd2","cmd3","help","YLAD"};//比定义二维数组的优点是当字符串长度不一致时更节省存储空间
size_t cmd_count = sizeof(rx_command) / sizeof(rx_command[0]);


volatile uint32_t rx_buff_num_used=0;//接收缓冲区使用量
//计时器微秒级
volatile uint8_t rx_flag=0;//中断中，新接收到一个字符后在中断中置1
volatile char rx_buff[usart_rx_buff_size];

// #ifdef USE_std_C11
// 	/*在main.c中实现接收处理回调函数*/
// 	typedef void (*RxDataProcCallback_t)(uint16_t);
// 	extern RxDataProcCallback_t rx_data_procCallback();
// #endif // DEBUG

void timer3_init(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);

	TIM_InternalClockConfig(TIM3);

	{
		TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;

		TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);

		TIM_TimeBaseInitStruct.TIM_ClockDivision=TIM_CKD_DIV1;
		TIM_TimeBaseInitStruct.TIM_CounterMode=TIM_CounterMode_Up;
		TIM_TimeBaseInitStruct.TIM_Period=65536-1;//1us~65ms
		TIM_TimeBaseInitStruct.TIM_Prescaler=psc;//1us
		TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStruct);
	}
	TIM_Cmd(TIM3,ENABLE);
}

#if (usartx == 1)
	void usart1_init(void)
	{
		timer3_init();//USART的中断接收功能使用
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA,ENABLE );

	    GPIO_InitTypeDef GPIO_InitStruct;
	    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_9;
	    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	    GPIO_Init(GPIOA,&GPIO_InitStruct);
	    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_10;
	    GPIO_Init(GPIOA,&GPIO_InitStruct);

	    USART_InitTypeDef USART_InitStruct;
	    USART_InitStruct.USART_BaudRate=usart_rx_baud;
	    USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	    USART_InitStruct.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
	    USART_InitStruct.USART_Parity=USART_Parity_No;
	    USART_InitStruct.USART_StopBits=USART_StopBits_1;
	    USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	    USART_Init(USART1,&USART_InitStruct);

		USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
		NVIC_InitTypeDef NVIC_InitStruct;
	    NVIC_InitStruct.NVIC_IRQChannel=USART1_IRQn;
	    NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=3;
	    NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	    NVIC_Init(&NVIC_InitStruct);

	    USART_Cmd(USART1,ENABLE );	
	}

	void usart1_send_Char(uint8_t character)
	{
	    USART_SendData(USART1,character);
	    while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
	}

	void usart1_send_str(char *str)
	{
	    while(*str)
	    {
	        usart1_send_Char(*str++);
	    }
	}

	void usart1_send_Hex(uint8_t *Arry,uint32_t len)
	{
		for(uint32_t i=0;i<len;i++)
		{
			USART_SendData(USART1,*(Arry+i));
			while(USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET);
		}
	}

	// int fputc(int ch, FILE *f)
	// {
	//   /* Your implementation of fputc(). */
	// 	usart1_send_Char(ch);
	//   return ch;
	// }

	void USART1_IRQHandler(void)
	{
		if(USART_GetITStatus(USART1,USART_IT_RXNE) == SET)
		{
			if(rx_buff_num_used < usart_rx_buff_size-1) //-1是为了保证缓冲区满了之后，最后一位是'\0'终止符;
			{
				rx_buff[rx_buff_num_used++] = USART_ReceiveData(USART1);
				rx_flag=1;
				TIM3->CNT=0;
			}
			else USART_ReceiveData(USART1);//缓冲区满了后，读接收寄存器用于清除RXNE标志位;
		}
	}
#elif (usartx ==2)
	void usart2_init(void)
	{
		timer3_init();//USART的中断接收功能使用
	    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE );
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);

	    GPIO_InitTypeDef GPIO_InitStruct;
	    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_AF_PP;
	    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_2;
	    GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	    GPIO_Init(GPIOA,&GPIO_InitStruct);
	    GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
	    GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3;
	    GPIO_Init(GPIOA,&GPIO_InitStruct);

	    USART_InitTypeDef USART_InitStruct;
	    USART_InitStruct.USART_BaudRate=usart_rx_baud;
	    USART_InitStruct.USART_HardwareFlowControl=USART_HardwareFlowControl_None;
	    USART_InitStruct.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;
	    USART_InitStruct.USART_Parity=USART_Parity_No;
	    USART_InitStruct.USART_StopBits=USART_StopBits_1;
	    USART_InitStruct.USART_WordLength=USART_WordLength_8b;
	    USART_Init(USART2,&USART_InitStruct);

		USART_ITConfig(USART2,USART_IT_RXNE,ENABLE);

		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
		NVIC_InitTypeDef NVIC_InitStruct;
	    NVIC_InitStruct.NVIC_IRQChannel=USART2_IRQn;
	    NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
	    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=3;
	    NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
	    NVIC_Init(&NVIC_InitStruct);

	    USART_Cmd(USART2,ENABLE );	
	}

	void usart2_send_Char(uint8_t character)
	{
	    USART_SendData(USART2,character);
	    while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
	}

	void usart2_send_Hex(uint8_t *Arry,uint32_t len)
	{
		for(uint32_t i=0;i<len;i++)
		{
			USART_SendData(USART2,*(Arry+i));
			while(USART_GetFlagStatus(USART2,USART_FLAG_TXE) == RESET);
		}
	}

	void usart2_send_str(char *str)
	{
	    while(*str)
	    {
	        usart2_send_Char(*str++);
	    }
	}

	// int fputc(int ch, FILE *f)
	// {
	//   /* Your implementation of fputc(). */
	// 	usart2_send_Char(ch);
	//   return ch;
	// }

	void USART2_IRQHandler(void)
	{
		if(USART_GetITStatus(USART2,USART_IT_RXNE) == SET)
		{
			if(rx_buff_num_used < usart_rx_buff_size-1) //-1是为了保证缓冲区满了之后，最后一位是'\0'终止符;
			{
				rx_buff[rx_buff_num_used++] = USART_ReceiveData(USART2);
				rx_flag=1;
				TIM3->CNT=0;
			}
			else USART_ReceiveData(USART2);//缓冲区满了后，读接收寄存器用于清除RXNE标志位;
		}
	}
#endif

#ifndef USE_std_C11
	/*接收缓冲区与命令集某一个命令匹配时会进入该函数一次;
	param:传进的cmd_num表示与命令集数组中的第几个元素匹配，范围：[0,cmd_count]
	*/
	__weak void rx_data_procCallback(uint16_t cmd_num)//Starting from 0
	{

	}
#endif

void rx_data_proc(void)
{
	static uint32_t rx_buff_num_used_temp;
	/*
		1.t微秒=1000000/(baud/10)  ->传1字节(10bit)需要多少微秒  如，t=1000000/(115200/10)=86.8us
		2.t微秒=((psc+1) * (cnt+1)) / (TIMCLK/1000000)  ->已知等待接收到一个字节(10bit)的时间求cnt,cnt大过这个时间，代表貌似没有数据可接受了   如，通常令系统时钟通过psc分频后为1us,易得cnt=t-1
		最终公式：cnt = (10,000,000/baud) - 1
		看这里！直观理解：默认通过psc分频后为1us,那么TIM3->CNT里的计数值就是等待多少个1us(TIM3->CNT=0相当于等待了1us，TIM3->CNT=1相当于等待了2us，以此类推)，由第一条可以得到接受1字节所需的时间，TIM3->CNT计数只要大于这个时间减1就代表没有数据可接受了;
	*/
	if(rx_flag && ( (TIM3->CNT)>(2*(uint16_t)cnt) ))//85.8us+50us，！经过深思熟虑，后面的50us没有必要，但一定要保证cnt一定大于接收1个字节所用的时间;这里我们取2倍的由公式计算出的cnt值更稳健;
	{
		rx_buff_num_used_temp = rx_buff_num_used;
		//遍历所有命令字符串
		for(size_t i=0;i<cmd_count;i++)
		{
			//比较接收缓冲区和命令字符数量是否相同，数量不相同肯定就不是同一个命令
			if( rx_buff_num_used_temp == strlen(rx_command[i]) )//strlen不包含'\0'字符
			{
				//从后向前逐个匹配字符
				while(rx_buff_num_used_temp)//从这里退出代表命令匹配
				{
					rx_buff_num_used_temp--;//将--放在while循环中会多减一个1，匹配时值为0-1，大坑！
					if( rx_buff[rx_buff_num_used_temp] != rx_command[i][rx_buff_num_used_temp])
						break;//在这里退出代表命令不匹配
				}
				if(rx_buff_num_used_temp == 0)//全部字符都相同
				{
					rx_data_procCallback(i);
					break;
				}
				rx_buff_num_used_temp = rx_buff_num_used;
			}
		}
		for(uint32_t i=0;i<rx_buff_num_used;i++) rx_buff[i]=0;
		rx_buff_num_used=0;
		rx_flag=0;
	}
}
//void test(void)
//{
////如何去重？
//printf("char *rx_command[]={\"cmd1er\",\"cmd2\",\"cmd3e\",\"helprrrr\"};");//比定义二维数组的优点是当字符串长度不一致时更节省存储空间

///*数组名和指向一维数组的指针变量求sizeof*/
//	printf("/*数组名和指向一维数组的指针变量求sizeof*/\r\n");
//	const char str[]="hhhhhhh";
//	printf("sizeof(str)=%zu\r\n",sizeof(str));//只能使用数组名
//	const char *str2="hhhhhhh";
//	printf("sizeof(str2)%zu\r\n",sizeof(str2));//求char *指针型变量所占的字节数
///*指针数组的数组名求sizeof;  指针数组的数组名解一层指针(本质是指向字符串的指针型变量)求sizeof*/
//	printf("/*指针数组的数组名求sizeof;  指针数组的数组名解一层指针(本质是指向字符串的指针型变量)求sizeof*/\r\n");
//	printf("char *=%zu\r\n",sizeof(char *));
//	printf("rx_command=%zu\r\n",sizeof(rx_command));
//	printf("rx_command[0]=%zu\r\n",sizeof(rx_command[0]));
//	printf("rx_command[1]=%zu\r\n",sizeof(rx_command[1]));
//	printf("rx_command[2]=%zu\r\n",sizeof(rx_command[2]));
//	printf("rx_command[3]=%zu\r\n",sizeof(rx_command[3]));	
///*求指针数组内指针变量的个数*/
//	printf("/*求指针数组内指针变量的个数*/\r\n");
//	size_t count = sizeof(rx_command) / sizeof(rx_command[0]);//rx_command相当于数组名; rx_command[0]相当于一个指向字符串的指针型变量并不是字符串数组名;
//	printf("rx_command / rx_command[0]= %zu\r\n",count);
///*strlen:传入数组命或数组指针，统计'\0'之前的字符个数*/
//	printf("/*strlen:传入数组命或数组指针，统计'\\0'之前的字符个数*/");
//	printf("strlen(rx_command[0])=%zu\r\n",strlen(rx_command[0]) );
//	printf("strlen(rx_command[3])=%zu\r\n",strlen(rx_command[3]) );
//}



