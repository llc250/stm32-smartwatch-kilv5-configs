#include "stm32f10x.h"
#include "bluetooth.h"
#include "stdio.h"
#include "delay.h"

// 全局变量扩展
u8 Recv_Flag = 0;    
u8 Recv_Hex_Data = 0;
u8 BT_Recv_Buf[8] = {0};  // 接收缓存（6位核心数据+2位CRC，共8字节）
u8 BT_Recv_Cnt = 0;       // 接收字节计数


u16 CRC16_C(u8 *buf, u8 length)
{
    u16 crc_result = 0xFFFF;  // 改为u16（原u32浪费2字节）
    u8 i;
    
    while(length--)
    {
        crc_result ^= *(buf++);  // 与当前字节异或
        for(i = 0; i < 8; i++)   // 逐位计算
        {
            if(crc_result & 0x01)
                crc_result = (crc_result >> 1) ^ 0xA001;
            else
                crc_result = crc_result >> 1;
        }
    }
    return crc_result;
}

/**
 * @brief  BT 初始化配置函数（适配PA2/PA3）
 */
void BT_Init(void)
{ 
    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;

    // 1. 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    // 2. 复位USART2
    USART_DeInit(USART2);

    // 3. 配置GPIO引脚
    GPIO_InitStructure.GPIO_Pin = BT_TX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(BT_TX_GPIO_PORT, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = BT_RX_GPIO_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(BT_RX_GPIO_PORT, &GPIO_InitStructure);

    // 4. 配置USART2参数
    USART_InitStructure.USART_BaudRate = BT_BaudRate;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);

    // 5. 配置中断
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

    // 6. 配置NVIC（适配FreeRTOS的NVIC_PriorityGroup_4）
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 5;  // 抢占优先级5（Group4）
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;         // 子优先级0
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    // 7. 使能USART2
    USART_Cmd(USART2, ENABLE);
}

/**
 * @brief  蓝牙发送单个字节（带超时保护）
 */
void USART2_SendByte(u8 ch)
{
    uint32_t timeout = 100000; // 超时保护，避免卡死
    while((USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET) && timeout--)
    {
        if(timeout == 0) return;
    }
    USART_SendData(USART2, ch);
}

/**
 * @brief  蓝牙发送字符串
 */
void USART2_SendString(char *pStr)
{
    if(pStr == NULL) return;
    while(*pStr != '\0')
    {
        USART2_SendByte(*pStr);
        pStr++;
    }
}

/**
 * @brief  USART2中断服务函数
 */
// 修改USART2中断服务函数（缓存接收数据，保留不变）
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
    {
        Recv_Hex_Data = USART_ReceiveData(USART2);
        
        // 缓存接收数据，最多8字节
        if(BT_Recv_Cnt < 8)
        {
            BT_Recv_Buf[BT_Recv_Cnt++] = Recv_Hex_Data;
        }
        
        Recv_Flag = 1;
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}
/**
 * @brief  重定向printf到USART2
 */
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif
PUTCHAR_PROTOTYPE
{
    USART2_SendByte((uint8_t)ch);
    return ch;
}

/**
 * @brief  发送16位16进制数据
 */
void Bluetooth_Send_Hex16(u16 data)
{
    USART2_SendByte(data & 0xFF);
}

/**
 * @brief  蓝牙发送多字节数据（含CRC）
 */
void BT_SendMultiByte(u8 *pData, u16 len, u8 crc_mode)
{
   if(pData == NULL || len == 0 || len > 254) return;
    u8 Data_C[256];
    u16 result = 0;

    // 复制数据到缓冲区
    for(u16 i=0; i<len; i++)
    {
        Data_C[i] = pData[i];
    }

    // 计算CRC
    if(crc_mode == 1)
    {
//        result = CRC16_S(pData, len);
    }
    else
    {
        result = CRC16_C(pData, len);
    }
    Data_C[len] = result & 0xFF;
    Data_C[len+1] = (result >> 8) & 0xFF;

    // 发送数据+CRC
    for(u16 i=0; i<(len + 2); i++)
    {
        USART2_SendByte(Data_C[i]);
//        delay_us(500);
    }
}

/**
 * @brief  发送换行符
 */
void USART2_SendNewLine(void)
{
    USART2_SendByte(0x0D);
    USART2_SendByte(0x0A);
}

/**
 * @brief  发送字符串+换行
 */
void USART2_SendStringWithNewLine(char *pStr)
{
    USART2_SendString(pStr);
    USART2_SendNewLine();
}

// 蓝牙接收数据处理函数（新增断连指令校验）
void BT_Recv_Data_Process(void)
{
	;
}



// 其他原有函数（BT_Init/USART2_SendByte等）保持不变
