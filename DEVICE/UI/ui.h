#ifndef _UI_H
#define _UI_H 

#include "sys.h"

extern u16 Current_Year;
extern u8 Current_Month;
extern u8 Current_Day;
extern u8 Current_Hour;
extern u8 Current_Minute;
extern u8 Current_Second;

// 函数声明
void LCD_DrawBatteryIcon( u16 vbat, u8 mode); // 显示电量图标
u8 IsLeapYear(u16 year); // 闰年判断
u8 GetMonthDays(u16 year, u8 month); // 获取月份天数
void LCD_ShowHeartRate(u8 heart_rate, u16 fc, u16 bc, u8 sizey, u8 mode); // 显示心率
void LCD_ShowRateStep(u16 step, u16 fc, u16 bc, u8 sizey, u8 mode); // 显示步数
void Date_AddOneDay(void); // 日期加一天（供时间更新调用）
void Time_AddOneSecond(void);
void LCD_Clear(u16 color);
void LCD_ShowBT_UIPic( u16 fc, u16 bc);
void ShowBT_UI(u8 i);//蓝牙连接图标显示
void LCD_ShowDateTime(u16 fc, u16 bc, u8 date_sizey, u8 time_sizey, u8 mode);// 显示日期和时间

#endif
