// // #include "LoRa.h"
// // #include <math.h>//roundf四舍五入


// // void usart2_init(void)
// // {
// //     RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
// //     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

// //     GPIO_InitTypeDef GPIO_InitStruct={
// //         .GPIO_Pin=GPIO_Pin_2,//TX
// //         .GPIO_Mode=GPIO_Mode_AF_PP,
// //         .GPIO_Speed=GPIO_Speed_50MHz
// //     };
// //     GPIO_Init(GPIOA, &GPIO_InitStruct);
// //     GPIO_InitStruct.GPIO_Pin=GPIO_Pin_3;//RX
// //     GPIO_InitStruct.GPIO_Mode=GPIO_Mode_IN_FLOATING;
// //     GPIO_Init(GPIOA, &GPIO_InitStruct);

// //     USART_InitTypeDef USART_InitStruct={
// //         .USART_BaudRate=9600,
// //         .USART_WordLength=USART_WordLength_8b,
// //         .USART_StopBits=USART_StopBits_1,
// //         .USART_Parity=USART_Parity_No,
// //         .USART_HardwareFlowControl=USART_HardwareFlowControl_None,
// //         .USART_Mode=USART_Mode_Rx | USART_Mode_Tx
// //     };
// //     USART_Init(USART2, &USART_InitStruct);

// //     USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
// //     USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);

// //     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
// //     NVIC_InitTypeDef NVIC_InitStruct={
// //         .NVIC_IRQChannel = USART2_IRQn,
// //         .NVIC_IRQChannelCmd = ENABLE,
// //         .NVIC_IRQChannelPreemptionPriority = 2,
// //         .NVIC_IRQChannelSubPriority = 0
// //     };
// //     NVIC_Init(&NVIC_InitStruct);

// //     USART_Cmd(USART2, ENABLE);
// // }

// // void UART2_IRQHandler_IDLE_and_RXNE_callback(void)
// // {
// //     if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
// //     {
// //         // 处理接收数据
// //         uint8_t data = USART_ReceiveData(USART2);
// //     }
// //     if(USART_GetITStatus(USART2, USART_IT_IDLE) == SET)//当RX引脚上持续1个字符时间（包括停止位）没有新数据时，硬件认为线路进入"空闲"状态
// //     {
// //         // 处理空闲中断
        

// //         // 注意：清除 IDLE后，此时如果线路一直空闲，IDLE 位不会再次被设置。直到后续"有数据→空闲"的变化沿触才会发一次
// //         volatile uint32_t tmp;  // 必须 volatile
// //         tmp      = USART2->SR;   // 先读SR
// //         tmp      = USART2->DR;   // 再读 DR → 清除 IDLE
// //         (void)  tmp;            // 消除警告
// //     }
// // }

// #include "LoRa.h"


// uint8_t LoRa_get_nodes_num(LoRa_t *lora)
// {
//     uint8_t count = 0;
//     for (uint8_t i = 0; i < LORA_NODE_MAX; i++)
//     {
//         if (lora->LoRa_node_online_flag[i] == 1) // 只统计在线节点
//         {
//             count++;
//         }
//     }
//     return count;
// }

// static void LoRa_usart1_init(uint32_t baud)
// {
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

//     GPIO_InitTypeDef GPIO_InitStruct;
//     GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9; // USART1 TX
//     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
//     GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_Init(GPIOA, &GPIO_InitStruct);

//     GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10; // USART1 RX
//     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//     GPIO_Init(GPIOA, &GPIO_InitStruct);

//     USART_InitTypeDef USART_InitStruct;
//     USART_InitStruct.USART_BaudRate = baud;
//     USART_InitStruct.USART_WordLength = USART_WordLength_8b;
//     USART_InitStruct.USART_StopBits = USART_StopBits_1;
//     USART_InitStruct.USART_Parity = USART_Parity_No;
//     USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//     USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//     USART_Init(USART1, &USART_InitStruct);

//     USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
//         NVIC_InitTypeDef NVIC_InitStruct={
//         .NVIC_IRQChannel = USART1_IRQn,
//         .NVIC_IRQChannelCmd = ENABLE,
//         .NVIC_IRQChannelPreemptionPriority = 2,
//         .NVIC_IRQChannelSubPriority = 0
//     };
//     NVIC_Init(&NVIC_InitStruct);

//     USART_Cmd(USART1, ENABLE);
// }

// static void LoRa_usart2_init(uint32_t baud)
// {
//     RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
//     RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

//     GPIO_InitTypeDef GPIO_InitStruct;
//     GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2; // USART2 TX
//     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
//     GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//     GPIO_Init(GPIOA, &GPIO_InitStruct);

//     GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3; // USART2 RX
//     GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
//     GPIO_Init(GPIOA, &GPIO_InitStruct);

//     USART_InitTypeDef USART_InitStruct;
//     USART_InitStruct.USART_BaudRate = baud;
//     USART_InitStruct.USART_WordLength = USART_WordLength_8b;
//     USART_InitStruct.USART_StopBits = USART_StopBits_1;
//     USART_InitStruct.USART_Parity = USART_Parity_No;
//     USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
//     USART_InitStruct.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
//     USART_Init(USART2, &USART_InitStruct);

//     USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
//     NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
//         NVIC_InitTypeDef NVIC_InitStruct={
//         .NVIC_IRQChannel = USART2_IRQn,
//         .NVIC_IRQChannelCmd = ENABLE,
//         .NVIC_IRQChannelPreemptionPriority = 2,
//         .NVIC_IRQChannelSubPriority = 0
//     };
//     NVIC_Init(&NVIC_InitStruct);

//     USART_Cmd(USART2, ENABLE);
// }

// void LoRa_init(uint32_t baud1, uint32_t baud2)
// {
//     LoRa_usart1_init(baud1);
//     LoRa_usart2_init(baud2);
// }

// void UART2_IRQHandler_RXNE_callback(void)
// {
//     if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
//     {
//         // 处理接收数据
//         uint8_t data = USART_ReceiveData(USART2);
//         USART_SendData(USART1, data);
//         USART_ClearITPendingBit(USART2, USART_FLAG_RXNE); // 清除 RXNE 标志位
//     }
// }
// void UART1_IRQHandler_RXNE_callback(void)
// {
//     if(USART_GetITStatus(USART1, USART_IT_RXNE) == SET)
//     {
//         // 处理接收数据
//         uint8_t data = USART_ReceiveData(USART1);
//         USART_SendData(USART2, data);
//         USART_ClearITPendingBit(USART1, USART_FLAG_RXNE); // 清除 RXNE 标志位
//     }
// }

// // void LoRa_bridge_poll(void)
// // {
// //     if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
// //     {
// //         uint8_t ch = USART_ReceiveData(USART1);
// //         LoRa_usart2_send_Char(ch);
// //     }

// //     if (USART_GetFlagStatus(USART2, USART_FLAG_RXNE) == SET)
// //     {
// //         uint8_t ch = USART_ReceiveData(USART2);
// //         LoRa_usart1_send_Char(ch);
// //     }
// // }










/**
 * @file LoRa.c
 * @brief LoRa 驱动实现
 *
 * 编译依赖：
 *   #include "GPS.h"     → GPS_t, gps.lwgps_handle.latitude/longitude/speed/is_valid/fix
 *   #include "bmp280.h"  → BMP280_t, bmp280.Pressure_ture (单位 Pa)
 *   #include "lwgps.h"   → lwgps_to_speed()
 *
 * printf 重定向说明：
 *   项目 syscalls.c 中 _write 目前是空实现（不输出），
 *   本文件改用 LoRa_usart1_send_str() 直接向 USART1 输出调试/MSG 文本，
 *   确保 PC 串口软件能收到内容。
 */

#include "LoRa.h"
#include "lwgps.h"
#include <string.h>
#include <stdio.h>   /* sprintf */

/* ═══════════════════════════════════════════════════════════
 *  私有：接收状态机
 * ═══════════════════════════════════════════════════════════ */

typedef enum {
    RX_WAIT_SYNC0 = 0,
    RX_WAIT_SYNC1,
    RX_WAIT_TYPE,
    RX_POS_DATA,      /* 收集位置载荷 14 字节 */
    RX_MSG_LEN,       /* 等待文本长度字节 */
    RX_MSG_DATA,      /* 收集文本数据 */
} rx_state_t;

#define RX_BUF_SIZE  220u
static struct {
    rx_state_t state;
    uint8_t    buf[RX_BUF_SIZE];
    uint16_t   cnt;          /* 已收到字节数 */
    uint8_t    msg_len;      /* 文本帧期望长度 */
} s_rx;

/* ─── 每节点在线计时器（单位：调用 LoRa_proto_task 的次数 × 100ms）─── */
/* 10s 超时 = 100 次（100ms × 100）*/
#define OFFLINE_TIMEOUT_CNT  100u
static uint8_t s_online_timer[LORA_NODE_MAX];

/* ─── PC 侧接收缓冲（收集完整一行再发 MSG 帧）─── */
#define PC_BUF_SIZE  (LORA_MSG_MAX_LEN + 4u)
static uint8_t  s_pc_buf[PC_BUF_SIZE];
static uint8_t  s_pc_len = 0;

/* ─── 位置帧发送计时（每 50 次 task 调用 = 5s 发一次）─── */
#define POS_SEND_INTERVAL  50u
static uint8_t s_pos_tick = 0;

/* 保存外部传入的主结构体指针，供中断回调使用 */
static LoRa_t *s_lora = NULL;

/* ═══════════════════════════════════════════════════════════
 *  私有：底层 UART 发送（阻塞逐字节，简单可靠）
 * ═══════════════════════════════════════════════════════════ */

static void _usart1_send(const uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, data[i]);
    }
    while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
}

static void _usart2_send(const uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);
        USART_SendData(USART2, data[i]);
    }
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
}

/* 方便输出字符串到 USART1 */
static void _usart1_send_str(const char *str)
{
    _usart1_send((const uint8_t *)str, (uint16_t)strlen(str));
}

/* ═══════════════════════════════════════════════════════════
 *  私有：协议接收处理
 * ═══════════════════════════════════════════════════════════ */

static void _rx_reset(void)
{
    s_rx.state   = RX_WAIT_SYNC0;
    s_rx.cnt     = 0;
    s_rx.msg_len = 0;
}

/* 位置帧处理：解包写入 lora 主结构体，刷新在线计时 */
static void _handle_pos_frame(void)
{
    if (s_lora == NULL) return;

    LoRa_pos_payload_t pld;
    memcpy(&pld, s_rx.buf, sizeof(pld));

    /* 过滤：ID 非法 或 是本机自己的广播回声 */
    if (pld.id >= LORA_NODE_MAX || pld.id == LORA_MY_ID) return;

    uint8_t idx = pld.id;
    s_lora->node[idx].LoRa_id      = pld.id;
    s_lora->node[idx].speed        = pld.speed_x10;
    s_lora->node[idx].latitude     = pld.lat_e6;
    s_lora->node[idx].longitude    = pld.lon_e6;
    s_lora->node[idx].pressure_pa  = pld.pres_pa;

    s_lora->LoRa_node_online_flag[idx] = 1;
    s_online_timer[idx] = OFFLINE_TIMEOUT_CNT; /* 刷新在线计时 */
}

/* 文本消息帧处理：转发到 USART1（PC 串口软件群聊窗口） */
static void _handle_msg_frame(void)
{
    /* 格式：[MSG] 内容\r\n */
    _usart1_send_str("[MSG] ");
    _usart1_send(s_rx.buf, s_rx.cnt);
    _usart1_send_str("\r\n");
}

/* 接收状态机：每字节调用一次（USART2 RXNE 中断调用） */
static void _rx_feed_byte(uint8_t byte)
{
    switch (s_rx.state) {

    case RX_WAIT_SYNC0:
        if (byte == LORA_SYNC0) s_rx.state = RX_WAIT_SYNC1;
        break;

    case RX_WAIT_SYNC1:
        if      (byte == LORA_SYNC1) s_rx.state = RX_WAIT_TYPE;
        else if (byte == LORA_SYNC0) s_rx.state = RX_WAIT_SYNC1; /* AA AA 55 防漏 */
        else    _rx_reset();
        break;

    case RX_WAIT_TYPE:
        s_rx.cnt = 0;
        if      (byte == LORA_TYPE_POS) s_rx.state = RX_POS_DATA;
        else if (byte == LORA_TYPE_MSG) s_rx.state = RX_MSG_LEN;
        else    _rx_reset(); /* 未知类型 */
        break;

    case RX_POS_DATA:
        if (s_rx.cnt < (uint16_t)sizeof(LoRa_pos_payload_t)) {
            s_rx.buf[s_rx.cnt++] = byte;
        }
        if (s_rx.cnt >= (uint16_t)sizeof(LoRa_pos_payload_t)) {
            _handle_pos_frame();
            _rx_reset();
        }
        break;

    case RX_MSG_LEN:
        s_rx.msg_len = byte;
        s_rx.cnt     = 0;
        if (byte == 0 || byte > LORA_MSG_MAX_LEN) {
            _rx_reset(); /* 长度异常 */
        } else {
            s_rx.state = RX_MSG_DATA;
        }
        break;

    case RX_MSG_DATA:
        if (s_rx.cnt < s_rx.msg_len && s_rx.cnt < RX_BUF_SIZE) {
            s_rx.buf[s_rx.cnt++] = byte;
        }
        if (s_rx.cnt >= s_rx.msg_len) {
            _handle_msg_frame();
            _rx_reset();
        }
        break;

    default:
        _rx_reset();
        break;
    }
}

/* ═══════════════════════════════════════════════════════════
 *  私有：发帧函数
 * ═══════════════════════════════════════════════════════════ */

/**
 * 发送位置帧到 LoRa 空中
 * lat_e6/lon_e6：double 纬经度 × 1000000 四舍五入后的 int32
 * speed_x10：float m/s × 10 四舍五入后的 uint16（最大 6553.5 m/s，足够）
 * pres_pa：uint32 气压 Pa
 */
static void _send_pos_frame(int32_t lat_e6, int32_t lon_e6,
                             uint16_t speed_x10, uint32_t pres_pa)
{
    uint8_t frame[LORA_POS_FRAME_SIZE];
    frame[0] = LORA_SYNC0;
    frame[1] = LORA_SYNC1;
    frame[2] = LORA_TYPE_POS;

    LoRa_pos_payload_t *p = (LoRa_pos_payload_t *)&frame[3];
    p->id       = LORA_MY_ID;
    p->speed_x10 = speed_x10;
    p->lat_e6   = lat_e6;
    p->lon_e6   = lon_e6;
    p->pres_pa  = pres_pa;

    _usart2_send(frame, sizeof(frame));
}

/**
 * 发送文本消息帧到 LoRa 空中
 * text：文本内容，len：字节数（0 < len <= LORA_MSG_MAX_LEN）
 */
static void _send_msg_frame(const uint8_t *text, uint8_t len)
{
    if (len == 0 || len > LORA_MSG_MAX_LEN) return;
    uint8_t header[4] = { LORA_SYNC0, LORA_SYNC1, LORA_TYPE_MSG, len };
    _usart2_send(header, 4);
    _usart2_send(text, len);
}

/* ═══════════════════════════════════════════════════════════
 *  硬件初始化
 * ═══════════════════════════════════════════════════════════ */

static void _usart1_hw_init(uint32_t baud)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef g;
    g.GPIO_Pin   = GPIO_Pin_9; /* PA9: TX */
    g.GPIO_Mode  = GPIO_Mode_AF_PP;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &g);
    g.GPIO_Pin  = GPIO_Pin_10; /* PA10: RX */
    g.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &g);

    USART_InitTypeDef u;
    u.USART_BaudRate            = baud;
    u.USART_WordLength          = USART_WordLength_8b;
    u.USART_StopBits            = USART_StopBits_1;
    u.USART_Parity              = USART_Parity_No;
    u.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    u.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART1, &u);

    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_InitTypeDef n;
    n.NVIC_IRQChannel                   = USART1_IRQn;
    n.NVIC_IRQChannelCmd                = ENABLE;
    n.NVIC_IRQChannelPreemptionPriority = 2; /* 高于 USART2，PC 输入优先响应 */
    n.NVIC_IRQChannelSubPriority        = 0;
    NVIC_Init(&n);

    USART_Cmd(USART1, ENABLE);
}

static void _usart2_hw_init(uint32_t baud)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    GPIO_InitTypeDef g;
    g.GPIO_Pin   = GPIO_Pin_2; /* PA2: TX */
    g.GPIO_Mode  = GPIO_Mode_AF_PP;
    g.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &g);
    g.GPIO_Pin  = GPIO_Pin_3; /* PA3: RX */
    g.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &g);

    USART_InitTypeDef u;
    u.USART_BaudRate            = baud;
    u.USART_WordLength          = USART_WordLength_8b;
    u.USART_StopBits            = USART_StopBits_1;
    u.USART_Parity              = USART_Parity_No;
    u.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    u.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &u);

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    NVIC_InitTypeDef n;
    n.NVIC_IRQChannel                   = USART2_IRQn;
    n.NVIC_IRQChannelCmd                = ENABLE;
    n.NVIC_IRQChannelPreemptionPriority = 3; /* 低于 USART1 */
    n.NVIC_IRQChannelSubPriority        = 0;
    NVIC_Init(&n);

    USART_Cmd(USART2, ENABLE);
}

/* ═══════════════════════════════════════════════════════════
 *  对外接口实现
 * ═══════════════════════════════════════════════════════════ */

void LoRa_init(uint32_t baud1, uint32_t baud2)
{
    _usart1_hw_init(baud1);
    _usart2_hw_init(baud2);
}

void LoRa_proto_init(LoRa_t *lora)
{
    s_lora = lora;
    _rx_reset();
    memset(s_online_timer, 0, sizeof(s_online_timer));
    s_pos_tick = 0;
    s_pc_len   = 0;
}

uint8_t LoRa_get_nodes_num(LoRa_t *lora)
{
    uint8_t n = 0;
    for (uint8_t i = 0; i < LORA_NODE_MAX; i++) {
        if (lora->LoRa_node_online_flag[i]) n++;
    }
    return n;
}

/**
 * @brief LoRa 协议任务，每 100ms 在 task_proc 中调用一次
 *
 * 内部逻辑：
 *   1. 在线超时计数：每次调用对所有在线节点计时器 -1，减到 0 则标记离线
 *   2. 位置帧发送：每 POS_SEND_INTERVAL 次（=5s）采集并发送本机位置
 */
void LoRa_proto_task(LoRa_t *lora, GPS_t *gps, BMP280_t *bmp, Type_Struct_Timezone_and_UTCxTime *rtc)
{
    /* ── 1. 离线超时检测（每 100ms 减一，100次=10s） ── */
    for (uint8_t i = 0; i < LORA_NODE_MAX; i++) {
        if (lora->LoRa_node_online_flag[i]) {
            if (s_online_timer[i] > 0) {
                s_online_timer[i]--;
            } else {
                lora->LoRa_node_online_flag[i] = 0; /* 超时，标记离线 */
            }
        }
    }

    /* ── 2. 定时发送本机位置帧（每 5s 一次，POS_SEND_INTERVAL=50，100ms×50=5s）── */
    s_pos_tick++;
    if (s_pos_tick >= POS_SEND_INTERVAL) {
        s_pos_tick = 0;

        /* 仅在 GPS 有效定位时才发送，避免广播无效数据干扰其他节点的在线状态判断 */
        GPS_t *g = (GPS_t *)gps;
        if (g->lwgps_handle.is_valid && g->lwgps_handle.fix) {
            int32_t  lat_e6    = 0;
            int32_t  lon_e6    = 0;
            uint16_t spd_x10   = 0;
            uint32_t pres_pa   = 101325; /* 标准大气压兜底 */

            /* 纬度/经度：double → ×1000000 → int32，四舍五入 */
            double abs_lat = g->lwgps_handle.latitude;
            double abs_lon = g->lwgps_handle.longitude;
            lat_e6 = (int32_t)(abs_lat * 1000000.0 + (abs_lat >= 0 ? 0.5 : -0.5));
            lon_e6 = (int32_t)(abs_lon * 1000000.0 + (abs_lon >= 0 ? 0.5 : -0.5));

            /* 速度：节（knots）→ m/s → ×10 → uint16，四舍五入 */
            float spd_mps = (float)lwgps_to_speed(g->lwgps_handle.speed, lwgps_speed_mps);
            spd_x10 = (uint16_t)(spd_mps * 10.0f + 0.5f);

            /* 气压：直接读 bmp280.Pressure_ture（单位已是 Pa）*/
            BMP280_t *b = (BMP280_t *)bmp;
            if (b->Pressure_ture > 0) {
                pres_pa = (uint32_t)b->Pressure_ture;
            }

            _send_pos_frame(lat_e6, lon_e6, spd_x10, pres_pa);
        }
    }
}

/* ═══════════════════════════════════════════════════════════
 *  中断回调实现
 * ═══════════════════════════════════════════════════════════ */

/**
 * USART2 RXNE 中断回调：LoRa 模块收到的字节 → 接收状态机
 * 在 stm32f10x_it.c 的 USART2_IRQHandler 中调用（已注册）
 */
void UART2_IRQHandler_RXNE_callback(void)
{
    if (USART_GetITStatus(USART2, USART_IT_RXNE) == SET) {
        uint8_t byte = (uint8_t)USART_ReceiveData(USART2);
        USART_ClearITPendingBit(USART2, USART_FLAG_RXNE);
        //if(是AT指令响应) { 接收到AT指令响应的处理逻辑，模仿_handle_msg_frame函数，[lora]AT响应\r\n }
        _rx_feed_byte(byte);
    }
}

/**
 * USART1 RXNE 中断回调：PC 发来的字节 → 缓存 → 回车触发 MSG 帧广播
 * 在 stm32f10x_it.c 的 USART1_IRQHandler 中调用（已注册）
 *
 * 使用方法（PC sscom 串口软件）：
 *   ① 不勾选"HEX发送"，直接输入文本
 *   ② 以回车（\r 或 \n）结尾发送
 *   ③ 所有在线 LoRa 节点收到后，USART1 输出: [MSG] 你输入的内容\r\n
 */
void UART1_IRQHandler_RXNE_callback(void)
{
    if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
        uint8_t byte = (uint8_t)USART_ReceiveData(USART1);
        USART_ClearITPendingBit(USART1, USART_FLAG_RXNE);

        /* 回显到 PC（sscom 的发送框里能看到自己输入的内容） */
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
        USART_SendData(USART1, byte);

        if (byte == '\r' || byte == '\n') {
            /* 回车/换行 → 触发广播 */
            if (s_pc_len > 0) {
                _send_msg_frame(s_pc_buf, s_pc_len);
                s_pc_len = 0;
            }
        } else if (byte == '\b' || byte == 0x7F) {
            /* 退格 */
            if (s_pc_len > 0) s_pc_len--;
        } else {
            /* 普通字符 → 加入缓冲 */
            if (s_pc_len < LORA_MSG_MAX_LEN) {
                s_pc_buf[s_pc_len++] = byte;
            }
            /* 缓冲满时自动触发发送 */
            if (s_pc_len >= LORA_MSG_MAX_LEN) {
                _send_msg_frame(s_pc_buf, s_pc_len);
                s_pc_len = 0;
            }
        }
    }
}