// #ifndef __LORA_H
// #define __LORA_H

// #include <stdint.h>
// #include "stm32f10x.h"


// #define LORA_NODE_MAX 16 // 最多支持的节点数


// //LoRa节点的详细信息
// typedef struct LoRa_location_info
// {
//     uint8_t  LoRa_id;      // LoRa设备ID
//     uint16_t speed;        // 速度，单位放大10倍，精确到0.1m/s
//     int32_t  latitude;     // 纬度，放大1000000倍后的整数
//     int32_t  longitude;    // 经度，放大1000000倍后的整数
//     uint32_t pressure_pa;  // 气压，单位Pa - 用来计算相对高度
// }LoRa_node_info_t;

// //LoRa设备的主结构体，包含所有节点信息和在线状态
// typedef struct LoRa{
//     uint8_t          LoRa_node_online_flag[LORA_NODE_MAX];  // 0表示离线，1表示在线
//     LoRa_node_info_t node[LORA_NODE_MAX];                   // 存储接收到的所有节点信息
//     //每次接收到新报文后，进行数据处理得到方位和距离,相对高度；在5s内没有接收到该节点的新报文，就把该节点的在线状态置为离线；UI界面根据在线状态来决定是否显示该节点；
// // //OLED使用
// //     double LoRa_node_bearing[LORA_NODE_MAX];  // 每个节点的方位角，单位为度
// //     double LoRa_node_distance[LORA_NODE_MAX]; // 每个节点的距离，单位为米
// //     int32_t LoRa_node_RH[LORA_NODE_MAX];      // 每个节点的相对高度，单位为米，正数表示高于我，负数表示低于我
// }LoRa_t;


// uint8_t LoRa_get_nodes_num(LoRa_t *lora); // 获取在线节点数量


// /**
//  * @brief Initialize USART1 (PC side) and USART2 (LoRa side) for transparent forwarding.
//  * @param baud1 USART1 baud rate
//  * @param baud2 USART2 baud rate
//  */
// void LoRa_init(uint32_t baud1, uint32_t baud2);

// void UART1_IRQHandler_RXNE_callback(void);
// void UART2_IRQHandler_RXNE_callback(void); 

// #endif // __LORA_H


// // #ifndef __LORA_PROTO_H
// // #define __LORA_PROTO_H

// // #include "stm32f10x.h"
// // #include <stdint.h>

// // /* ═══ 配置区（每台设备烧录时修改 MY_ID） ═══ */
// // #define LORA_MY_ID          1           // 本机节点ID，0~15，组内唯一
// // #define LORA_DEFAULT_HOP    3           // Gossip初始跳数
// // #define LORA_MAX_MEMBERS    16          // 最大成员数
// // #define LORA_POS_INTERVAL   5000        // 本机位置广播周期(ms)
// // #define LORA_MSG_MAX_LEN    180         // 短信最大字节数

// // /* ═══ 帧头 ═══ */
// // #define FRAME_SYNC0         0xAA
// // #define FRAME_SYNC1         0x55
// // #define TYPE_POS            0x01        // 位置帧
// // #define TYPE_MSG            0x02        // 消息帧

// // /* ═══ AUX引脚（HIGH=忙，LOW=空闲）根据实际接线修改 ═══ */
// // #define LORA_AUX_PORT       GPIOB
// // #define LORA_AUX_PIN        GPIO_Pin_1

// // /* ═══ 位置帧载荷 17字节 ═══ */
// // typedef struct __attribute__((packed)) {
// //     float    lat;       // 纬度（度，正北）
// //     float    lon;       // 经度（度，正东）
// //     float    alt;       // 海拔(m)
// //     float    speed;     // 速度(m/s)
// //     uint32_t utc;       // UTC时间戳(秒)
// //     uint8_t  battery;   // 电量(%)
// // } LoRa_PosPld_t;        // sizeof = 17

// // /* ═══ 成员信息表（UI层直接读取） ═══ */
// // typedef struct {
// //     uint8_t      valid;             // 是否有效
// //     LoRa_PosPld_t pos;             // 最新位置
// //     uint32_t     last_update_ms;   // 最后更新时刻(ms)
// // } LoRa_Member_t;

// // /* ═══ 消息日志（UI层直接读取） ═══ */
// // #define MSG_LOG_SIZE        8
// // typedef struct {
// //     uint8_t  valid;
// //     uint8_t  src_id;
// //     char     text[LORA_MSG_MAX_LEN + 1];
// //     uint8_t  len;
// //     uint32_t tick_ms;
// // } MsgLog_t;

// // /* ═══ 导出 ═══ */
// // extern LoRa_Member_t lora_members[LORA_MAX_MEMBERS];
// // extern MsgLog_t      msg_log[MSG_LOG_SIZE];
// // extern uint8_t       msg_unread;        // 未读消息数，UI显示角标
// // extern uint8_t       lora_online_cnt;  // 当前在线节点数（5min内有更新）

// // void    LoRa_Proto_Init(void);
// // void    LoRa_Proto_Task(void);          // 放入task_proc()，~每100ms调用
// // void    LoRa_RxByte_FromUSART2(uint8_t b); // 在USART2中断中调用
// // void    LoRa_SendMsg(const char *text, uint16_t len); // 发送短信

// // #endif











#ifndef __LORA_H
#define __LORA_H

/**
 * @file LoRa.h
 * @brief LoRa 驱动：USART 初始化 + 应用层协议（位置帧 & 文本消息帧）
 *
 * 硬件接线：
 *   USART1 (PA9/PA10, baud1=9600) <---> CH340 <---> PC 串口软件(sscom)
 *   USART2 (PA2/PA3,  baud2=9600) <---> DX-LR31-433T22S LoRa 模块
 *
 * LoRa 模块需预先配置（透明传输，两端完全一致）：
 *   +++
 *   AT+MODE0        透明传输
 *   AT+LEVEL2       2148bps 空中速率
 *   AT+CHANNEL01    信道01 (434.15MHz)
 *   AT+RESET
 *
 * ═══════════════════ 自定义应用层协议 ═══════════════════
 *
 * 【位置帧 TYPE=0xA1】  总长 18 字节，发给所有人（广播）
 *  SYNC(2) | TYPE(1) | id(1) | speed_x10(2) | lat_e6(4) | lon_e6(4) | pres_pa(4)
 *  0xAA 0x55  0xA1
 *
 * 【文本消息帧 TYPE=0xB2】  总长 4+LEN 字节，发给所有人
 *  SYNC(2) | TYPE(1) | LEN(1) | DATA(LEN bytes)
 *  0xAA 0x55  0xB2  LEN
 *
 * 接收路由：
 *   0xA1 → 写入 LoRa_t.node[id]，刷新在线计时
 *   0xB2 → 转发到 USART1，PC 串口软件看到 "[MSG] 内容\r\n"
 *
 * ═══════════════════ 调用方式（main.c）═══════════════════
 *
 * 初始化：
 *   LoRa_init(9600, 9600);          // 已在 main.c 中调用
 *   LoRa_proto_init(&lora);         // 新增：协议状态机清零
 *
 * 任务调度（在 task_proc() 的 lora_cnt==100 分支）：
 *   LoRa_proto_task(&lora, &gps, &bmp280);  // 每 100ms 调用一次
 *
 * PC 发来文本消息：
 *   USART1 中断收到字节 → 缓存 → 回车触发 → 自动广播 MSG 帧（无需额外调用）
 */

#include <stdint.h>
#include "stm32f10x.h"
#include "GPS.h"
#include "bmp280.h"
#include "RTC.h"

/* ─── 本机节点 ID（每台设备烧录时修改，0~15 内唯一）─── */
#define LORA_MY_ID      1


#define LORA_NODE_MAX   16  /* 最多支持 16 个节点 */

/* ─── 帧同步字 & 类型 ─── */
#define LORA_SYNC0      0xAA
#define LORA_SYNC1      0x55
#define LORA_TYPE_POS   0xA1   /* 位置帧 */
#define LORA_TYPE_MSG   0xB2   /* 文本消息帧 */

/* ─── 位置帧载荷（紧凑打包，14字节）─── */
typedef struct __attribute__((packed)) {
    uint8_t  id;           /* 节点 ID（0~15） */
    uint16_t speed_x10;   /* 速度 ×10，单位 0.1 m/s */
    int32_t  lat_e6;       /* 纬度 ×1000000，南纬为负 */
    int32_t  lon_e6;       /* 经度 ×1000000，西经为负 */
    uint32_t pres_pa;      /* 气压，单位 Pa（用于相对高度计算） */
} LoRa_pos_payload_t;      /* sizeof = 14 */

#define LORA_POS_FRAME_SIZE  (3u + sizeof(LoRa_pos_payload_t))  /* 3+14=17 */
#define LORA_MSG_MAX_LEN     210u  /* 文本最大字节数，含帧头4B不超过214B < 模块230B分包上限 */

/* ─── LoRa 节点信息（UI 层只读，协议层写入）─── */
typedef struct LoRa_location_info{
    uint8_t  LoRa_id;       /* 节点 ID（0~15） */
    uint16_t speed;         /* 速度 ×10，单位 0.1 m/s */
    int32_t  latitude;      /* 纬度 ×1000000 */
    int32_t  longitude;     /* 经度 ×1000000 */
    uint32_t pressure_pa;   /* 气压，单位 Pa */
} LoRa_node_info_t;

/* ─── LoRa 主结构体（全局唯一，main.c 中声明）─── */
typedef struct LoRa{
    uint8_t          LoRa_node_online_flag[LORA_NODE_MAX]; /* 0=离线 1=在线 */
    LoRa_node_info_t node[LORA_NODE_MAX];
} LoRa_t;


/* ══════════════════════════════════════════════
 *  对外接口
 * ══════════════════════════════════════════════ */

/**
 * @brief 初始化 USART1（PC/CH340）和 USART2（LoRa 模块）
 *        已在 main.c 中调用: LoRa_init(9600, 9600)
 * @param baud1 USART1 波特率
 * @param baud2 USART2 波特率（需与 LoRa 模块配置一致，默认 9600）
 */
void LoRa_init(uint32_t baud1, uint32_t baud2);

/**
 * @brief 协议层初始化（状态机清零，在 LoRa_init 之后调用一次）
 * @param lora 全局 LoRa 主结构体指针
 */
void LoRa_proto_init(LoRa_t *lora);

/**
 * @brief LoRa 协议任务，放入 task_proc() 的 lora_cnt==100 分支，每 100ms 调用一次
 *        内部负责：
 *          ① 离线超时计数（10s 未收到该节点报文则标记离线）
 *          ② 每 5s 采集本机 GPS+气压数据，发送位置帧
 * @param lora   全局 LoRa 主结构体指针
 * @param gps    全局 GPS 结构体指针（lwgps_handle 内有 latitude/longitude/speed）
 * @param bmp280 全局 BMP280 结构体指针（Pressure_ture 字段，单位 Pa）
 */
void LoRa_proto_task(LoRa_t *lora, GPS_t *gps, BMP280_t *bmp, Type_Struct_Timezone_and_UTCxTime *rtc);

/**
 * @brief 获取当前在线节点数量（UI 层调用）
 */
uint8_t LoRa_get_nodes_num(LoRa_t *lora);

/* ─── 中断回调（在 stm32f10x_it.c 中调用，已注册）─── */
void UART1_IRQHandler_RXNE_callback(void);  /* PC→STM32: 收集文本消息 */
void UART2_IRQHandler_RXNE_callback(void);  /* LoRa→STM32: 接收帧字节 */

#endif /* __LORA_H */