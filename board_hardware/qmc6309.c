/**
 * @file qmc6309.c
 * @author YLAD (yhlad0801@gmail.com)
 * @brief qmc6309磁力计驱动
 * @version 0.1
 * @date 2026-03-10
 * 
 * @copyright Copyright (c) 2026
 * 
 * @note 1.改完参数后需重新做一次硬铁/软铁校准（模式改变有时会影响 offset）。
 */
#include "qmc6309.h"
#include <string.h>
#include "iic.h"

#include <stdio.h> //printf
#include "Delay.h"

//判断变量val是否在区间[low,high]中；  0：不在该区间  1：在该区间
#define IS_IN_RANGE(val, low, high)  (((val) >= (low)) && ((val) <= (high)))


/**
 * @brief 设置低通滤波器的深度
 * 
 * @param qmc6309 
 * @param OSR2_xxx QMC6309_Low_Pass_Filter_e:
 *                     OSR2_x1 = 0,   //000 - 最小滤波深度，带宽和噪声大；功耗低；
 *                     OSR2_x2 = 1,   //001
 *                     OSR2_x4 = 2,   //010
 *                     OSR2_x8 = 3,   //011
 *                     OSR2_x16= 4    //100 - 最大滤波深度，带宽和噪声小；功耗高；
 * @return uint8_t 
 */
uint8_t QMC6309_Set_CTRL1_LPF(QMC6309_t *qmc6309, QMC6309_Low_Pass_Filter_e OSR2_xxx)
{
    if(OSR2_xxx > OSR2_x16) return 1;
    qmc6309->QMC6309_CTRL_1_REG_config = (qmc6309->QMC6309_CTRL_1_REG_config & 0x1f) | (OSR2_xxx<<5);
    return 0;
}
/**
 * @brief 设置过采样率
 * 
 * @param qmc6309 
 * @param OSR1_xxx QMC6309_Over_Sample_Ratio_e:
 *                     OSR1_x8 = 0,   //00 - 带宽和噪声小；功耗高；
 *                     OSR1_x4 = 1,   //01
 *                     OSR1_x2 = 2,   //10
 *                     OSR1_x1 = 3    //11 - 带宽和噪声大；功耗低；
 * @return uint8_t 
 */
uint8_t QMC6309_Set_CTRL1_Over_Sample_Ratio(QMC6309_t *qmc6309, QMC6309_Over_Sample_Ratio_e OSR1_xxx)
{
    if(OSR1_xxx > OSR1_x1) return 1;
    qmc6309->QMC6309_CTRL_1_REG_config = (qmc6309->QMC6309_CTRL_1_REG_config & 0xe7) | (OSR1_xxx<<3);
    return 0;
}
/**
 * @brief 设置工作模式
 * 
 * @param qmc6309 
 * @param Mode QMC6309_Mode_Control_e:
 *                     Suspend_Mode    = 0,   //00 - 关闭模式
 *                     Normal_Mode     = 1,   //01 - 正常测量模式 - 可调数据输出速率
 *                     Single_Mode     = 2,   //01 - 单次测量模式
 *                     Continuous_Mode = 3    //11 - 连续测量模式
 * @return uint8_t 
 */
uint8_t QMC6309_Set_CTRL1_Mode_Control(QMC6309_t *qmc6309, QMC6309_Mode_Control_e Mode)
{
    if(Mode > Continuous_Mode) return 1;
    qmc6309->QMC6309_CTRL_1_REG_config = (qmc6309->QMC6309_CTRL_1_REG_config & 0xfc) | (Mode);
    return 0;
}
/**
 * @brief 设置输出数据速率
 * 
 * @param qmc6309 
 * @param ODR_xxxHZ QMC6309_Output_data_Rate_e:
 *                     ODR_100HZ = 0,   //000 - 100Hz
 *                     ODR_200HZ = 1,   //001 - 200Hz
 *                     ODR_400HZ = 2,   //010 - 400Hz
 *                     ODR_800HZ = 3    //011 - 800Hz
 * @return uint8_t 
 */
uint8_t QMC6309_Set_CTRL2_ODR(QMC6309_t *qmc6309, QMC6309_Output_data_Rate_e ODR_xxxHZ)
{
    if(ODR_xxxHZ > ODR_200HZ) return 1;
    qmc6309->QMC6309_CTRL_2_REG_config = (qmc6309->QMC6309_CTRL_2_REG_config & 0x8f) | (ODR_xxxHZ<<4);
    return 0;
}
/**
 * @brief 设置满量程范围
 * 
 * @param qmc6309 
 * @param RNG_xxxGuass QMC6309_Full_Scale_Range_e:
 *                     RNG_32Guass =0,   //00 or 11 - 大量程，精度小[-32,32]Gauss
 *                     RNG_16Guass =1,   //01 - 中量程，精度中[-16,16]Gauss
 *                     RNG_8Guass  =2    //10 - 小量程，精度高[-8,8]Gauss
 * @return uint8_t 
 */
uint8_t QMC6309_Set_CTRL2_Full_Scale_Range(QMC6309_t *qmc6309, QMC6309_Full_Scale_Range_e RNG_xxxGuass)
{
    if(RNG_xxxGuass > RNG_8Guass) return 1;
    qmc6309->QMC6309_CTRL_2_REG_config = (qmc6309->QMC6309_CTRL_2_REG_config & 0xf3) | (RNG_xxxGuass<<2);
    return 0;
}
/**
 * @brief 是否开启Set/Reset技术，开启后可修正漂移，精度更高；
 * 
 * @param qmc6309 
 * @param on_off QMC6309_Set_and_reset_mode_control_e：
 *                                      Set_and_reset_on  = 0,   //00 - (首选)高精度，可修正漂移
 *                                      Set_only_on       = 1,   //01
 *                                      Set_and_reset_off = 3,   //11
 * @return uint8_t 0：设置成功  1：设置失败（参数值无效）
 */
uint8_t QMC6309_Set_CTRL2_Set_and_reset_mode_control(QMC6309_t *qmc6309, QMC6309_Set_and_reset_mode_control_e on_off)
{
    if( (on_off > Set_and_reset_off) || (on_off == 2)) return 1;
    qmc6309->QMC6309_CTRL_2_REG_config = (qmc6309->QMC6309_CTRL_2_REG_config & 0xfc) | on_off;
    return 0;
}


/*************************************************************************************************** */

/**
 * @brief 读取芯片ID,检查QMC6309是否存在
 * 
 * @return u8 0：芯片存在且正常  1：实例指针无效或函数指针未赋值  2：无法读取数据  3：读取成功但芯片ID不匹配
 */
uint8_t QMC6309_Get_ID_Check(QMC6309_t *qmc6309)
{
	uint8_t chip_id,res,error_flag;
	uint8_t num=5;//最多尝试5次
	if(qmc6309 == NULL || qmc6309->iic_Read_Len == NULL) return 1;//如果函数指针没有被正确赋值，直接返回1表示检查失败
	do{
		res = qmc6309->iic_Read_Len(qmc6309->iic_SlaveAddr,QMC6309_CHIPID_REG,1,&chip_id);//0:读取成功  1：读取失败
		if (res != 0) {
            error_flag = 2; // 读取失败
        } else if (chip_id != QMC6309_CHIPID_VALUE) {
            error_flag = 3; // 读取成功但ID不匹配
        } else {
            error_flag = 0; // 成功
            break;
        }
	}while(--num);
	return error_flag;
}
/**
 * @brief DRDY 位表示数据的状态，当三个轴的数据都准备好并加载到每个模式下的输出数据寄存器中时该位会被置位。通过 I2C 命令读取状态寄存器可将其重置为“0”。
 * 
 * @param qmc6309 
 * @param DRDY 返回该状态位 0：没有新数据  1：有新数据准备就绪
 * @return uint8_t 0：读取成功  1：读取失败
 */
uint8_t QMC6309_Get_STATUS_DRDY(QMC6309_t *qmc6309, u8 *DRDY)
{
    uint8_t temp;
    if(qmc6309->iic_Read_Len(qmc6309->iic_SlaveAddr, QMC6309_STATUS_REG, 1, &temp) != 0) return 1;
    *DRDY = (temp & 0x01)==0 ? 0 : 1; // DRDY位是状态寄存器的最低位
    return 0;
}
/**
 * @brief 表示内置自测试测量的状态
 * 
 * @param qmc6309 
 * @param ST_RDY 返回该状态位 0：自测试未完成  1：自测试已完成，数据已准备好可供读取
 * @return uint8_t 0：读取成功  1：读取失败
 */
uint8_t QMC6309_Get_STATUS_ST_RDY(QMC6309_t *qmc6309, u8 *ST_RDY)
{
    uint8_t temp;
    if(qmc6309->iic_Read_Len(qmc6309->iic_SlaveAddr, QMC6309_STATUS_REG, 1, &temp) != 0) return 1;
    *ST_RDY = (temp & 0x04)==0 ? 0 : 1; 
    return 0;
}
/**(貌似使用不到？)
 * @brief 表示任一轴的输出超出[-32000，32000]最低有效位的范围时置为高电平；并在读取状态寄存器后重置为“0”。
 * 
 * @param qmc6309 
 * @param OVFL 返回该状态位 0：没有数据溢出  1：发生数据溢出
 * @return uint8_t 0：读取成功  1：读取失败
 */
uint8_t QMC6309_Get_STATUS_OVFL(QMC6309_t *qmc6309, u8 *OVFL)
{
    uint8_t temp;
    if(qmc6309->iic_Read_Len(qmc6309->iic_SlaveAddr, QMC6309_STATUS_REG, 1, &temp) != 0) return 1;
    *OVFL = (temp & 0x02)==0 ? 0 : 1; // OVFL位是状态寄存器的第二位
    return 0;
}
/**
 * @brief 读取QMC6309的控制寄存器1和控制寄存器2的值，保存到实例的成员变量中，方便用户随时查看当前的设置参数；注意：用户更改qmc6309的控制寄存器设置各种高级参数时，先修改实例的QMC6309_CTRL_1_REG_config和QMC6309_CTRL_2_REG_config这两个成员的值，最后调用QMC6309_Send_Refresh_Settings函数统一将设置真正的写入到寄存器；
 * 
 * @param qmc6309 
 * @return uint8_t 0：读取成功  1：读取失败
 */
uint8_t QMC6309_Get_CTRL_1_and_CTRL_2_reg(QMC6309_t *qmc6309)
{
	uint8_t data[2]={0,0};
	if(qmc6309->iic_Read_Len(qmc6309->iic_SlaveAddr,QMC6309_CTRL_1_REG,2,data)) return 1;
	qmc6309->QMC6309_CTRL_1_REG_copy = data[0];
	qmc6309->QMC6309_CTRL_2_REG_copy = data[1];
    qmc6309->QMC6309_CTRL_1_REG_config = qmc6309->QMC6309_CTRL_1_REG_copy;
    qmc6309->QMC6309_CTRL_2_REG_config = qmc6309->QMC6309_CTRL_2_REG_copy;
	return 0;
}
/**
 * @brief 读取3轴磁力数据
 * 
 * @param qmc6309 
 * @return uint8_t 0：读取成功  1：没有新数据可读(比bmp280有更好的防重复读机制)  2：读取数据失败
 */
uint8_t QMC6309_Get_Magnetic(QMC6309_t *qmc6309)
{
    /*防止读取到重复数据*/
    uint8_t drdy;
    if( (QMC6309_Get_STATUS_DRDY(qmc6309, &drdy) != 0) || (drdy == 0)) return 1;/*添加这一句非常必要，提供比bmp280更好的机制；能保证即使读取频率远超 ODR，也绝不重复读取上一次已读的旧数据*/
    uint8_t data[6];
    if(qmc6309->iic_Read_Len(qmc6309->iic_SlaveAddr, QMC6309_X_LSB_REG, 6, data) != 0) return 2;
    qmc6309->x = (int16_t)(( (uint16_t)data[1] << 8) | data[0]);
    qmc6309->y = (int16_t)(( (uint16_t)data[3] << 8) | data[2]);
    qmc6309->z = (int16_t)(( (uint16_t)data[5] << 8) | data[4]);
    return 0;
}

/*************************************************************************************************** */

/**
 * @brief 将所有寄存器重置为初始值；软件复位可以在任何模式的任意时间触发。设置为高电平后，SOFT_RST位不会自动清除。因此，在执行软件复位命令0BH=80后，总是需要执行另一个命令0BH=00，不需要等待任何标志位；
 * 
 * @param qmc6309 
 * @return uint8_t 0：成功复位  1：向SOFT_RST位写1失败  2：向SOFT_RST位写0失败
 */
uint8_t QMC6309_Send_SoftReset(QMC6309_t *qmc6309)
{
	uint8_t temp=0x80;
	if(qmc6309->iic_Write_Len(qmc6309->iic_SlaveAddr,QMC6309_CTRL_2_REG,1,&temp)) return 1;//向控制软件复位的位写1触发软件复位；
    temp = 0x00;
	if(qmc6309->iic_Write_Len(qmc6309->iic_SlaveAddr,QMC6309_CTRL_2_REG,1,&temp)) return 2;//不会自动清0,应手动清0；
	return 0;
}
/**
 * @brief 设置为连续模式进行3轴自检
 * 
 * @param qmc6309 
 * @return uint8_t 0：自检成功  1：设置挂起模式失败  2：设置连续模式失败  (x：轮询DRDY一直是0,表示连续模式一直不输出新数据)  3：设置自检使能位失败  4：轮询ST_RDY一直为0,表示自检一直处于未完成  5：读取自检数据失败    6：自检数据不在合理范围内
 */
uint8_t QMC6309_Self_test(QMC6309_t *qmc6309)
{
    //1.Set Suspend Mode
        uint8_t temp = 0x00;
        if(qmc6309->iic_Write_Len(qmc6309->iic_SlaveAddr,QMC6309_CTRL_1_REG,1,&temp)) return 1;
    //2.Set Continuous Mode - 只有当芯片处于连续模式时，才能将自检位设置为高电平以使芯片进入自检模式；
        temp = 0x03;
        if(qmc6309->iic_Write_Len(qmc6309->iic_SlaveAddr,QMC6309_CTRL_1_REG,1,&temp)) return 2;
    //3.轮询DRDY标志位变为1，等到上一步设置的连续模式能正常工作输出数据，再进行下面的步骤；大约等20ms
        Delay_ms(20);//还是严格按照手册延时，不要自以为是:(
        // uint8_t drdy;
        // while( (QMC6309_Get_STATUS_DRDY(qmc6309, &drdy) != 0) || (drdy == 0) ) printf("Waiting for qmc6309 continuous mode...\r\n");//[几乎使用不到，只轮询容易出bug:(]手册只需等20ms，但我觉得不如直接轮询DRDY更保险，能保证连续模式已经正常工作了；如果20ms后DRDY还没变为1，说明连续模式根本没设置成功，直接返回错误了；如果20ms后DRDY已经变为1了，说明连续模式已经正常工作了，可以安全地进行下一步了；
    //4.Set Selftest enable - 自检使能位：自检完成后且自检信号生成后，此位将自动清除。
        temp = 0x80;
        if(qmc6309->iic_Write_Len(qmc6309->iic_SlaveAddr,QMC6309_CTRL_3_REG,1,&temp)) return 3;
    //5.自测开始，轮询ST_RDY标志位变为1，等待自测完成，再进行下面的步骤；大约等150ms
        Delay_ms(150);//还是严格按照手册延时，不要自以为是:(
        uint8_t ST_RDY;
        if( (QMC6309_Get_STATUS_ST_RDY(qmc6309, &ST_RDY) != 0) || (ST_RDY == 0) )
        {
            // printf("Waiting for qmc6309 self-test completed...\r\n");//我调试的时候用的
            return 4;
        }
        // uint8_t ST_RDY;
        // while( (QMC6309_Get_STATUS_ST_RDY(qmc6309, &ST_RDY) != 0) || (ST_RDY == 0) ) printf("Waiting for qmc6309 self-test completed...\r\n");//手册只需等150ms，但我觉得不如直接轮询ST_RDY更保险，能保证自测已经完成了；如果150ms后ST_RDY还没变为1，说明自测根本没完成，直接返回错误了；如果150ms后ST_RDY已经变为1了，说明自测已经完成了，可以安全地进行下一步了；
    //6.自测完成，读取自测数据并且判断每个轴的偏差值是否在手册p13表格中所列范围内，在范围内则表明芯片运行正常。
        int8_t Selftest_data[3];
        if(qmc6309->iic_Read_Len(qmc6309->iic_SlaveAddr, QMC6309_X_Self_Test_REG, 3, (uint8_t *)Selftest_data)) return 5;
        if( !IS_IN_RANGE(Selftest_data[0], -50, -1) || //X轴
            !IS_IN_RANGE(Selftest_data[1], -50, -1) || //Y轴
            !IS_IN_RANGE(Selftest_data[2], -50, -1) )  //Z轴
        {
            printf("QMC6309 self-test failed! Selftest_data: X=%+d, Y=%+d, Z=%+d\r\n", Selftest_data[0], Selftest_data[1], Selftest_data[2]);
            return 6;
        }
    return 0;
}
/**
 * @brief 将结构体成员XXX_config预配置，真正的写入到器件寄存器中；函数内部会先将工作模式设置为挂起模式模式；无须等待直接写入所有的新的设置参数；最后读出器件寄存器实际的值，并保存到结构体XXX_copy成员变量中，并且也同步结构体的XXX_config成员变量，方便后续可以直接调用Set_XXX()函数用于修改该配置而不需要手动调用QMC6309_Get_CTRL_1_and_CTRL_2_reg()来进行读取&&同步；
 * 
 * @param qmc6309 
 * @return uint8_t 0:设置成功  1：设置挂起模式失败  2：写入设置参数到CTRL_2寄存器失败  3：写入设置参数到CTRL_1寄存器失败  4：读取器件寄存器实际的值&&同步失败
 */
uint8_t QMC6309_Send_Refresh_Settings(QMC6309_t *qmc6309)
{
    //1.先将工作模式设置为挂起模式，才能修改其他设置参数；因为只有挂起模式下才保证寄存器安全写入
    uint8_t temp = 0x00;
    if(qmc6309->iic_Write_Len(qmc6309->iic_SlaveAddr,QMC6309_CTRL_1_REG,1,&temp)) return 1;
    //x.无须等待，会立即进入挂起模式；注意：根据手册中P13,7.3自测示例(写入挂起命令后立即可以写其他参数配置)
    //2.写入设置参数
    uint8_t res=0;
    if(qmc6309->iic_Write_Len(qmc6309->iic_SlaveAddr,QMC6309_CTRL_2_REG,1,&qmc6309->QMC6309_CTRL_2_REG_config)) res = 2;
    if(qmc6309->iic_Write_Len(qmc6309->iic_SlaveAddr,QMC6309_CTRL_1_REG,1,&qmc6309->QMC6309_CTRL_1_REG_config)) res = 3;
    //3.读取器件真实的配置 && 同步到结构体的4个成员变量中
    if(QMC6309_Get_CTRL_1_and_CTRL_2_reg(qmc6309) != 0) res = 4;
    return res;
}

/*************************************************************************************************** */

/**
 * @brief 初始化QMC6309传感器
 * 
 * @param qmc6309 
 * @param iic_Read_Len 和qmc6309通信时读取数据的函数指针，参数分别是：iic从机地址，寄存器地址，要读写的字节数，数据缓冲区指针；函数返回值：0：操作成功  1：操作失败
 * @param iic_Write_Len 和qmc6309通信时写入数据的函数指针，参数分别是：iic从机地址，寄存器地址，要读写的字节数，数据缓冲区指针；函数返回值：0：操作成功  1：操作失败
 * @param iic_HW_Init IIC硬件接口初始化函数指针，参数为void，返回值为void；如果不需要初始化IIC硬件接口或之前已初始化，可以直接传入NULL
 * @return uint8_t 0：初始化成功  1：芯片ID检查失败  2：自测尝试5次都失败  3：设置参数无效  4：将预设置真正刷新写入到qmc6309失败
 */
uint8_t QMC6309_Init(QMC6309_t *qmc6309, QMC6309_IIC_RW_LEN_p iic_Read_Len, QMC6309_IIC_RW_LEN_p iic_Write_Len, QMC6309_IIC_HW_Init_p iic_HW_Init)
{
    memset(qmc6309, 0, sizeof(QMC6309_t));  // 初始化结构体成员为0
    qmc6309->iic_SlaveAddr = 0x7c; // 设置IIC从设备地址
    qmc6309->iic_Read_Len = iic_Read_Len;
    qmc6309->iic_Write_Len = iic_Write_Len;
    qmc6309->iic_HW_Init = iic_HW_Init;

    if(qmc6309->iic_HW_Init != NULL)
        qmc6309->iic_HW_Init(); // 初始化IIC硬件接口

    if(QMC6309_Get_ID_Check(qmc6309) != 0)
        return 1;//芯片ID检查失败，可能芯片不存在或通信异常
/*继续执行其他初始化步骤，例如配置控制寄存器等*/
    // if(QMC6309_Self_test(qmc6309) != 0)//自测
        // return 2;
    for(uint8_t i=0; i<5; i++)//自测容易出问题导致自测失败；最多尝试5次；
    {
        if(QMC6309_Self_test(qmc6309) == 0) 
            break;//自测成功
        else if(i == 4) //自测失败且已经是最后一次尝试了
        {
            printf("QMC6309 self-test failed after 5 attempts!\r\n");//5次自测都失败了，说明芯片可能有问题了；
            return 2;
        }
    }
    if(
       QMC6309_Set_CTRL1_LPF(qmc6309, OSR2_x8)                                != 0 ||  // x16（最大滤波，航向最稳）
       QMC6309_Set_CTRL1_Over_Sample_Ratio(qmc6309, OSR1_x8)                   != 0 ||  // x8（已知最低噪声 场分辨率可达典型的2.5mG）
       QMC6309_Set_CTRL1_Mode_Control(qmc6309, Normal_Mode)                    != 0 ||  //ODR 只设了 50Hz（人转动/走路几 Hz 就够），Normal Mode 完全够用且更省电。
       QMC6309_Set_CTRL2_ODR(qmc6309, ODR_50HZ)                                != 0 ||  //人转动/走路几Hz就够；50Hz已足够平滑，且功耗更低、噪声更易滤除。100Hz也完全可用（如果你需要更快响应，如手持快速转动）
       QMC6309_Set_CTRL2_Full_Scale_Range(qmc6309, RNG_8Guass)                 != 0 ||  //最高分辨率，±8高斯；如果你需要测量更强的磁场（如靠近扬声器、变压器等强磁场源），可以选择±16或±32高斯，但分辨率会降低。
       QMC6309_Set_CTRL2_Set_and_reset_mode_control(qmc6309, Set_and_reset_on) != 0)    //这是AMR磁阻传感器的核心技术！开启后，每测量前自动施加Set+Reset脉冲，消除offset漂移和磁滞。关闭后长时间使用会积累误差，导致航向慢慢偏。
        return 3;  //设置参数无效
    if(QMC6309_Send_Refresh_Settings(qmc6309)) //将预设置真正刷新写入到qmc6309中！
        return 4;
    return 0;//初始化成功
}