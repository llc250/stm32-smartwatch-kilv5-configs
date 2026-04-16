#ifndef _UI_CONFIG_H
#define _UI_CONFIG_H 

#include "sys.h"

// 默认时间日期（仅作为初始值，运行时可修改）
#define DEFAULT_YEAR  2001
#define DEFAULT_MONTH 12
#define DEFAULT_DAY   31

#define TIME_SIZE     12
#define DATE_SIZE     33


// 显示坐标配置
#define Time_x     16
#define Time_y     40
#define Date_x     4
#define Date_y     0
#define Heart_x    55
#define Heart_y    85
#define Step_x     55
#define Step_y     100
#define Bt_Ui_x    122
#define Bt_Ui_y    0



// 月份天数表（平年）
const u8 Month_D[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};

// 全局时间日期变量（供外部修改）



#endif
