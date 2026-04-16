#include "adxl345.h"
#include "delay.h"
#include "math.h"
#include "i2c.h"      // 添加统一 I2C 头文件
#include "FreeRTOS.h"

// 删除原来的 ADXL345_IIC_SDA_OUT、ADXL345_IIC_SDA_IN、ADXL345_IIC_Init 等函数
// 改用 IIC 驱动中的函数

// 传感器初始化
void ADXL345_Init()
{
    IIC_Init();                     // 初始化 I2C 总线（PB6/PB7）

    adxl345_write_reg(0X31, 0X0B);  // 全分辨率 ±16g
    adxl345_write_reg(0x2C, 0x0B);  // 100Hz
    adxl345_write_reg(0x2D, 0x08);  // 测量模式
    adxl345_write_reg(0X2E, 0x80);  // 禁用中断
    adxl345_write_reg(0X1E, 0x00);
    adxl345_write_reg(0X1F, 0x00);
    adxl345_write_reg(0X20, 0x05);
}

// 写寄存器
void adxl345_write_reg(u8 addr, u8 val)
{
    IIC_Start();
    IIC_Send_Byte(slaveaddress);    // 写地址
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);            // 寄存器地址
    IIC_Wait_Ack();
    IIC_Send_Byte(val);             // 数据
    IIC_Wait_Ack();
    IIC_Stop();
}

// 读寄存器
u8 adxl345_read_reg(u8 addr)
{
    u8 temp = 0;
    IIC_Start();
    IIC_Send_Byte(slaveaddress);    // 写地址
    IIC_Wait_Ack();
    IIC_Send_Byte(addr);            // 寄存器地址
    IIC_Wait_Ack();

    IIC_Start();                    // 重启总线
    IIC_Send_Byte(regaddress);      // 读地址
    IIC_Wait_Ack();
    temp = IIC_Read_Byte(0);        // 读一个字节，不发送应答
    IIC_Stop();
    return temp;
}

// 读取加速度数据（6个字节）
void adxl345_read_data(short *x, short *y, short *z)
{
    u8 buf[6];
    u8 i;

    IIC_Start();
    IIC_Send_Byte(slaveaddress);    // 写地址
    IIC_Wait_Ack();
    IIC_Send_Byte(0x32);            // 数据起始地址
    IIC_Wait_Ack();

    IIC_Start();                    // 重启
    IIC_Send_Byte(regaddress);      // 读地址
    IIC_Wait_Ack();

    for (i = 0; i < 6; i++) {
        if (i == 5)
            buf[i] = IIC_Read_Byte(0);  // 最后一个字节不发送应答
        else
            buf[i] = IIC_Read_Byte(1);  // 其他字节发送应答
    }
    IIC_Stop();

    *x = (short)(((u16)buf[1] << 8) + buf[0]);
    *y = (short)(((u16)buf[3] << 8) + buf[2]);
    *z = (short)(((u16)buf[5] << 8) + buf[4]);
}

// 连续读取多次取平均值函数
// times 取平均值的次数
void adxl345_read_average(float *x, float *y, float *z, u8 times)
{
    u8 i;
    short tx, ty, tz;
    *x = 0;
    *y = 0;
    *z = 0;
    if (times)
    {
        for (i = 0; i < times; i++)
        {
            adxl345_read_data(&tx, &ty, &tz);
            *x += tx;
            *y += ty;
            *z += tz;
//            delay_ms(5); // 让系统切换任务，灯才会闪，时钟才会走！

        }
        *x /= times;
        *y /= times;
        *z /= times;
    }
}

//void get_angle(float *x_angle, float *y_angle, float *z_angle)
//{
//    float ax, ay, az;
//    adxl345_read_average(&ax, &ay, &az, 10);

//    // X轴与水平面的夹角（-90° ~ 90°）
//    *x_angle = atan2(ax, sqrt(ay*ay + az*az)) * 180 / 3.1415926f;
//    // Y轴与水平面的夹角
//    *y_angle = atan2(ay, sqrt(ax*ax + az*az)) * 180 / 3.1415926f;
//    // Z轴与竖直方向的夹角（0° 表示竖直向上，90° 表示水平）
//    *z_angle = atan2(sqrt(ax*ax + ay*ay), az) * 180 / 3.1415926f;

//    // 可选：将 Z 角限制在 [0, 90] 范围内
//    if (*z_angle < 0) *z_angle = -(*z_angle);
//}


void get_angle(float *x_angle, float *y_angle, float *z_angle)
{
    short ax_raw, ay_raw, az_raw;
    adxl345_read_data(&ax_raw, &ay_raw, &az_raw);

    float ax = (float)ax_raw;
    float ay = (float)ay_raw;
    float az = (float)az_raw;

    *x_angle = atan2(ax, sqrt(ay*ay + az*az)) * 180 / 3.1415926f;
    *y_angle = atan2(ay, sqrt(ax*ax + az*az)) * 180 / 3.1415926f;
    *z_angle = atan2(sqrt(ax*ax + ay*ay), az) * 180 / 3.1415926f;
    if (*z_angle < 0) *z_angle = -(*z_angle);
}