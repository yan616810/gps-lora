/**
 * @file DMA_USART.c
 * @author your name (you@domain.com)
 * @brief DMA双缓冲机制实现无缝接收串口数据并拷贝到LWRB环形缓冲区中 DMA+USART1+LWRB
 * 1.先在外部，如主程序中初始化一个LWRB环形缓冲区;
 * 2.使用DMA_link_lwrb_t()链接外部环形缓冲区;（x）
 * 3.调用DMA_usart1_to_arrybuffer_init() 1.初始化DMA功能及其中断; 2.初始化usart1的空闲中断(前提是usart1时钟等已提前初始化); 3.并立即开始DMA转运;
 * 
 * 注意：应该设置串口空闲中断优先级低于DMA1通道5的中断优先级,这是保险措施，但不是致命的必要条件。
 * 
 * 2025/8/31：
 *  1. 增加GPS模块初始化函数等；
 *  2. 增加GPS数据解析函数等；
 *  3. 目前只支持挂载一个GPS芯片，后续考虑面向对象......
 * @version 0.2
 * @date 2025-06-03
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "DMA_USART.h"
#include "stm32f10x.h"
#include "lwrb.h"
#include "lwgps.h"
#include "USART.h"
#define __DMA2_UART4_RX_DMA__

/*定义双缓冲区double buffer*/
#ifdef __DMA1_USART1_RX_DMA__
static uint8_t  dma1_buf1[DMA_BUF_SIZE], dma1_buf2[DMA_BUF_SIZE];  //usart1双缓冲，大小考量看笔记项目定位器
static uint8_t dma1_buf_index = 0;                                 // 指示当前正在使用的缓冲区 0: buf1, 1: buf2
#endif
static uint8_t  dma2_buf1[DMA_BUF_SIZE], dma2_buf2[DMA_BUF_SIZE];  //uart4双缓冲，大小考量看笔记项目定位器
static uint8_t dma2_buf_index = 0;                                 // 指示当前正在使用的缓冲区 0: buf1, 1: buf2

/*环形缓冲区LWRB*/
static lwrb_t buff_1;                       //定义一个环形缓冲区实例
static uint8_t buffdata_1[DMA_BUF_SIZE*32];  //大于等于DMA缓冲区4倍，因一帧数据都1K多，动态调整为4K

/**
 * @brief 初始化一个环形缓冲区
 * 
 */
static void init_lwrb(void)
{
    lwrb_init(&buff_1, buffdata_1,sizeof(buffdata_1));   //初始化gps模块使用到的环形缓冲区
}

#ifdef __DMA2_UART4_RX_DMA__
/**
 * @brief 初始化UART4的DMA接受功能，配置DMA双缓冲机制，并开启相关中断
 * 
 */
static void DMA_UART4_to_arrybuffer_init(void)
{
    /*1.开启DMA2时钟*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
    /*2.开启UART4时钟*/
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);
    /*3.开启GPIOC时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    /*4.配置GPIOA9和GPIOA10为复用推挽输出和输入*/
    GPIO_InitTypeDef GPIO_InitStruct = {
        .GPIO_Pin  = GPIO_Pin_10,        // PC10
        .GPIO_Mode = GPIO_Mode_AF_PP,   // 复用推挽输出
        .GPIO_Speed = GPIO_Speed_50MHz,
    };
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_11;            // PA11
    GPIO_Init(GPIOC, &GPIO_InitStruct);
    /*5.配置UART4波特率115200,8位数据位,无校验位,1位停止位*/
    USART_InitTypeDef USART_InitStruct = {
        .USART_BaudRate = UART4_BaudRate_bps,
        .USART_WordLength = USART_WordLength_8b,
        .USART_StopBits = USART_StopBits_1,
        .USART_Parity = USART_Parity_No,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
        .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
    };
    USART_Init(UART4, &USART_InitStruct);
    /*6.开启UART4接收中断*/
    USART_ITConfig(UART4, USART_IT_IDLE, ENABLE); // 接收数据寄存器空闲
    /*7.配置UART4空闲中断优先级*/
    // 注意：UART4的中断优先级必须低于DMA2通道5的中断优先级，这是保险措施
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel                   = UART4_IRQn;  // UART4空闲中断
	NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 0;
	NVIC_Init(&NVIC_InitStruct);
    /*8.开启DMA2通道3接收UART4数据*/
    USART_DMACmd(UART4,USART_DMAReq_Rx,ENABLE);/*1.每个通道连接着多个外设，通过逻辑或输入到DMA2控制器，这意味着同时只能有一个请求有效.
                                                  2.外设的DMA请求，可以通过设置相应外设寄存器中的控制位，被独立地开启或关闭。*/
/********************************************************分界线下为配置DMA****************************************************************** */
    DMA_InitTypeDef DMA_InitStruct={
        .DMA_BufferSize         = DMA_BUF_SIZE,                  //每次传输的数据量
        .DMA_M2M                = DMA_M2M_Disable,               //禁止内存到内存传输
        .DMA_Mode               = DMA_Mode_Normal,               //工作模式：非循环模式
        .DMA_Priority           = DMA_Priority_VeryHigh,         // DMA优先级：非常高
        .DMA_DIR                = DMA_DIR_PeripheralSRC,         //数据传输方向：外设到内存
        .DMA_MemoryBaseAddr     = (uint32_t)dma2_buf1,            //注意传入的应该是缓冲区的地址
        .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,       //内存数据大小：字节
        .DMA_MemoryInc          = DMA_MemoryInc_Enable,          //内存地址递增
        .DMA_PeripheralBaseAddr = (uint32_t)&UART4->DR,         //注意传入的应该是DR寄存器的地址
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,   //外设数据大小：字节
        .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,     //外设地址不递增
    };
    DMA_Init(DMA2_Channel3,&DMA_InitStruct);
    /*9.使能DMA2通道3的传输完成中断*/
    DMA_ITConfig(DMA2_Channel3,DMA_IT_TC,ENABLE);
    /*10.配置DMA2通道3中断优先级*/
	NVIC_InitStruct.NVIC_IRQChannel                   = DMA2_Channel3_IRQn;  // DMA2通道3中断
	NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;                   // DMA2通道3中断优先级必须高于UART4的空闲中断
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 0;
	NVIC_Init(&NVIC_InitStruct);
    /*11.立即开启DMA2通道3和UART4，准备自动接受*/
    DMA_Cmd(DMA2_Channel3, ENABLE);
    /*12.立即开启UART4*/
    USART_Cmd(UART4, ENABLE);
}

void DMA2_Channel3_IRQHandler_uart4_rxFULL_callback(void)
{
    /*每一个通道都有一组配置寄存器，此处串口4接收功能使用DMA2的通道3*/
    if(DMA_GetITStatus(DMA2_IT_TC3) == SET)  // 判断DMA2通道3传输完成中断
    {
        /*1.失能DMA即可改变CMAR和CNDTR*/
        DMA2_Channel3->CCR &= ~DMA_CCR3_EN;
        /*2.切换缓冲区*/
        if(dma2_buf_index)  //dma2_buf2刚刚接收满了，准备切换到dma2_buf1继续接收
        {
            DMA2_Channel3->CMAR = (uint32_t)dma2_buf1;  // 切换到dma2_buf1缓冲区
            dma2_buf_index      = 0;                    // 指示将会使用dma2_buf1缓冲区进行下一轮接收；0: buf1, 1: buf2
        }
        else  //dma2_buf1刚刚接收满了，准备切换到dma2_buf2继续接收
        {
            DMA2_Channel3->CMAR = (uint32_t)dma2_buf2;  // 切换到dma2_buf2缓冲区
            dma2_buf_index      = 1;                    // 指示将会使用dma2_buf2缓冲区进行下一轮接收；0: buf1, 1: buf2
        }
        /*3.重新设置传输数据量*/
        DMA2_Channel3->CNDTR  = DMA_BUF_SIZE;  // 注意：在STM32F10x系列中，CNDTR寄存器的值在DMA传输过程中会自动递减，表示剩余的数据量。因此，在每次DMA传输完成后，需要重新设置CNDTR寄存器的值，以准备下一次传输。
        /*4.重新使能DMA通道*/
        DMA2_Channel3->CCR   |= DMA_CCR3_EN;
        /*5.将双缓冲区中的数据写入环形缓冲区*/
        if(dma2_buf_index)  //dma2_buf1刚刚接收满了，准备将dma2_buf1中的数据写入环形缓冲区
        {
            lwrb_write(&buff_1, dma2_buf1, DMA_BUF_SIZE);  // 将dma2_buf1中的数据写入环形缓冲区
        }
        else
        {
            lwrb_write(&buff_1, dma2_buf2, DMA_BUF_SIZE);  // 将dma2_buf2中的数据写入环形缓冲区
        }
        DMA_ClearITPendingBit(DMA2_IT_TC3);  // 清除DMA2通道3传输完成中断标志
    }
}

void UART4_IRQHandler_IDLE_callback(void)
{
    if(USART_GetITStatus(UART4, USART_IT_IDLE) == SET)  // 判断UART4空闲中断
    {
        uint16_t received_bytes = DMA_BUF_SIZE - DMA2_Channel3->CNDTR;  // 计算本次接收的数据量
        if(received_bytes > 0)  // 如果received_bytes > 0，即可判断出当前不是DMA缓冲区满&&正好也是空闲！！！
        {
            /*1.失能DMA即可改变CMAR和CNDTR*/
            DMA2_Channel3->CCR &= ~DMA_CCR3_EN;
            /*2.切换缓冲区*/
            if(dma2_buf_index)  //dma2_buf2刚刚接收满了，准备切换到dma2_buf1继续接收
            {
                DMA2_Channel3->CMAR = (uint32_t)dma2_buf1;  // 切换到dma2_buf1缓冲区
                dma2_buf_index      = 0;                    // 指示将会使用dma2_buf1缓冲区进行下一轮接收；0: buf1, 1: buf2
            }
            else  //dma2_buf1刚刚接收满了，准备切换到dma2_buf2继续接收
            {
                DMA2_Channel3->CMAR = (uint32_t)dma2_buf2;  // 切换到dma2_buf2缓冲区
                dma2_buf_index      = 1;                    // 指示将会使用dma2_buf2缓冲区进行下一轮接收；0: buf1, 1: buf2
            }
            /*3.重新设置传输数据量*/
            DMA2_Channel3->CNDTR  = DMA_BUF_SIZE;  // 注意：在STM32F10x系列中，CNDTR寄存器的值在DMA传输过程中会自动递减，表示剩余的数据量。因此，在每次DMA传输完成后，需要重新设置CNDTR寄存器的值，以准备下一次传输。
            /*4.重新使能DMA通道*/
            DMA2_Channel3->CCR   |= DMA_CCR3_EN;
            /*5.将双缓冲区中的数据写入环形缓冲区*/
            if(dma2_buf_index)  //dma2_buf1刚刚接收满了，准备将dma2_buf1中的数据写入环形缓冲区
            {
                lwrb_write(&buff_1, dma2_buf1, DMA_BUF_SIZE);  // 将dma2_buf1中的数据写入环形缓冲区
            }
            else
            {
                lwrb_write(&buff_1, dma2_buf2, DMA_BUF_SIZE);  // 将dma2_buf2中的数据写入环形缓冲区
            }
        }
        /*6.清除UART4空闲中断标志；注意：在 RXNE 位自身被设置(即串口重新接收到数据)并再次处于空闲之前，IDLE 位不会再次被设置。*/
        volatile uint32_t tmp;  // 必须 volatile
        tmp      = UART4->SR;   // 先读SR
        tmp      = UART4->DR;   // 再读 DR → 清除 IDLE
        (void)  tmp;            // // 消除警告
    }
}
#endif

#ifdef __DMA1_USART1_RX_DMA__
/**
 * @brief 配置usart1 + 使用DMA1的通道5，通过双缓冲机制
 * 
 */
static void DMA_usart1_to_arrybuffer_init(void)
{
    /*1.开启DMA1时钟*/
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
    /*2.开启USART1时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    /*3.开启GPIOA时钟*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    /*4.配置GPIOA9和GPIOA10为复用推挽输出和输入*/
    GPIO_InitTypeDef GPIO_InitStruct = {
        .GPIO_Pin  = GPIO_Pin_9,        // PA9
        .GPIO_Mode = GPIO_Mode_AF_PP,   // 复用推挽输出
        .GPIO_Speed = GPIO_Speed_50MHz,
    };
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;  // 浮空输入
    GPIO_InitStruct.GPIO_Pin  = GPIO_Pin_10;            // PA10
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    /*5.配置USART1波特率9600,8位数据位,无校验位,1位停止位*/
    USART_InitTypeDef USART_InitStruct = {
        .USART_BaudRate = USART1_BaudRate_bps,
        .USART_WordLength = USART_WordLength_8b,
        .USART_StopBits = USART_StopBits_1,
        .USART_Parity = USART_Parity_No,
        .USART_HardwareFlowControl = USART_HardwareFlowControl_None,
        .USART_Mode = USART_Mode_Rx | USART_Mode_Tx,
    };
    USART_Init(USART1, &USART_InitStruct);
    /*6.开启USART1接收中断*/
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE); // 接收数据寄存器空闲
    /*7.配置USART1空闲中断优先级*/
    // 注意：USART1的中断优先级必须低于DMA1通道5的中断优先级，这是保险措施
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel                   = USART1_IRQn;  // USART1空闲中断
	NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 0;
	NVIC_Init(&NVIC_InitStruct);
    /*8.开启DMA1通道5接收USART1数据*/
    USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);/*1.每个通道连接着多个外设，通过逻辑或输入到DMA1控制器，这意味着同时只能有一个请求有效.
                                                  2.外设的DMA请求，可以通过设置相应外设寄存器中的控制位，被独立地开启或关闭。*/
/********************************************************分界线下为配置DMA****************************************************************** */    
    /*8.配置DMA1通道5为接收USART1数据到双缓冲区*/
    DMA_InitTypeDef DMA_InitStruct={
        .DMA_BufferSize         = DMA_BUF_SIZE,                  //每次传输的数据量
        .DMA_M2M                = DMA_M2M_Disable,               //禁止内存到内存传输
        .DMA_Mode               = DMA_Mode_Normal,               //工作模式：非循环模式
        .DMA_Priority           = DMA_Priority_VeryHigh,         // DMA优先级：非常高
        .DMA_DIR                = DMA_DIR_PeripheralSRC,         //数据传输方向：外设到内存
        .DMA_MemoryBaseAddr     = (uint32_t)dma1_buf1,            //注意传入的应该是缓冲区的地址
        .DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte,       //内存数据大小：字节
        .DMA_MemoryInc          = DMA_MemoryInc_Enable,          //内存地址递增
        .DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR,         //注意传入的应该是DR寄存器的地址
        .DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte,   //外设数据大小：字节
        .DMA_PeripheralInc      = DMA_PeripheralInc_Disable,     //外设地址不递增
    };
    DMA_Init(DMA1_Channel5,&DMA_InitStruct);
    /*9.使能DMA1通道5的传输完成中断*/
    DMA_ITConfig(DMA1_Channel5,DMA_IT_TC,ENABLE);

//  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
//	NVIC_InitTypeDef NVIC_InitStruct;
	NVIC_InitStruct.NVIC_IRQChannel                   = DMA1_Channel5_IRQn;  // DMA1通道5中断
	NVIC_InitStruct.NVIC_IRQChannelCmd                = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;                   // DMA1通道5中断优先级必须高于USART1的空闲中断
	NVIC_InitStruct.NVIC_IRQChannelSubPriority        = 0;
	NVIC_Init(&NVIC_InitStruct);

    DMA_Cmd(DMA1_Channel5,ENABLE);
    /*开启USART1*/
    USART_Cmd(USART1, ENABLE);
}

/**
 * @brief 1.DMA1通道5中断处理函数，串口1接收双缓冲满中断; 
 *        2.改变缓冲区; 
 *        3.将双缓冲数据拷贝到环形缓冲区;
 */
void DMA1_Channel5_IRQHandler_usart1_rxFULL_callback(void)
{
    /*每一个通道都有一组配置寄存器，此处串口1接收功能使用DMA1的通道5*/
    if(DMA_GetITStatus(DMA1_IT_TC5) == SET)
    {
        DMA1_Channel5->CCR &= ~DMA_CCR5_EN;                     //关闭DMA才可向寄存器写配置

        uint8_t *full_buf;
        if(dma1_buf_index)//buf2 满中断
        {
            //切换缓冲区到buf1
            DMA1_Channel5->CMAR = (uint32_t)dma1_buf1;
            dma1_buf_index = 0;
            //此时dma1_buf2中存有新数据
            full_buf = dma1_buf2;
        }
        else//buf1 满中断
        {
            //切换缓冲区到buf2
            DMA1_Channel5->CMAR = (uint32_t)dma1_buf2;
            dma1_buf_index = 1;
            //此时dma1_buf1中存有新数据
            full_buf = dma1_buf1;
        }
        DMA1_Channel5->CNDTR = (uint16_t)DMA_BUF_SIZE;          // 重新设置传输数据量
        DMA1_Channel5->CCR |= DMA_CCR5_EN;                     //开启DMA传输

        //将full_buf指向的填满数据的dma_bufx拷贝到LWRB环形缓冲区
        lwrb_write(&buff_1,full_buf,DMA_BUF_SIZE);
        
        DMA_ClearITPendingBit(DMA1_IT_TC5);
    }
}

/**
 * @brief USART1空闲中断处理函数（DMA方式，处理未满缓冲区数据）
 * 1.USART1空闲中断回调函数，处理未满缓冲区数据，保证GPS全帧报文都能被转移到LWRB环形缓冲区中
 * @note 空闲中断发生时，DMA缓冲区可能未满，应将已接收数据从缓冲区头部写入环形缓冲区，并及时重启DMA。
 */
void USART1_IRQHandler_IDLE_callback(void)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        volatile uint32_t tmp;
        tmp = USART1->SR;  // 先读SR
        tmp = USART1->DR;  // 再读DR，清除空闲中断标志（必须，防止中断重复进入）
        (void)tmp;

        DMA1_Channel5->CCR &= ~DMA_CCR5_EN;  // 关闭DMA

        uint16_t received = DMA_BUF_SIZE - DMA1_Channel5->CNDTR;  // 计算实际接收到的数据量
        uint8_t  *cur_buf ;                                       // 当前缓冲区指针
        uint8_t  *next_buf;                                       // 下一个缓冲区指针

        if(dma1_buf_index == 0)
        {
            cur_buf       = dma1_buf1;  // buf1未满
            next_buf      = dma1_buf2;  // buf2为下一个缓冲区
            dma1_buf_index = 1;         // 切换到buf2
        }
        else
        {
            cur_buf       = dma1_buf2;  // buf2未满
            next_buf      = dma1_buf1;  // buf1为下一个缓冲区
            dma1_buf_index = 0;         // 切换到buf1
        }
        
        DMA1_Channel5->CMAR   = (uint32_t)next_buf;      // 立即切换到下一个缓冲区
        DMA1_Channel5->CNDTR  = (uint32_t)DMA_BUF_SIZE;  // 重新设置传输数据量
        DMA1_Channel5->CCR   |= DMA_CCR5_EN;             // 重新使能DMA

        // 将实际接收到的数据从当前缓冲区写入LWRB环形缓冲区
        if (received > 0 && received <= DMA_BUF_SIZE)
        {
            // 从缓冲区头部起始写入实际接收到的数据
            lwrb_write(&buff_1, cur_buf, received);
        }
    }
}
#endif

void GPS_Init_all_module(void)
{
    // 初始化GPS所依赖的的软件：初始化stm32内的一个大小合适的环形缓冲区
    init_lwrb();
    // 初始化GPS模块所依赖硬件：USART+DMA配置并启动工作
    DMA_UART4_to_arrybuffer_init();
}

/**
 * @brief GPS解析任务，解析器流式解析环形缓冲区的数据
 * 
 * @param lwgps_handle lwgps库中的lwgps_t指针型变量;
 * 作用：将从环形缓冲区解析得到的GPS模块所有的信息，存储到此指针指向的外部lwgps_t类型的变量！
 */
void GPS_Parser_lwrb(lwgps_t* lwgps_handle)
{
	uint8_t temp_buf[DMA_BUF_SIZE];//建议temp_buf大小与DMA缓冲区一致
    size_t len; 
    /*尝试从环形缓冲区读取数据*/
    while ((len = lwrb_read(&buff_1, temp_buf, sizeof(temp_buf))) > 0) {
        /*送入LWGPS解析*/
        lwgps_process(lwgps_handle, temp_buf, len);//lwgps_process()本身是流式解析，处理速度很快。
        usart1_send_Hex((uint8_t *)temp_buf, len);
    }
}

