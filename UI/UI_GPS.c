#include "UI_GPS.h"

void UI_GPS_display_earth(void)
{
    if (gps.lwgps_handle.is_valid) //lwgps_is_valid()
	{
		if(earth_flag)//以文本形式显示实时坐标
		{
			u8g2_SetFont(&u8g2,u8g2_font_courB08_tr);  //w=7  h=10
			u8g2_SetFontPosTop(&u8g2);
			u8g2_SetFontMode(&u8g2,0);  //显示字体的背景，不透明
			u8g2_SetDrawColor(&u8g2,1);
			// 可选：解析结果输出
    		// if (lwgps_handle.is_valid) {
    		    // printf("Lat: %.6f, Lon: %.6f, Alt: %.4f\r\n",
    		    //     latitude, longitude, altitude);
    		// }
			u8g2_ClearBuffer(&u8g2);
        //显示纬度
			// sprintf(u8g2_buf,"[>Lon:%03.7f<]",lwgps_handle.latitude);
			char     var_sign  = (gps.lwgps_handle.latitude >= 0.0f) ? '+' : '-';
			double   abs_var   = fabs(gps.lwgps_handle.latitude);
			uint32_t temp      = abs_var * 1000000.0f + 0.5f;                      //四舍五入保留小数点后6位，可精确到0.11米
			uint16_t int_part  = temp / 1000000;                                   //整数部分
			uint32_t frac_part = temp % 1000000;                                   //小数部分
			memset(u8g2_buf, 0, sizeof(u8g2_buf));
			sprintf(u8g2_buf,"[Lat:%c%u.%06lu]",var_sign, int_part, frac_part);
			u8g2_SetDrawColor(&u8g2,0);
			u8g2_DrawBox(&u8g2,0*7,0*10,18*7,10);
			u8g2_SetDrawColor(&u8g2,1);
			u8g2_DrawStr(&u8g2,0*7,0*10,u8g2_buf);
        //显示经度
			// sprintf(u8g2_buf,"[>Lon:%03.7f<]",lwgps_handle.longitude);
			var_sign  = (gps.lwgps_handle.longitude >= 0.0f) ? '+' : '-';
			abs_var   = fabs(gps.lwgps_handle.longitude);
			temp      = abs_var * 1000000.0f + 0.5f;                      //四舍五入保留小数点后6位，可精确到0.11米
			int_part  = temp / 1000000;                                   //整数部分
			frac_part = temp % 1000000;                                   //小数部分
			memset(u8g2_buf, 0, sizeof(u8g2_buf));
			sprintf(u8g2_buf,"[Lon:%c%u.%06lu]",var_sign, int_part, frac_part);
			u8g2_SetDrawColor(&u8g2,0);
			u8g2_DrawBox(&u8g2,0*7,1*10,18*7,10);
			u8g2_SetDrawColor(&u8g2,1);
			u8g2_DrawStr(&u8g2,0*7,1*10,u8g2_buf);
        //显示海拔高度
			// sprintf(u8g2_buf,"[>Alt:%03.5f<]", lwgps_handle.altitude);
			var_sign  = (gps.lwgps_handle.altitude >= 0.0f) ? '+' : '-';
			abs_var   = fabs(gps.lwgps_handle.altitude);
			temp      = abs_var * 100.0f + 0.5f;                      //四舍五入保留小数点后2位，可精确到厘米
			int_part  = temp / 100;                                   //整数部分
			frac_part = temp % 100;                                   //小数部分
			memset(u8g2_buf, 0, sizeof(u8g2_buf));
			sprintf(u8g2_buf,"[Alt:%c%u.%02lu]",var_sign, int_part, frac_part);
			u8g2_SetDrawColor(&u8g2,0);
			u8g2_DrawBox(&u8g2,0*7,2*10,18*7,10);
			u8g2_SetDrawColor(&u8g2,1);
			u8g2_DrawStr(&u8g2,0*7,2*10,u8g2_buf);
        //显示磁偏角
		    //得到当前日期对应的WMM模型参数，计算磁偏角
			float Date_WMM = wmm_get_date(gps.lwgps_handle.year % 100, gps.lwgps_handle.month, gps.lwgps_handle.date);
			float Magnetic_variation;
			E0000(gps.lwgps_handle.latitude, gps.lwgps_handle.longitude, Date_WMM, &Magnetic_variation);//磁偏角结果是正值表示东偏，负值表示西偏，单位是度；输出浮点数
			qmc6309.Magnetic_variation = Magnetic_variation;//将磁偏角传给qmc6309实例使用；用于使航向校正为地理正北；
		    //显示磁偏角，四舍五入保留三位小数，检验能否正常将-0.9567度显示为-0.957度
			var_sign  = (Magnetic_variation >= 0.0f) ? '+' : '-';  //符号部分，正值表示东偏，负值表示西偏
			abs_var   = fabsf(Magnetic_variation);                 //计算float类型的绝对值
			temp      = abs_var * 1000.0f + 0.5f;                  //保留小数点后三位，四舍五入；单位是0.001度；float强制转int时向下截断如： 1.999会变成1，所以加0.5实现四舍五入
			int_part  = temp / 1000;                               //整数部分
			frac_part = temp % 1000;                               //小数部分
			memset(u8g2_buf, 0, sizeof(u8g2_buf));
			sprintf(u8g2_buf,"[Mag:%c%u.%03lu]",var_sign, int_part, frac_part);
			u8g2_SetDrawColor(&u8g2,0);
			u8g2_DrawBox(&u8g2,0*7,3*10,18*7,10);
			u8g2_SetDrawColor(&u8g2,1);
			u8g2_DrawStr(&u8g2,0*7,3*10,u8g2_buf);
			// u8g2_SendBuffer(&u8g2);
		}
		else{//全球缩略图
			u8g2_ClearBuffer(&u8g2);
			u8g2_oled_draw_earth(&u8g2);//在全幅缓冲区内绘制全球缩略图
			u8g2_oled_draw_earth_pixel_VHxvLine(&u8g2,gps.lwgps_handle.latitude,gps.lwgps_handle.longitude);//在全球缩略图上绘制实时经纬度坐标点
			// u8g2_SendBuffer(&u8g2);
		}
			
	}
	else
	{
		// u8g2_ClearBuffer(&u8g2);
		memset(u8g2_buf, 0, sizeof(u8g2_buf));
		sprintf(u8g2_buf,"[>GPS No Data<]");
		u8g2_SetDrawColor(&u8g2,1);
		u8g2_DrawBox(&u8g2,3*7,27,13*7,15);
		u8g2_SetDrawColor(&u8g2,0);
		u8g2_DrawStr(&u8g2,3*7,3*10,u8g2_buf);
		// u8g2_SendBuffer(&u8g2);
	}
}