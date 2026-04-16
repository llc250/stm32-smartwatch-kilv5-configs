#ifndef __BLUETOOTH_H
#define __BLUETOOTH_H
#include "sys.h"  // 确保u8/u16类型定义

// ------------------ USART2引脚定义（PA2/PA3） ------------------
#define BT_TX_GPIO_PORT        GPIOA   // TX引脚端口（PA2）
#define BT_TX_GPIO_PIN         GPIO_Pin_2
#define BT_RX_GPIO_PORT        GPIOA   // RX引脚端口（PA3）
#define BT_RX_GPIO_PIN         GPIO_Pin_3

#define BT_BaudRate   9600  // 蓝牙波特率（匹配手机APP）

// 全局变量声明（需在main.c中定义）
extern u8 Recv_Flag;
extern u8 Recv_Hex_Data;
extern u8 BT_Connect_Status; // 蓝牙连接状态：0=未连接(灰)，1=连接成功(蓝)

// 函数声明
void BT_Init(void);
void USART2_SendByte(u8 ch);
void USART2_SendString(char *pStr);
void Bluetooth_Send_Hex16(u16 data);
void BT_SendMultiByte(u8 *pData, u16 len, u8 crc_mode);//蓝牙发送多字节数据
void USART2_SendNewLine(void);
void USART2_SendStringWithNewLine(char *pStr);
void BT_Recv_Data_Process(void);
	
#endif
