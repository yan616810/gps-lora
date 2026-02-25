#include "hw_iic.h"
#include "Delay.h"


/* helper: recover stuck bus by toggling SCL and generating a stop
   condition, then re‑initializing the peripheral.  Exported in hw_iic.h so
   callers (eg. u8g2 adapter) can invoke it after a timeout. */
void hw_iic_bus_recover(I2C_TypeDef *I2Cx)
{
    uint16_t scl_pin, sda_pin;
    GPIO_InitTypeDef gpio;

    /* determine pins; only PB6/PB7 for I2C1, PB10/PB11 for I2C2 */
    if (I2Cx == I2C1) {
        scl_pin = GPIO_Pin_6;
        sda_pin = GPIO_Pin_7;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    } else if (I2Cx == I2C2) {
        scl_pin = GPIO_Pin_10;
        sda_pin = GPIO_Pin_11;
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    } else {
        return; // Invalid I2C peripheral
    }

    /* disable I2C peripheral and do software reset just in case 
    1.清除所有容易“卡死”的标志位：BUSY、ADDR、BTF、STOPF、AF、OVR、BERR 等全部清零。
    2.状态机强制回到空闲（Idle）状态。
    3.内部时钟计数器、移位寄存器等全部清空。*/
    I2Cx->CR1 &= ~I2C_CR1_PE;     // 第1步：关闭 I2C 外设（PE = 0）
    I2Cx->CR1 |= I2C_CR1_SWRST;   // 第2步：触发软件复位（SWRST = 1）
    I2Cx->CR1 &= ~I2C_CR1_SWRST;  // 第3步：退出复位状态（SWRST = 0）

    /* configure pins as open-drain gpio outputs */
    gpio.GPIO_Mode = GPIO_Mode_Out_OD;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Pin = scl_pin | sda_pin;
    GPIO_Init(GPIOB, &gpio);

    /* ensure SDA high, then clock SCL up to 9 times */
    GPIO_SetBits(GPIOB, sda_pin);
    for (int i = 0; i < 9; i++) {
        GPIO_ResetBits(GPIOB, scl_pin);
        Delay_us(5);
        GPIO_SetBits(GPIOB, scl_pin);
        Delay_us(5);
        if (GPIO_ReadInputDataBit(GPIOB, sda_pin) == Bit_SET)//判断从机是否释放了SDA线，如果SDA线被释放了，说明总线已经不再被占用，可以退出时钟脉冲的循环
            break;
    }

    /* generate a manual STOP: SDA low->high while SCL high */
    GPIO_ResetBits(GPIOB, sda_pin);
    Delay_us(5);
    GPIO_SetBits(GPIOB, sda_pin);
    Delay_us(5);

    // /* restore AF configuration */
    // gpio.GPIO_Mode = GPIO_Mode_AF_OD;
    // gpio.GPIO_Pin = scl_pin | sda_pin;
    // GPIO_Init(GPIOB, &gpio);

    // /* finally re-enable and reinitialize peripheral */
    // I2C_Cmd(I2Cx, ENABLE);

    /* 初始化硬件iic外设和引脚 */
    hw_iic_init(I2Cx);
}

/**
 * @brief 硬件iic外设&引脚初始化函数
 * 
 * @param I2Cx 选择初始化哪个iic外设，I2C1或I2C2
 */
void hw_iic_init(I2C_TypeDef *I2Cx)
{
    uint16_t scl_pin, sda_pin;
    GPIO_TypeDef *gpio_port;
    if (I2Cx == I2C1) {
        //时钟开启一定要在最前边，否则会产生Bug
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1,ENABLE);
        scl_pin = GPIO_Pin_6;
        sda_pin = GPIO_Pin_7;
        gpio_port = GPIOB;
    } else if(I2Cx == I2C2) {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2,ENABLE);
        scl_pin = GPIO_Pin_10;
        sda_pin = GPIO_Pin_11;
        gpio_port = GPIOB;
    } else {
        return; // Invalid I2C peripheral
    } 
    //IO端口模式初始化
    GPIO_InitTypeDef GPIO_InitStructure={
    .GPIO_Mode=GPIO_Mode_AF_OD,
    .GPIO_Speed=GPIO_Speed_50MHz,
    .GPIO_Pin =scl_pin | sda_pin
    };
    GPIO_Init(gpio_port, &GPIO_InitStructure);
    //iic外设初始化
    I2C_InitTypeDef I2C_InitStruct={
            .I2C_Ack=I2C_Ack_Enable,
            .I2C_AcknowledgedAddress=I2C_AcknowledgedAddress_7bit,
            .I2C_OwnAddress1=0x00,
            .I2C_ClockSpeed=400000,
            .I2C_DutyCycle=I2C_DutyCycle_2,
            .I2C_Mode=I2C_Mode_I2C,
        };
    I2C_Init(I2Cx,&I2C_InitStruct);
    I2C_Cmd(I2Cx,ENABLE);
}

/**
 * @brief 检查I2C事件超时函数
 * 
 * @param I2Cx I2C外设指针
 * @param I2C_EVENT 事件标志
 * @return uint8_t 0表示成功，1表示超时失败
 */
uint8_t hw_iic_CheckEvent_timeout(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT)
{
    uint32_t timeout = 0;
    while (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS) {
        if (++timeout >= HW_IIC_TIMEOUT_US) {
            I2C_GenerateSTOP(I2Cx, ENABLE);
            if (I2C_CheckEvent(I2Cx, I2C_EVENT) != SUCCESS) {
                hw_iic_bus_recover(I2Cx);
            }
            return 1;
        }
        Delay_us(1);
    }
    return 0;
}

/* hardware I2C byte callback rewritten to use the common helpers in hw_iic.
   the previous implementation duplicated event loops and its own timeout
   logic; here we simply call hw_iic_CheckEvent_timeout() which already
   issues a STOP and calls hw_iic_bus_recover() on failure.  A small helper
   is added to wait for BUSY with the same timeout semantics. */
/**
 * @brief 等待I2C外设不处于忙碌状态
 * 
 * @param I2Cx I2C外设指针
 * @return uint8_t 0表示成功，1表示超时失败，虽然失败但是直接调用了总线恢复函数hw_iic_bus_recover()，所以总线状态已经被恢复了
 */
uint8_t hw_iic_wait_not_busy(I2C_TypeDef *I2Cx)
{
    uint32_t tout = 0;
    while (I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)) {
        if (++tout >= HW_IIC_TIMEOUT_US) {
            hw_iic_bus_recover(I2Cx);
            return 1;
        }
        Delay_us(1);
    }
    return 0;
}

/**
 * @brief 写入一个字节数据到指定地址和寄存器
 * 
 * @param I2Cx 使用I2C外设的指针
 * @param addr 7位从机地址
 * @param reg 寄存器地址
 * @param data 要写入的数据
 * @return uint8_t 0表示成功，1表示失败

 */
uint8_t hw_iic_write_byte(I2C_TypeDef *I2Cx,uint8_t addr,uint8_t reg,uint8_t data)
{
    I2C_GenerateSTART(I2Cx,ENABLE);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_MODE_SELECT))return 1;
    I2C_Send7bitAddress(I2Cx,(addr<<1),I2C_Direction_Transmitter);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))return 1;
    I2C_SendData(I2Cx,reg);//此行执行完，数据寄存器非空，移位寄存器空
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTING))return 1;//等待(移位寄存器非空，数据寄存器空)事件出现跳出while;该事件也就是I2C_EVENT_MASTER_BYTE_TRANSMITTING其本身的含义->从数据寄存器转移到移位寄存器中的比特正在传输中
    //程序运行到这里时，DR为空，移位寄存器刚要发送数据，如果下面程序紧接着向DR再传个数据
    //这里是个窗口期，等待从机应答前如果向DR发数据继续发送；否则就表示主机不想发数据了
    I2C_SendData(I2Cx,data);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))return 1;//DR和数据移位寄存器都是空
    I2C_GenerateSTOP(I2Cx,ENABLE);
	return 0;
}
/**
 * @brief 写入多个字节数据到指定地址和寄存器
 * 
 * @param I2Cx 使用I2C外设的指针
 * @param addr 7位从机地址
 * @param reg 寄存器地址
 * @param len 要写入的数据长度
 * @param data_buf 要写入的数据缓冲区指针
 * @return uint8_t 
 */
uint8_t hw_iic_write_byte_len(I2C_TypeDef *I2Cx,uint8_t addr,uint8_t reg,uint32_t len,uint8_t *data_buf)
{
    I2C_GenerateSTART(I2Cx,ENABLE);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_MODE_SELECT))return 1;

    I2C_Send7bitAddress(I2Cx,(addr<<1),I2C_Direction_Transmitter);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))return 1;
    I2C_SendData(I2Cx,reg);
    while(len)
    {
        len--;
        if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTING))return 1;//DR和数据移位寄存器都是非空，就会一直在这里循环，直到移位寄存器发完一个字节，DR转移到移位寄存器中，接收到从机应答后，退出循环
        I2C_SendData(I2Cx,*data_buf++);
    }
    //程序运行到这里时，DR为空，移位寄存器刚要发送数据，如果下面程序紧接着向DR再传个数据
    //这里是个窗口期，等待从机应答前如果向DR发数据继续发送；否则就表示主机不想发数据了
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))return 1;//DR和数据移位寄存器都是空
    I2C_GenerateSTOP(I2Cx,ENABLE);
	return 0;
}
/**
 * @brief 读取一个字节数据从指定地址和寄存器
 * 
 * @param I2Cx 使用I2C外设的指针
 * @param addr 7位从机地址
 * @param reg 寄存器地址
 * @return uint8_t 读取到的数据
 */
uint8_t hw_iic_read_byte(I2C_TypeDef *I2Cx,uint8_t addr,uint8_t reg)
{
    I2C_GenerateSTART(I2Cx,ENABLE);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_MODE_SELECT))return 1;
    I2C_Send7bitAddress(I2Cx,(addr<<1),I2C_Direction_Transmitter);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))return 1;
    I2C_SendData(I2Cx,reg);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))return 1;
//接收
    I2C_GenerateSTART(I2Cx,ENABLE);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_MODE_SELECT))return 1;//EV5
    I2C_Send7bitAddress(I2Cx,(addr<<1),I2C_Direction_Receiver);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))return 1;//I2C 控制器进入接收模式，状态机自动触发 SCL 脉冲（8 个脉冲读取 1 字节数据）;解释了为何接收到从机的ACK后，主机竟然知道主动驱动SCL时钟引脚来驱动从机发字节
    I2C_AcknowledgeConfig(I2Cx,DISABLE);//设置读完最后一个字节后主机不回应，提前设置不回应，才能在下面接受完成后立马发送主不回应
    I2C_GenerateSTOP(I2Cx,ENABLE);//提前设置停止位，才能在下面接受完成后立马停止
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED))return 1;//EV7表示接收完成并转移到了DR
    uint8_t dat = I2C_ReceiveData(I2Cx);
/****************************恢复默认的IIC应答配置**************************************** */
    I2C_AcknowledgeConfig(I2Cx,ENABLE);//只有这里不需要主机回应，其他的命令都需要主机回应，恢复默认的需要需要主机回应从机
    return dat;
}
/**
 * @brief 读取多个字节数据从指定地址和寄存器
 * 
 * @param I2Cx 使用I2C外设的指针
 * @param addr 7位从机地址
 * @param reg 寄存器地址
 * @param len 要读取的数据长度
 * @param data_buf 读取到的数据缓冲区指针
 * @return uint8_t 
 */
uint8_t hw_iic_read_byte_len(I2C_TypeDef *I2Cx,uint8_t addr,uint8_t reg,uint32_t len,uint8_t *data_buf)
{
    I2C_GenerateSTART(I2Cx,ENABLE);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_MODE_SELECT))return 1;
    I2C_Send7bitAddress(I2Cx,(addr<<1),I2C_Direction_Transmitter);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))return 1;
    I2C_SendData(I2Cx,reg);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_BYTE_TRANSMITTED))return 1;
//读数据
    I2C_GenerateSTART(I2Cx,ENABLE);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_MODE_SELECT))return 1;
    I2C_Send7bitAddress(I2Cx,(addr<<1),I2C_Direction_Receiver);
    if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))return 1;
    while (len)
    {
        len--;
        if(!len)
        {
            I2C_AcknowledgeConfig(I2Cx,DISABLE);//设置读完最后一个字节后主机不回应，提前设置不回应，才能在下面接受完成后立马发送主不回应
            I2C_GenerateSTOP(I2Cx,ENABLE);//提前设置停止位，才能在下面接受完成后立马停止
        }
        if(hw_iic_CheckEvent_timeout(I2Cx,I2C_EVENT_MASTER_BYTE_RECEIVED))return 1;
        *data_buf++ = I2C_ReceiveData(I2Cx);
    }
/**************************恢复默认的IIC应答配置****************************************** */
    I2C_AcknowledgeConfig(I2Cx,ENABLE);//恢复默认的需要需要主机回应从机
    return 0;
}

/***************************其他函数*****************/

// //返回0，表示已搜索到该7位地址的设备
// uint8_t hw_iic_search_devices(uint8_t addr_7bit)
// {
//     hw_iic_init();
//     I2C_GenerateSTART(I2C1,ENABLE);
//     if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_MODE_SELECT))return 1;

//     I2C_Send7bitAddress(I2C1,(addr_7bit<<1),I2C_Direction_Transmitter);
//     if(hw_iic_CheckEvent_timeout(I2C1,I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))return 1;
//     return 0;
// }


