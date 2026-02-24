#include "stm32f10x.h"                  // Device header
uint8_t key_value=0;

void key_init()
{
	// RCC->APB2ENR |= 0X08;//使能GPIOB
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct={
		.GPIO_Mode = GPIO_Mode_IN_FLOATING,
		.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15,
		.GPIO_Speed = GPIO_Speed_10MHz,
	};
	GPIO_Init(GPIOB,&GPIO_InitStruct);
}

void get_key()
{
	static uint8_t status=0,mid,num=0,double_delay=0;
	static uint16_t long_delay=0;
	switch(status)
	{
		case 0:
			if((GPIOB->IDR & GPIO_Pin_15)==0)status=1;//B15按键接GND
			if((GPIOB->IDR & GPIO_Pin_14)==0)status=1;//B14按键接GND
			if((GPIOB->IDR & GPIO_Pin_13)==0)status=1;//B13按键接GND
			break;
		case 1:
			if((GPIOB->IDR & GPIO_Pin_15)==0){mid=4;status=2;}
			else if((GPIOB->IDR & GPIO_Pin_14)==0){mid=3;status=2;}
			else if((GPIOB->IDR & GPIO_Pin_13)==0){mid=2;status=2;}
			else status=0;
			break;
		case 2:
			if((GPIOB->IDR & GPIO_Pin_15)==0){}
			else if((GPIOB->IDR & GPIO_Pin_14)==0){}
			else if((GPIOB->IDR & GPIO_Pin_13)==0){
				if(long_delay<200)long_delay++;
				if(long_delay==200)
				{
					long_delay=201;
					key_value='L';
				}

			}
			else{
				if(long_delay==201){}
				else if(mid==2)num++;
				else key_value=mid;
				status=0;long_delay=0;
			}
			break;
		default:break;
	}
	
	if(num==1)
	{
		if(double_delay<25)double_delay++;
		else{
			num=0;
			double_delay=0;
			key_value=2;
		}
	}
	else if(num==2)
	{
		num=0;
		double_delay=0;
		key_value='D';
	}
}
