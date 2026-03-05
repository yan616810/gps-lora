#include "bmp280.h"
#include "iic.h"
#include <string.h>
#include <math.h>
/*
1.连接BMP280时，如果连接的是模块&从设备地址是0x76，SDO地址位可不用接地(其模块内部已经接地)。
2.CSB接3.3V选择IIC接口协议。

*/
// struct bmp280 p_bmp280;


// u8 BMP280_Set_TemperatureOversampling(BMP280_T_OVERSAMPLING oversamp_temperature)
// {
// 	u8 v_data_u8=0,res=0;
// 	if (oversamp_temperature<=BMP280_T_MODE_x5) 
// 	{
// 		p_bmp280.oversamp_temperature=oversamp_temperature;
// 		v_data_u8=IIC_Read_Byte(BMP280_SlaveAddr,BMP280_CTRLMEAS_REG);//读取出控制寄存器的值
// 		v_data_u8=((v_data_u8&~0xE0)|((p_bmp280.oversamp_temperature<<5)&0xE0));
// 		res=IIC_Write_Byte(BMP280_SlaveAddr,BMP280_CTRLMEAS_REG,v_data_u8);
// 	}
// 	else res=1;
// 	return res;
// }
// u8 BMP280_CalibParam(void)
// {
// 	u8 a_data_u8[24],res=0;
// 	//memset(a_data_u8,0,24*sizeof(u8));
// 	res=IIC_Read_Len(BMP280_SlaveAddr,BMP280_DIG_T1_LSB_REG,24,a_data_u8);
// 	p_bmp280.calib_param.dig_T1=(u16)((((u16)((u8)a_data_u8[1]))<<8)|a_data_u8[0]);
// 	p_bmp280.calib_param.dig_T2=(s16)((((s16)((s8)a_data_u8[3]))<<8)|a_data_u8[2]);
// 	p_bmp280.calib_param.dig_T3=(s16)((((s16)((s8)a_data_u8[5]))<<8)|a_data_u8[4]);
// 	p_bmp280.calib_param.dig_P1=(u16)((((u16)((u8)a_data_u8[7]))<<8)|a_data_u8[6]);
// 	p_bmp280.calib_param.dig_P2=(s16)((((s16)((s8)a_data_u8[9]))<<8)|a_data_u8[8]);
// 	p_bmp280.calib_param.dig_P3=(s16)((((s16)((s8)a_data_u8[11]))<<8)|a_data_u8[10]);
// 	p_bmp280.calib_param.dig_P4=(s16)((((s16)((s8)a_data_u8[13]))<<8)|a_data_u8[12]);
// 	p_bmp280.calib_param.dig_P5=(s16)((((s16)((s8)a_data_u8[15]))<<8)|a_data_u8[14]);
// 	p_bmp280.calib_param.dig_P6=(s16)((((s16)((s8)a_data_u8[17]))<<8)|a_data_u8[16]);
// 	p_bmp280.calib_param.dig_P7=(s16)((((s16)((s8)a_data_u8[19]))<<8)|a_data_u8[18]);
// 	p_bmp280.calib_param.dig_P8=(s16)((((s16)((s8)a_data_u8[21]))<<8)|a_data_u8[20]);
// 	p_bmp280.calib_param.dig_P9=(s16)((((s16)((s8)a_data_u8[23]))<<8)|a_data_u8[22]);
// 	return res;
// }
// u8 BMP280_SetPowerMode(BMP280_POWER_MODE_e mode)
// {
// 	u8 v_mode_u8=0,res=0;
// 	if (mode<=BMP280_NORMAL_MODE)
// 	{
// 		v_mode_u8=(p_bmp280.oversamp_temperature<<5)+(p_bmp280.oversamp_pressure<<2)+mode;
// 		res=IIC_Write_Byte(BMP280_SlaveAddr,BMP280_CTRLMEAS_REG,v_mode_u8);
// 		}
// 	else res=2;
// 		return res;
// }
// u8 BMP280_SetWorkMode(WORKING_MODE mode)
// {
// 	u8 res=0,v_data_u8=0;
// 	if (mode<=0x04) 
// 	{
// 		v_data_u8=IIC_Read_Byte(BMP280_SlaveAddr,BMP280_CTRLMEAS_REG);//读取出控制寄存器的值
// 		switch(mode)
// 		{
// 			case BMP280_ULTRA_LOW_POWER_MODE:
// 				p_bmp280.oversamp_temperature=BMP280_P_MODE_x1;
// 			p_bmp280.oversamp_pressure=BMP280_P_MODE_x1;
// 			break;
			
// 			case BMP280_LOW_POWER_MODE:
// 				p_bmp280.oversamp_temperature=BMP280_P_MODE_x1;
// 			p_bmp280.oversamp_pressure=BMP280_P_MODE_x2;
// 			break;
			
// 			case BMP280_STANDARD_RESOLUTION_MODE:
// 				p_bmp280.oversamp_temperature=BMP280_P_MODE_x1;
// 			p_bmp280.oversamp_pressure=BMP280_P_MODE_x4;				
// 			break;
			
// 			case BMP280_HIGH_RESOLUTION_MODE:
// 				p_bmp280.oversamp_temperature=BMP280_P_MODE_x1;
// 			p_bmp280.oversamp_pressure=BMP280_P_MODE_x8;
// 			break;
			
// 			case BMP280_ULTRA_HIGH_RESOLUTION_MODE:
// 				p_bmp280.oversamp_temperature=BMP280_P_MODE_x2;
// 			p_bmp280.oversamp_pressure=BMP280_P_MODE_x16;
// 			break;
// 		}
// 		v_data_u8=((v_data_u8&~0xE0)|((p_bmp280.oversamp_temperature<<5)&0xE0));
// 		v_data_u8=((v_data_u8&~0x1C)|((p_bmp280.oversamp_pressure<<2)&0x1C));
// 		res=IIC_Write_Byte(BMP280_SlaveAddr,BMP280_CTRLMEAS_REG,v_data_u8);
// 	} 
// 	else res=1;
// 	return res;
// }
// u8 BMP280_SetStandbyDurn(BMP280_T_SB standby_durn)
// {
// 	u8 v_data_u8=0;
// 	v_data_u8=IIC_Read_Byte(BMP280_SlaveAddr,BMP280_CONFIG_REG);                             //读取出寄存器的值
// 	v_data_u8=((v_data_u8&~0xE0)|((standby_durn<<5)&0xE0));                    //高3位
// 	return IIC_Write_Byte(BMP280_SlaveAddr,BMP280_CONFIG_REG,v_data_u8);
// }
// u8 bmp_filter(BMP280_FILTER_COEFFICIENT filter)
// {
// 	u8 v_data_u8=0;
// 	v_data_u8=IIC_Read_Byte(BMP280_SlaveAddr,BMP280_CONFIG_REG);
// 	v_data_u8=((v_data_u8&~0x1C)|((filter<<3)&0x1C)); //此处应为<<2
// 	return IIC_Write_Byte(BMP280_SlaveAddr,BMP280_CONFIG_REG,v_data_u8);
// }
// u8 BMP280_SetSoftReset(void)
// {
// 	return (IIC_Write_Byte(BMP280_SlaveAddr,BMP280_RESET_REG,BMP280_RESET_VALUE));

// }

// u8 BMP280_Init(void)
// {
// 	BMP280_iic_or_spi_init();//bmp280依赖的主控软硬件初始化

// 	// if(BMP280_SetSoftReset()) return 1; 
// 	// delay_ms(200); //软件上电复位后需要等待2ms才可通信
// 	if(BMP280_Chack()) return 2;
// 	else
// 	{
// 		if(BMP280_CalibParam())
// 			return 3;
// 		if(BMP280_SetSoftReset())
// 			return 4;
// 		if(qingling())
// 			return 5;
// 		if(BMP280_SetPowerMode(BMP280_NORMAL_MODE))
// 			return 6;
// 		if(BMP280_SetWorkMode(BMP280_ULTRA_HIGH_RESOLUTION_MODE))
// 			return 7;
// 		if(BMP280_SetStandbyDurn(BMP280_T_SB_0_5MS))
// 			return 8;
// 		if(bmp_filter(BMP280_FILTER_MODE_4))
// 			return 9;
// 	}
// 	return 0;
// }
// /********************************/
// u8 BMP280_ReadUncompPressureTemperature(s32 *un_press,s32 *un_temp)
// {
// 	u8 a_data_u8[6]={0,0,0,0,0,0},res = 0;
// 	res=IIC_Read_Len(BMP280_SlaveAddr,BMP280_PRESSURE_MSB_REG,6,a_data_u8);
// 	*un_press=(s32)((((u32)(a_data_u8[0]))<<12)|(((u32)(a_data_u8[1]))<<4)|((u32)a_data_u8[2]>>4));/* 气压 */
// 	*un_temp=(s32)((((u32)(a_data_u8[3]))<<12)| (((u32)(a_data_u8[4]))<<4)|((u32)a_data_u8[5]>>4));/* 温度 */
// 	return res;
// }

// // Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// // t_fine carries fine temperature as global value
// BMP280_S32_t t_fine;

// BMP280_S32_t bmp280_compensate_T_int32(BMP280_S32_t adc_T)
// {
//     BMP280_S32_t var1, var2, T;

//     var1 = (((adc_T>>3) - ((BMP280_S32_t)dig_T1<<1)) * ((BMP280_S32_t)dig_T2)) >> 11;
//     var2 = (((((adc_T>>4) - ((BMP280_S32_t)dig_T1)) * ((adc_T>>4) - ((BMP280_S32_t)dig_T1))) >> 12) *
//             ((BMP280_S32_t)dig_T3)) >> 14;

//     t_fine = var1 + var2;
//     T = (t_fine * 5 + 128) >> 8;

//     return T;
// }
// /*s32 BMP280_CompensateTemperatureInt32(s32 un_temp)
// {
// 	s32 v_x1_u32r=0;
// 	s32 v_x2_u32r=0;
// 	s32 temperature=0;
// 	v_x1_u32r=((((un_temp>>3)-((s32)p_bmp280.calib_param.dig_T1<<1)))*((s32)p_bmp280.calib_param.dig_T2))>>11;
// 	v_x2_u32r=(((((un_temp>>4)-((s32)p_bmp280.calib_param.dig_T1))*((un_temp>>4)-((s32)p_bmp280.calib_param.dig_T1)))>>12)*((s32)p_bmp280.calib_param.dig_T3))>>14;
// 	p_bmp280.calib_param.t_fine=v_x1_u32r+v_x2_u32r;
// 	temperature=(p_bmp280.calib_param.t_fine*5+128)>> 8;
// 	return temperature;
// }*/
// s32 BMP280_CompensateTemperatureInt32(s32 un_temp,
//                                      struct bmp280 *dev)
// {
//     s32 var1, var2;
//     s32 temperature;

//     var1 =
//         ((((un_temp>>3) - ((s32) dev->calib_param.dig_T1<<1))) *
//          ((s32) dev->calib_param.dig_T2))>>11;
//     var2 =
//         (((((un_temp>>4) - ((s32) dev->calib_param.dig_T1)) *
//            ((un_temp>>4) - ((s32) dev->calib_param.dig_T1)))>>12) *
//          ((s32) dev->calib_param.dig_T3))>>14;

//     dev->calib_param.t_fine = var1 + var2;

//     temperature =(dev->calib_param.t_fine * 5 + 128)>>8;
//     return temperature;
// }

// /*u32 BMP280_CompensatePressureInt32(s32 un_press)
// {
// 	s32 v_x1_u32r=0;
// 	s32 v_x2_u32r=0;
// 	u32 v_pressure_u32=0;
// 	v_x1_u32r=(((s32)p_bmp280.calib_param.t_fine)>>1)-(s32)64000;
// 	v_x2_u32r=(((v_x1_u32r>>2)* (v_x1_u32r>>2))>>11)*((s32)p_bmp280.calib_param.dig_P6);
// 	v_x2_u32r=v_x2_u32r+((v_x1_u32r *((s32)p_bmp280.calib_param.dig_P5))<< 1);
// 	v_x2_u32r=(v_x2_u32r>>2)+(((s32)p_bmp280.calib_param.dig_P4)<<16);
// 	v_x1_u32r=(((p_bmp280.calib_param.dig_P3*(((v_x1_u32r>>2)*(v_x1_u32r>>2))>>13))>>3)+((((s32)p_bmp280.calib_param.dig_P2)* v_x1_u32r)>>1))>>18;
// 	v_x1_u32r=((((32768 + v_x1_u32r))* ((s32)p_bmp280.calib_param.dig_P1))>>15);
// 	v_pressure_u32=(((u32)(((s32)1048576)-un_press)-(v_x2_u32r>>12)))* 3125;
// 	if(v_pressure_u32<0x80000000)
// 		if(v_x1_u32r!=0)
// 			v_pressure_u32=(v_pressure_u32<<1)/((u32)v_x1_u32r);
// 		else return 0;
// 	else if (v_x1_u32r!=0)
// 		v_pressure_u32=(v_pressure_u32/(u32)v_x1_u32r)*2;
// 	else return 0;
// 	v_x1_u32r=(((s32)p_bmp280.calib_param.dig_P9)*((s32)(((v_pressure_u32>>3)*(v_pressure_u32>>3))>>3)))>>12;
// 	v_x2_u32r=(((s32)(v_pressure_u32>>2))*((s32)p_bmp280.calib_param.dig_P8))>>13;
// 	v_pressure_u32=(u32)((s32)v_pressure_u32+((v_x1_u32r+v_x2_u32r+ p_bmp280.calib_param.dig_P7)>>4));
// 	return v_pressure_u32;
// }*/
// u32 BMP280_CompensatePressureInt32(s32 un_press,
//                                   const struct bmp280 *dev)
// {
//     s32 var1, var2;
//     u32 pressure = 0;

//     var1 = (((s32) dev->calib_param.t_fine) / 2) - (s32) 64000;
//     var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((s32) dev->calib_param.dig_P6);
//     var2 = var2 + ((var1 * ((s32) dev->calib_param.dig_P5)) * 2);
//     var2 = (var2 / 4) + (((s32) dev->calib_param.dig_P4) * 65536);
//     var1 =
//         (((dev->calib_param.dig_P3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8) +
//          ((((s32) dev->calib_param.dig_P2) * var1) / 2)) / 262144;
//     var1 = ((((32768 + var1)) * ((s32) dev->calib_param.dig_P1)) / 32768);

//     /* Avoid exception caused by division with zero */
//     if (var1 != 0)
//     {
//         pressure = (((u32)(((s32)1048576) - un_press) - (var2 / 4096))) * 3125;

//         /* Check for overflows against UINT32_MAX/2; if pressure is left-shifted by 1 */
//         if (pressure < 0x80000000)
//         {
//             pressure = (pressure * 2) / ((u32) var1);
//         }
//         else
//         {
//             pressure = (pressure / (u32) var1) * 2;
//         }

//         var1 = (((s32) dev->calib_param.dig_P9) * ((s32) (((pressure / 8) * (pressure / 8)) / 8192))) / 4096;
//         var2 = (((s32) (pressure / 4)) * ((s32) dev->calib_param.dig_P8)) / 8192;
//         pressure = (u32) ((s32) pressure + ((var1 + var2 + dev->calib_param.dig_P7) / 16));
//     }

//     return pressure;
// }
// u8 BMP280_ReadPressureTemperature(u32 *press,s32 *temp)
// {
// 	s32 un_press=0;
// 	s32 un_temp=0;
// 	u8 res=0;
// 	res=BMP280_ReadUncompPressureTemperature(&un_press,&un_temp);
	
// 	/* 读取真实的温度值和气压值*/
// 	*temp=BMP280_CompensateTemperatureInt32(un_temp,&p_bmp280);
// 	*press=BMP280_CompensatePressureInt32(un_press,&p_bmp280);
// 	return res;
// }




/**
 * @brief 单独设置BMP280的温度过采样率
 * 
 * @param bmp280 结构体体实例的指针，函数内部将修改该实例的ctrl_meas成员以设置温度过采样率
 * @param oversamp_temperature BMP280_T_OVERSAMPLING_e类型的枚举值，表示要设置的温度过采样率，必须是BMP280_T_MODE_SKIP、BMP280_T_MODE_x1、BMP280_T_MODE_x2、BMP280_T_MODE_x3、BMP280_T_MODE_x4或BMP280_T_MODE_x5之一
 * @return u8 0：设置成功  1：设置失败（参数值无效）
 */
u8 BMP280_Set_CTRLMEAS_oversamp_temperature(BMP280_t *bmp280, BMP280_T_OVERSAMPLING_e oversamp_temperature)
{
	if (oversamp_temperature > BMP280_T_MODE_x5) return 1;
	bmp280->ctrl_meas = (bmp280->ctrl_meas & ~BMP280_CTRLMEAS_oversamp_temperature_mask) | (oversamp_temperature<<5);
	return 0;
}

u8 BMP280_Set_CTRLMEAS_oversamp_pressure(BMP280_t *bmp280, BMP280_P_OVERSAMPLING_e oversamp_pressure)
{
	if (oversamp_pressure > BMP280_P_MODE_x16) return 1;
	bmp280->ctrl_meas = (bmp280->ctrl_meas & ~BMP280_CTRLMEAS_oversamp_pressure_mask) | (oversamp_pressure<<2);
	return 0;
}

u8 BMP280_Set_CTRLMEAS_power_mode(BMP280_t *bmp280, BMP280_POWER_MODE_e power_mode)
{
	if (power_mode > BMP280_NORMAL_MODE) return 1;
	bmp280->ctrl_meas = (bmp280->ctrl_meas & ~BMP280_CTRLMEAS_power_mode_mask) | power_mode;
	return 0;
}

u8 BMP280_Set_CONFIG_t_sb(BMP280_t *bmp280, BMP280_T_SB_e standby_durn)
{
	if (standby_durn > BMP280_T_SB_4000MS) return 1;
	bmp280->config = (bmp280->config & ~BMP280_CONFIG_t_sb_mask) | (standby_durn<<5);
	return 0;
}

u8 BMP280_Set_CONFIG_filter(BMP280_t *bmp280, BMP280_FILTER_COEFFICIENT_e filter_coefficient)
{
	if (filter_coefficient > BMP280_FILTER_MODE_16) return 1;
	bmp280->config = (bmp280->config & ~BMP280_CONFIG_filter_mask) | (filter_coefficient<<2);
	return 0;
}
/**
 * @brief 配置为3-wire SPI接口模式
 * 
 * @param bmp280 BMP280_t结构体实例的指针
 * @param spi3w_en 3-wire SPI接口模式使能位，0表示禁用，1表示启用
 * @return u8 0：设置成功  1：设置失败（参数值无效）
 */
u8 BMP280_Set_CONFIG_spi3w_en(BMP280_t *bmp280, u8 spi3w_en)
{
	if (spi3w_en > 1) return 1;
	bmp280->config = (bmp280->config & ~BMP280_CONFIG_spi3w_en_mask) | (spi3w_en);
	return 0;
}

u8 BMP280_Set_Recommended_Oversampling(BMP280_t *bmp280, BMP280_Recommended_oversampling_e recommended_oversampling)
{
	if(recommended_oversampling > BMP280_ULTRA_HIGH_RESOLUTION_MODE) return 1;
	switch(recommended_oversampling)
	{
		case BMP280_ULTRA_LOW_POWER_MODE:
			BMP280_Set_CTRLMEAS_oversamp_temperature(bmp280, BMP280_T_MODE_x1);
			BMP280_Set_CTRLMEAS_oversamp_pressure(bmp280, BMP280_P_MODE_x1);
			break;
		case BMP280_LOW_POWER_MODE:
			BMP280_Set_CTRLMEAS_oversamp_temperature(bmp280, BMP280_T_MODE_x1);
			BMP280_Set_CTRLMEAS_oversamp_pressure(bmp280, BMP280_P_MODE_x2);
			break;
		case BMP280_STANDARD_RESOLUTION_MODE:
			BMP280_Set_CTRLMEAS_oversamp_temperature(bmp280, BMP280_T_MODE_x1);
			BMP280_Set_CTRLMEAS_oversamp_pressure(bmp280, BMP280_P_MODE_x4);
			break;
		case BMP280_HIGH_RESOLUTION_MODE:
			BMP280_Set_CTRLMEAS_oversamp_temperature(bmp280, BMP280_T_MODE_x1);
			BMP280_Set_CTRLMEAS_oversamp_pressure(bmp280, BMP280_P_MODE_x8);
			break;
		case BMP280_ULTRA_HIGH_RESOLUTION_MODE:
			BMP280_Set_CTRLMEAS_oversamp_temperature(bmp280, BMP280_T_MODE_x2);
			BMP280_Set_CTRLMEAS_oversamp_pressure(bmp280, BMP280_P_MODE_x16);
			break;
	}
	return 0;
}

u8 BMP280_Set_Application_to(BMP280_t *bmp280, BMP280_Application_to_e application_to)
{
	if(application_to > BMP280_INDOOR_NAVIGATION) return 1;
	bmp280->application_to       = application_to;		// 设置应用场景
	switch(application_to)
	{
		case BMP280_HANDHELD_DEVICE_LOW_POWER:
			BMP280_Set_Recommended_Oversampling(bmp280, BMP280_ULTRA_HIGH_RESOLUTION_MODE);
			BMP280_Set_CTRLMEAS_power_mode(bmp280, BMP280_NORMAL_MODE);
			BMP280_Set_CONFIG_t_sb(bmp280, BMP280_T_SB_62_5MS);
			BMP280_Set_CONFIG_filter(bmp280, BMP280_FILTER_MODE_4);
			break;
		case BMP280_HANDHELD_DEVICE_DYNAMIC:
			BMP280_Set_Recommended_Oversampling(bmp280, BMP280_STANDARD_RESOLUTION_MODE);
			BMP280_Set_CTRLMEAS_power_mode(bmp280, BMP280_NORMAL_MODE);
			BMP280_Set_CONFIG_t_sb(bmp280, BMP280_T_SB_0_5MS);
			BMP280_Set_CONFIG_filter(bmp280, BMP280_FILTER_MODE_16);
			break;
		case BMP280_WEATHER_MONITORING:
			BMP280_Set_Recommended_Oversampling(bmp280, BMP280_ULTRA_LOW_POWER_MODE);
			BMP280_Set_CTRLMEAS_power_mode(bmp280, BMP280_FORCED_MODE);
			// BMP280_Set_CONFIG_t_sb(bmp280, BMP280_T_SB_62_5MS);//强制模式下待机时间无效，传感器在每次测量完成后都会自动进入休眠状态
			BMP280_Set_CONFIG_filter(bmp280, BMP280_FILTER_OFF);
			break;
		case BMP280_ELEVATOR_FLOOR_CHANGE_DETECTION_MEASUREMENT:
			BMP280_Set_Recommended_Oversampling(bmp280, BMP280_STANDARD_RESOLUTION_MODE);
			BMP280_Set_CTRLMEAS_power_mode(bmp280, BMP280_NORMAL_MODE);
			BMP280_Set_CONFIG_t_sb(bmp280, BMP280_T_SB_125MS);
			BMP280_Set_CONFIG_filter(bmp280, BMP280_FILTER_MODE_4);
			break;
		case BMP280_DROP_DETECTION_MEASUREMENT:
			BMP280_Set_Recommended_Oversampling(bmp280, BMP280_LOW_POWER_MODE);
			BMP280_Set_CTRLMEAS_power_mode(bmp280, BMP280_NORMAL_MODE);
			BMP280_Set_CONFIG_t_sb(bmp280, BMP280_T_SB_0_5MS);
			BMP280_Set_CONFIG_filter(bmp280, BMP280_FILTER_OFF);
			break;
		case BMP280_INDOOR_NAVIGATION:
			BMP280_Set_Recommended_Oversampling(bmp280, BMP280_ULTRA_HIGH_RESOLUTION_MODE);
			BMP280_Set_CTRLMEAS_power_mode(bmp280, BMP280_NORMAL_MODE);
			BMP280_Set_CONFIG_t_sb(bmp280, BMP280_T_SB_0_5MS);
			BMP280_Set_CONFIG_filter(bmp280, BMP280_FILTER_MODE_16);
			break;
		default:
			break;
	}
	return 0;
}

/****************************************************************************************** */

/**
 * @brief 读取芯片ID,检查BMP280是否存在
 * 
 * @return u8 0：芯片存在且正常  1：实例指针无效或函数指针未赋值  2：无法读取数据  3：读取成功但芯片ID不匹配
 */
u8 BMP280_Get_ID_Check(BMP280_t *bmp280)
{
	u8 chip_id,res,error_flag;
	u8 num=5;//最多尝试5次
	if(bmp280 == NULL || bmp280->iic_or_spi_Read_Len == NULL) return 1;//如果函数指针没有被正确赋值，直接返回1表示检查失败
	do{
		res = bmp280->iic_or_spi_Read_Len(bmp280->iic_SlaveAddr,BMP280_CHIPID_REG,1,&chip_id);//0:读取成功  1：读取失败
		
		// if(res == 0)
		// {
		// 	if(chip_id == BMP280_CHIPID_VALUE)
		// 	{
		// 		error_flag = 0;
		// 		break;//如果读取成功且芯片ID正确，返回0表示检查成功
		// 	}
		// 	else error_flag = 3;//能从设备读取数据但芯片ID不正确，返回3表示检查失败
		// }else{
		// 	error_flag = 2;//根本无法从设备读取数据，返回2表示检查失败
		// }

		if (res != 0) {
            error_flag = 2; // 读取失败
        } else if (chip_id != BMP280_CHIPID_VALUE) {
            error_flag = 3; // 读取成功但ID不匹配
        } else {
            error_flag = 0; // 成功
            break;
        }
	}while(--num);
	return error_flag;
}

/**
 * @brief 获取BMP280的测量状态
 * 
 * @param bmp280 BMP280_t结构体实例的指针
 * @param measuring 用于返回测量状态的指针，0表示未测量，非0表示正在测量...
 * @return u8 0：成功获取测量状态  1：读取失败
 */
u8 BMP280_Get_STATUS_measuring(BMP280_t *bmp280, u8 *measuring)
{
	u8 status=0;
	if(bmp280->iic_or_spi_Read_Len(bmp280->iic_SlaveAddr,BMP280_STATUS_REG,1,&status)) return 1;
	*measuring=status&BMP280_STATUS_measuring_mask;//0表示未测量，非0表示正在转换中...
	return 0;
}
/**
 * @brief 获取BMP280的校准寄存器中有没有正确的校准数据
 * 
 * @param bmp280 BMP280_t结构体实例的指针
 * @param im_update 用于返回校准数据状态的指针，0表示校准数据有效可以读校准寄存器了，非0表示校准数据无效或正在更新中...
 * @return u8 0：成功获取校准数据状态  1：读取失败
 */
u8 BMP280_Get_STATUS_im_update(BMP280_t *bmp280, u8 *im_update)
{
	u8 status=0;
	if(bmp280->iic_or_spi_Read_Len(bmp280->iic_SlaveAddr,BMP280_STATUS_REG,1,&status)) return 1;
	*im_update=status&BMP280_STATUS_im_update_mask;//0表示上电复位后NVM数据更新完成，非0表示正在更新中...
	return 0;
}

/**
 * @brief 读取BMP280校准参数并保存到实例的calib_param中
 * 
 * @param calib_param 实例BMP280_t的calib_param成员的地址，用于保存读取到的校准参数，后续计算温度和气压时需要使用这些参数进行补偿计算
 * @return u8 0：读取成功  1：读取失败
 */
u8 BMP280_Get_CalibParam(BMP280_t *bmp280)
{
	u8 a_data_u8[24];
	//memset(a_data_u8,0,24*sizeof(u8));
	u8 im_update=1;
	while(BMP280_Get_STATUS_im_update(bmp280, &im_update) != 0 || im_update != 0);//成功读取到im_update且im_update为0表示校准参数没有被更新，可以安全地读取校准参数

	if(bmp280->iic_or_spi_Read_Len(bmp280->iic_SlaveAddr,BMP280_DIG_T1_LSB_REG,24,a_data_u8)) return 1;
	bmp280->calib_param.dig_T1=(u16)( ( ((u16)a_data_u8[1])<<8 ) | a_data_u8[0] );
	bmp280->calib_param.dig_T2=(s16)( ( ((u16)a_data_u8[3])<<8 ) | a_data_u8[2] );//类型强制转换过程不会改变底层数据，只是类型系统将其重新解读为有符号数。位模式保持不变
	bmp280->calib_param.dig_T3=(s16)( ( ((u16)a_data_u8[5])<<8 ) | a_data_u8[4] );
	bmp280->calib_param.dig_P1=(u16)( ( ((u16)a_data_u8[7])<<8 ) | a_data_u8[6] );
	bmp280->calib_param.dig_P2=(s16)( ( ((u16)a_data_u8[9])<<8 ) | a_data_u8[8] );
	bmp280->calib_param.dig_P3=(s16)( ( ((u16)a_data_u8[11])<<8 ) | a_data_u8[10] );
	bmp280->calib_param.dig_P4=(s16)( ( ((u16)a_data_u8[13])<<8 ) | a_data_u8[12] );
	bmp280->calib_param.dig_P5=(s16)( ( ((u16)a_data_u8[15])<<8 ) | a_data_u8[14] );
	bmp280->calib_param.dig_P6=(s16)( ( ((u16)a_data_u8[17])<<8 ) | a_data_u8[16] );
	bmp280->calib_param.dig_P7=(s16)( ( ((u16)a_data_u8[19])<<8 ) | a_data_u8[18] );
	bmp280->calib_param.dig_P8=(s16)( ( ((u16)a_data_u8[21])<<8 ) | a_data_u8[20] );
	bmp280->calib_param.dig_P9=(s16)( ( ((u16)a_data_u8[23])<<8 ) | a_data_u8[22] );
	return 0;
}

u8 BMP280_Get_config_and_ctrlmeas_reg(BMP280_t *bmp280)
{
	u8 data[2]={0,0};
	if(bmp280->iic_or_spi_Read_Len(bmp280->iic_SlaveAddr,BMP280_CTRLMEAS_REG,2,data)) return 1;
	bmp280->ctrl_meas_copy = data[0];
	bmp280->config_copy = data[1];
	return 0;
}

u8 BMP280_Get_PressureTemperature_ADC(BMP280_t *bmp280)
{
	u8 a_data_u8[6]={0,0,0,0,0,0};
	if(bmp280->iic_or_spi_Read_Len(bmp280->iic_SlaveAddr,BMP280_PRESSURE_MSB_REG,6,a_data_u8)) return 1;
	bmp280->Pressure_ADC =(s32)((((u32)(a_data_u8[0]))<<12)|(((u32)(a_data_u8[1]))<<4)|((u32)a_data_u8[2]>>4));/* 气压 */
	bmp280->Temperature_ADC =(s32)((((u32)(a_data_u8[3]))<<12)| (((u32)(a_data_u8[4]))<<4)|((u32)a_data_u8[5]>>4));/* 温度 */
	return 0;
}

// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature as global value
s32 BMP280_Get_Temperature_ture_int32(BMP280_t *bmp280)
{
	s32 var1, var2;
	var1 = ((((bmp280->Temperature_ADC>>3) - ((s32)bmp280->calib_param.dig_T1<<1))) * ((s32)bmp280->calib_param.dig_T2)) >> 11;
	var2 = (((((bmp280->Temperature_ADC>>4) - ((s32)bmp280->calib_param.dig_T1)) * ((bmp280->Temperature_ADC>>4) - ((s32)bmp280->calib_param.dig_T1))) >> 12) *
	((s32)bmp280->calib_param.dig_T3)) >> 14;
	bmp280->calib_param.t_fine = var1 + var2;
	bmp280->Temperature_ture = (bmp280->calib_param.t_fine * 5 + 128) >> 8;
	return bmp280->Temperature_ture;
}

// Returns pressure in Pa as unsigned 32 bit integer. Output value of “96386” equals 96386 Pa = 963.86 hPa
u32 BMP280_Get_Pressure_ture_int32(BMP280_t *bmp280)
{
	s32 var1, var2;
	var1 = (((s32)bmp280->calib_param.t_fine)>>1) - (s32)64000;
	var2 = (((var1>>2) * (var1>>2)) >> 11 ) * ((s32)bmp280->calib_param.dig_P6);
	var2 = var2 + ((var1*((s32)bmp280->calib_param.dig_P5))<<1);
	var2 = (var2>>2)+(((s32)bmp280->calib_param.dig_P4)<<16);
	var1 = (((bmp280->calib_param.dig_P3 * (((var1>>2) * (var1>>2)) >> 13 )) >> 3) + ((((s32)bmp280->calib_param.dig_P2) * var1)>>1))>>18;
	var1 =((((32768+var1))*((s32)bmp280->calib_param.dig_P1))>>15);
	if (var1 == 0)
	{
		return 0; // avoid exception caused by division by zero
	}
	bmp280->Pressure_ture = (((u32)(((s32)1048576)-bmp280->Pressure_ADC)-(var2>>12)))*3125;
	if (bmp280->Pressure_ture < 0x80000000)
	{
		bmp280->Pressure_ture = (bmp280->Pressure_ture << 1) / ((u32)var1);
	}
	else
	{
		bmp280->Pressure_ture = (bmp280->Pressure_ture / (u32)var1) * 2;
	}
	var1 = (((s32)bmp280->calib_param.dig_P9) * ((s32)(((bmp280->Pressure_ture>>3) * (bmp280->Pressure_ture>>3))>>13)))>>12;
	var2 = (((s32)(bmp280->Pressure_ture>>2)) * ((s32)bmp280->calib_param.dig_P8))>>13;
	bmp280->Pressure_ture = (u32)((s32)bmp280->Pressure_ture + ((var1 + var2 + bmp280->calib_param.dig_P7) >> 4));
	return bmp280->Pressure_ture;
}

/****************************************************************************************** */

u8 BMP280_Send_SoftReset(BMP280_t *bmp280)
{
	u8 RESET_VALUE=BMP280_RESET_VALUE;
	if(bmp280->iic_or_spi_Write_Len(bmp280->iic_SlaveAddr,BMP280_RESET_REG,1,&RESET_VALUE)) return 1;
	return 0;
}

/**
 * @brief 刷新BMP280的设置参数，函数内部会先将工作模式设置为睡眠模式，等待传感器进入睡眠模式后再写入新的设置参数，最后读取设置参数并检查是否写入成功了，如果写入的设置参数与读取回来的不一致，返回6表示检查失败了
 * 
 * @param bmp280 
 * @return u8 0：设置参数刷新成功  1：写入睡眠模式失败  2：写入config寄存器失败  3：写入ctrl_meas寄存器失败
 */
u8 BMP280_Send_Refresh_Settings(BMP280_t *bmp280)
{
	u8 sleep_set=0x00;
	if(bmp280->iic_or_spi_Write_Len(bmp280->iic_SlaveAddr,BMP280_CTRLMEAS_REG,1,&sleep_set)) return 1;//先将工作模式设置为睡眠模式，才能修改其他设置参数
	//等待传感器进入睡眠模式
	u8 measuring=1;
	while(BMP280_Get_STATUS_measuring(bmp280, &measuring) != 0 || measuring != 0);//成功读取到measuring且measuring为0表示传感器已经进入睡眠模式，可以安全地修改设置参数了
	//写入设置参数
	if(bmp280->iic_or_spi_Write_Len(bmp280->iic_SlaveAddr,BMP280_CONFIG_REG,1,&bmp280->config)) return 2;
	if(bmp280->iic_or_spi_Write_Len(bmp280->iic_SlaveAddr,BMP280_CTRLMEAS_REG,1,&bmp280->ctrl_meas)) return 3;
	return 0;
}

u8 BMP280_Init(BMP280_t *bmp280, BMP280_Application_to_e application_to, u8 iic_slaveaddr, BMP280_IIC_or_SPI_LEN_p iic_read_len, BMP280_IIC_or_SPI_LEN_p iic_write_len, BMP280_IIC_or_SPI_HW_Init_p iic_or_spi_hw_init)
{
	memset(bmp280, 0, sizeof(BMP280_t));  // 初始化结构体成员为0
	bmp280->iic_SlaveAddr        = iic_slaveaddr;       // 设置IIC从设备地址
	bmp280->iic_or_spi_Read_Len  = iic_read_len;        // 设置IIC读取函数指针
	bmp280->iic_or_spi_Write_Len = iic_write_len;       // 设置IIC写入函数指针
	bmp280->iic_or_spi_HW_Init   = iic_or_spi_hw_init;  // 设置IIC或SPI硬件接口初始化函数
	
	// 加判空保护（非常重要！）
    if (bmp280->iic_or_spi_HW_Init != NULL) {
        bmp280->iic_or_spi_HW_Init();// 调用IIC或SPI硬件接口初始化函数
    }

	if(BMP280_Get_ID_Check(bmp280))
		return 1;
	else{
		if(BMP280_Get_CalibParam(bmp280))
			return 2;
		if(BMP280_Set_Application_to(bmp280, application_to))
			return 3;
		if(BMP280_Send_Refresh_Settings(bmp280))
			return 4;
	}
	return 0;
}





/**
 * @brief 根据气压计算海拔高度（米）
 * 使用标准大气压公式近似计算
 * @param pressure_pa 当前气压，单位Pa
 * @return float 海拔高度，单位米
 */
float calculate_altitude(uint32_t pressure_pa) {
    const float P0 = 101325.0f; // 海平面标准大气压，Pa
    const float exponent = 1.0f / 5.25588f;
    float ratio = (float)pressure_pa / P0;
    return 44330.77f * (1.0f - powf(ratio, exponent));
}
