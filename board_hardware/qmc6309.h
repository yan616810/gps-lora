#ifndef __QMC6309_H
#define __QMC6309_H
#include "stm32f10x.h"

#define QMC6309_CHIPID_VALUE 0x90
// 通过地磁计芯片，我焊接在pcb上并不平整，我如何软件校准？是不是我如果知道我本地地磁大小和磁倾角，当我将我的pcb朝向通过芯片输出的3轴磁大小可以合成唯一一个方向确定，大小确定的向量

/*memory map*/
#define QMC6309_CHIPID_REG 0x00      // 芯片ID寄存器(R-Only)
#define QMC6309_X_LSB_REG 0x01       // X轴数据寄存器(R-Only)
#define QMC6309_X_MSB_REG 0x02       // X轴数据寄存器(R-Only)
#define QMC6309_Y_LSB_REG 0x03       // Y轴数据寄存器(R-Only)
#define QMC6309_Y_MSB_REG 0x04       // Y轴数据寄存器(R-Only)
#define QMC6309_Z_LSB_REG 0x05       // Z轴数据寄存器(R-Only)
#define QMC6309_Z_MSB_REG 0x06       // Z轴数据寄存器(R-Only)
#define QMC6309_STATUS_REG 0x09      // 状态寄存器(R-Only)
#define QMC6309_CTRL_1_REG 0x0A      // 控制寄存器(R/W)
#define QMC6309_CTRL_2_REG 0x0B      // 控制寄存器(R/W)
#define QMC6309_CTRL_3_REG 0x0E      // 控制寄存(R/W)
#define QMC6309_X_Self_Test_REG 0x13 // X轴自检寄存器(R-Only)
#define QMC6309_Y_Self_Test_REG 0x14 // Y轴自检寄存器(R-Only)
#define QMC6309_Z_Self_Test_REG 0x15 // Z轴自检寄存器(R-Only)


/*QMC6309_CTRL_1_REG相关配置*/
typedef enum{
    OSR2_x1 =0,   //000
    OSR2_x2 ,     //001
    OSR2_x4 ,     //010
    OSR2_x8 ,     //011
    OSR2_x16,     //[100,111]
} QMC6309_Low_Pass_Filter_e;//为了获得更好的噪声性能，还添加了另一个滤波器；其深度可以通过OSR2进行调整。
typedef enum{
    OSR1_x8 =0,   //00 - 带宽和噪声小；功耗高；
    OSR1_x4 ,     //01
    OSR1_x2 ,     //10
    OSR1_x1 ,     //11
} QMC6309_Over_Sample_Ratio_e;//用于控制内部数字滤波器的带宽。较大的OSR1值会导致滤波器带宽更小，带内噪声更少，但功耗更高。
typedef enum{
    Suspend_Mode    =0,   //00
    Normal_Mode     ,     //01
    Single_Mode     ,     //10
    Continuous_Mode ,     //11
} QMC6309_Mode_Control_e;//四种模式分别是挂起模式、正常模式、单次模式和连续模式。上电复位（POR）后的默认模式是挂起模式。在连续模式、单次模式和正常模式之间切换时，应在中间加入挂起模式。

/*QMC6309_CTRL_2_REG相关配置*/
// typedef enum{//（使用不到!）
//     SOFT_RST_No_reset =0,   //0 - 应手动清零
//     SOFT_RST_reset    ,     //1 - 复位，所有寄存器值变为初始值
// } QMC6309_Soft_reset_e;//通过将SOFT_RST寄存器设为高电平可以执行软复位。软复位可以在任何模式的任何时间触发。设置为高电平后，SOFT_RST位不会自动清除。因此，在执行软复位命令0BH=80后，总是需要另一个命令0BH=00。
typedef enum{
    ODR_1HZ   =0,   //000
    ODR_10HZ  ,     //001
    ODR_50HZ  ,     //010
    ODR_100HZ ,     //011
    ODR_200HZ,      //[100,111]
} QMC6309_Output_data_Rate_e;//输出数据速率由ODR寄存器控制。可以选择四种数据更新频率：10Hz、50Hz、100Hz或200Hz。
typedef enum{
    RNG_32Guass =0,   //00 or 11 - 大量程，精度小
    RNG_16Guass,      //01
    RNG_8Guass ,      //10 - 小量程，精度大
} QMC6309_Full_Scale_Range_e;//磁传感器的量程可以通过RNG寄存器进行选择。满量程由应用环境决定。最低量程具有最高灵敏度，因此分辨率也最高。
typedef enum{
    Set_and_reset_on  = 0,   //00 - (首选)高精度，可修正漂移
    Set_only_on       = 1,   //01
    Set_and_reset_off = 3,   //11
} QMC6309_Set_and_reset_mode_control_e;/*在SET ONLY ON或SET AND RESET OFF模式下，测量过程中偏移量不会被更新。后两种模式offset漂移没人帮你修正！
                                        原理：QMC6309 内部是 AMR 磁阻传感器（各向异性磁阻），它的磁芯像一块“磁铁”，会慢慢“疲劳”：
                                        温度变化、时间久了、被磁铁靠近过……都会产生 offset 漂移（零点不准，X/Y/Z 读数偏移几百 LSB）。
                                        Set/Reset 技术 就是芯片内置的“磁芯刷新器”：
                                        SET：给传感器通一个强电流，把磁芯“磁化”到正方向
                                        RESET：再通反向电流，把磁芯“磁化”到反方向
                                        两次测量结果相减 → 完美抵消 offset！*/
/*QMC6309_CTRL_3_REG相关配置*/
//自测设能位；


typedef uint8_t (*QMC6309_IIC_RW_LEN_p)(uint8_t iic_SlaveAddr, uint8_t reg, uint8_t len, uint8_t *data); // 定义一个函数指针类型，参数分别是：iic从机地址，寄存器地址，要读写的字节数，数据缓冲区指针；函数返回值：0：操作成功  1：操作失败
typedef void (*QMC6309_IIC_HW_Init_p)(void);                                    // 定义一个函数指针
typedef struct qmc6309
{
    uint8_t iic_SlaveAddr;
    uint8_t QMC6309_CTRL_1_REG_config;  // 用户更改qmc6309的控制寄存器设置各种高级参数时，会先修改这两个成员的值，最后调用QMC6309_Send_Refresh_Settings函数统一将设置真正的写入到寄存器；
    uint8_t QMC6309_CTRL_2_REG_config;  // 用户更改qmc6309的控制寄存器设置各种高级参数时，会先修改这两个成员的值，最后调用QMC6309_Send_Refresh_Settings函数统一将设置真正的写入到寄存器；
    uint8_t QMC6309_CTRL_1_REG_copy;    // 读寄存器后保存在这里，方便用户随时查看当前的设置参数
    uint8_t QMC6309_CTRL_2_REG_copy;    // 读寄存器后保存在这里，方便用户随时查看当前的设置参数
    int16_t x;
    int16_t y;
    int16_t z;
    QMC6309_IIC_RW_LEN_p iic_Read_Len;
    QMC6309_IIC_RW_LEN_p iic_Write_Len;
    QMC6309_IIC_HW_Init_p iic_HW_Init;
} QMC6309_t;


uint8_t QMC6309_Set_CTRL1_LPF(QMC6309_t *qmc6309, QMC6309_Low_Pass_Filter_e OSR2_xxx);
uint8_t QMC6309_Set_CTRL1_Over_Sample_Ratio(QMC6309_t *qmc6309, QMC6309_Over_Sample_Ratio_e OSR1_xxx);
uint8_t QMC6309_Set_CTRL1_Mode_Control(QMC6309_t *qmc6309, QMC6309_Mode_Control_e Mode);
uint8_t QMC6309_Set_CTRL2_ODR(QMC6309_t *qmc6309, QMC6309_Output_data_Rate_e ODR_xxxHZ);
uint8_t QMC6309_Set_CTRL2_Full_Scale_Range(QMC6309_t *qmc6309, QMC6309_Full_Scale_Range_e RNG_xxxGuass);
uint8_t QMC6309_Set_CTRL2_Set_and_reset_mode_control(QMC6309_t *qmc6309, QMC6309_Set_and_reset_mode_control_e on_off);

uint8_t QMC6309_Get_ID_Check(QMC6309_t *qmc6309);
uint8_t QMC6309_Get_STATUS_DRDY(QMC6309_t *qmc6309, u8 *DRDY);
uint8_t QMC6309_Get_STATUS_ST_RDY(QMC6309_t *qmc6309, u8 *ST_RDY);
uint8_t QMC6309_Get_STATUS_OVFL(QMC6309_t *qmc6309, u8 *OVFL);
uint8_t QMC6309_Get_CTRL_1_and_CTRL_2_reg(QMC6309_t *qmc6309);
uint8_t QMC6309_Get_Magnetic(QMC6309_t *qmc6309);

uint8_t QMC6309_Send_SoftReset(QMC6309_t *qmc6309);
uint8_t QMC6309_Self_test(QMC6309_t *qmc6309);
uint8_t QMC6309_Send_Refresh_Settings(QMC6309_t *qmc6309);

uint8_t QMC6309_Init(QMC6309_t *qmc6309, QMC6309_IIC_RW_LEN_p iic_Read_Len, QMC6309_IIC_RW_LEN_p iic_Write_Len, QMC6309_IIC_HW_Init_p iic_HW_Init);

#endif /* __QMC6309_H */