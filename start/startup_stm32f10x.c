/*--------------------------------------------------------------/
/  Startup Module for STM32F100 Microcontrollers                /
/                                                               /
/ * This module defines vector table, startup code, default     /
/   exception handlers, main stack and miscellanous functions.  /
/ * This file is a non-copyrighted public domain software.      /
/--------------------------------------------------------------*/

#include "stm32f10x.h"


/*栈向低地址增长，栈大小设置为0的时候向量表第一个值为_endof_sram，芯片内核寄存器MSP一上电是从向量表中的第一项获取的值，此时栈顶指向RAM最高地址*/
#define STACK_SIZE	0		/* Size of Main stack (must be multiple of 8, If zero, MSP is set to last RAM address) */


/*--------------------------------------------------------------------/
/ Declareations                                                       /
/--------------------------------------------------------------------*/
/* C library initializers */
extern void __libc_init_array(void);

/* Section address defined in linker script */
extern long _sidata[], _sdata[], _edata[], _sbss[], _ebss[], _endof_sram[];
extern int main (void);

/* Exception/IRQ Handlers */
void Reset_Handler (void)		__attribute__ ((noreturn, naked));
void NMI_Handler (void)			__attribute__ ((weak, alias ("Exception_Trap")));
void HardFault_Hander (void)	__attribute__ ((weak, alias ("Exception_Trap")));
void MemManage_Handler (void)	__attribute__ ((weak, alias ("Exception_Trap")));
void BusFault_Handler (void)	__attribute__ ((weak, alias ("Exception_Trap")));
void UsageFault_Handler (void)	__attribute__ ((weak, alias ("Exception_Trap")));
void SVC_Handler (void)			__attribute__ ((weak));
void DebugMon_Handler (void)	__attribute__ ((weak, alias ("Exception_Trap")));
void PendSV_Handler (void)		__attribute__ ((weak, alias ("Exception_Trap")));
void SysTick_Handler (void)		__attribute__ ((weak, alias ("Exception_Trap")));
void WWDG_IRQHandler (void)		__attribute__ ((weak, alias ("IRQ_Trap")));
void PVD_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TAMPER_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void RTC_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void FLASH_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void RCC_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void EXTI0_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void EXIT1_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void EXTI2_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void EXTI3_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void EXTI4_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void DMA1_CH1_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void DMA1_CH2_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void DMA1_CH3_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void DMA1_CH4_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void DMA1_CH5_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void DMA1_CH6_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void DMA1_CH7_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void ADC1_2_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void USB_HP_CAN_TX_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void USB_LP_CAN_RX0_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void CAN_RX1_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void CAN_SCE_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void EXTI9_5_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM1_BRK_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM1_UP_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM1_TRG_COM_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM1_BRK_TIM15_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM1_UP_TIM16_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM1_TRG_COM_TIM17_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM1_CC_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM2_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM3_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM4_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void I2C1_EV_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void I2C1_ER_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void I2C2_EV_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void I2C2_ER_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void SPI1_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void SPI2_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void USART1_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void USART2_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void USART3_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void EXTI15_10_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void RTC_Alarm_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void USB_WKUP_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM8_BRK_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM8_UP_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM8_TRG_COM_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM8_CC_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void ADC3_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void FSMC_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void SDIO_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM5_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void SPI3_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void UART4_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void UART5_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM6_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void TIM7_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void DMA2_CH1_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void DMA2_CH2_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void DMA2_CH3_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));
void DMA2_CH4_5_IRQHandler (void)	__attribute__ ((weak, alias ("IRQ_Trap")));



/*--------------------------------------------------------------------/
/ Main Stack                                                          /
/--------------------------------------------------------------------*/

#if STACK_SIZE > 0
static
char mstack[STACK_SIZE] __attribute__ ((aligned(8), section(".STACK")));
#define INITIAL_MSP	&mstack[STACK_SIZE]
#else
#define INITIAL_MSP	_endof_sram
#endif



/*--------------------------------------------------------------------/
/ Exception Vector Table for STM32F10x                                /
/--------------------------------------------------------------------*/

void* const vector[] __attribute__ ((section(".VECTOR"))) =
{
	INITIAL_MSP, Reset_Handler, NMI_Handler, HardFault_Hander,
	MemManage_Handler, BusFault_Handler, UsageFault_Handler, 0,
	0, 0, 0, SVC_Handler,
	DebugMon_Handler, 0, PendSV_Handler, SysTick_Handler,
	WWDG_IRQHandler, PVD_IRQHandler, TAMPER_IRQHandler, RTC_IRQHandler,
	FLASH_IRQHandler, RCC_IRQHandler, EXTI0_IRQHandler, EXIT1_IRQHandler,
	EXTI2_IRQHandler, EXTI3_IRQHandler, EXTI4_IRQHandler, DMA1_CH1_IRQHandler,
	DMA1_CH2_IRQHandler, DMA1_CH3_IRQHandler, DMA1_CH4_IRQHandler, DMA1_CH5_IRQHandler,
	DMA1_CH6_IRQHandler, DMA1_CH7_IRQHandler, ADC1_2_IRQHandler, USB_HP_CAN_TX_IRQHandler,
	USB_LP_CAN_RX0_IRQHandler, CAN_RX1_IRQHandler, CAN_SCE_IRQHandler, EXTI9_5_IRQHandler,
	TIM1_BRK_IRQHandler, TIM1_UP_IRQHandler, TIM1_TRG_COM_IRQHandler,
	TIM1_CC_IRQHandler, TIM2_IRQHandler,
	TIM3_IRQHandler, TIM4_IRQHandler, I2C1_EV_IRQHandler, I2C1_ER_IRQHandler,
	I2C2_EV_IRQHandler, I2C2_ER_IRQHandler, SPI1_IRQHandler, SPI2_IRQHandler,
	USART1_IRQHandler, USART2_IRQHandler, USART3_IRQHandler, EXTI15_10_IRQHandler,
	RTC_Alarm_IRQHandler, USB_WKUP_IRQHandler, TIM8_BRK_IRQHandler, TIM8_UP_IRQHandler,
	TIM8_TRG_COM_IRQHandler, TIM8_CC_IRQHandler, ADC3_IRQHandler, FSMC_IRQHandler,
	SDIO_IRQHandler, TIM5_IRQHandler, SPI3_IRQHandler, UART4_IRQHandler,
	UART5_IRQHandler, TIM6_IRQHandler, TIM7_IRQHandler, DMA2_CH1_IRQHandler,
	DMA2_CH2_IRQHandler, DMA2_CH3_IRQHandler, DMA2_CH4_5_IRQHandler
};


/*---------------------------------------------------------------------/
/ Reset_Handler is the start-up code. It configures processor core,    /
/ system clock generator, memory controller, then initialize .data     /
/ and .bss sections and then start main().                             /
/---------------------------------------------------------------------*/


void Reset_Handler (void)
{
	long *s, *d;

	/* Set interrupt vector table address */
	SCB->VTOR = (uint32_t)vector;/*VTOR上电复位值默认是0x00000000,此处设置为.ld文件中默认的0x08000000表示向量表存在FLASH中不偏移，
								只有在有bootloader+APP架构的项目中才需要设置APP应用固件的向量表地址，
								会在APP应用固件的链接脚本中设置向量表地址为FLASH_BASE+VECT_TAB_OFFSET;*/

	/* Configure FSMC */
	/*
		*** Nothing to do ***
	*/

	/*set system clock*///需要确认 SystemInit 不读取未初始化的全局/静态变量！！
	SystemInit();/*注释掉该函数末尾的设置向量表偏移寄存器！函数末尾会设置向量表偏移地址，在.ld链接脚本中指定向量表位置后，还需要修改system_stm32f10x.c文件中的宏定义的方式定义偏移量；
				但是此处我们使用.c启动文件，直接在.ld文件中设置时变量vector会被自动正确设置！*/

	/* Initialize .data/.bss section and static objects get ready to use after this process */
	for (s = _sidata, d = _sdata; d < _edata; *d++ = *s++) ;
	for (d = _sbss; d < _ebss; *d++ = 0) ;

	/* Call newlib C++ global constructors ，负责初始化 Newlib 的运行时环境，包括标准 I/O 缓冲区;所以要在这里进行初始化*/
	__libc_init_array();

	/* Start main() with MSP and privileged mode */
	main();

	for (;;) ;
}



/*--------------------------------------------------------------------/
/ Unexpected Exception/IRQ Trap                                       /
/--------------------------------------------------------------------*/

void Exception_Trap (void)
{
	for (;;) ;
}


void IRQ_Trap (void)
{
	for (;;) ;
}



/*--------------------------------------------------------------------/
/ Default SVC Handler                                                 /
/--------------------------------------------------------------------*/

void SVC_Handler (void)
{
#if USE_SV_SERVICE
	asm (
	"    UBFX R3, LR, #0, #4\n"	/* Get the task SP into R3 */
	"    CMP R3, #13\n"
	"    ITE EQ\n"
	"    MRSEQ R3, PSP\n"
	"    MOVNE R3, SP\n"
	"    LDR R2, [R3, #24]\n"	/* Get stacked PC into R2 */
	"    LDRB R2, [R2, #-2]\n"	/* Get SVC number into R2 */
	"    TBB [PC, R2]\n"		/* Jump to the function specified by SVC number */
	"btbl:\n"
	"   .byte (cpsiei - btbl) / 2\n"
	"   .byte (cpsidi - btbl) / 2\n"
	"   .byte (cpsief - btbl) / 2\n"
	"   .byte (cpsidf - btbl) / 2\n"
	"   .byte (rd_scs - btbl) / 2\n"
	"   .byte (wr_scs - btbl) / 2\n"

	"cpsiei:\n"
	"   CPSIE i\n"
	"   BX LR\n"
	"cpsidi:\n"
	"   CPSID i\n"
	"   BX LR\n"
	"cpsief:\n"
	"   CPSIE f\n"
	"   BX LR\n"
	"cpsidf:\n"
	"   CPSID f\n"
	"   BX LR\n"
	"rd_scs:\n"
	"   LDR R1, [R0]\n"	/* Read scs register */
	"   STR R1, [R3]\n"	/* Set it into stacked R0 */
	"   BX LR\n"
	"wr_scs:\n"
	"   STR R1, [R0]\n"	/* Write scs register */
	"   BX LR\n"
	);
#else
	for (;;) ;
#endif
}


#if USE_SV_SERVICE

uint32_t __get_scs_reg (volatile uint32_t *reg)
{
	uint32_t res;

	asm (
	"@ MOV R0, %0\n"
	"SVC #4\n"
	"@ MOV %1, R0\n"
	"BX LR\n" : "=r" (res) : "r" (reg)
	);
	return res;
}

void __set_scs_reg (volatile uint32_t *reg, uint32_t val)
{
	asm (
	"@ MOV R0, %0\n"
	"@ MOV R1, %1\n"
	"SVC #5\n"
	"BX LR\n" : : "r" (reg), "r" (val)
	);
}

#endif
