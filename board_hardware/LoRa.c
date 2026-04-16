// #include "LoRa.h"
// #include <math.h>//roundf四舍五入


// void usart2_init(void)
// {
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

//     GPIO_InitTypeDef GPIO_InitStruct={
//         .GPIO_Pin=GPIO_Pin_2,//TX
//         .GPIO_Mode=GPIO_Mode_AF_PP,
//         .GPIO_Speed=GPIO_Speed_50MHz
//     };
//     GPIO_Init(GPIOA, &GPIO_InitStruct);
//     GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3;//RX
//     GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
//     GPIO_Init(GPIOA, &GPIO_InitStruct);

//     USART_InitTypeDef USART_InitStruct={
//         .USART_BaudRate=9600,
//         .USART_WordLength=USART_WordLength_8b,
//         .USART_StopBits=USART_StopBits_1,
//         .USART_Parity=USART_Parity_No,
//         .USART_HardwareFlowControl=USART_HardwareFlowControl_None,
//         .USART_Mode=USART_Mode_Rx | USART_Mode_Tx
//     };
//     USART_Init(USART2, &USART_InitStruct);

//     USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//     USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

//     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
//     NVIC_InitTypeDef NVIC_InitStruct={
//         .NVIC_IRQChannel = USART2_IRQn,
//         .NVIC_IRQChannelCmd = ENABLE,
//         .NVIC_IRQChannelPreemptionPriority = 2,
//         .NVIC_IRQChannelSubPriority = 0
//     };
//     NVIC_Init(&NVIC_InitStruct);

//     USART_Cmd(USART2, ENABLE);
// }

// void UART2_IRQHandler_IDLE_and_RXNE_callback(void)
// {
//     if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
//     {
//         // 处理接收数据
//         uint8_t data = USART_ReceiveData(USART2);
//     }
//     if(USART_GetITStatus(USART2, USART_IT_IDLE) == SET)//当RX引脚上持续1个字符时间（包括停止位）没有新数据时，硬件认为线路进入"空闲"状态
//     {
//         // 处理空闲中断
        

//         // 注意：清除 IDLE后，此时如果线路一直空闲，IDLE 位不会再次被设置。直到后续"有数据→空闲"的变化沿触才会发一次
//         volatile uint32_t tmp;  // 必须 volatile
//         tmp      = USART2->SR;   // 先读SR
//         tmp      = USART2->DR;   // 再读 DR → 清除 IDLE
//         (void)  tmp;            // 消除警告
//     }
// }

#include "LoRa.h"

static void LoRa_usart1_init(uint32_t baud)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9; // USART1 TX
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; // USART1 RX
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = baud;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &USART_InitStruct);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
        NVIC_InitTypeDef NVIC_InitStruct={
        .NVIC_IRQChannel = USART1_IRQn,
        .NVIC_IRQChannelCmd = ENABLE,
        .NVIC_IRQChannelPreemptionPriority = 2,
        .NVIC_IRQChannelSubPriority = 0
    };
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART1, ENABLE);
}

static void LoRa_usart2_init(uint32_t baud)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2; // USART2 TX
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3; // USART2 RX
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    USART_InitTypeDef USART_InitStruct;
    USART_InitStruct.USART_BaudRate = baud;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStruct);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
        NVIC_InitTypeDef NVIC_InitStruct={
        .NVIC_IRQChannel = USART2_IRQn,
        .NVIC_IRQChannelCmd = ENABLE,
        .NVIC_IRQChannelPreemptionPriority = 2,
        .NVIC_IRQChannelSubPriority = 0
    };
    NVIC_Init(&NVIC_InitStruct);

    USART_Cmd(USART2, ENABLE);
}

void LoRa_init(uint32_t baud1, uint32_t baud2)
{
    LoRa_usart1_init(baud1);
    LoRa_usart2_init(baud2);
}

void UART2_IRQHandler_RXNE_callback(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
    {
        // 处理接收数据
        uint8_t data = USART_ReceiveData(USART2);
        USART_SendData(USART1, data);
        USART_ClearITPendingBit(USART2, USART_FLAG_RXNE); // 清除 RXNE 标志位
    }
}
void UART1_IRQHandler_RXNE_callback(void)
{
    if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
    {
        // 处理接收数据
        uint8_t data = USART_ReceiveData(USART1);
        USART_SendData(USART2, data);
        USART_ClearITPendingBit(USART1, USART_FLAG_RXNE); // 清除 RXNE 标志位
    }
}

// void LoRa_bridge_poll(void)
// {
//     if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)u8g2_SetFontPosBaseline
//     {
//         uint8_t ch = USART_ReceiveData(USART1);
//         LoRa_usart2_send_Char(ch);
//     }

//     if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)
//     {
//         uint8_t ch = USART_ReceiveData(USART2);
//         LoRa_usart1_send_Char(ch);
//     }
// }
