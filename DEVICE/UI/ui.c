#include "ui.h"
#include "lcd.h"
#include "ui_config.h"


// 全局时间日期变量初始化
u16 Current_Year = DEFAULT_YEAR;
u8 Current_Month = DEFAULT_MONTH;
u8 Current_Day = DEFAULT_DAY;
u8 Current_Hour = 23;
u8 Current_Minute = 59;
u8 Current_Second = 55;

/******************************************************************************
  函数说明：显示电量图标
  入口数据：x,y      显示坐标
            vbat     电池电压/百分比（0~100）
            mode     显示模式：0-非叠加 1-叠加
  返回值：  无
******************************************************************************/
void LCD_DrawBatteryIcon( u16 vbat, u8 mode)
{
     u8 batt_data = vbat;

		LCD_DrawRectangle(140, 3, 154,13,BLACK);
		LCD_DrawRectangle(154, 5, 157,11,BLACK);    
    // 根据电量百分比选择对应点阵
    if (vbat >= 100)     LCD_Fill(142,5,153-(10-vbat/10),12,GREEN);
    else if (vbat >= 90) LCD_Fill(142,5,153-(10-vbat/10),12,GREEN);
    else if (vbat >= 80) LCD_Fill(142,5,153-(10-vbat/10),12,GREEN);
    else if (vbat >= 70) LCD_Fill(142,5,153-(10-vbat/10),12,GREEN);
    else if (vbat >= 60) LCD_Fill(142,5,153-(10-vbat/10),12,GREEN);
    else if (vbat >= 50) LCD_Fill(142,5,153-(10-vbat/10),12,GREEN);
    else if (vbat >= 40) LCD_Fill(142,5,153-(10-vbat/10),12,GREEN);
    else if (vbat >= 30) LCD_Fill(142,5,153-(10-vbat/10),12,GREEN);
    else if (vbat >= 20) LCD_Fill(142,5,153-(10-vbat/10),12,GREEN);
    else if (vbat >= 10) LCD_Fill(142,5,153-(10-vbat/10),12,RED);
    else if (vbat >= 5)  LCD_Fill(142,5,153-(10-vbat/10),12,RED);
    else  LCD_Fill(142,5,153-(10-vbat/10),12,RED);

    // 显示16x16的电量图标（点阵数据32字节=16*16/8）    
}

/******************************************************************************
  函数说明：判断公历年份是否为闰年
******************************************************************************/
u8 IsLeapYear(u16 year)
{
    if ((year % 400 == 0) || ((year % 4 == 0) && (year % 100 != 0)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

/******************************************************************************
  函数说明：时间加1秒（整合日期/月/年进位）
******************************************************************************/
void Time_AddOneSecond(void)
{
    u8 month_days;
    
    // 秒进位
    Current_Second++;
    if (Current_Second >= 60)
    {
        Current_Second = 0;
        Current_Minute++;
        
        // 分进位
        if (Current_Minute >= 60)
        {
            Current_Minute = 0;
            Current_Hour++;
            
            // 时进位 → 日期+1
            if (Current_Hour >= 24)
            {
                Current_Hour = 0;
                Current_Day++;
                
                // 日期进位
                month_days = Month_D[Current_Month];
                if (Current_Month == 2 && IsLeapYear(Current_Year))
                {
                    month_days = 29;
                }
                
                if (Current_Day > month_days)
                {
                    Current_Day = 1;
                    Current_Month++;
                    
                    // 月份进位
                    if (Current_Month > 12)
                    {
                        Current_Month = 1;
                        Current_Year++;
                    }
                }
            }
        }
    }
}

/******************************************************************************
  函数说明：显示日期+时间（合二为一，日期/时间支持不同字号）
  核心特性：
  1. 时间（HH:MM:SS）：手动补零，确保两位显示（0时→00、5分→05、9秒→09）
  2. 日期（YYYY-MM-DD）：固定两位补零显示
  3. 日期/时间支持不同字号
  入口数据：
  fc/bc         前景色/背景色
  date_sizey   日期显示字号
  time_sizey   时间显示字号
  mode         字符显示模式（传给LCD_ShowChar）
******************************************************************************/
void LCD_ShowDateTime(u16 fc, u16 bc, u8 date_sizey, u8 time_sizey, u8 mode)
{
    u16 x, y;
    u8 date_sizex = date_sizey / 2; // 日期字符宽度
    u8 time_sizex = time_sizey / 2; // 时间字符宽度
    
    // ===================== 显示日期（YYYY-MM-DD）=====================
    x = Date_x;
    y = Date_y;
    
    // 1. 显示4位年份（手动拆分，确保4位）
    LCD_ShowChar(x, y, (Current_Year / 1000) + '0', fc, bc, date_sizey, mode);
    x += date_sizex;
    LCD_ShowChar(x, y, ((Current_Year % 1000) / 100) + '0', fc, bc, date_sizey, mode);
    x += date_sizex;
    LCD_ShowChar(x, y, ((Current_Year % 100) / 10) + '0', fc, bc, date_sizey, mode);
    x += date_sizex;
    LCD_ShowChar(x, y, (Current_Year % 10) + '0', fc, bc, date_sizey, mode);
    x += date_sizex;
    
    // 2. 显示分隔符 "-"
    LCD_ShowChar(x, y, '-', fc, bc, date_sizey, mode);
    x += date_sizex;
    
    // 3. 显示2位月份（手动补零）
    if (Current_Month < 10)
    {
        LCD_ShowChar(x, y, '0', fc, bc, date_sizey, mode); // 补零
        x += date_sizex;
        LCD_ShowChar(x, y, Current_Month + '0', fc, bc, date_sizey, mode);
    }
    else
    {
        LCD_ShowChar(x, y, (Current_Month / 10) + '0', fc, bc, date_sizey, mode);
        x += date_sizex;
        LCD_ShowChar(x, y, (Current_Month % 10) + '0', fc, bc, date_sizey, mode);
    }
    x += date_sizex;
    
    // 4. 显示分隔符 "-"
    LCD_ShowChar(x, y, '-', fc, bc, date_sizey, mode);
    x += date_sizex;
    
    // 5. 显示2位日期（手动补零）
    if (Current_Day < 10)
    {
        LCD_ShowChar(x, y, '0', fc, bc, date_sizey, mode); // 补零
        x += date_sizex;
        LCD_ShowChar(x, y, Current_Day + '0', fc, bc, date_sizey, mode);
    }
    else
    {
        LCD_ShowChar(x, y, (Current_Day / 10) + '0', fc, bc, date_sizey, mode);
        x += date_sizex;
        LCD_ShowChar(x, y, (Current_Day % 10) + '0', fc, bc, date_sizey, mode);
    }
    
    // ===================== 显示时间（HH:MM:SS，手动强制补零）=====================
    x = Time_x;
    y = Time_y;
    
    // 1. 显示2位小时（手动补零：0时→00、8时→08）
    if (Current_Hour < 10)
    {
        LCD_ShowChar(x, y, '0', HATSUNE_BLUE, bc, time_sizey, mode); // 高位补零
        x += time_sizex;
        LCD_ShowChar(x, y, Current_Hour + '0', HATSUNE_BLUE, bc, time_sizey, mode);
    }
    else
    {
        LCD_ShowChar(x, y, (Current_Hour / 10) + '0', HATSUNE_BLUE, bc, time_sizey, mode);
        x += time_sizex;
        LCD_ShowChar(x, y, (Current_Hour % 10) + '0', HATSUNE_BLUE, bc, time_sizey, mode);
    }
    x += time_sizex;
    
    // 2. 显示分隔符 ":"
    LCD_ShowChar(x, y, ':', HATSUNE_BLUE, bc, time_sizey, mode);
    x += time_sizex;
    
    // 3. 显示2位分钟（手动补零：5分→05）
    if (Current_Minute < 10)
    {
        LCD_ShowChar(x, y, '0', HATSUNE_BLUE, bc, time_sizey, mode); // 高位补零
        x += time_sizex;
        LCD_ShowChar(x, y, Current_Minute + '0', HATSUNE_BLUE, bc, time_sizey, mode);
    }
    else
    {
        LCD_ShowChar(x, y, (Current_Minute / 10) + '0', HATSUNE_BLUE, bc, time_sizey, mode);
        x += time_sizex;
        LCD_ShowChar(x, y, (Current_Minute % 10) + '0', HATSUNE_BLUE, bc, time_sizey, mode);
    }
    x += time_sizex;
    
    // 4. 显示分隔符 ":"
    LCD_ShowChar(x, y, ':', HATSUNE_BLUE, bc, time_sizey, mode);
    x += time_sizex;
    
    // 5. 显示2位秒数（手动补零：9秒→09）
    if (Current_Second < 10)
    {
        LCD_ShowChar(x, y, '0', HATSUNE_BLUE, bc, time_sizey, mode); // 高位补零
        x += time_sizex;
        LCD_ShowChar(x, y, Current_Second + '0', HATSUNE_BLUE, bc, time_sizey, mode);
    }
    else
    {
        LCD_ShowChar(x, y, (Current_Second / 10) + '0', HATSUNE_BLUE, bc, time_sizey, mode);
        x += time_sizex;
        LCD_ShowChar(x, y, (Current_Second % 10) + '0', HATSUNE_BLUE, bc, time_sizey, mode);
    }
}

/******************************************************************************
  函数说明：显示心率（0~999，2位显示2位、3位显示3位，无前导零）
  入口数据：heart_rate  心率（0~999）
            fc/bc       前景色/背景色
            sizey       字号
            mode        显示模式
******************************************************************************/
void LCD_ShowHeartRate(u8 heart_rate, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    // 边界防护：限制心率范围（0~999，超出则显示999）
    if (heart_rate > 255) heart_rate = 255;
    
    u8 x = Heart_x;
    u8 y = Heart_y;
    u8 sizex = sizey / 2;

    // 1. 显示固定前缀 "心率："
    LCD_ShowChinese(x, y, (u8*)"心", fc, bc, sizey, mode);
    x += sizex * 2;  // 汉字占2个字符宽度
    LCD_ShowChinese(x, y, (u8*)"率", fc, bc, sizey, mode);
    x += sizex * 2;
    LCD_ShowChar(x, y, ':', fc, bc, sizey, mode);
    x += sizex;

    // 2. 拆分心率数字，统计有效位数（几位就显示几位）
    u8 digit_h = heart_rate / 100;    // 百位（3位时显示）
    u8 digit_t = (heart_rate % 100) / 10; // 十位（2/3位时显示）
    u8 digit_o = heart_rate % 10;     // 个位（始终显示）

    // 3. 百位：仅3位时显示（digit_h≠0）
    if (digit_h != 0)
    {
        LCD_ShowChar(x, y, digit_h + '0', fc, bc, sizey, mode);
        x += sizex;
    }

    // 4. 十位：2/3位时显示（百位非0 或 十位非0）
    if (digit_h != 0 || digit_t != 0)
    {
        LCD_ShowChar(x, y, digit_t + '0', fc, bc, sizey, mode);
        x += sizex;
    }

    // 5. 个位：始终显示（0~999都显示个位）
    LCD_ShowChar(x, y, digit_o + '0', fc, bc, sizey, mode);
}

/******************************************************************************
  函数说明：显示步数（任意位数，有几位显示几位，无数值上限、无前导零）
  入口数据：step     步数（无上限，支持0~任意正整数）
            fc/bc    前景色/背景色
            sizey    字号
            mode     显示模式
******************************************************************************/
void LCD_ShowRateStep(u16 step, u16 fc, u16 bc, u8 sizey, u8 mode)
{
    u8 sizex = sizey / 2;
    u8 curr_x = Step_x;
    u8 y = Step_y;    
    u8 digit_buf[20] = {0}; // 存储拆分后的数字（支持最多20位，足够覆盖所有场景）
    u8 digit_cnt = 0;       // 实际有效数字位数
    u32 temp_step = step;   // 临时变量，用于拆分数字（u32支持0~4294967295）

    // 1. 显示固定前缀 "步数："
    LCD_ShowChinese(curr_x, y, (u8*)"步", fc, bc, sizey, mode);
    curr_x += sizex * 2;  // 汉字占2个字符宽度
    LCD_ShowChinese(curr_x, y, (u8*)"数", fc, bc, sizey, mode);
    curr_x += sizex * 2;
    LCD_ShowChar(curr_x, y, ':', fc, bc, sizey, mode);
    curr_x += sizex;

    // 2. 拆分数字：从个位到高位，存入digit_buf（逆序）
    if (temp_step == 0)
    {
        digit_buf[0] = 0;  // 步数为0时，直接存0
        digit_cnt = 1;     // 0算1位
    }
    else
    {
        // 循环拆分：直到数字为0，统计有效位数
        while (temp_step > 0)
        {
            digit_buf[digit_cnt++] = temp_step % 10; // 取当前个位数字
            temp_step /= 10;                         // 去掉已拆分的个位
        }
    }

    // 3. 显示数字：从高位到低位（反转digit_buf），有几位显几位
    for (s8 i = digit_cnt - 1; i >= 0; i--)
    {
        LCD_ShowChar(curr_x, y, digit_buf[i] + '0', fc, bc, sizey, mode);
        curr_x += sizex; // 每显示一位，X坐标右移对应宽度
    }
}

void LCD_Clear(u16 color)
{
    // 填充整个屏幕区域（以1.8寸ST7735屏为例，分辨率128*160）
    // LCD_Fill(起始X, 起始Y, 结束X, 结束Y, 填充颜色)
    LCD_Fill(0, 0, 127, 159, color); 
}


u8 BT_UI[32] = 
{0x00,0x00,0x00,0x00,0x00,
 0x00,0x01,0x00,0x03,0x00,
 0x05,0x00,0x09,0x20,0x05,
 0x40,0x03,0x80,0x03,0x80,
 0x05,0x40,0x09,0x20,0x05,
 0x00,0x03,0x00,0x01,0x00,
 0x00,0x00,/*"未命名文件",0*/	
};
/**
 * @brief  显示由BT_UI数组定义的位图图片（左右翻转版）
 * @param  x: 图片左上角X坐标
 * @param  y: 图片左上角Y坐标
 * @param  fc: 前景色（像素点亮的颜色，如0xF800红色）
 * @param  bc: 背景色（像素点灭的颜色，如0xFFFF白色）
 * @note   核心修改：列索引从右到左解析，实现图片左右翻转
 */
void LCD_ShowBT_UIPic( u16 fc, u16 bc)
{
    u16 pic_width = 16;    // 图片宽度（16像素）
    u16 pic_height = 16;   // 图片高度（16像素）
    u8 byte_index = 0;     // BT_UI数组字节索引
    u8 bit_index = 0;      // 字节内的位索引
    u16 col_reverse = 0;   // 翻转后的列索引

    // 逐行绘制图片
    for(u16 row = 0; row < pic_height; row++)
    {
        // 逐列绘制图片（列索引从右到左，实现左右翻转）
        for(u16 col = 0; col < pic_width; col++)
        {
            // 核心修改：计算翻转后的列索引
            col_reverse = pic_width - 1 - col; 

            // 计算当前像素对应的位（用翻转后的列索引）
            byte_index = (row * pic_width + col_reverse) / 8;  
            bit_index = 7 - ((row * pic_width + col_reverse) % 8); 

            // 超出数组范围则停止（防止越界）
            if(byte_index >= sizeof(BT_UI)) break;

            // 判断当前位是1（前景色）还是0（背景色），绘制像素点
            if(BT_UI[byte_index] & (1 << bit_index))
            {
                LCD_DrawPoint(Bt_Ui_x + col, Bt_Ui_y + row, fc); // 绘制前景色像素
            }
            else
            {
                LCD_DrawPoint(Bt_Ui_x + col, Bt_Ui_y + row, bc); // 绘制背景色像素
            }
        }
    }
}

void ShowBT_UI(u8 i)
{
	if(i)
	{
	LCD_ShowBT_UIPic( BLUE, WHITE)	;	
	}
	else
	{
	LCD_ShowBT_UIPic( LGRAY, WHITE);	
	}
}