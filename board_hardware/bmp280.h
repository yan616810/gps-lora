#ifndef __bmp280_h
#define __bmp280_h
#include "stm32f10x.h"                  // Device header


// #define BMP280_SlaveAddr 0x77  //BMP280的器件地址

#define BMP280_CHIPID_VALUE                       0x58                              /*BMP280的芯片ID值*/
#define BMP280_RESET_VALUE                        0xB6                              /*向BMP280_RESET_REG寄存器写入该值将会启动完整的上电复位*/
/*bit mask*/
#define BMP280_STATUS_measuring_mask              0x08                              /*转换数据是否完成；1：数据正在转换中  0：数据已经转运到数据寄存器,可以读取数据寄存器获得此刻的气压值和温度值*/
#define BMP280_STATUS_im_update_mask              0x01                              /*NVM数据是否复制到了校准寄存器；1：正在复制中  0：复制完成，可以从calib0~25共26个校准寄存器中读取数据；只在上电复位后判断一次即可，每个器件有唯一且不变的校准值;上电复位和每次转换前，会将校准数据从NVM复制到用户仅读的校准数据寄存器中；*/
#define BMP280_CTRLMEAS_oversamp_temperature_mask 0xE0                              /*温度过采样掩码；000：跳过；001：x1；010：x2；011：x4；100：x8；101或其他：x16*/
#define BMP280_CTRLMEAS_oversamp_pressure_mask    0x1C                              /*气压过采样掩码；000：跳过；001：x1；010：x2；011：x4；100：x8；101或其他：x16*/
#define BMP280_CTRLMEAS_power_mode_mask           0x03                              /*电源模式掩码；00：睡眠模式；01或10：强制模式；11：正常模式*/
#define BMP280_CONFIG_t_sb_mask                   0xE0                              /*待机时间掩码；000：0.5ms；001：62.5ms；010：125ms；011：250ms；100：500ms；101：1000ms；110：2000ms；111：4000ms*/
#define BMP280_CONFIG_filter_mask                 0x1C                              /*滤波系数掩码；000：滤波器关闭；001：0.223*ODR；010：0.092*ODR；011：0.042*ODR；100：0.021*ODR；数字滤波器截止频率与ODR成正比*/
#define BMP280_CONFIG_spi3w_en_mask               0x01                              /*SPI接口3-wire模式使能掩码；0：3-wire模式禁用；1：3-wire模式使能；3-wire模式使能后，从原先的4-wire切换成3-wire，SDO引脚变成高阻态，SDI引脚变为input/output*/
/*memory map*/
#define BMP280_DIG_T1_LSB_REG                0x88  
#define BMP280_DIG_T1_MSB_REG                0x89  
#define BMP280_DIG_T2_LSB_REG                0x8A  
#define BMP280_DIG_T2_MSB_REG                0x8B  
#define BMP280_DIG_T3_LSB_REG                0x8C  
#define BMP280_DIG_T3_MSB_REG                0x8D  
#define BMP280_DIG_P1_LSB_REG                0x8E  
#define BMP280_DIG_P1_MSB_REG                0x8F  
#define BMP280_DIG_P2_LSB_REG                0x90  
#define BMP280_DIG_P2_MSB_REG                0x91  
#define BMP280_DIG_P3_LSB_REG                0x92  
#define BMP280_DIG_P3_MSB_REG                0x93  
#define BMP280_DIG_P4_LSB_REG                0x94  
#define BMP280_DIG_P4_MSB_REG                0x95  
#define BMP280_DIG_P5_LSB_REG                0x96  
#define BMP280_DIG_P5_MSB_REG                0x97  
#define BMP280_DIG_P6_LSB_REG                0x98  
#define BMP280_DIG_P6_MSB_REG                0x99  
#define BMP280_DIG_P7_LSB_REG                0x9A  
#define BMP280_DIG_P7_MSB_REG                0x9B  
#define BMP280_DIG_P8_LSB_REG                0x9C  
#define BMP280_DIG_P8_MSB_REG                0x9D  
#define BMP280_DIG_P9_LSB_REG                0x9E  
#define BMP280_DIG_P9_MSB_REG                0x9F  
#define BMP280_CHIPID_REG                    0xD0                              /*Chip ID Register */  
#define BMP280_RESET_REG                     0xE0                              /*Softreset Register */  
#define BMP280_STATUS_REG                    0xF3                              /*Status Register */  
#define BMP280_CTRLMEAS_REG                  0xF4                              /*Ctrl Measure Register-随时可以写，以改变电源模式*/  
#define BMP280_CONFIG_REG                    0xF5                              /*Configuration Register-正常模式下写入可能会被忽略，睡眠模式下写入不会忽略；设置正常模式下ODR(输出数据速率),滤波器,设备接口选择；*/  
#define BMP280_PRESSURE_MSB_REG              0xF7                              /*Pressure MSB Register */  
#define BMP280_PRESSURE_LSB_REG              0xF8                              /*Pressure LSB Register */  
#define BMP280_PRESSURE_XLSB_REG             0xF9                              /*Pressure XLSB Register */  
#define BMP280_TEMPERATURE_MSB_REG           0xFA                              /*Temperature MSB Reg */  
#define BMP280_TEMPERATURE_LSB_REG           0xFB                              /*Temperature LSB Reg */  
#define BMP280_TEMPERATURE_XLSB_REG          0xFC                              /*Temperature XLSB Reg */  


/*BMP280_CTRLMEAS_REG相关设置*/
typedef enum 
{
    BMP280_T_MODE_SKIP=0x0,                                                    /*skipped*/  
    BMP280_T_MODE_x1,                                                           /*x1*/  
    BMP280_T_MODE_x2,                                                           /*x2*/  
    BMP280_T_MODE_x3,                                                           /*x4*/  
    BMP280_T_MODE_x4,                                                           /*x8*/  
    BMP280_T_MODE_x5                                                            /*x16*/  
} BMP280_T_OVERSAMPLING_e;                                                       
typedef enum
{
    BMP280_P_MODE_SKIP=0x0,                                                    /*skipped*/  
    BMP280_P_MODE_x1,                                                          /*x1*/  
    BMP280_P_MODE_x2,                                                          /*x2*/  
    BMP280_P_MODE_x4,                                                          /*x4*/  
    BMP280_P_MODE_x8,                                                          /*x8*/  
    BMP280_P_MODE_x16                                                          /*x16*/  
} BMP280_P_OVERSAMPLING_e;
typedef enum
{
    BMP280_SLEEP_MODE=0x00,
    BMP280_FORCED_MODE=0x01,
    BMP280_NORMAL_MODE=0x03  
} BMP280_POWER_MODE_e;  

/*BMP280_CONFIG_REG相关设置*/
typedef enum
{
    BMP280_T_SB_0_5MS=0x00,                                                    /*0.5ms */  
    BMP280_T_SB_62_5MS=0x01,                                                   /*62.5ms*/  
    BMP280_T_SB_125MS=0x02,                                                    /*125ms */  
    BMP280_T_SB_250MS=0x03,                                                    /*250ms */  
    BMP280_T_SB_500MS=0x04,                                                    /*500ms */  
    BMP280_T_SB_1000MS=0x05,                                                   /*1000ms*/  
    BMP280_T_SB_2000MS=0x06,                                                   /*2000ms*/  
    BMP280_T_SB_4000MS=0x07,                                                   /*4000ms*/  
} BMP280_T_SB_e;
typedef enum 
{
    BMP280_FILTER_OFF=0x0,                                                     /*filter off*/  
    BMP280_FILTER_MODE_2,                                                      /*0.223*ODR 登山时想更快看到海拔变化，帕金森手抖依然会被滤掉大半，同时响应更快*/  
    BMP280_FILTER_MODE_4,                                                      /*0.092*ODR 该数据可从手册19页得到*/  
    BMP280_FILTER_MODE_8,                                                      /*0.042*ODR*/  
    BMP280_FILTER_MODE_16                                                      /*0.021*ODR*/  
} BMP280_FILTER_COEFFICIENT_e;


/************************************************/
/**@name	          应用场景
*************************************************/
typedef enum
{
	BMP280_ULTRA_LOW_POWER_MODE       = 0x00, 
	BMP280_LOW_POWER_MODE             = 0x01, 
	BMP280_STANDARD_RESOLUTION_MODE   = 0x02, 
	BMP280_HIGH_RESOLUTION_MODE       = 0x03,
	BMP280_ULTRA_HIGH_RESOLUTION_MODE = 0x04
} BMP280_Recommended_oversampling_e;//推荐的气压和温度过采样率组合，用户可以直接使用这些组合
typedef enum
{
    BMP280_Customization                               = 0x00, /*Customization*/
    BMP280_HANDHELD_DEVICE_LOW_POWER                   = 0x01, /*Handheld device low-power (e.g. Android) - 10HZ*/
    BMP280_HANDHELD_DEVICE_DYNAMIC                     = 0x02, /*Handheld device dynamic (e.g. Android)-动态手持设备 - 83.3HZ*/
    BMP280_WEATHER_MONITORING                          = 0x03, /*Weather monitoring (lowest power)-气象站 - 1/60HZ(推荐1分钟读一次数据)*/
    BMP280_ELEVATOR_FLOOR_CHANGE_DETECTION_MEASUREMENT = 0x04, /*Elevator / floor change detection-电梯/楼层变化检测 - 7.3HZ*/
    BMP280_DROP_DETECTION_MEASUREMENT                  = 0x05, /*Drop detection-掉落检测 - 125HZ*/
    BMP280_INDOOR_NAVIGATION                           = 0x06, /*Indoor navigation-室内导航 - 26.3HZ*/
} BMP280_Application_to_e;


typedef struct BMP280_calib
{
	u16 dig_T1;                                                                /*校准T1数据*/
	s16 dig_T2;                                                                /*校准T2数据*/
	s16 dig_T3;                                                                /*校准T3数据*/
	u16 dig_P1;                                                                /*校准P1数据*/
	s16 dig_P2;                                                                /*校准P2数据*/
	s16 dig_P3;                                                                /*校准P3数据*/
	s16 dig_P4;                                                                /*校准P4数据*/
	s16 dig_P5;                                                                /*校准P5数据*/
	s16 dig_P6;                                                                /*校准P6数据*/
	s16 dig_P7;                                                                /*校准P7数据*/
	s16 dig_P8;                                                                /*校准P8数据*/
	s16 dig_P9;                                                                /*校准P9数据*/
	s32 t_fine;                                                                /*校准t_fine数据*/
} BMP280_calib_t;

// typedef struct bmp280
// {
//  u8             chip_id;               //传感器ID                                                 
//  u8             oversamp_temperature;  //温度采样
//  u8             oversamp_pressure;     //气压采样
//  u8             power_mode;            //电源模式
//  u8             standby_durn;          //待机时间
//  u8             filter_coefficient;    //滤波系数
//  u8             spi3w_en;              //SPI接口3-wire模式使能
//  u8             application_to;        //应用场景
//  BMP280_calib_t calib_param;           //校准数据
// } BMP280_t;
typedef u8 (*BMP280_IIC_or_SPI_LEN_p)(u8 iic_SlaveAddr, u8 reg, u8 len, u8 *data); //定义一个函数指针类型，参数分别是：iic从机地址，寄存器地址，要读写的字节数，数据缓冲区指针；函数返回值：0：操作成功  1：操作失败
typedef void (*BMP280_IIC_or_SPI_HW_Init_p)(void); //定义一个函数指针
typedef struct bmp280
{
    BMP280_calib_t calib_param;                                             //校准数据
    u8 application_to;                                                      //应用场景
    u8 ctrl_meas;                                                           //配置传感器的温度，气压过采样率，电源模式
    u8 config;                                                              //配置传感器的待机时间、滤波系数，SPI接口3-wire模式使能
    u8 ctrl_meas_copy;                                                      //读寄存器后保存在这里
    u8 config_copy;                                                         //读寄存器后保存在这里
    s32 Temperature_ADC;                                                    //读取原始温度数据，保存在bmp280结构体的Temperature_ADC成员中
    s32 Pressure_ADC;                                                       //读取原始气压数据，保存在bmp280结构体的Pressure_ADC成员中
    s32 Temperature_ture;                                                   //将校准后的温度值保存在bmp280结构体的Temperature_ture成员中，单位是0.01摄氏度，输出值5123表示51.23摄氏度
    u32 Pressure_ture;                                                      //将校准后的气压值保存在bmp280结构体的Pressure_ture成员中，单位是Pa，输出值96386表示96386Pa=963.86hPa
    //以下成员要手动初始化    
    u8 iic_SlaveAddr;                                                       //iic协议从机地址                                 
    BMP280_IIC_or_SPI_LEN_p iic_or_spi_Read_Len;                            //iic或spi读函数指针，参数分别是：iic从机地址，寄存器地址，要读取的字节数，数据缓冲区指针；函数返回值：0：读取成功  1：读取失败
    BMP280_IIC_or_SPI_LEN_p iic_or_spi_Write_Len;                           //iic或spi写函数指针，参数分别是：iic从机地址，寄存器地址，要写入的字节数，数据缓冲区指针；函数返回值：0：写入成功  1：写入失败
    BMP280_IIC_or_SPI_HW_Init_p iic_or_spi_HW_Init;
} BMP280_t;



/*函数*/
u8 BMP280_Init(BMP280_t *bmp280, BMP280_Application_to_e application_to, u8 iic_slaveaddr, BMP280_IIC_or_SPI_LEN_p iic_read_len, BMP280_IIC_or_SPI_LEN_p iic_write_len, BMP280_IIC_or_SPI_HW_Init_p iic_or_spi_hw_init);


u8 BMP280_Send_Refresh_Settings(BMP280_t *bmp280);
u8 BMP280_Send_SoftReset(BMP280_t *bmp280);


u8 BMP280_Set_CTRLMEAS_oversamp_temperature(BMP280_t *bmp280, BMP280_T_OVERSAMPLING_e oversamp_temperature);
u8 BMP280_Set_CTRLMEAS_oversamp_pressure(BMP280_t *bmp280, BMP280_P_OVERSAMPLING_e oversamp_pressure);
u8 BMP280_Set_CTRLMEAS_power_mode(BMP280_t *bmp280, BMP280_POWER_MODE_e power_mode);
u8 BMP280_Set_CONFIG_t_sb(BMP280_t *bmp280, BMP280_T_SB_e standby_durn);
u8 BMP280_Set_CONFIG_filter(BMP280_t *bmp280, BMP280_FILTER_COEFFICIENT_e filter_coefficient);
u8 BMP280_Set_CONFIG_spi3w_en(BMP280_t *bmp280, u8 spi3w_en);
u8 BMP280_Set_Recommended_Oversampling(BMP280_t *bmp280, BMP280_Recommended_oversampling_e recommended_oversampling);
u8 BMP280_Set_Application_to(BMP280_t *bmp280, BMP280_Application_to_e application_to);


u8 BMP280_Get_ID_Check(BMP280_t *bmp280);
u8 BMP280_Get_STATUS_measuring(BMP280_t *bmp280, u8 *measuring);
u8 BMP280_Get_STATUS_im_update(BMP280_t *bmp280, u8 *im_update);
u8 BMP280_Get_CalibParam(BMP280_t *bmp280);
u8 BMP280_Get_config_and_ctrlmeas_reg(BMP280_t *bmp280);  //读取寄存器值后保存在bmp280结构体的config_copy和ctrl_meas_copy成员中
u8  BMP280_Get_PressureTemperature_ADC(BMP280_t *bmp280);  //读取原始气压和温度数据，保存在bmp280结构体的Pressure_ADC和Temperature_ADC成员中
s32 BMP280_Get_Temperature_ture_int32(BMP280_t *bmp280);   //返回温度值，单位是0.01摄氏度，输出值5123表示51.23摄氏度
u32 BMP280_Get_Pressure_ture_int32(BMP280_t *bmp280);      //返回气压值，单位是Pa，输出值96386表示96386Pa=963.86hPa


float calculate_altitude(uint32_t pressure_pa, float sea_level_pressure_pa);
#endif 
