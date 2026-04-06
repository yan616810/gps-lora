#include "UI.h"

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
/*qmc6309*/
char     var_sign_MV  = '+';
uint16_t int_part_MV  = 0;
uint16_t frac_part_MV = 0;



/*GPS数据+全球缩略图OLED界面所需的数据处理函数*/
void UI_GPS_display_earth_data_proc(void)
{
    if(gps.lwgps_handle.is_valid && gps.lwgps_handle.fix)//报文有效且已解算出定位
	{
        //如果gps.lwgps_handle.seconds发生变化了说明GPS时间更新了，才进行一次坐标转换和显示更新；如果GPS时间没有更新，就不进行坐标转换和显示更新；这样可以避免在GPS时间没有更新时，频繁地进行坐标转换和显示更新，节省CPU资源和电量；因为坐标转换和显示更新是比较耗时的操作，不需要每次都进行；
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
	    	//得到当前日期对应的WMM模型参数，计算磁偏角
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

/*GPS无信号时的弹窗*/
void UI_GPS_display_earth_no_data(u8g2_t *u8g2)
{
    u8g2_SetFont(u8g2,u8g2_font_courB08_tr);  //w=7  h=10
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
        u8g2_SetDrawColor(u8g2,1);
        u8g2_DrawStr(u8g2,0*7,0*10+13,u8g2_buf);
        //显示经度
        sprintf(u8g2_buf,"[Lon:%c%u.%06lu%c]",var_sign_lon, int_part_lon, frac_part_lon, 176);
        u8g2_SetDrawColor(u8g2,1);
        u8g2_DrawStr(u8g2,0*7,1*10+13,u8g2_buf);
        //显示海拔高度
        sprintf(u8g2_buf,"[Alt:%c%u.%02um]",var_sign_alt, int_part_alt, frac_part_alt);
        u8g2_SetDrawColor(u8g2,1);
        u8g2_DrawStr(u8g2,0*7,2*10+13,u8g2_buf);
        //显示磁偏角
        sprintf(u8g2_buf,"[Mag:%c%u.%03u%c]",var_sign_MV, int_part_MV, frac_part_MV, 176);
        u8g2_SetDrawColor(u8g2,1);
        u8g2_DrawStr(u8g2,0*7,3*10+13,u8g2_buf);
        //bmp280推算出的相对高度，四舍五入保留小数点后1位，单位是米
        sprintf(u8g2_buf,"[R-H:%c%u.%01um]", altitude_sign, altitude_int_part, altitude_frac_part);//相对高度，最大相对高度65535m
        u8g2_SetDrawColor(u8g2,1);
        u8g2_DrawStr(u8g2,0*7,4*10+13,u8g2_buf);
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
    u8g2_DrawXBMP(u8g2, 125, 30, 3, 5, image_ButtonRightSmall_bits);
}
/*GPS全球缩略图模式*/
void UI_GPS_display_earth_image(u8g2_t *u8g2)
{
    u8g2_SetFont(u8g2,u8g2_font_courB08_tr);  //w=7  h=10
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
    u8g2_DrawXBMP(u8g2, 0, 30, 3, 5, image_ButtonLeftSmall_bits);
}
// void UI_GPS_display_earth(u8g2_t *u8g2, uint8_t earth_flag)//earth_flag: 1表示以文本形式显示实时坐标，0表示以全球缩略图的形式显示实时坐标
// {
//     // u8g2_ClearBuffer(u8g2);
//     u8g2_SetFont(u8g2,u8g2_font_courB08_tr);  //w=7  h=10
// 	u8g2_SetFontPosTop(u8g2);
// 	u8g2_SetFontMode(u8g2,0);  //显示字体的背景，不透明
//     u8g2_SetDrawColor(u8g2,1);
//     if (gps.lwgps_handle.is_valid && gps.lwgps_handle.fix) //lwgps_is_valid()
// 	{
// 		if(earth_flag)//以文本形式显示实时坐标
// 		{
//         //显示纬度
// 			sprintf(u8g2_buf,"[Lat:%c%u.%06lu]",var_sign_lat, int_part_lat, frac_part_lat);
// 			u8g2_SetDrawColor(u8g2,1);
// 			u8g2_DrawStr(u8g2,0*7,0*10,u8g2_buf);
//         //显示经度
// 			sprintf(u8g2_buf,"[Lon:%c%u.%06lu]",var_sign_lon, int_part_lon, frac_part_lon);
// 			u8g2_SetDrawColor(u8g2,1);
// 			u8g2_DrawStr(u8g2,0*7,1*10,u8g2_buf);
//         //显示海拔高度
// 			sprintf(u8g2_buf,"[Alt:%c%u.%02u]",var_sign_alt, int_part_alt, frac_part_alt);
// 			u8g2_SetDrawColor(u8g2,1);
// 			u8g2_DrawStr(u8g2,0*7,2*10,u8g2_buf);
//         //显示磁偏角
// 			sprintf(u8g2_buf,"[Mag:%c%u.%03u]",var_sign_MV, int_part_MV, frac_part_MV);
// 			u8g2_SetDrawColor(u8g2,1);
// 			u8g2_DrawStr(u8g2,0*7,3*10,u8g2_buf);
//         //bmp280推算出的相对高度，四舍五入保留小数点后1位，单位是米
//             sprintf(u8g2_buf,"[R-H:%c%u.%01um]", altitude_sign, altitude_int_part, altitude_frac_part);//相对高度，最大相对高度65535m
// 			u8g2_SetDrawColor(u8g2,1);
// 			u8g2_DrawStr(u8g2,0*7,4*10,u8g2_buf);
//         }
// 		else{//全球缩略图
// 			u8g2_oled_draw_earth(u8g2);//在全幅缓冲区内绘制全球缩略图
// 			u8g2_oled_draw_earth_pixel_VHxvLine(u8g2,gps.lwgps_handle.latitude,gps.lwgps_handle.longitude);//在全球缩略图上绘制实时经纬度坐标点
// 		}
// 	}
// 	else//GPS无效或无定位
// 	{
//         if(earth_flag)//以文本形式显示实时坐标
//         {
//             u8g2_DrawStr(u8g2,0*7,0*10,"[Lat:No Data]");
//             u8g2_DrawStr(u8g2,0*7,1*10,"[Lon:No Data]");
//             u8g2_DrawStr(u8g2,0*7,2*10,"[Alt:No Data]");
//             u8g2_DrawStr(u8g2,0*7,3*10,"[Mag:No Data]");
//             u8g2_DrawStr(u8g2,0*7,4*10,"[R-H:No Data]");
//         }
//         else{
//             u8g2_oled_draw_earth(u8g2);//在全幅缓冲区内绘制全球缩略图
//         }

// 		u8g2_SetDrawColor(u8g2,1);
// 		u8g2_DrawBox(u8g2,3*7,27,13*7,15);
// 		u8g2_SetDrawColor(u8g2,0);
// 		u8g2_DrawStr(u8g2,3*7,3*10,"[>GPS No Data<]");
//     }
// }



uint16_t LoRa_num=16;
uint8_t is_charge=0;
uint8_t Power=50;//电量剩余26%

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
    // //卫星数
    // sprintf(u8g2_buf,"%-3d",gps.lwgps_handle.sats_in_view_total);
    // u8g2_DrawStr(u8g2, 21, 9, u8g2_buf);
    // //LoRa数
    // sprintf(u8g2_buf,"%-3d",LoRa_num);
    // u8g2_DrawStr(u8g2, 66, 9, u8g2_buf);
    // //电量数
    // sprintf(u8g2_buf,"%-3d",Power);
    // u8g2_DrawStr(u8g2, 107, 9, u8g2_buf);
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
        case 1 || 2://地球文本或地球缩略图
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
