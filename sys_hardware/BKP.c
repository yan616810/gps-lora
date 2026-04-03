#include "BKP.h"
#include "stm32f10x.h"
/*侵入检测：1.不开启侵入中断宏的话，外部侵入只会导致将BKP_CSR寄存器的标志位TEF始终置1，且不会自动清零，BKP_DRx寄存器一直被复位为0;
        2.而在中断中我们可以对侵入事件标志位清零和更加方便的执行一些清除Flash程序或发送"设备已被拆"警报信号等程序;
        3.只有备用电源供电的时候，外部侵入事件依然可以导致BKP_DR寄存器清零;
        实验：问1：验证在只有Vbat时，Tamper事件检测仍然可以将BKP_DR寄存器清零?
             问2：同时备份域不会备份BKP控制寄存器比如TEF标志位？
              1.VDD供电给BKP_DR写数据;
              2.只用备用电源供电时,手动触发Tamper(一定要直接接高或低电平！)后TEF置1，理论BKP_DR全清零;
              3.使Tamper引脚不在处于被侵入状态，之后VDD给芯片供电，再次向BKP_DR写数据,若能写进去则说明TEF已被上电复位初始化清零;
              结论：两个问题都正确！
*/

//#define Tamper_IT



/**
 * @brief 向某个BKP_DRx寄存器中写入一个16位数据
 * 
 * @param addr BKP_DR1 - BKP_DR10
 * @param data 0x0000 - 0xffff
 */
void BKP_write(uint16_t addr,uint16_t data)
{
    //1.根据参考手册
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP|RCC_APB1Periph_PWR,ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    //2.向备份寄存器写数据
    BKP_WriteBackupRegister(addr,data);

    //3.失能访问备份寄存器和RTC
    PWR_BackupAccessCmd(DISABLE);
}

/**
 * @brief 不加侵入中断处理的侵入功能初始化，若有外部侵入只清空BKP_DR.特别的引脚电平若一直处于侵入状态，则BKP_DR一直保持复位为状态
 * @param BKP_TamperPinLevel_Low_Or_Hight:表示引脚为低电平时看作入侵，还是高电平时看作入侵
 * BKP_TamperPinLevel_Low or BKP_TamperPinLevel_High
 */
void tamper_init(uint16_t BKP_TamperPinLevel_Low_Or_Hight)
{
    /*数字电路都要有时钟信号驱动，经实验发现下面GPIO初始化不是必要的！！*/
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE );//使能Tamper引脚GPIOC，可省略
    // {
    //     GPIO_InitTypeDef GPIO_InitStructure;
    //     GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
    //     GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; // 浮空输入，复位时默认模式
    //     GPIO_Init(GPIOC, &GPIO_InitStructure);
    // }

//1.时钟+解锁备份域
    /* Enable PWR and BKP clock */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);//初始化时必须要开启相应外设时钟才可对其寄存器进行读写操作！！
    /* Enable write access to Backup domain */
    PWR_BackupAccessCmd(ENABLE);//！！！也管理备份控制/状态寄存器

//2.禁止产生侵入事件，使能Tamper引脚
    /* Disable Tamper pin */
    BKP_TamperPinCmd(DISABLE);

//3.是否允许产生侵入事件中断
#ifdef Tamper_IT
    /* Disable Tamper interrupt */
    BKP_ITConfig(DISABLE);
#endif

//4.设置入侵电平
    /* Tamper pin active on low level */
    BKP_TamperPinLevelConfig(BKP_TamperPinLevel_Low_Or_Hight);//引脚为低电平时代表被侵入

//5.初始化时先清除一次侵入事件标志位
    BKP_ClearFlag();

//6.是否允许产生侵入事件中断
#ifdef Tamper_IT
    /* Disable Tamper interrupt */
    BKP_ITConfig(ENABLE );
#endif

//7.允许产生侵入事件
    /* Disable Tamper pin */
    BKP_TamperPinCmd(ENABLE );

//8.禁止写备份域寄存器
    PWR_BackupAccessCmd(DISABLE);

#ifdef Tamper_IT
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
    NVIC_InitTypeDef NVIC_InitStruct;
    NVIC_InitStruct.NVIC_IRQChannel=TAMPER_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelCmd=ENABLE;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority=0;
    NVIC_InitStruct.NVIC_IRQChannelSubPriority=0;
    NVIC_Init(&NVIC_InitStruct);
#endif
}

