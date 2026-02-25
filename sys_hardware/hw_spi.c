#include "hw_spi.h"


void hw_spi_init(void)
{
    //开启SPI1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
    //开启GPIOA时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    
    //配置PA5为SPI1_SCK
    GPIO_InitTypeDef GPIO_InitStruct = {
        .GPIO_Pin = GPIO_Pin_5,
        .GPIO_Mode = GPIO_Mode_AF_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
    };
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    //配置PA6为SPI1_MISO
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    //配置PA7为SPI1_MOSI
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;  // 推挽输出
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    //配置SPI1参数
    SPI_InitTypeDef SPI_InitStruct = {
        .SPI_Direction         = SPI_Direction_2Lines_FullDuplex,   // 双线全双工模式
        .SPI_Mode              = SPI_Mode_Master,
        .SPI_DataSize          = SPI_DataSize_8b,
        .SPI_CPOL              = SPI_CPOL_Low,                      //时钟空闲时为低电平
        .SPI_CPHA              = SPI_CPHA_1Edge,                    //第一个边沿采样
        .SPI_NSS               = SPI_NSS_Soft,                      //需手动控制GPIO实现片选
        .SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2,          // 若系统时钟72MHz → SCK = 72/2 = 36MHz,其他分频选项（2/4/8/16/32/64/128/256）
        .SPI_FirstBit          = SPI_FirstBit_MSB,                  // 数据传输从最高位开始
        .SPI_CRCPolynomial     = 0,                                 //普通传输可忽略（设为0时禁用）
    };
    
    SPI_Init(SPI1, &SPI_InitStruct);
    
    //使能SPI1
    SPI_Cmd(SPI1, ENABLE);
}

uint8_t hw_spi_transfer(uint8_t data)//去除对RXNE标志位的检查和返回值的话，LCD帧率从4.1FPS提升到7.6FPS
{
    //等待SPI1发送缓冲区空
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);//数据从发送缓冲器传输到移位寄存器时TXE标志将被置位 
/****TXE=1，表明发送缓冲区的数据已经转移到了移位寄存器，此时发送缓冲区为空，可以再写进一个数据到发送缓冲区并等待移位寄存器传输完成******/   
    //发送数据
    SPI_I2S_SendData(SPI1, data);
	
//    //等待接收缓冲区非空
//    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
///****RXNE=1，表明接收移位寄存器的数据已经转移到了接收缓冲区，此时接收缓冲区非空，可以读取接收缓冲区数据使RXNE=0******/
//    //读取接收到的数据
//    return SPI_I2S_ReceiveData(SPI1);//读SPI_DR寄存器将清除RXNE位
    return 0;
}


