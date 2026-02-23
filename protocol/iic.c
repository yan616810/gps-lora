/**
 * @file iic.c
 * @author YLAD (you@domain.com)
 * @brief 可自由切换引脚的software iic
 * 使用方法：
 * 1.IIC_InitPins_or_ChangePins函数用于指定iic作用的IO口;
 * 2.IIC_Set_speed函数调整IIC通信速率;
 * @version 0.1
 * @date 2025-05-09
 * 1.最快的方法：将delay()直接注释掉，不适用延时
 * 2.默认初始化iic速度为理论500kHZ，实际400kHZ都没到;
 * 3.待使用bitband功能，硬件上加快读写速度;貌似速度相差不大？
 * @copyright Copyright (c) 2025
 * 
 */
#include "iic.h"
#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include <stdlib.h>//malloc
#include <stdio.h>//printf

#if (is_use_delay == 1)
/*默认理论速度500kHZ，对不同的器件，所需的协议最大速度不同*/
uint32_t i2c_speed=1;//取值范围:[1,5]   1.25=1/400kHZ/2    5=1/100kHZ/2
#endif

/**************************标准库函数操作IO口**************************///开漏输出模式,默认io口都已经外接上拉电阻了
#if (is_use_delay == 1)
    #define IIC_SCL_out(x)  GPIO_WriteBit(SCL_port,SCL_pin, (BitAction)(x));Delay_us(i2c_speed);
    #define IIC_SDA_out(x)  GPIO_WriteBit(SDA_port,SDA_pin, (BitAction)(x));Delay_us(i2c_speed);
#else
    #define IIC_SCL_out(x)  GPIO_WriteBit(SCL_port,SCL_pin, (BitAction)(x));
    #define IIC_SDA_out(x)  GPIO_WriteBit(SDA_port,SDA_pin, (BitAction)(x));
#endif

#define IIC_SDA_in      GPIO_ReadInputDataBit(SDA_port,SDA_pin)
/**************************bitbang宏定义操作IO口**************************/


//无外部上拉电阻的输入输出模式切换待实现

/****************************以下为固定函数和变量不用修改*****************************************/
/*SCL，SDA变量，可由程序自由改为其他引脚*/
uint32_t SCL_RCC_APB2Periph,SDA_RCC_APB2Periph;
GPIO_TypeDef *SCL_port,*SDA_port;
uint16_t SCL_pin,SDA_pin;

#if (is_use_delay == 1)
/**
 * @brief 改变i2c速度
 * 
 * @param us 1~5整数  公式：（500/iic频率kHZ=时间微妙）
 * 理想情况下(1-500kHZ)
 *          (2-250kHZ)
 *          (3-166kHZ)
 *          (4-125kHZ)
 *          (5-100kHZ)
 */
void IIC_Set_speed(uint32_t us)
{
   i2c_speed=us;
}
#endif

 /**
  * @brief 初始化iic引脚 && 修改iic引脚;
  *        1.首次调用该函数初始化iic引脚并且设置默认速度500kHZ，运行时第二次调用该函数切换iic引脚，并重置旧引脚
  *        2.可能不保证原子性，不要在中断中调用
  * 
  * @param SCL_CHANGE_RCC_APB2Periph SCL切换端口后需要开启的时钟 RCC_APB2Periph_GPIOx
  * @param SCL_change_port 切换SCL端口到该参数 范围：GPIOA~GPIOD等
  * @param SCL_change_pin  切换SCL引脚到该参数 范围GPIO_Pin_0 ~ GPIO_Pin_15
  * @param SDA_CHANGE_RCC_APB2Periph SDA切换端口后需要开启的时钟 RCC_APB2Periph_GPIOx
  * @param SDA_change_port 切换SDA端口到该参数 范围：GPIOA~GPIOD等
  * @param SDA_change_pin  切换SDA引脚到该参数 范围GPIO_Pin_0 ~ GPIO_Pin_15
  */
void IIC_InitPins_or_ChangePins(uint32_t SCL_CHANGE_RCC_APB2Periph,GPIO_TypeDef *SCL_change_port,uint16_t SCL_change_pin,
                                uint32_t SDA_CHANGE_RCC_APB2Periph,GPIO_TypeDef *SDA_change_port,uint16_t SDA_change_pin)
{
    GPIO_InitTypeDef GPIO_InitStructure_reset={
    .GPIO_Mode=GPIO_Mode_IN_FLOATING,
    .GPIO_Speed=GPIO_Speed_50MHz,
    };
    GPIO_InitTypeDef GPIO_InitStructure_set={
    .GPIO_Mode=GPIO_Mode_Out_OD,
    .GPIO_Speed=GPIO_Speed_50MHz,
    };
    static uint8_t is_first_call=1;
    if(is_first_call == 0)
    {//第二次调用，需要重置上一次的引脚
        //重置旧引脚
        GPIO_InitStructure_reset.GPIO_Pin = SCL_pin;
        GPIO_Init(SCL_port, &GPIO_InitStructure_reset);
        GPIO_InitStructure_reset.GPIO_Pin = SDA_pin;
        GPIO_Init(SDA_port, &GPIO_InitStructure_reset);
	    RCC_APB2PeriphClockCmd(SCL_RCC_APB2Periph | SDA_RCC_APB2Periph,DISABLE);
 	    //初始新引脚
        SCL_RCC_APB2Periph = SCL_CHANGE_RCC_APB2Periph;
        SCL_port = SCL_change_port;
        SCL_pin  = SCL_change_pin;
        SDA_RCC_APB2Periph = SDA_CHANGE_RCC_APB2Periph;
        SDA_port = SDA_change_port;
        SDA_pin  = SDA_change_pin;
        RCC_APB2PeriphClockCmd(SCL_RCC_APB2Periph | SDA_RCC_APB2Periph,ENABLE);
        GPIO_InitStructure_set.GPIO_Pin = SCL_pin;
        GPIO_Init(SCL_port, &GPIO_InitStructure_set);
        GPIO_InitStructure_set.GPIO_Pin = SDA_pin;
        GPIO_Init(SDA_port, &GPIO_InitStructure_set);
    }
    else{
        //第一次调用，直接初始化引脚
        is_first_call=0;
        SCL_RCC_APB2Periph = SCL_CHANGE_RCC_APB2Periph;
        SCL_port = SCL_change_port;
        SCL_pin  = SCL_change_pin;
        SDA_RCC_APB2Periph = SDA_CHANGE_RCC_APB2Periph;
        SDA_port = SDA_change_port;
        SDA_pin  = SDA_change_pin;
        RCC_APB2PeriphClockCmd(SCL_RCC_APB2Periph | SDA_RCC_APB2Periph,ENABLE);
        GPIO_InitStructure_set.GPIO_Pin = SCL_pin;
        GPIO_Init(SCL_port, &GPIO_InitStructure_set);
        GPIO_InitStructure_set.GPIO_Pin = SDA_pin;
        GPIO_Init(SDA_port, &GPIO_InitStructure_set);
    }
    IIC_SCL_out(1);
	IIC_SDA_out(1);
}



/*******************下面的程序通过上边宏定义中的，存储在RW-data字段的，描述端口位置的变量作为接口，将I2C软件与I2C硬件端口解耦*********************** */
void IIC_Start(void)
{   
    IIC_SCL_out(0);
	IIC_SDA_out(1);//先拉高SDA，主要目的是延时，次要目的是匹配wait_ACK时序;\
                    不像江协所说主要目的是匹配时序，因为wait_ACK等待读从机ACK前，已经手动将SDA拉高了，\
                    即使中途从机的ACK将SDA拉低，但最后的SCL下降沿会使从机释放SDA引脚，使得SDA又变为读从机ACK前手动将SDA拉高的高电平状态；\
                    但是为了在任何位置单独使用IIC_Start，所以还是得加上	  	  
	IIC_SCL_out(1);
 	IIC_SDA_out(0);
	IIC_SCL_out(0);
}

void IIC_Stop(void)
{
	IIC_SCL_out(0);//wait_ACK与NAck最后一句都是IIC_SCL_out(0);所以冗余；但是wait_ACK中的超时检测调用IIC_Stop，所以还得加上
	IIC_SDA_out(0);
	IIC_SCL_out(1);
	IIC_SDA_out(1);							   	
}
//0：ACK     1:NACK
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;     
	IIC_SDA_out(1);//先拉高使从机发出的SDA信号更稳定	   
	IIC_SCL_out(1);//虽然可以放在while后面,但放在读SDA前面使得从机发出SDA信号进一步得到延时稳定
	while(IIC_SDA_in)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL_out(0);//下降沿时从机释放SDA总线	   
	return 0;
} 

void IIC_Ack(void)
{
    IIC_SCL_out(0);
	IIC_SDA_out(0);
	IIC_SCL_out(1);
	IIC_SCL_out(0);
}
		    
void IIC_NAck(void)
{
    IIC_SCL_out(0);
	IIC_SDA_out(1);//冗余
	IIC_SCL_out(1);
	IIC_SCL_out(0);
}					 				     
		  
void IIC_Send_8bit(u8 txd)
{                        
    u8 t;    	    
    IIC_SCL_out(0);//貌似冗余了
    for(t=0;t<8;t++)
    {            
	    IIC_SDA_out((txd&0x80)>>7);
        txd<<=1; 	    
	    IIC_SCL_out(1);
	    IIC_SCL_out(0);	
    }	 
} 	    
//1：连续读   0：读一次  
u8 IIC_Read_8bit(unsigned char ack)
{
	unsigned char i,receive=0;
	IIC_SDA_out(1);//冗余，wait_ACK在前面调用，从机释放SDA后SDA自动变为高电平；但调用主机ACK后，还要将SDA置1
    for(i=0;i<8;i++ )
	{
        IIC_SCL_out(0);
		IIC_SCL_out(1);
        receive<<=1;
        if(IIC_SDA_in)receive++;   //此处甚妙	 
    }
    // IIC_SCL_out(0);//使从机释放SDA引脚
    if (!ack)
        IIC_NAck();
    else
        IIC_Ack();
    return receive;
}
//***********************************************************************//
//IIC写一个字节 
//devaddr:器件IIC地址
//reg:寄存器地址
//data:数据
//返回值:0,正常
//    其他,错误代码
u8 IIC_Write_Byte(u8 addr,u8 reg,u8 data)
{
    IIC_Start();
    IIC_Send_8bit((addr<<1)|0); //发送器件地址+写命令
    if(IIC_Wait_Ack())          //等待应答
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_8bit(reg);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答
    IIC_Send_8bit(data);        //发送数据
    if(IIC_Wait_Ack())          //等待ACK
    {
        IIC_Stop();
        return 1;
    }
    IIC_Stop();
    return 0;
}

//IIC写len长度个字节 
//devaddr:器件IIC地址
//reg:首寄存器地址
//len:写字节长度
//data:要写入的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 IIC_Write_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{
    IIC_Start();
    IIC_Send_8bit((addr<<1)|0); //发送器件地址+写命令
    if(IIC_Wait_Ack())          //等待应答
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_8bit(reg);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答
    while(len)
    {
        len--;
        IIC_Send_8bit(*buf++);        //发送数据
        if(IIC_Wait_Ack())          //等待ACK
        {
            IIC_Stop();
            return 1;
        }
    }
    IIC_Stop();
    return 0;
}

//IIC读一个字节 
//reg:寄存器地址 
//返回值:读到的数据
u8 IIC_Read_Byte(u8 addr,u8 reg)
{
    u8 res;
    IIC_Start();
    IIC_Send_8bit((addr<<1)|0); //发送器件地址+写命令
    IIC_Wait_Ack();             //等待应答
    IIC_Send_8bit(reg);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答
	IIC_Start();                
    IIC_Send_8bit((addr<<1)|1); //发送器件地址+读命令
    IIC_Wait_Ack();             //等待应答
    res=IIC_Read_8bit(0);		    //读数据,发送nACK  
    IIC_Stop();                 //产生一个停止条件
    return res;  
}

//IIC连续读
//addr:器件地址
//reg:要读取的寄存器地址
//len:要读取的长度
//buf:读取到的数据存储区
//返回值:0,正常
//    其他,错误代码
u8 IIC_Read_Len(u8 addr,u8 reg,u8 len,u8 *buf)
{ 
    IIC_Start();
    IIC_Send_8bit((addr<<1)|0); //发送器件地址+写命令
    if(IIC_Wait_Ack())          //等待应答
    {
        IIC_Stop();
        return 1;
    }
    IIC_Send_8bit(reg);         //写寄存器地址
    IIC_Wait_Ack();             //等待应答
	  
	IIC_Start();                
    IIC_Send_8bit((addr<<1)|1); //发送器件地址+读命令
    IIC_Wait_Ack();             //等待应答
    while(len)
    {
        if(len==1)*buf=IIC_Read_8bit(0);//读数据,发送nACK 
		else *buf=IIC_Read_8bit(1);		//读数据,发送ACK  
		len--;
		buf++;  
    }
    IIC_Stop();                 //产生一个停止条件
    return 0;       
}


/*************************功能函数************************/

#if (is_use_low_memory_IIC_Search_all_devices == 1)//低内存占用，但耗废时间长
/**
 * @brief 搜索iic挂载的所有设备地址，并将搜索到的设备地址存入堆中;
 *      1.<stdlib.h>  -> 使用库中的malloc函数分配堆中内存
 *      2.<stdio.h>   -> 使用库中的printf函数用于打印malloc函数堆分配失败信息，记得将printf重定向到usart等
 * 
 * @param devices_addr_info_heap_ptr 用于传出堆地址，请先定义一个uint8_t *指针型变量，用于访问堆空间
 * @return uint8_t 返回设备数量(也是堆中保存的地址数量)
 */
uint8_t IIC_Search_all_devices(uint8_t **devices_addr_info_heap_ptr)//时间换空间
{
    uint8_t devices_total=0;
/*遍历搜索iic接口挂载的所有设备个数*/
    for(uint8_t i=0;i<128;i++)
    {
        IIC_Start();
        IIC_Send_8bit(i<<1);//这里要使用|0写命令，保证主机不释放总线;读命令|1时，下降沿时从机输出数据，控制权会转移到从机;
        if(IIC_Wait_Ack()==0)
        {
        	IIC_Stop();
            devices_total++;//统计iic总线上挂载设备地址数量
        }
    }
/*在堆中动态开辟不多不少的空间,存放所有设备地址信息*/
    uint8_t *ptr = malloc(devices_total*sizeof(*ptr));/* 等效展开：sizeof(*ptr) → sizeof(uint8_t) → 1 */
    if(ptr == NULL){printf("malloc heap overflow!!!\r\n");while(1);}//提示堆空间溢出
    *devices_addr_info_heap_ptr = ptr;/*将设备信息堆指针传出，用于后续外部函数访问*/
/*再次遍历地址0~127,将存在的设备地址从小到大依次存到堆中*/
    devices_total=0;
    for(uint8_t i=0;i<128;i++)
    {
        IIC_Start();
        IIC_Send_8bit(i<<1);
        if(IIC_Wait_Ack()==0)
        {
        	IIC_Stop();
            *(ptr+devices_total) = i;
            devices_total++;
        }
    }
    return devices_total;//堆中保存的地址信息数量(正在挂载着的设备数量)
}
#else
uint8_t IIC_Search_all_devices(uint8_t **devices_addr_info_heap_ptr)//空间换时间
{
    uint8_t devices_total=0;
    uint8_t devices_addr_info_buff[128];
/*遍历搜索地址0~127,将存在的设备地址从小到大依次存到主栈缓冲数组中*/
    for(uint8_t i=0;i<128;i++)
    {
        IIC_Start();
        IIC_Send_8bit(i<<1);//这里要使用|0写命令，保证主机不释放总线;读命令|1时，下降沿时从机输出数据，控制权会转移到从机;
        if(!IIC_Wait_Ack())
        {
        	IIC_Stop();
            devices_addr_info_buff[devices_total] = i;
            devices_total++;//统计iic总线上挂载设备地址数量
        }
    }
/*在堆中动态开辟不多不少的空间,正好存放所有设备地址信息*/
    uint8_t *ptr = malloc(devices_total*sizeof(*ptr));/* 等效展开：sizeof(*ptr) → sizeof(uint8_t) → 1 */
    if(ptr == NULL){printf("iic.c IIC_Search_all_devices() -> malloc heap overflow!!!\r\n");while(1);}//提示堆空间溢出
    *devices_addr_info_heap_ptr = ptr;/*将设备信息堆指针传出，用于外部函数访问*/
/*将缓存数组中的所有iic设备地址复制到堆中*/
    for(uint8_t i=0;i<devices_total;i++)
    {
        *(ptr+i) = devices_addr_info_buff[i];
    }
    return devices_total;//堆中保存的地址信息数量(正在挂载着的设备数量)
}
#endif // !low_monery

/**
 * @brief 使用搜索设备函数(IIC_Search_all_devices)的例子;
 * 
 */
void IIC_Search_all_devices_printf_example(void)
{
    uint8_t *devices_addr_info_heap_ptr;
	uint8_t devices_num;
	devices_num = IIC_Search_all_devices(&devices_addr_info_heap_ptr);
	printf("Number of devices found -> %d\r\n",devices_num);
	for(uint8_t i=0;i<devices_num;i++)
	{
		printf("heap_ptr_addr=0x%p -> device_addr=0x%x\r\n",devices_addr_info_heap_ptr+i,*(devices_addr_info_heap_ptr+i));
	}
}

