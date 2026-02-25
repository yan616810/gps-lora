#include "u8g2_monochrome_display.h"
#include "u8g2.h"
#include "stm32f10x.h"
#include "Delay.h"
#include "hw_iic.h"
#include "u8g2_monochrome_display_earth_image.h"

//软件iic，u8x8_gpio_and_delay_for_sw_iic()函数中用于初始化软件iic引脚
void oled_pin_init(void)//SCL=PB8     SDA=PB9
{
	RCC_APB2PeriphClockCmd(u8g2_iic_sw_scl_RCC_APB2Periph_GPIOx|u8g2_iic_sw_sda_RCC_APB2Periph_GPIOx,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_OD;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin=u8g2_iic_sw_scl_pin;//SCL
	GPIO_Init(u8g2_iic_sw_scl_port,&GPIO_InitStruct);
	GPIO_InitStruct.GPIO_Pin=u8g2_iic_sw_sda_pin;//SDA
	GPIO_Init(u8g2_iic_sw_sda_port,&GPIO_InitStruct);
}

// 软件iic使用的回调
uint8_t u8x8_gpio_and_delay_for_sw_iic(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  (void)arg_ptr;
	uint32_t delay_us;
  switch(msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:	// called once during init phase of u8g2/u8x8
		oled_pin_init();//SCL和SDA引脚初始化
      break;							// can be used to setup pins
    // case U8X8_MSG_DELAY_NANO:			// delay arg_int * 1 nano second
		// 
    //   break;    
    // case U8X8_MSG_DELAY_100NANO:		// delay arg_int * 100 nano seconds
		// __NOP();
    //   break;
    // case U8X8_MSG_DELAY_10MICRO:		// delay arg_int * 10 micro seconds
		// Delay_us(10*arg_int);
    //   break;
    case U8X8_MSG_DELAY_MILLI:			// delay arg_int * 1 milli second
		Delay_ms(1*arg_int);//OLED上电复位开机顺序，等待100ms才可写寄存器
      break;
    case U8X8_MSG_DELAY_I2C:				// arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
		delay_us = 5000 / arg_int; // 单位：ns
		delay_us /= 1000; // 转换为μs
		Delay_us(delay_us);//默认是1us，400KHZ
		// Delay_us(5);
      break;							// arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
    // case U8X8_MSG_GPIO_D0:				// D0 or SPI clock pin: Output level in arg_int
    // case U8X8_MSG_GPIO_SPI_CLOCK:
    //   break;
    // case U8X8_MSG_GPIO_D1:				// D1 or SPI data pin: Output level in arg_int
    // case U8X8_MSG_GPIO_SPI_DATA:
    //   break;
    // case U8X8_MSG_GPIO_D2:				// D2 pin: Output level in arg_int
    //   break;
    // case U8X8_MSG_GPIO_D3:				// D3 pin: Output level in arg_int
    //   break;
    // case U8X8_MSG_GPIO_D4:				// D4 pin: Output level in arg_int
    //   break;
    // case U8X8_MSG_GPIO_D5:				// D5 pin: Output level in arg_int
    //   break;
    // case U8X8_MSG_GPIO_D6:				// D6 pin: Output level in arg_int
    //   break;
    // case U8X8_MSG_GPIO_D7:				// D7 pin: Output level in arg_int
    //   break;
    // case U8X8_MSG_GPIO_E:				// E/WR pin: Output level in arg_int
    //   break;
    // case U8X8_MSG_GPIO_CS:				// CS (chip select) pin: Output level in arg_int
    //   break;
    // case U8X8_MSG_GPIO_DC:				// DC (data/cmd, A0, register select) pin: Output level in arg_int
    //   break;
    case U8X8_MSG_GPIO_RESET:			// Reset pin: Output level in arg_int
		//在这里我们不使用重置引脚，貌似我的显示屏也没有？？
		//iic协议中有对这个引脚进行操作(具体在u8g2_InitDisplay(u8g2);函数中U8X8_MSG_DISPLAY_INIT条件下的u8x8_d_helper_display_init();的第3)步会进行操作)，1->delay_ms(100)->0->delay_ms(100)->1->delay_ms(100);
      break;
    // case U8X8_MSG_GPIO_CS1:				// CS1 (chip select) pin: Output level in arg_int
    //   break;
    // case U8X8_MSG_GPIO_CS2:				// CS2 (chip select) pin: Output level in arg_int
    //   break;
    case U8X8_MSG_GPIO_I2C_CLOCK:		// arg_int=0: Output low at I2C clock pin
		arg_int ? GPIO_SetBits(u8g2_iic_sw_scl_port,u8g2_iic_sw_scl_pin) : GPIO_ResetBits(u8g2_iic_sw_scl_port,u8g2_iic_sw_scl_pin);
      break;							// arg_int=1: Input dir with pullup high for I2C clock pin
    case U8X8_MSG_GPIO_I2C_DATA:			// arg_int=0: Output low at I2C data pin
		arg_int ? GPIO_SetBits(u8g2_iic_sw_sda_port,u8g2_iic_sw_sda_pin) : GPIO_ResetBits(u8g2_iic_sw_sda_port,u8g2_iic_sw_sda_pin);
      break;							// arg_int=1: Input dir with pullup high for I2C data pin
    // case U8X8_MSG_GPIO_MENU_SELECT:
    //   u8x8_SetGPIOResult(u8x8, /* get menu select pin state */ 0);
    //   break;
    // case U8X8_MSG_GPIO_MENU_NEXT:
    //   u8x8_SetGPIOResult(u8x8, /* get menu next pin state */ 0);
    //   break;
    // case U8X8_MSG_GPIO_MENU_PREV:
    //   u8x8_SetGPIOResult(u8x8, /* get menu prev pin state */ 0);
    //   break;
    // case U8X8_MSG_GPIO_MENU_HOME:
    //   u8x8_SetGPIOResult(u8x8, /* get menu home pin state */ 0);
    //   break;
    default:
      u8x8_SetGPIOResult(u8x8, 1);			// default return value
      break;
  }
  return 1;
}

/***************************以下两个回调函数实现u8g2与硬件iic之间的接口**********************************/

//硬件iic使用的回调
uint8_t u8x8_gpio_and_delay_for_hw_iic(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
  (void)arg_ptr;
  uint8_t delay_us;
  switch(msg)
  {
    case U8X8_MSG_GPIO_AND_DELAY_INIT:	// called once during init phase of u8g2/u8x8
      //hw_iic初始化
      hw_iic_init(I2C2);
      break;							// can be used to setup pins
    case U8X8_MSG_DELAY_NANO:			// delay arg_int * 1 nano second
      break;    
    case U8X8_MSG_DELAY_100NANO:		// delay arg_int * 100 nano seconds
      break;
    case U8X8_MSG_DELAY_10MICRO:		// delay arg_int * 10 micro seconds
      Delay_ms(10*arg_int);
      break;
    case U8X8_MSG_DELAY_MILLI:			// delay arg_int * 1 milli second
      Delay_ms(arg_int);//屏幕上电复位会使用到，如ssd1306的100ms延时
      break;
    case U8X8_MSG_DELAY_I2C:				// arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
      delay_us = 5000 / arg_int; // 单位：ns
		  delay_us /= 1000; // 转换为μs
		  Delay_us(delay_us);//默认是1us，400KHZ
      break;							// arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
    case U8X8_MSG_GPIO_RESET:			// Reset pin: Output level in arg_int
      break;
    case U8X8_MSG_GPIO_I2C_CLOCK:		// arg_int=0: Output low at I2C clock pin
      break;							// arg_int=1: Input dir with pullup high for I2C clock pin
    case U8X8_MSG_GPIO_I2C_DATA:			// arg_int=0: Output low at I2C data pin
      break;							// arg_int=1: Input dir with pullup high for I2C data pin
    default:
      u8x8_SetGPIOResult(u8x8, 1);			// default return value
      break;
  }
  return 1;
}


//仿照官方的软件iic接口u8x8_byte_sw_i2c()，仿写出硬件iic接口回调
uint8_t u8x8_byte_hw_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    uint8_t *data;
    switch(msg) {
    case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;
        while (arg_int--) {
            I2C_SendData(I2C2, *data++);
            if (hw_iic_CheckEvent_timeout(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
                return 0;
        }
      // data = (uint8_t *)arg_ptr;
      // while( arg_int > 0 )
      // {
	      // i2c_write_byte(u8x8, *data);
	      // data++;
	      // arg_int--;
      // }
        break;

    case U8X8_MSG_BYTE_INIT:
        /* no action; peripheral initialised by GPIO callback */
        // i2c_init(u8x8);
        break;

    case U8X8_MSG_BYTE_SET_DC:
        /* not used for I2C */
        break;

    case U8X8_MSG_BYTE_START_TRANSFER:
        if (hw_iic_wait_not_busy(I2C2))
            return 0;
        I2C_GenerateSTART(I2C2, ENABLE);
        if (hw_iic_CheckEvent_timeout(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
            return 0;
        I2C_Send7bitAddress(I2C2, u8x8_GetI2CAddress(u8x8), I2C_Direction_Transmitter);
        if (hw_iic_CheckEvent_timeout(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
            return 0;
      // i2c_start(u8x8);
      // i2c_write_byte(u8x8, u8x8_GetI2CAddress(u8x8));
      // //i2c_write_byte(u8x8, 0x078);
        break;

    case U8X8_MSG_BYTE_END_TRANSFER:
        I2C_GenerateSTOP(I2C2, ENABLE);
        // i2c_stop(u8x8);
        break;

    default:
        return 0;
    }
    return 1;
}

/***************************初始化一个u8g2屏幕实例************************/

void u8g2_oled_init(u8g2_t *u8g2)
{
	u8g2_Setup_ssd1306_i2c_128x64_noname_f(u8g2,U8G2_R0,u8x8_byte_hw_i2c,u8x8_gpio_and_delay_for_hw_iic);
	  u8g2_InitDisplay(u8g2);// send init sequence to the display, display is in sleep mode after this
	  u8g2_SetPowerSave(u8g2,0);//disable sleep
	  u8g2_ClearBuffer(u8g2);
}

/************************************以下是非必要函数，都是u8g2的上层使用**************************************/

void u8g2_oled_play_Animation(u8g2_t *u8g2)
{
//	u8g2_DrawBox(u8g2,0,0,128,128);
//	u8g2_SetFont(u8g2,u8g2_font_5x7_tr);
//	u8g2_SetFontMode(u8g2,0);//0：填充  1：透明  
//	u8g2_SetDrawColor(u8g2,0);//字符由像素点亮表示   2：异或
//	u8g2_SetFontPosTop(u8g2);
//	u8g2_DrawStr(u8g2,0,0,"[==hello,world==]");

	u8g2_SetFont(u8g2,u8g2_font_unifont_t_76);//u8g2_font_unifont_t_cards  32-117
	u8g2_SetFontPosTop(u8g2);
	u8g2_SetFontMode(u8g2,1);
	u8g2_SetDrawColor(u8g2,1);
	u8g2_DrawGlyphX2(u8g2,30,20,9762);//核警告
	u8g2_DrawGlyphX2(u8g2,70,20,9763);//生化警告
	
	// 9728-9856
	u8g2_SetFont(u8g2,u8g2_font_koleeko_tr);
	u8g2_DrawStr(u8g2,20,2,">-WARNING-<");
	u8g2_DrawRFrame(u8g2,0,0,128,64,5);
	u8g2_DrawHLine(u8g2,3,1,122);
	u8g2_DrawHLine(u8g2,0,16,127);
	u8g2_DrawBox(u8g2,0,13,128,3);
	u8g2_SendBuffer(u8g2);
	
	Delay_ms(500);
	u8g2_ClearBuffer(u8g2);
	u8g2_DrawStr(u8g2,20,2,">-WARNING-<");
	u8g2_DrawRFrame(u8g2,0,0,128,64,5);
	u8g2_DrawHLine(u8g2,3,1,122);
	u8g2_DrawHLine(u8g2,0,16,127);
	u8g2_DrawBox(u8g2,0,13,128,3);
{
	for(uint16_t i=32;i<=118;i++)
	{
		u8g2_SetFont(u8g2,u8g2_font_unifont_t_cards);
		u8g2_DrawGlyphX2(u8g2,50-u8g2_GetXOffsetGlyph(u8g2,i),25+2*13,i);//x,y都有偏移,50,20才是真正坐标
		u8g2_SendBuffer(u8g2);
		u8g2_SetFont(u8g2,u8g2_font_koleeko_tr);
		u8g2_ClearBuffer(u8g2);
		u8g2_DrawStr(u8g2,20,2,">-WARNING-<");
		u8g2_DrawRFrame(u8g2,0,0,128,64,5);
		u8g2_DrawHLine(u8g2,3,1,122);
		u8g2_DrawHLine(u8g2,0,16,127);
		u8g2_DrawBox(u8g2,0,13,128,3);
//		Delay_ms(1);
	}
}
  u8g2_ClearDisplay(u8g2);

}

//空间中画一个长方体，
void u8g2_oled_draw_cube(void)
{

}
void creat_matrix(uint16_t w,uint16_t h)
{
  (void)w;
  (void)h;
}

/*****************************GPS实时坐标以两条虚线显示在OLED12864全球缩略图上********************************************/
/**
 * @brief 在OLED上画地球缩略图
 */
void u8g2_oled_draw_earth(u8g2_t *u8g2)
{
  u8g2_SetDrawColor(u8g2, 1); // 设置绘制颜色为白色
  u8g2_DrawXBM(u8g2, 0, 0, earth_width, earth_height, gImage_earth);
}

/**
 * @brief 将经纬度转换为OLED显示屏的像素坐标
 * 
 * @param x 以OLED左上角为原点(0,0)的x坐标[0, earth_width-1]
 * @param y 以OLED左上角为原点(0,0)的y坐标[0, earth_height-1]
 * @param lat 纬度 [-90, 90]
 * @param lon 经度 [-180, 180]
 */
void Latlon2pixel(uint16_t *x, uint16_t *y, float lat, float lon)
{
  // 假设地球图像的宽度和高度分别为 earth_width 和 earth_height
  // 纬度范围为 -90 到 90， 经度范围为 -180 到 180
  // 将经纬度转换为像素坐标
  *x = (uint16_t)((lon + 180.0) * (earth_width-1) / 360.0 + 0.5f); // +0.5f用于四舍五入
  *y = (uint16_t)((90.0 - lat) * (earth_height-1) / 180.0 + 0.5f); // +0.5f用于四舍五入
}

/**
 * @brief 画水平虚线
 * 
 * @param y 
 */
void u8g2_oled_draw_HxvLine(u8g2_t *u8g2,uint16_t y)
{
  uint16_t x0 = 0;
  uint16_t x1 = earth_width; // 获取显示屏的宽度
  uint16_t step = 2 ; // 虚线的间隔为2个像素
  for(uint16_t i = x0; i < x1; i +=1) {
    if ((i / step) % 2 == 0) { // 每隔step个像素绘制一条线
      u8g2_SetDrawColor(u8g2, 1); // 设置绘制颜色为白色
    } else {
      u8g2_SetDrawColor(u8g2, 0); // 设置绘制颜色为黑色
    }
    u8g2_DrawPixel(u8g2, i, y); // 绘制像素点
  }
}

/**
 * @brief 画垂直虚线
 * 
 * @param x 
 */
void u8g2_oled_draw_VxvLine(u8g2_t *u8g2,uint16_t x)
{
  uint16_t y0 = 0;
  uint16_t y1 = earth_height; // 获取显示屏的高度
  uint16_t step = 2 ; // 虚线的间隔为2个像素
  for(uint16_t i = y0; i < y1; i +=1) {
    if ((i/step) % 2 == 0) { // 每隔step个像素绘制一条线
      u8g2_SetDrawColor(u8g2, 1); // 设置绘制颜色为白色
    } else {
      u8g2_SetDrawColor(u8g2, 0); // 设置绘制颜色为黑色
    }
    u8g2_DrawPixel(u8g2, x, i); // 绘制像素点
  }
}

/**
 * @brief 以经纬度为中心点，画水平和垂直虚线
 * 
 * @param u8g2 图形库实例
 * @param lat 纬度 [-90, 90]
 * @param lon 经度 [-180, 180]
 */
void u8g2_oled_draw_earth_pixel_VHxvLine(u8g2_t *u8g2,float lat, float lon)
{
  uint16_t x,y;
	Latlon2pixel(&x,&y,lat,lon);//将经纬度转换为像素坐标
  u8g2_oled_draw_HxvLine(u8g2,y);//画水平虚线
  u8g2_oled_draw_VxvLine(u8g2,x);//画垂直虚线
  // u8g2_SetDrawColor(u8g2, 1); // 设置绘制颜色为白色
  // u8g2_DrawPixel(u8g2, x, y); // 绘制像素点
}