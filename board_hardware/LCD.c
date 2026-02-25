#include "LCD.h"
#include "LCD_Font.h"
#include "Delay.h"
#include <stdlib.h> // for rand()
#include "hw_spi.h"

static uint16_t LCD_TextColor = 0x0000; // 默认黑色
static uint16_t LCD_BackColor = 0xFFFF; // 默认白色

/*********************软件SPI接口*******************************/
void LCD_PIN_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Pin   = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_5 | GPIO_Pin_10 | GPIO_Pin_11,
        .GPIO_Mode  = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    LCD_RES_SET();//低电平复位
    LCD_DC_SET();//高电平像素数据
    LCD_CLK_CLR();//上升沿采样，MODE 0初始时是低电平
    LCD_MOSI_SET();
    LCD_CS1_SET();
}

void LCD_WriteCommand(uint8_t cmd)
{
    LCD_CS1_CLR();
    LCD_DC_CLR(); // 发送命令
    for (int i = 0; i < 8; i++)
    {
        if (cmd & 0x80)
            LCD_MOSI_SET();
        else
            LCD_MOSI_CLR();
        LCD_CLK_SET();
        // Delay_us(1);
        LCD_CLK_CLR();
        cmd <<= 1;
    }
    LCD_CS1_SET();
}

void LCD_WriteData(uint8_t data)
{
    LCD_CS1_CLR();
    LCD_DC_SET(); // 发送数据
    for (int i = 0; i < 8; i++)
    {
        if (data & 0x80)
            LCD_MOSI_SET();
        else
            LCD_MOSI_CLR();
        LCD_CLK_SET();
        // Delay_us(1);
        LCD_CLK_CLR();
        data <<= 1;
    }
    LCD_CS1_SET();
}

void LCD_WriteData16(uint16_t data)
{
    LCD_CS1_CLR();
    LCD_DC_SET(); // 发送数据
    for (int i = 0; i < 16; i++)
    {
        if (data & 0x8000)
            LCD_MOSI_SET();
        else
            LCD_MOSI_CLR();
        LCD_CLK_SET();
        // Delay_us(1);
        LCD_CLK_CLR();
        data <<= 1;
    }
    LCD_CS1_SET();
}

void LCD_WriteData24(uint32_t data)
{
    LCD_CS1_CLR();
    LCD_DC_SET(); // 发送数据
    for (int i = 0; i < 24; i++)
    {
        if (data & 0x800000)
            LCD_MOSI_SET();
        else
            LCD_MOSI_CLR();
        LCD_CLK_SET();
        // Delay_us(1);
        LCD_CLK_CLR();
        data <<= 1;
    }
    LCD_CS1_SET();
}
/*********************硬件SPI接口*****************************************************************/
void LCD_PIN_Init_HW_SPI(void)
{
// 初始化硬件SPI,MOSI引脚为PA7,CLK引脚为PA5,MISO引脚为PA6
    hw_spi_init();
//cs,res,dc引脚初始化
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure = {
        .GPIO_Pin   = GPIO_Pin_1,         //RES引脚
        .GPIO_Mode  = GPIO_Mode_Out_PP,
        .GPIO_Speed = GPIO_Speed_50MHz,
    };
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; // DC引脚
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; // CS引脚
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    LCD_RES_SET();//低电平复位
    LCD_DC_SET();//高电平像素数据
    LCD_CS1_SET();
}
void LCD_Reset(void)
{
    LCD_RES_CLR();
    Delay_ms(120);
    LCD_RES_SET();
    Delay_ms(10);
}
/**********************************/
static inline void LCD_WriteCommand_Start(void)
{
    LCD_DC_CLR(); // 发送命令
    LCD_CS1_CLR(); // 选择LCD
}
static inline void LCD_WriteData_Start(void)
{
    LCD_DC_SET(); // 发送数据
    LCD_CS1_CLR(); // 选择LCD
}
static inline void LCD_Write_DC_End(void)
{
    LCD_CS1_SET(); // 取消选择LCD
}
/**********************************/
void LCD_WriteCommand_HW_SPI(uint8_t cmd)
{
    LCD_WriteCommand_Start(); // 准备发送命令
    hw_spi_transfer(cmd); // 使用硬件SPI发送命令
    LCD_Write_DC_End(); // 结束命令发送
}
void LCD_WriteData_HW_SPI(uint8_t data)
{
    LCD_WriteData_Start(); // 准备发送数据
    hw_spi_transfer(data); // 使用硬件SPI发送数据
    LCD_Write_DC_End(); // 结束数据发送
}
void LCD_WriteData16_HW_SPI(uint16_t data)
{
    LCD_WriteData_Start(); // 准备发送数据
    hw_spi_transfer(data >> 8); // 发送高字节
    hw_spi_transfer(data & 0xFF); // 发送低字节
    LCD_Write_DC_End(); // 结束数据发送
}

/************************************************************************************/

void LCD_SetAddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // LCD_WriteCommand(0x2A); // 列地址设置
    // LCD_WriteData(x1 >> 8);
    // LCD_WriteData(x1);
    // LCD_WriteData(x2 >> 8);
    // LCD_WriteData(x2);
    // LCD_WriteCommand(0x2B); // 行地址设置
    // LCD_WriteData(y1 >> 8);
    // LCD_WriteData(y1);
    // LCD_WriteData(y2 >> 8);
    // LCD_WriteData(y2);
    // LCD_WriteCommand(0x2C); // 开始写入GRAM
    LCD_WriteCommand_HW_SPI(0x2A); // 列地址设置
    LCD_WriteData16_HW_SPI(x1);
    LCD_WriteData16_HW_SPI(x2);
    LCD_WriteCommand_HW_SPI(0x2B); // 行地址设置
    LCD_WriteData16_HW_SPI(y1);
    LCD_WriteData16_HW_SPI(y2);
    LCD_WriteCommand_HW_SPI(0x2C); // 开始写入GRAM
}

void LCD_IC_Init(void)
{
    //************* Start Initial Sequence **********
    LCD_WriteCommand_HW_SPI(0x3A);  //8.2.33. COLMOD: Pixel Format Set (3Ah)用于设置像素格式，决定每个像素占用的位数，从而影响颜色深度和显示效果
    LCD_WriteData_HW_SPI(0x55);     //0x55表示每个像素占用16位（5-6-5格式），即红色占5位，绿色占6位，蓝色占5位。这种格式可以显示65536种颜色，适合大多数应用场景，同时在内存和带宽方面也比较高效。0x66表示每个像素占用18位（6-6-6格式），即红色、绿色和蓝色各占6位，可以显示262144种颜色，提供更丰富的颜色表现，但同时也会增加内存和带宽的需求。

    LCD_WriteCommand_HW_SPI(0xF6);
    LCD_WriteData_HW_SPI(0x01);
    LCD_WriteData_HW_SPI(0x33);

    LCD_WriteCommand_HW_SPI(0xB5);
    LCD_WriteData_HW_SPI(0x04);
    LCD_WriteData_HW_SPI(0x04);
    LCD_WriteData_HW_SPI(0x0A);
    LCD_WriteData_HW_SPI(0x14);

    LCD_WriteCommand_HW_SPI(0x35);
    LCD_WriteData_HW_SPI(0x00);

    LCD_WriteCommand_HW_SPI(0xCF);
    LCD_WriteData_HW_SPI(0x00);
    LCD_WriteData_HW_SPI(0xEA);
    LCD_WriteData_HW_SPI(0xF0);

    LCD_WriteCommand_HW_SPI(0xED);
    LCD_WriteData_HW_SPI(0x64);
    LCD_WriteData_HW_SPI(0x03);
    LCD_WriteData_HW_SPI(0x12);
    LCD_WriteData_HW_SPI(0x81);

    LCD_WriteCommand_HW_SPI(0xE8);
    LCD_WriteData_HW_SPI(0x85);
    LCD_WriteData_HW_SPI(0x00);
    LCD_WriteData_HW_SPI(0x78);

    LCD_WriteCommand_HW_SPI(0xCB);
    LCD_WriteData_HW_SPI(0x39);
    LCD_WriteData_HW_SPI(0x2C);
    LCD_WriteData_HW_SPI(0x00);
    LCD_WriteData_HW_SPI(0x33);
    LCD_WriteData_HW_SPI(0x06);

    LCD_WriteCommand_HW_SPI(0xF7);
    LCD_WriteData_HW_SPI(0x20);

    LCD_WriteCommand_HW_SPI(0xEA);
    LCD_WriteData_HW_SPI(0x00);
    LCD_WriteData_HW_SPI(0x00);

    LCD_WriteCommand_HW_SPI(0xC0);    //Power control
    LCD_WriteData_HW_SPI(0x21);   //VRH[5:0]

    LCD_WriteCommand_HW_SPI(0xC1);    //Power control
    LCD_WriteData_HW_SPI(0x10);   //BT[3:0]

    LCD_WriteCommand_HW_SPI(0xC5);    //VCM control
    LCD_WriteData_HW_SPI(0x4F);
    LCD_WriteData_HW_SPI(0x38);

    LCD_WriteCommand_HW_SPI(0xC7);
    LCD_WriteData_HW_SPI(0x98);

    LCD_WriteCommand_HW_SPI(0x36);    // Memory Access Control
    LCD_WriteData_HW_SPI(0x08);

    LCD_WriteCommand_HW_SPI(0xB1);
    LCD_WriteData_HW_SPI(0x00);
    LCD_WriteData_HW_SPI(0x13);

    LCD_WriteCommand_HW_SPI(0xB6);    // Display Function Control
    LCD_WriteData_HW_SPI(0x0A);
    LCD_WriteData_HW_SPI(0xA2);

    LCD_WriteCommand_HW_SPI(0xF2);    // 3Gamma Function Disable
    LCD_WriteData_HW_SPI(0x02);

    LCD_WriteCommand_HW_SPI(0xE0);    //Set Gamma
    LCD_WriteData_HW_SPI(0x0F);
    LCD_WriteData_HW_SPI(0x27);
    LCD_WriteData_HW_SPI(0x24);
    LCD_WriteData_HW_SPI(0x0C);
    LCD_WriteData_HW_SPI(0x10);
    LCD_WriteData_HW_SPI(0x08);
    LCD_WriteData_HW_SPI(0x55);
    LCD_WriteData_HW_SPI(0X87);
    LCD_WriteData_HW_SPI(0x45);
    LCD_WriteData_HW_SPI(0x08);
    LCD_WriteData_HW_SPI(0x14);
    LCD_WriteData_HW_SPI(0x07);
    LCD_WriteData_HW_SPI(0x13);
    LCD_WriteData_HW_SPI(0x08);
    LCD_WriteData_HW_SPI(0x00);

    LCD_WriteCommand_HW_SPI(0xE1);    //Set Gamma
    LCD_WriteData_HW_SPI(0x00);
    LCD_WriteData_HW_SPI(0x0F);
    LCD_WriteData_HW_SPI(0x12);
    LCD_WriteData_HW_SPI(0x05);
    LCD_WriteData_HW_SPI(0x11);
    LCD_WriteData_HW_SPI(0x06);
    LCD_WriteData_HW_SPI(0x25);
    LCD_WriteData_HW_SPI(0x34);
    LCD_WriteData_HW_SPI(0x37);
    LCD_WriteData_HW_SPI(0x01);
    LCD_WriteData_HW_SPI(0x08);
    LCD_WriteData_HW_SPI(0x07);
    LCD_WriteData_HW_SPI(0x2B);
    LCD_WriteData_HW_SPI(0x34);
    LCD_WriteData_HW_SPI(0x0F);

    LCD_WriteCommand_HW_SPI(0x11);    //Exit Sleep
    Delay_ms(120);
    LCD_WriteCommand_HW_SPI(0x29);    //Display on
}

void LCD_Init_All(void)
{
    // LCD_PIN_Init();
    // LCD_Reset();
    // LCD_IC_Init();

    LCD_PIN_Init_HW_SPI();
    LCD_Reset();
    LCD_IC_Init();
}

void LCD_DrawPoint(uint16_t x, uint16_t y, uint16_t color)
{
    LCD_SetAddress(x, y, x, y);
    LCD_WriteData16_HW_SPI(color);
}

void LCD_DrawLine(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2, e2;
    while (1) {
        LCD_DrawPoint(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x1 += sx; }
        if (e2 < dy)  { err += dx; y1 += sy; }
    }
}

void LCD_DrawRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    LCD_DrawLine(x1, y1, x2, y1, color);
    LCD_DrawLine(x2, y1, x2, y2, color);
    LCD_DrawLine(x2, y2, x1, y2, color);
    LCD_DrawLine(x1, y2, x1, y1, color);
}

void LCD_FillRect(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2, uint16_t color)
{
    LCD_SetAddress(x1, y1, x2, y2); // 只设置一次窗口
    uint32_t total = (x2 - x1 + 1) * (y2 - y1 + 1);//从左上角到右下角的像素总数

    LCD_WriteData_Start();// 准备发送数据
    for (uint32_t i = 0; i < total; i++) {
        hw_spi_transfer(color >> 8); // 发送高字节
        hw_spi_transfer(color & 0xFF); // 发送低字节
    }
    LCD_Write_DC_End(); // 结束数据发送
}

void LCD_SetTextColor(uint16_t color) {
    LCD_TextColor = color;
}

void LCD_SetBackColor(uint16_t color) {
    LCD_BackColor = color;
}

void LCD_Clear(uint16_t color) {
    LCD_FillRect(0, 0, LCD_WIDTH - 1, LCD_HEIGHT - 1, color);
}

// #define LCD_LINE_HEIGHT 16
// #define LCD_LINE_MAX    (LCD_HEIGHT / LCD_LINE_HEIGHT)

// void LCD_DisplayStringLine(uint16_t line, uint8_t *str) {
//     if (line >= LCD_LINE_MAX) return;
//     LCD_DrawString(0, line * LCD_LINE_HEIGHT, (const char *)str, LCD_TextColor, LCD_BackColor);
// }

// void LCD_DrawChar(uint16_t x, uint16_t y, char chr, uint16_t color, uint16_t bgcolor)
// {
//     extern const uint8_t ascii_8x16[];
//     const uint8_t *p = &ascii_8x16[(chr - 32) * 16];
//     for (uint8_t i = 0; i < 16; i++) {
//         uint8_t line = p[i];
//         for (uint8_t j = 0; j < 8; j++) {
//             if (line & 0x80)
//                 LCD_DrawPoint(x + j, y + i, color);
//             else
//                 LCD_DrawPoint(x + j, y + i, bgcolor);
//             line <<= 1;
//         }
//     }
// }

// void LCD_DrawString(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bgcolor)
// {
//     while (*str) {
//         LCD_DrawChar(x, y, *str, color, bgcolor);
//         x += 8;
//         str++;
//     }
// }

void LCD_ShowSnow(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    LCD_SetAddress(x1, y1, x2, y2); // 只设置一次窗口
    uint32_t total = (x2 - x1 + 1) * (y2 - y1 + 1);//从左上角到右下角的像素总数

    LCD_WriteData_Start();// 准备发送数据
    for (uint32_t i = 0; i < total; i++) {
        // 双色雪花（黑白）
        // uint16_t color = (rand() & 1) ? 0xFFFF : 0x0000;
        
        // 彩色雪花（16位色随机）
        uint16_t color = (uint16_t)rand();

        hw_spi_transfer(color >> 8); // 发送高字节
        hw_spi_transfer(color & 0xFF); // 发送低字节
    }
    LCD_Write_DC_End(); // 结束数据发送
}




