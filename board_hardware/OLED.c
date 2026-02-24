#include "OLED.h"
#include "OLED_Font.h"
#include "stm32f10x.h"
#include "stdint.h"
#include "Delay.h"
#include "iic.h"
#include "hw_iic.h"

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	hw_iic_write_byte(OLED_device_addr,OLED_Command_register_addr,Command);
	// IIC_Write_Byte(OLED_device_addr,OLED_Command_register_addr,Command);
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	hw_iic_write_byte(OLED_device_addr,OLED_Data_register_addr,Data);
	// IIC_Write_Byte(OLED_device_addr,OLED_Data_register_addr,Data);
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}

/**
  * @brief  发送OLED启动序列初始化
  * @param  无
  * @retval 无
  */
void OLED_Init(void)
{
	hw_iic_init();//初始化硬件IIC总线

	Delay_ms(100);//上电延时100ms

	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}
/****************************
		以此向下为自写函数
**************************8***/
void oled_all_open(void)
{
	uint8_t i,t;
	for(i=0;i<8;i++)
	{
		OLED_SetCursor(i,0);
		for(t=0;t<128;t++)
		{
			if(t%2==0)
			OLED_WriteData(0xaa);
			else
				OLED_WriteData(0x55);
		}
	}
}
void oled_all_open2(void)
{
	uint8_t i,t;
	for(i=0;i<8;i++)
	{
		OLED_SetCursor(i,0);
		for(t=0;t<128;t++)
		{
			if(t%2==0)
			OLED_WriteData(0x55);
			else
				OLED_WriteData(0xaa);
		}
	}
}

/**
  *@brief 显示一个中文字体
  *@param line 1~4
  *@param column 1~8
  *@param chinese_sernum 中文字库序号

  */
void oled_chinese(uint8_t Line,uint8_t Colum,uint8_t chinese_sernum)
{
	uint8_t i;
	OLED_SetCursor((Line-1)*2,(Colum-1)*16);
	for(i=0;i<16;i++)
	{
		OLED_WriteData(~OLED_F16x16[chinese_sernum-1][i]);
	}
	OLED_SetCursor((Line-1)*2+1,(Colum-1)*16);
	for(i=0;i<16;i++)
	{
		OLED_WriteData(~OLED_F16x16[chinese_sernum-1][i+16]);
	}
}


/**
  *@brief 显示一张图片
  *@param 
*@param X_hight 此为图片高65*X_hight
  *@param 

  */
void oled_image_leige(uint8_t X_hight)
{
	uint8_t i,j;
	for(j=0;j<8;j++)
	{
		OLED_SetCursor(j,0);
		for(i=0;i<X_hight;i++)
		{
			OLED_WriteData(~gImage_leige[j][i]);
		}
	}
}
void oled_image_binbin(void)
{
	uint8_t i,j;
	for(j=0;j<8;j++)
	{
		OLED_SetCursor(j,0);
		for(i=0;i<83;i++)
		{
			OLED_WriteData(~gImage_binbin2[j][i]);
		}
	}
}
void oled_image_jinxin(uint8_t X_hight)
{
	uint8_t i,j;
	for(j=0;j<8;j++)
	{
		OLED_SetCursor(j,0);
		for(i=0;i<X_hight;i++)
		{
			OLED_WriteData(~gImage_jinxin[j][i]);
		}
	}
}
void oled_image_meinv(uint8_t X_hight)
{
	uint8_t i,j;
	for(j=0;j<8;j++)
	{
		OLED_SetCursor(j,0);
		for(i=0;i<X_hight;i++)
		{
			OLED_WriteData(~gImage_meinv[j][i]);
		}
	}
}
void oled_image_hongzhong(uint8_t X_hight)
{
	uint8_t i,j;
	for(j=0;j<8;j++)
	{
		OLED_SetCursor(j,0);
		for(i=0;i<X_hight;i++)
		{
			OLED_WriteData(~gImage_hongzhong[j][i]);
		}
	}
}
void oled_image_zongyao(uint8_t X_hight)
{
	uint8_t i,j;
	for(j=0;j<8;j++)
	{
		OLED_SetCursor(j,0);
		for(i=0;i<X_hight;i++)
		{
			OLED_WriteData(~gImage_zongyao[j][i]);
		}
	}
}
void oled_compress_image_zongyao(uint8_t X_hight)
{
	uint8_t i,j;
	for(j=0;j<8;j++)
	{
		OLED_SetCursor(j,0);
		for(i=0;i<X_hight;i+=2)
		{
			OLED_WriteData(~gImage_zongyao[j][i]);
			OLED_WriteData(~gImage_zongyao[j][i]);
		}
	}
}
void oled_image_yanhui(void)
{
	uint8_t i,j;
	for(j=0;j<8;j++)
	{
		OLED_SetCursor(j,0);
		for(i=0;i<85;i++)
		{
			OLED_WriteData(~gImage_yanhui[j][i]);
		}
	}
}
void oled_compress_image_yanhui(uint8_t X_hight)
{
	uint8_t i,j;
	for(j=0;j<8;j++)
	{
		OLED_SetCursor(j,0);
		for(i=0;i<X_hight;i+=2)
		{
			OLED_WriteData(~gImage_yanhui[j][i]);
			OLED_WriteData(~gImage_yanhui[j][i]);
		}
	}
}
