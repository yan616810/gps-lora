#include "UI.h"
#include <math.h>

/*GPS*/
char     var_sign_lat  = '+';  //纬度符号，默认正号不显示
uint16_t int_part_lat  = 0;    //纬度整数部分，单位是度 
uint32_t frac_part_lat = 0;    //纬度小数部分，单位是度的小数
char     var_sign_lon  = '+';  //经度符号，默认正号不显示
uint16_t int_part_lon  = 0;    //经度整数部分，单位是度
uint32_t frac_part_lon = 0;    //经度小数部分，单位是度的小数
char     var_sign_alt  = '+';  //相对高度符号，默认正号不显示
uint16_t int_part_alt  = 0;    //相对高度整数部分，单位是米
uint16_t frac_part_alt = 0;	   //相对高度小数部分，单位是米的小数
uint16_t int_part_spd  = 0;    //速度整数部分，单位是米每秒
uint16_t frac_part_spd = 0;	   //速度小数部分，单位是米每秒的小数
/*qmc6309*/
char     var_sign_MV  = '+';
uint16_t int_part_MV  = 0;
uint16_t frac_part_MV = 0;
/*bmp280*/
float    fake_sea_level_pressure = 103019.0f;  //相对标准大气压，单位是Pa
char     altitude_sign           = '+';        //bmp280推算出的相对高度符号，默认正号
uint16_t altitude_int_part       = 0;          //bmp280推算出的相对高度整数部分，单位是米
uint16_t altitude_frac_part      = 0;          //bmp280推算出的相对高度小数部分，单位是0.1米
char     *temp_sign              = "";         //温度符号，默认正号不显示
uint16_t temp_labs_int           = 0;          //温度整数部分，单位是摄氏度
uint16_t temp_labs_frac          = 0;          //温度小数部分，单位是0.01摄氏度



/*GPS数据+全球缩略图OLED界面所需的数据处理函数*/
void UI_GPS_display_earth_data_proc(void)
{
    if(gps.lwgps_handle.is_valid && gps.lwgps_handle.fix)//报文有效且已解算出定位
	{
        //如果gps.lwgps_handle.seconds发生变化了说明GPS时间更新了，才进行一次坐标转换和显示更新；如果GPS时间没有更新，就不进行坐标转换和显示更新；这样可以避免在GPS时间没有更新时，频繁地进行坐标转换和显示更新，节省CPU资源和电量；因为坐标转换和显示更新是比较耗时的操作，不需要每次都进行；
        static uint8_t last_minutes = 0xFF;//上一次的GPS年日，初始值设置为不可能的值0xFF，这样第一次进入函数时就会进行磁偏角转换和显示更新；之后当设备开始一直开启并只在新的一天开始才会进行磁偏角转换和显示更新；  
	    static uint8_t last_seconds = 0xFF;//上一次的GPS秒数，初始值设置为不可能的值0xFF，这样第一次进入函数时就会进行坐标转换和显示更新；之后只有当GPS秒数发生变化时才会进行坐标转换和显示更新；
        if(gps.lwgps_handle.seconds != last_seconds)
	    {
	    	last_seconds = gps.lwgps_handle.seconds;
	    	//纬度处理成OLED可显示的格式
	    		var_sign_lat  = (gps.lwgps_handle.latitude >= 0.0f) ? '+' : '-';
	    		double   abs_var_lat   = fabs(gps.lwgps_handle.latitude);
	    		uint32_t temp_lat      = abs_var_lat * 1000000.0f + 0.5f;  //四舍五入保留小数点后6位，可精确到0.11米
	    		int_part_lat  = temp_lat / 1000000;                        //整数部分
	    		frac_part_lat = temp_lat % 1000000;                        //小数部分
	    	//经度处理成OLED可显示的格式
	    		var_sign_lon  = (gps.lwgps_handle.longitude >= 0.0f) ? '+' : '-';
	    		double   abs_var_lon   = fabs(gps.lwgps_handle.longitude);
	    		uint32_t temp_lon      = abs_var_lon * 1000000.0f + 0.5f;  //四舍五入保留小数点后6位，可精确到0.11米
	    		int_part_lon  = temp_lon / 1000000;                        //整数部分
	    		frac_part_lon = temp_lon % 1000000;                        //小数部分
	    	//高度处理成OLED可显示的格式
	    		var_sign_alt  = (gps.lwgps_handle.altitude >= 0.0f) ? '+' : '-';
	    		double   abs_var_alt   = fabs(gps.lwgps_handle.altitude);
	    		uint32_t temp_alt      = abs_var_alt * 100.0f + 0.5f;
	    		int_part_alt  = temp_alt / 100;          //整数部分
	    		frac_part_alt = temp_alt % 100;          //小数
            //速度处理成OLED可显示的格式
        		uint32_t temp_spd      = lwgps_to_speed(gps.lwgps_handle.speed, lwgps_speed_mps) * 10.0f + 0.5f;
        		int_part_spd  = temp_spd / 10;          //整数部分
        		frac_part_spd = temp_spd % 10;          //小数
            //得到当前日期对应的WMM模型参数，计算磁偏角；(因为你的经纬度可能变化非常快，所以实时计算磁偏角可以让航向校正更准确；如果你觉得计算磁偏角比较耗时，可以考虑只在每分钟变化时计算磁偏角，因为磁偏角的变化主要是随着时间的变化，而不是经纬度的变化；这样可以减少磁偏角的计算次数，节省CPU资源和电量；因为磁偏角的计算是比较耗时的操作，不需要每次都进行；)
            if(gps.lwgps_handle.minutes != last_minutes)
            {
                last_minutes = gps.lwgps_handle.minutes;
                    
	    		float Date_WMM = wmm_get_date(gps.lwgps_handle.year % 100, gps.lwgps_handle.month, gps.lwgps_handle.date);
	    		float Magnetic_variation;
	    		E0000(gps.lwgps_handle.latitude, gps.lwgps_handle.longitude, Date_WMM, &Magnetic_variation);//磁偏角结果是正值表示东偏，负值表示西偏，单位是度；输出浮点数
	    		qmc6309.Magnetic_variation = Magnetic_variation;//将磁偏角传给qmc6309实例使用；用于使航向校正为地理正北；
	    		//显示磁偏角，四舍五入保留三位小数，检验能否正常将-0.9567度显示为-0.957度
	    		var_sign_MV  = (Magnetic_variation >= 0.0f) ? '+' : '-';  //符号部分，正值表示东偏，负值表示西偏
	    		float abs_var_MV   = fabsf(Magnetic_variation);                 //计算float类型的绝对值
	    		uint32_t temp      = abs_var_MV * 1000.0f + 0.5f;                  //保留小数点后三位，四舍五入；单位是0.001度；float强制转int时向下截断如： 1.999会变成1，所以加0.5实现四舍五入
	    		int_part_MV  = temp / 1000;                               //整数部分
	    		frac_part_MV = temp % 1000;                               //小数部分
            }
	    }
    }
}

/*GPS无信号时的弹窗*/
void UI_GPS_display_earth_no_data(u8g2_t *u8g2)
{
    u8g2_SetFont(u8g2,u8g2_font_courB08_tf);  //w=7  h=10
    u8g2_SetFontPosTop(u8g2);
    u8g2_SetFontMode(u8g2,0);  //显示字体的背景，不透明   
    u8g2_SetDrawColor(u8g2,1);

	u8g2_DrawBox(u8g2,3*7,27,13*7,15);
	u8g2_SetDrawColor(u8g2,0);
	u8g2_DrawStr(u8g2,3*7,3*10,"[>GPS No Data<]");
}
/*GPS文本模式*/
void UI_GPS_display_earth_txt(u8g2_t *u8g2)
{
    // u8g2_ClearBuffer(u8g2);
    u8g2_SetFont(u8g2,u8g2_font_courB08_tf);  //w=7  h=10
    u8g2_SetFontPosTop(u8g2);
    u8g2_SetFontMode(u8g2,0);  //显示字体的背景，不透明
    u8g2_SetDrawColor(u8g2,1);
    if (gps.lwgps_handle.is_valid && gps.lwgps_handle.fix) //lwgps_is_valid()
    {
        //显示纬度
        sprintf(u8g2_buf,"[Lat:%c%u.%06lu%c]",var_sign_lat, int_part_lat, frac_part_lat, 176);
        u8g2_DrawStr(u8g2,0*7,0*10+13,u8g2_buf);
        //显示经度
        sprintf(u8g2_buf,"[Lon:%c%u.%06lu%c]",var_sign_lon, int_part_lon, frac_part_lon, 176);
        u8g2_DrawStr(u8g2,0*7,1*10+13,u8g2_buf);
        //显示海拔高度
        sprintf(u8g2_buf,"[Alt:%c%u.%02um]",var_sign_alt, int_part_alt, frac_part_alt);
        u8g2_DrawStr(u8g2,0*7,2*10+13,u8g2_buf);
        //显示速度
        sprintf(u8g2_buf,"[spd:%u.%01um/s]", int_part_spd, frac_part_spd);
        u8g2_DrawStr(u8g2,0*7,3*10+13,u8g2_buf);
        //显示磁偏角
        sprintf(u8g2_buf,"[Mag:%c%u.%03u%c]",var_sign_MV, int_part_MV, frac_part_MV, 176);
        u8g2_DrawStr(u8g2,0*7,4*10+13,u8g2_buf);
        // //bmp280推算出的相对高度，四舍五入保留小数点后1位，单位是米
        // sprintf(u8g2_buf,"[R-H:%c%u.%01um]", altitude_sign, altitude_int_part, altitude_frac_part);//相对高度，最大相对高度65535m
        // u8g2_SetDrawColor(u8g2,1);
        // u8g2_DrawStr(u8g2,0*7,4*10+13,u8g2_buf);
    }
    else//GPS无效或无定位
    {
        u8g2_DrawStr(u8g2,0*7,0*10+13,"[Lat:No Data]");
        u8g2_DrawStr(u8g2,0*7,1*10+13,"[Lon:No Data]");
        u8g2_DrawStr(u8g2,0*7,2*10+13,"[Alt:No Data]");
        u8g2_DrawStr(u8g2,0*7,3*10+13,"[Mag:No Data]");
        u8g2_DrawStr(u8g2,0*7,4*10+13,"[R-H:No Data]");

        UI_GPS_display_earth_no_data(u8g2);//在文本模式下，如果GPS无效或无定位，在屏幕中间显示一个明显的弹窗，提示用户GPS无数据；如果GPS有效且有定位，就不显示这个弹窗；这样可以让用户一眼就能看出GPS是否有数据；因为在文本模式下，如果GPS无效或无定位，屏幕上会显示[Lat:No Data][Lon:No Data][Alt:No Data][Mag:No Data][R-H:No Data]，但这些信息可能不够显眼，用户可能会忽略掉；所以加一个弹窗来突出显示GPS无数据的状态；当GPS有效且有定位时，这个弹窗就不会显示了；这样可以提高用户体验，让用户更容易察觉到GPS的状态；
    }
    //箭头
    u8g2_SetDrawColor(u8g2,1);
    u8g2_SetBitmapMode(u8g2, 1);  //设置为透明模式，绘制时不会覆盖背景
    u8g2_DrawXBMP(u8g2, 123, 35, 3, 5, image_ButtonRightSmall_bits);
}
/*GPS全球缩略图模式*/
void UI_GPS_display_earth_image(u8g2_t *u8g2)
{
    u8g2_SetFont(u8g2,u8g2_font_courB08_tf);  //w=7  h=10
    u8g2_SetFontPosTop(u8g2);
    u8g2_SetFontMode(u8g2,0);  //显示字体的背景，不透明
    u8g2_SetDrawColor(u8g2,1);
    if (gps.lwgps_handle.is_valid && gps.lwgps_handle.fix)
    {
        u8g2_oled_draw_earth(u8g2);//在全幅缓冲区内绘制全球缩略图
		u8g2_oled_draw_earth_pixel_VHxvLine(u8g2,gps.lwgps_handle.latitude,gps.lwgps_handle.longitude);//在全球缩略图上绘制实时经纬度坐标点
    }
    else//GPS无效或无定位
    {
        u8g2_oled_draw_earth(u8g2);//在全幅缓冲区内绘制全球缩略图

        UI_GPS_display_earth_no_data(u8g2);
    }
    //箭头
    u8g2_SetDrawColor(u8g2,1);
    u8g2_SetBitmapMode(u8g2, 1);  //设置为透明模式，绘制时不会覆盖背景
    u8g2_DrawXBMP(u8g2, 2, 35, 3, 5, image_ButtonLeftSmall_bits);
}

/***************************************家界面******************************************************** */

void UI_BPM280_data_proc(void)
{
    //温度只显示负号
    	temp_sign=(bmp280.Temperature_ture<0) ? "-" : "";//要用字符串
    	uint16_t temp_labs=(bmp280.Temperature_ture<0) ? -bmp280.Temperature_ture : bmp280.Temperature_ture;
		temp_labs += 5;//四舍五入，单位是0.01摄氏度
    	temp_labs_int = temp_labs/100;
		temp_labs_frac = (temp_labs%100)/10;//四舍五入保留小数点后一位
	//相对高度差，浮点float转符号整数部分和小数部分
		// float altitude = calculate_altitude(bmp280.Pressure_ture, fake_sea_level_pressure);
		// altitude_sign = (altitude >= 0) ? '+' : '-';
		// float abs_var = fabsf(altitude);
		// uint32_t temp = abs_var * 10.0f + 0.5f;//四舍五入保留小数点后两位
		// altitude_int_part = temp / 10;//整数部分
		// altitude_frac_part = temp % 10;//小数部分
}

uint16_t LoRa_num=16;
uint8_t is_charge=0;
uint8_t Power=50;//电量剩余50%

static const char * const wday_data[]={"Sun","Mon","Tue","Wed","Thur","Fri","Sat"};

//电池图标数组统一管理
static const unsigned char* const battery_icons[] = {
    image_Battery_0_10_bits,      // 索引 0： 0~10%
    image_Battery_10_20_bits,     // 索引 1：10~20%
    image_Battery_20_30_bits,     // 索引 2：20~30%
    image_Battery_30_40_bits,     // 索引 3：30~40%
    image_Battery_40_50_bits,     // 索引 4：40~50%
    image_Battery_50_60_bits,     // 索引 5：50~60%
    image_Battery_60_70_bits,     // 索引 6：60~70%
    image_Battery_70_80_bits,     // 索引 7：70~80% 
    image_Battery_80_90_bits,     // 索引 8：80~90% 
    image_Battery_90_100_bits,    // 索引 9：90~100% 
    image_Battery_charge_bits     // 索引 10：充电中
};
void UI_battery(u8g2_t *u8g2, u8g2_uint_t x, u8g2_uint_t y, uint8_t is_charge, uint8_t power)//(计划优化),使用指针数组来统一管理！！！
{
    if(is_charge){
        u8g2_DrawXBMP(u8g2, x, y, 14, 8, battery_icons[10]);
        return;
    }
    uint8_t index=0;
    if(power<=10) index=0;
    else if (power<=20) index=1;
    else if (power<=30) index=2;
    else if (power<=40) index=3;
    else if (power<=50) index=4;
    else if (power<=60) index=5;
    else if (power<=70) index=6;
    else if (power<=80) index=7;
    else if (power<=90) index=8;
    else if (power<=100) index=9;
    u8g2_DrawXBMP(u8g2, x, y, 14, 7, battery_icons[index]);
}
void UI_Top_info(u8g2_t *u8g2)
{
    u8g2_SetFont(u8g2, u8g2_font_6x12_tf);
    u8g2_SetFontPosBaseline(u8g2);//以默认的基线为参考点，使的高为7像素的数字贴顶
    u8g2_SetFontMode(u8g2,1);//背景透明
    u8g2_SetDrawColor(u8g2,1);//像素亮来表示字体
//框架
    u8g2_DrawXBMP(u8g2, 0, 0, 128, 11, image_home_frame_bits);//顶部框架，包括卫星图标+LoRa图标
    UI_battery(u8g2, 91, 2, is_charge, Power);//电池电量
//数据
    //卫星数
    // if(gps.lwgps_handle.is_valid)//报文合法即可；不需要fix定位成功；
    // {
        sprintf(u8g2_buf,"%-3d",gps.lwgps_handle.sats_in_view_total);
        u8g2_DrawStr(u8g2, 21, 9, u8g2_buf);
    // }
    //LoRa数
    sprintf(u8g2_buf,"%-3d",LoRa_num);
    u8g2_DrawStr(u8g2, 66, 9, u8g2_buf);
    //电量数
    sprintf(u8g2_buf,"%-3d",Power);
    u8g2_DrawStr(u8g2, 107, 9, u8g2_buf);
}
void UI_HOME(u8g2_t *u8g2)//主界面显示函数：显示(卫星数量)、LoRa数量、电量百分比、日期、时区、星期、时间、温度、(气压)，并绘制相应的图标和框架
{
    // u8g2_ClearBuffer(u8g2);
	u8g2_SetFontPosBaseline(u8g2);//以默认的基线为参考点，使的高为7像素的数字贴顶
	u8g2_SetFontMode(u8g2,1);//背景透明
    u8g2_SetDrawColor(u8g2,1);//字体像素亮
//框架
    // u8g2_DrawXBMP(u8g2, 0, 0, 128, 11, image_home_frame_bits);//顶部框架，包括卫星图标+LoRa图标
    // UI_battery(u8g2, 91, 2, is_charge, Power);//电池电量
    UI_Top_info(u8g2);
    u8g2_DrawFrame(u8g2, 0, 11, 128, 53);
    u8g2_DrawXBMP(u8g2, 8, 32, 16, 16, image_temperature_bits);//温度计图标
    u8g2_DrawXBMP(u8g2, 104, 31, 15, 16, image_Baro_wind_bits);//气压图标
    u8g2_DrawRFrame(u8g2, 30, 11, 66, 41, 4);//时间大框架
    u8g2_DrawLine(u8g2, 34, 23, 63, 23);//UTC框架
    u8g2_DrawLine(u8g2, 68, 33, 34, 33);//UTC框架
    u8g2_DrawLine(u8g2, 33, 24, 33, 32);//UTC框架
    u8g2_DrawXBMP(u8g2, 63, 24, 30, 9, image_UTC_frame_bits);//星期几的平行四边形框架
    u8g2_DrawRBox(u8g2, 55, 51, 17, 10, 3);//秒背景框
    u8g2_DrawLine(u8g2, 45, 51, 56, 62);
    u8g2_DrawLine(u8g2, 80, 52, 70, 62);
    
    u8g2_SetFont(u8g2, u8g2_font_t0_14_tr);
    u8g2_DrawStr(u8g2, 1, 26, "TEMP");
    u8g2_DrawStr(u8g2, 98, 26, "BARO");
//数据
    u8g2_SetFont(u8g2, u8g2_font_6x12_tf);
    u8g2_SetFontMode(u8g2,1);//背景透明
    u8g2_SetDrawColor(u8g2,0);//像素灭来表示字体
    //反色星期
    if(Struct_RTC.UTCxTime.tm_wday != 4)
        u8g2_DrawStr(u8g2, 70, 32, wday_data[Struct_RTC.UTCxTime.tm_wday]);
    else 
        u8g2_DrawStr(u8g2, 66, 32, wday_data[Struct_RTC.UTCxTime.tm_wday]);
    //秒
    u8g2_SetFont(u8g2, u8g2_font_profont12_tr);
    // char u8g2_buf[25];//不需要每次都调用memset；sprintf 都会从缓冲区开头重新写入内容，并且自动在字符串末尾加上 \0（空终止符）。u8g2_DrawStr 只读取到 \0 为止，不会读后面的垃圾数据
    sprintf(u8g2_buf,"%02d",Struct_RTC.UTCxTime.tm_sec);
    u8g2_DrawStr(u8g2, 58, 60, u8g2_buf);

    u8g2_SetDrawColor(u8g2,1);//像素亮来表示字体
    u8g2_SetFont(u8g2, u8g2_font_6x12_tf);
    //日期
    sprintf(u8g2_buf,"%04d-%02d-%02d",Struct_RTC.UTCxTime.tm_year, Struct_RTC.UTCxTime.tm_mon, Struct_RTC.UTCxTime.tm_mday);
    u8g2_DrawStr(u8g2, 34, 21, u8g2_buf);
    //温度
    sprintf(u8g2_buf,"%s%d.%01d%cC", temp_sign, temp_labs_int, temp_labs_frac,176);
        // u8g2_DrawGlyph(u8g2, 40, 60, (uint16_t)176);//度
    u8g2_DrawStr(u8g2, 3, 62, u8g2_buf);
    //气压
    sprintf(u8g2_buf,"%luPa", bmp280.Pressure_ture);
    u8g2_DrawStr(u8g2, 79, 62, u8g2_buf);
    //UTC
    u8g2_SetFont(u8g2, u8g2_font_5x8_tr);
    sprintf(u8g2_buf,"UTC%+2d",Struct_RTC.timezone/100);
    u8g2_DrawStr(u8g2, 35, 32, u8g2_buf);
    //时分
    u8g2_SetFont(u8g2, u8g2_font_profont22_tr);
    sprintf(u8g2_buf,"%02d:%02d", Struct_RTC.UTCxTime.tm_hour, Struct_RTC.UTCxTime.tm_min);
    u8g2_DrawStr(u8g2, 34, 50, u8g2_buf);    
}

/******************************界面切换************************************************* */

Icon_t icon_location = {image_location_bits, 13, 16};
Icon_t icon_earth = {image_earth_bits, 15, 15};
Icon_t icon_home = {image_menu_home_bits, 15, 15};
Icon_t icon_message = {image_message_bits, 16, 16};
Icon_t icon_device_power_button = {image_device_power_button_bits, 15, 16};
Icon_t icon_about = {image_about_bits, 15, 16};
Icon_t icon_Compass = {image_Compass_bits, 14, 14};
Icon_t icon_settings_gear = {image_menu_settings_gear_bits, 16, 16};
Icon_t icon_blank = {image_blank, 2, 2};

Icon_t * const icon_names[] = {
    &icon_settings_gear       ,   //0 16*16 设置
    &icon_about               ,   //1 15*16 关于
    &icon_device_power_button ,   //2 15*16 电源按钮
    &icon_home                ,   //3 15*15 家主页
    &icon_earth               ,   //4 15*15 地球
    &icon_Compass             ,   //5 14*14 指南针
    &icon_location            ,   //6 13*16 定位
    &icon_message             ,   //7 16*16 消息
    &icon_blank               ,   //8 2*2   空白图标，用于占位
};
//界面切换弹窗，根据ui_root来显示
void UI_switch(u8g2_t *u8g2, int8_t ui_root)//ui_root:当前界面; direction: 1表示从左向右切换，0表示从右向左切换
{
    u8g2_SetFontPosTop(u8g2);
    u8g2_SetFontMode(u8g2,1);  //透明
    
    u8g2_SetDrawColor(u8g2,0);
    u8g2_DrawBox(u8g2, 18, -1, 92, 26);

    u8g2_SetDrawColor(u8g2,1);
    u8g2_DrawRBox(u8g2, 20, -5, 88, 27, 7);
    
    u8g2_SetDrawColor(u8g2,0);
    u8g2_DrawRBox(u8g2, 28, 1, 72, 20, 8);

    u8g2_SetDrawColor(u8g2,1);
    u8g2_DrawRBox(u8g2, 52, 2, 25, 18, 8);

    u8g2_SetDrawColor(u8g2,0);
    if(ui_root != 5) u8g2_DrawXBMP(u8g2, 103, 9, 3, 5, image_ButtonRightSmall_bits);
    if(ui_root != -3) u8g2_DrawXBMP(u8g2, 22, 9, 3, 5, image_ButtonLeftSmall_bits);

    Icon_t *first_icon=NULL, *second_icon=NULL, *third_icon=NULL;
    switch (ui_root)
    {
    //家的前面的页面
        case -3://设置
            first_icon = icon_names[8];
            second_icon = icon_names[0];
            third_icon = icon_names[1];
            break;
        case -2://关于
            first_icon = icon_names[0];
            second_icon = icon_names[1];
            third_icon = icon_names[2];
            break;
        case -1://电源关机
            first_icon = icon_names[1];
            second_icon = icon_names[2];
            third_icon = icon_names[3];
            break;
    //开机初始
        case 0://家
            first_icon = icon_names[2];
            second_icon = icon_names[3];
            third_icon = icon_names[4];
            break;
        case 1:
        case 2://地球文本或地球缩略图
            first_icon = icon_names[3];
            second_icon = icon_names[4];
            third_icon = icon_names[5];
            break;
        case 3://指南针
            first_icon = icon_names[4];
            second_icon = icon_names[5];
            third_icon = icon_names[6];
            break;
        case 4://定位
            first_icon = icon_names[5];
            second_icon = icon_names[6];
            third_icon = icon_names[7];
            break;
        case 5://消息
            first_icon = icon_names[6];
            second_icon = icon_names[7];
            third_icon = icon_names[8];
            break;
        default:
            break;
    }
    
    if(first_icon == NULL || second_icon == NULL || third_icon == NULL) return;//如果图标指针有一个是NULL，就直接返回，不进行绘制；这样可以避免在图标指针为NULL时，调用u8g2_DrawXBMP函数绘制图标，导致程序崩溃；因为u8g2_DrawXBMP函数需要一个有效的图像数据指针，如果传入NULL，就会访问非法内存地址，导致程序崩溃；所以在绘制之前，先检查图标指针是否为NULL，如果是NULL，就直接返回，不进行绘制；只有当三个图标指针都不为NULL时，才进行绘制；
    u8g2_SetDrawColor(u8g2,1);
    u8g2_DrawXBMP(u8g2, 32, 3, first_icon->width, first_icon->height, first_icon->data);//第一个图标位置
    u8g2_DrawXBMP(u8g2, 82, 3, third_icon->width, third_icon->height, third_icon->data);//第三个图标位置

    u8g2_SetBitmapMode(u8g2,1);//设置为透明模式
    u8g2_SetDrawColor(u8g2,0);
    u8g2_DrawXBMP(u8g2, 58, 3, second_icon->width, second_icon->height, second_icon->data);//第二个图标位置
}

/*****************************指南针界面******************************************* */

/* ─── 参数配置 ─── */
#define COMPASS_CX       26    // 圆心X（屏幕左半边）
#define COMPASS_CY       37    // 圆心Y（128x64屏幕垂直居中）
#define COMPASS_R        25    // 外圆半径
#define COMPASS_LABEL_R  18    // 字符 NESW 到圆心的距离
#define COMPASS_TICK_R   (COMPASS_R - 1)  // 刻度起点（贴内壁）

/* ─── 角度转弧度 ─── */
#define DEG2RAD(d) ((d) * 3.14159265f / 180.0f)

/* ─── 辅助：将角度+半径转成屏幕坐标 ───
 * angle_deg：以"正上方=0°、顺时针为正"的度数
 */
static inline int16_t px(float angle_deg, float r) {
    return (int16_t)(COMPASS_CX + r * sinf(DEG2RAD(angle_deg)));
}
static inline int16_t py(float angle_deg, float r) {
    return (int16_t)(COMPASS_CY - r * cosf(DEG2RAD(angle_deg)));
}

void UI_Compass_display(u8g2_t *u8g2, int16_t qmc6309_heading)
{
    u8g2_SetFontPosTop(u8g2);
    u8g2_SetFontMode(u8g2, 0);   // 不透明背景
    u8g2_SetDrawColor(u8g2, 1);
//框架
    u8g2_DrawFrame(u8g2, 0, 11, 53, 53);
    u8g2_DrawFrame(u8g2, 52, 10, 76, 54);
    u8g2_DrawLine(u8g2, 52, 49, 127, 49);
    u8g2_DrawXBMP(u8g2, 4, 15, 4, 4, image_menu_arrow_up_left_bits);
    u8g2_DrawXBMP(u8g2, 45, 15, 4, 4, image_menu_arrow_up_right_bits);
    u8g2_DrawXBMP(u8g2, 45, 56, 4, 4, image_menu_arrow_down_right_bits);
    u8g2_DrawXBMP(u8g2, 4, 56, 4, 4, image_menu_arrow_down_left_bits);
//指南针圆盘
    float h = (float)(qmc6309_heading); // 0~359

    /* ── 1. 外圆 ── */
    u8g2_DrawCircle(u8g2, COMPASS_CX, COMPASS_CY, COMPASS_R, U8G2_DRAW_ALL);

    // /* ── 2. 刻度线（随圆盘旋转） ──
    //  * 每15°一根，共24根；
    //  * 主刻度（0/90/180/270°）长5px，次刻度（45°）长3px，其余长2px
    //  */
    // for (int i = 0; i < 24; i++) {
    //     float angle = (float)(i * 15) - h;
    //     uint8_t tick_len;
    //     if      (i % 6 == 0) tick_len = 5;   // 主刻度（NESW位置）
    //     else if (i % 3 == 0) tick_len = 3;   // 次刻度（NE/SE/SW/NW）
    //     else                 tick_len = 2;   // 小刻度

    //     int16_t x1 = px(angle, COMPASS_TICK_R);
    //     int16_t y1 = py(angle, COMPASS_TICK_R);
    //     int16_t x2 = px(angle, COMPASS_TICK_R - tick_len);
    //     int16_t y2 = py(angle, COMPASS_TICK_R - tick_len);
    //     u8g2_DrawLine(u8g2, x1, y1, x2, y2);
    // }

    /* ── 2. 刻度线（随圆盘旋转） ──
     * 每30°一根，共12根；
     * 主刻度（0/90/180/270°）长5px，其余长2px
     */
    for (int i = 0; i < 12; i++) {
        float angle = (float)(i * 30) - h;
            uint8_t tick_len;
            if      (i % 3 == 0) tick_len = 3;   // 主刻度（NESW位置）
            else                 tick_len = 1;   // 小刻度

        int16_t x1 = px(angle, COMPASS_TICK_R);
        int16_t y1 = py(angle, COMPASS_TICK_R);
        int16_t x2 = px(angle, COMPASS_TICK_R - tick_len);
        int16_t y2 = py(angle, COMPASS_TICK_R - tick_len);
        u8g2_DrawLine(u8g2, x1, y1, x2, y2);
    }

    /* ── 3. NESW 字符（随圆盘旋转） ──
     * 手动居中绘制
     */
    u8g2_SetFont(u8g2, u8g2_font_courB08_tf);
    const uint8_t FW = 6, FH = 8; // 字体宽高（用于居中偏移）

    typedef struct { const char *label; float base_angle; } CardinalDir;
    const CardinalDir dirs[4] = {
        { "N",   0.0f },
        { "E",  90.0f },
        { "S", 180.0f },
        { "W", 270.0f },
    };

    for (int i = 0; i < 4; i++) {
        float angle = dirs[i].base_angle - h;
        int16_t lx = px(angle, COMPASS_LABEL_R) - FW / 2;
        int16_t ly = py(angle, COMPASS_LABEL_R) - FH / 2;

        // /* N 字用高亮色（异或模式反显）突出显示 */
        // if (i == 0) {
        //     u8g2_SetDrawColor(u8g2, 2); // XOR，反色高亮
        //     u8g2_DrawRBox(u8g2, lx - 1, ly - 1, FW + 2, FH + 2, 2); // 背景框，稍微大于字体尺寸
        // }
        u8g2_SetDrawColor(u8g2, 1);
        u8g2_DrawStr(u8g2, lx, ly, dirs[i].label);
    }

    // /* ── 4. 固定北向三角指针（不随圆盘旋转） ──
    //  * 位于圆盘正上方外侧，始终固定
    //  */
    // u8g2_DrawTriangle(u8g2,
    //     COMPASS_CX,                   COMPASS_CY - COMPASS_R - 1,   // 顶点
    //     COMPASS_CX - 3,               COMPASS_CY - COMPASS_R + 5,   // 左底
    //     COMPASS_CX + 3,               COMPASS_CY - COMPASS_R + 5);  // 右底

    /* ── 5. 圆心小点 ── */
    // u8g2_DrawDisc(u8g2, COMPASS_CX, COMPASS_CY, 2, U8G2_DRAW_ALL);
    u8g2_DrawXBMP(u8g2, 24, 30, 5, 11, image_Compass_arrow_bits);
//文字信息
    /* ── 6. 右侧信息区：数字航向 + 方位名称 ──
     * 利用屏幕右半部分显示辅助信息
     */
    u8g2_SetFontPosBaseline(u8g2);
    u8g2_SetFont(u8g2, u8g2_font_6x12_tr);
    u8g2_DrawStr(u8g2, 61, 20, "Real North");
    // 三位数字航向 + 一位度数
    u8g2_SetFont(u8g2, u8g2_font_profont22_tf);
    sprintf(u8g2_buf,"%d%c", qmc6309_heading, 176);
    u8g2_DrawStr(u8g2, 67, 37, u8g2_buf);

    // 方位名称（N/NE/E/SE/S/SW/W/NW）
    u8g2_SetFont(u8g2, u8g2_font_6x12_tr);
    const char *dir_names[] = {
        "N","NE","E","SE","S","SW","W","NW"
    };
    int dir_idx = ((int)(qmc6309_heading + 22) / 45) % 8;
    u8g2_DrawStr(u8g2, 83, 47, dir_names[dir_idx]);
//校准按钮
    
}

/****************************************************************** */

void UI_OLED_display(u8g2_t *u8g2, int8_t ui_root)
{
    u8g2_ClearBuffer(u8g2);

    switch (ui_root)
    {
        case 0://家
            UI_HOME(u8g2);
            break;
        case 1://地球文本
            UI_Top_info(u8g2);
            UI_GPS_display_earth_txt(u8g2);
            break;
        case 2://地球缩略图
            UI_GPS_display_earth_image(u8g2);
            break;
        case 3://指南针
            UI_Top_info(u8g2);
            UI_Compass_display(u8g2, qmc6309.heading);
            break;
        default:
            break;
    }

//ui菜单切换弹窗边栏显示
    if(ui_switch_window_flag == 1)
    {
        UI_switch(u8g2, ui_root);
    }

    u8g2_SendBuffer(u8g2);


}
