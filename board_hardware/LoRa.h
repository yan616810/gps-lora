#ifndef __LORA_H
#define __LORA_H

#include <stdint.h>
#include "stm32f10x.h"


#define LORA_NODE_MAX 16 // 最多支持的节点数


//LoRa节点的详细信息
typedef struct LoRa_location_info
{
    uint8_t  LoRa_id;      // LoRa设备ID
    uint16_t speed;        // 速度，单位放大10倍，精确到0.1m/s
    int32_t  latitude;     // 纬度，放大1000000倍后的整数
    int32_t  longitude;    // 经度，放大1000000倍后的整数
    uint32_t pressure_pa;  // 气压，单位Pa - 用来计算相对高度
}LoRa_node_info_t;

//LoRa设备的主结构体，包含所有节点信息和在线状态
typedef struct LoRa{
    uint8_t          LoRa_node_online_flag[LORA_NODE_MAX];  // 0表示离线，1表示在线
    LoRa_node_info_t node[LORA_NODE_MAX];                   // 存储接收到的所有节点信息
    //每次接收到新报文后，进行数据处理得到方位和距离,相对高度；在5s内没有接收到该节点的新报文，就把该节点的在线状态置为离线；UI界面根据在线状态来决定是否显示该节点；
// //OLED使用
//     double LoRa_node_bearing[LORA_NODE_MAX];  // 每个节点的方位角，单位为度
//     double LoRa_node_distance[LORA_NODE_MAX]; // 每个节点的距离，单位为米
//     int32_t LoRa_node_RH[LORA_NODE_MAX];      // 每个节点的相对高度，单位为米，正数表示高于我，负数表示低于我
}LoRa_t;


uint8_t LoRa_get_nodes_num(LoRa_t *lora); // 获取在线节点数量


/**
 * @brief Initialize USART1 (PC side) and USART2 (LoRa side) for transparent forwarding.
 * @param baud1 USART1 baud rate
 * @param baud2 USART2 baud rate
 */
void LoRa_init(uint32_t baud1, uint32_t baud2);

void UART1_IRQHandler_RXNE_callback(void);
void UART2_IRQHandler_RXNE_callback(void); 

#endif // __LORA_H


// #ifndef __LORA_PROTO_H
// #define __LORA_PROTO_H

// #include "stm32f10x.h"
// #include <stdint.h>

// /* ═══ 配置区（每台设备烧录时修改 MY_ID） ═══ */
// #define LORA_MY_ID          1           // 本机节点ID，0~15，组内唯一
// #define LORA_DEFAULT_HOP    3           // Gossip初始跳数
// #define LORA_MAX_MEMBERS    16          // 最大成员数
// #define LORA_POS_INTERVAL   5000        // 本机位置广播周期(ms)
// #define LORA_MSG_MAX_LEN    180         // 短信最大字节数

// /* ═══ 帧头 ═══ */
// #define FRAME_SYNC0         0xAA
// #define FRAME_SYNC1         0x55
// #define TYPE_POS            0x01        // 位置帧
// #define TYPE_MSG            0x02        // 消息帧

// /* ═══ AUX引脚（HIGH=忙，LOW=空闲）根据实际接线修改 ═══ */
// #define LORA_AUX_PORT       GPIOB
// #define LORA_AUX_PIN        GPIO_Pin_1

// /* ═══ 位置帧载荷 17字节 ═══ */
// typedef struct __attribute__((packed)) {
//     float    lat;       // 纬度（度，正北）
//     float    lon;       // 经度（度，正东）
//     float    alt;       // 海拔(m)
//     float    speed;     // 速度(m/s)
//     uint32_t utc;       // UTC时间戳(秒)
//     uint8_t  battery;   // 电量(%)
// } LoRa_PosPld_t;        // sizeof = 17

// /* ═══ 成员信息表（UI层直接读取） ═══ */
// typedef struct {
//     uint8_t      valid;             // 是否有效
//     LoRa_PosPld_t pos;             // 最新位置
//     uint32_t     last_update_ms;   // 最后更新时刻(ms)
// } LoRa_Member_t;

// /* ═══ 消息日志（UI层直接读取） ═══ */
// #define MSG_LOG_SIZE        8
// typedef struct {
//     uint8_t  valid;
//     uint8_t  src_id;
//     char     text[LORA_MSG_MAX_LEN + 1];
//     uint8_t  len;
//     uint32_t tick_ms;
// } MsgLog_t;

// /* ═══ 导出 ═══ */
// extern LoRa_Member_t lora_members[LORA_MAX_MEMBERS];
// extern MsgLog_t      msg_log[MSG_LOG_SIZE];
// extern uint8_t       msg_unread;        // 未读消息数，UI显示角标
// extern uint8_t       lora_online_cnt;  // 当前在线节点数（5min内有更新）

// void    LoRa_Proto_Init(void);
// void    LoRa_Proto_Task(void);          // 放入task_proc()，~每100ms调用
// void    LoRa_RxByte_FromUSART2(uint8_t b); // 在USART2中断中调用
// void    LoRa_SendMsg(const char *text, uint16_t len); // 发送短信

// #endif