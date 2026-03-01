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
 * 2026/3/1：
 *  1.重构DMA_USART.c和DMA_USART.h，添加GPS.c和GPS.h使用GPS_t结构体实例来管理双缓冲区和环形缓冲区，支持多模块或单元测试场景；
 *  2.重构DMA_USART.c中的中断处理函数，使用传入的GPS_t实例来访问双缓冲区和环形缓冲区，避免使用全局静态变量，增强代码的模块化和可测试性；
 *  3.重构DMA_USART.h中的函数声明，明确传入GPS_t实例或GPS_db_t符合最小接口原则；
 *  
 * @version 0.3
 * @date 2025-06-03
 * 
 * @copyright Copyright (c) 2026
 * 
 */
#include "DMA_USART.h"

/*
 * UART4/DMA2 双缓冲和环形缓冲区的数据不再存在于本文件的静态变量中，
 * 而是通过传入的 GPS_t 结构体进行访问。这样每个 GPS 实例都拥有独立的
 * 缓冲区，可以支持多个模块或单元测试。
 */


/* 之前的 init_lwrb 和 buff_2/buffdata_2 不再需要，初始化由 GPS_init 负责 */
#ifdef __DMA2_UART4_RX_DMA__
/**
 * @brief 初始化UART4的DMA硬件接受功能，配置DMA双缓冲机制，并开启相关中断
 * 
 * @param db 双缓冲区结构体指针，包含两个缓冲区和一个索引，DMA配置将根据索引自动选择当前缓冲区
 * @note  传入GPS_db_t而不是GPS_t原因：
 *          1.初始化 DMA/USART 的过程只需要知道双缓冲区的位置，明确表达“只需要这部分数据”；
 *          2.符合“最小接口原则”，只传递硬件所需的数据
 */
void DMA2_uart4_to_db_HW_Init(GPS_db_t *db)
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
        .DMA_MemoryBaseAddr     = db->double_buffering_index == 0 ? (uint32_t)db->double_buffering1 : (uint32_t)db->double_buffering2,  //注意传入的应该是缓冲区的地址
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


/*
 * 硬件中断回调，移植到GPS_t结构体后由ISR通过gps指针调用
 * 函数体与以前GPS.c中的版本类似，不过引用gps实例中的缓冲区和环形缓冲区。
 */

void DMA2_Channel3_IRQHandler_uart4_rxFULL_callback(GPS_t *gps)
{
    /*每一个通道都有一组配置寄存器，此处串口4接收功能使用DMA2的通道3*/
    if(DMA_GetITStatus(DMA2_IT_TC3) == SET)  // 判断DMA2通道3传输完成中断
    {
        /*1.失能DMA即可改变CMAR和CNDTR*/
        DMA2_Channel3->CCR &= ~DMA_CCR3_EN;
        /*2.切换缓冲区*/
        uint8_t *full_buff;  // 指向刚刚接收满的缓冲区
        if(gps->db.double_buffering_index)  //dma2_buf2刚刚接收满了，准备切换到dma2_buf1继续接收
        {
            DMA2_Channel3->CMAR = (uint32_t)gps->db.double_buffering1;  // 切换到dma2_buf1缓冲区
            gps->db.double_buffering_index = 0;                    // 指示将会使用dma2_buf1缓冲区进行下一轮接收；0: buf1, 1: buf2
            full_buff = gps->db.double_buffering2;
        }
        else  //dma2_buf1刚刚接收满了，准备切换到dma2_buf2继续接收
        {
            DMA2_Channel3->CMAR = (uint32_t)gps->db.double_buffering2;  // 切换到dma2_buf2缓冲区
            gps->db.double_buffering_index = 1;                    // 指示将会使用dma2_buf2缓冲区进行下一轮接收；0: buf1, 1: buf2
            full_buff = gps->db.double_buffering1;
        }
        /*3.重新设置传输数据量*/
        DMA2_Channel3->CNDTR  = DMA_BUF_SIZE;  // 注意：在STM32F10x系列中，CNDTR寄存器的值在DMA传输过程中会自动递减，表示剩余的数据量。因此，在每次DMA传输完成后，需要重新设置CNDTR寄存器的值，以准备下一次传输。
        /*4.重新使能DMA通道*/
        DMA2_Channel3->CCR   |= DMA_CCR3_EN;
        /*5.将双缓冲区中的数据写入环形缓冲区*/
        lwrb_write(&gps->rb.ring_buffer, full_buff, DMA_BUF_SIZE);
        /*6.清除DMA2通道3的传输完成中断标志*/
        DMA_ClearITPendingBit(DMA2_IT_TC3);
    }
}

void UART4_IRQHandler_IDLE_callback(GPS_t *gps)
{
    if(USART_GetITStatus(UART4, USART_IT_IDLE) == SET)  // 判断UART4空闲中断
    {
        uint16_t received_bytes = DMA_BUF_SIZE - DMA2_Channel3->CNDTR;  // 计算本次接收的数据量
        if(received_bytes > 0)  // 如果received_bytes > 0，即可判断出当前不是DMA缓冲区满&&正好也是空闲！！！
        {
            /*1.失能DMA即可改变CMAR和CNDTR*/
            DMA2_Channel3->CCR &= ~DMA_CCR3_EN;
            /*2.切换缓冲区*/
            uint8_t *cur_buff;   // 指向当前正在接收数据的缓冲区
            if(gps->db.double_buffering_index)  //dma2_buf2刚刚接收数据遇到串口空闲了，准备切换到dma2_buf1继续接收
            {
                DMA2_Channel3->CMAR = (uint32_t)gps->db.double_buffering1;  // 切换到dma2_buf1缓冲区
                gps->db.double_buffering_index = 0;                    // 指示将会使用dma2_buf1缓冲区进行下一轮接收；0: buf1, 1: buf2
                cur_buff = gps->db.double_buffering2;
            }
            else  //dma2_buf1刚刚接收数据遇到串口空闲了，准备切换到dma2_buf2继续接收
            {
                DMA2_Channel3->CMAR = (uint32_t)gps->db.double_buffering2;  // 切换到dma2_buf2缓冲区
                gps->db.double_buffering_index = 1;                    // 指示将会使用dma2_buf2缓冲区进行下一轮接收；0: buf1, 1: buf2
                cur_buff = gps->db.double_buffering1;
            }
            /*3.重新设置传输数据量*/
            DMA2_Channel3->CNDTR  = DMA_BUF_SIZE;  // 注意：在STM32F10x系列中，CNDTR寄存器的值在DMA传输过程中会自动递减，表示剩余的数据量。因此，在每次DMA传输完成后，需要重新设置CNDTR寄存器的值，以准备下一次传输。
            /*4.重新使能DMA通道*/
            DMA2_Channel3->CCR   |= DMA_CCR3_EN;
            /*5.将双缓冲区中的数据写入环形缓冲区*/
            lwrb_write(&gps->rb.ring_buffer, cur_buff, received_bytes);
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
void DMA1_usart1_to_db_HW_Init(GPS_db_t *db)
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
        .DMA_MemoryBaseAddr     = db->double_buffering_index == 0 ? (uint32_t)db->double_buffering1 : (uint32_t)db->double_buffering2,  //注意传入的应该是缓冲区的地址
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
void DMA1_Channel5_IRQHandler_usart1_rxFULL_callback(GPS_t *gps)
{
    /*每一个通道都有一组配置寄存器，此处串口1接收功能使用DMA1的通道5*/
    if(DMA_GetITStatus(DMA1_IT_TC5) == SET)
    {
        DMA1_Channel5->CCR &= ~DMA_CCR5_EN;                     //关闭DMA才可向寄存器写配置

        uint8_t *full_buf;
        if(gps->db.double_buffering_index)//buf2 满中断
        {
            //切换缓冲区到buf1
            DMA1_Channel5->CMAR = (uint32_t)gps->db.double_buffering1;
            gps->db.double_buffering_index = 0;
            //此时dma1_buf2中存有新数据
            full_buf = gps->db.double_buffering2;
        }
        else//buf1 满中断
        {
            //切换缓冲区到buf2
            DMA1_Channel5->CMAR = (uint32_t)gps->db.double_buffering2;
            gps->db.double_buffering_index = 1;
            //此时dma1_buf1中存有新数据
            full_buf = gps->db.double_buffering1;
        }
        DMA1_Channel5->CNDTR = (uint16_t)DMA_BUF_SIZE;          // 重新设置传输数据量
        DMA1_Channel5->CCR |= DMA_CCR5_EN;                     //开启DMA传输

        //将full_buf指向的填满数据的dma_bufx拷贝到LWRB环形缓冲区
        lwrb_write(&gps->rb.ring_buffer,full_buf,DMA_BUF_SIZE);
        
        DMA_ClearITPendingBit(DMA1_IT_TC5);
    }
}

/**
 * @brief USART1空闲中断处理函数（DMA方式，处理未满缓冲区数据）
 * 1.USART1空闲中断回调函数，处理未满缓冲区数据，保证GPS全帧报文都能被转移到LWRB环形缓冲区中
 * @note 空闲中断发生时，DMA缓冲区可能未满，应将已接收数据从缓冲区头部写入环形缓冲区，并及时重启DMA。
 */
void USART1_IRQHandler_IDLE_callback(GPS_t *gps)
{
    if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        uint16_t received_bytes = DMA_BUF_SIZE - DMA1_Channel5->CNDTR;  // 计算实际接收到的数据量
        if(received_bytes > 0)
        {
            DMA1_Channel5->CCR &= ~DMA_CCR5_EN;  // 关闭DMA

            uint8_t  *cur_buf ;                                       // 当前缓冲区指针
            uint8_t  *next_buf;                                       // 下一个缓冲区指针

            if(gps->db.double_buffering_index)
            {
                cur_buf       = dma1_buf2;  // 当前buf2未满
                next_buf      = dma1_buf1;  // buf1为下一个缓冲区
                dma1_buf_index = 0;         // 切换到buf1
            }
            else
            {
                cur_buf       = dma1_buf1;  // 当前buf1未满
                next_buf      = dma1_buf2;  // buf2为下一个缓冲区
                dma1_buf_index = 1;         // 切换到buf2
            }

            DMA1_Channel5->CMAR   = (uint32_t)next_buf;      // 立即切换到下一个缓冲区
            DMA1_Channel5->CNDTR  = (uint32_t)DMA_BUF_SIZE;  // 重新设置传输数据量
            DMA1_Channel5->CCR   |= DMA_CCR5_EN;             // 重新使能DMA

            // 将实际接收到的数据从当前缓冲区写入LWRB环形缓冲区
            lwrb_write(&buff_1, cur_buf, received_bytes);

        }
        volatile uint32_t tmp;
        tmp = USART1->SR;  // 先读SR
        tmp = USART1->DR;  // 再读DR，清除空闲中断标志（必须，防止中断重复进入）
        (void)tmp;
    }
}
#endif
