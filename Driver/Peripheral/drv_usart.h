/**
  ******************************************************************************
  * @file    drv_usart.h
  * @brief   USART外设驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - 支持 USART1 / USART2 / USART3
  *   - 异步模式初始化（波特率、数据位、停止位、校验）
  *   - 阻塞发送字节/缓冲/字符串
  *   - 阻塞接收字节（带超时）
  *   - 状态标志查询与清除
  *
  * 使用示例:
  *   USART_Cfg_t cfg = { USART1, 115200, USART_WordLength_8b,
  *                       USART_StopBits_1, USART_Parity_No,
  *                       USART_Mode_Tx | USART_Mode_Rx };
  *   DRV_USART_Init(&cfg);
  *   DRV_USART_SendString(USART1, "Hello\r\n");
  ******************************************************************************
  */

#ifndef __DRV_USART_H
#define __DRV_USART_H

#include "stm32f10x.h"
#include "stm32f10x_usart.h"

/*===========================================================================
 * 数据结构
 *===========================================================================*/

/** USART初始化配置结构体 */
typedef struct
{
    USART_TypeDef  *usart;      /**< USART实例：USART1, USART2, USART3 */
    uint32_t        baudRate;   /**< 波特率，如 9600, 115200 */
    uint16_t        wordLength; /**< 数据位：USART_WordLength_8b / _9b */
    uint16_t        stopBits;   /**< 停止位：USART_StopBits_1 / _2 等 */
    uint16_t        parity;     /**< 校验：USART_Parity_No / _Even / _Odd */
    uint16_t        mode;       /**< 收发模式：USART_Mode_Tx | USART_Mode_Rx */
} USART_Cfg_t;

/*===========================================================================
 * 返回状态码
 *===========================================================================*/
#define DRV_USART_OK        (0)   /**< 操作成功 */
#define DRV_USART_TIMEOUT   (-1)  /**< 操作超时 */
#define DRV_USART_ERROR     (-2)  /**< 操作错误 */

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  初始化USART外设（自动配置对应GPIO和时钟）
 * @param  cfg  配置结构体指针
 */
void DRV_USART_Init(const USART_Cfg_t *cfg);

/**
 * @brief  关闭并复位USART外设
 * @param  usart  USART实例
 */
void DRV_USART_DeInit(USART_TypeDef *usart);

/**
 * @brief  阻塞发送单字节
 * @param  usart  USART实例
 * @param  byte   待发送字节
 */
void DRV_USART_SendByte(USART_TypeDef *usart, uint8_t byte);

/**
 * @brief  阻塞发送数据缓冲
 * @param  usart  USART实例
 * @param  buf    数据指针
 * @param  len    数据长度（字节）
 */
void DRV_USART_SendBuffer(USART_TypeDef *usart, const uint8_t *buf, uint16_t len);

/**
 * @brief  阻塞发送以 '\0' 结尾的字符串
 * @param  usart  USART实例
 * @param  str    字符串指针
 */
void DRV_USART_SendString(USART_TypeDef *usart, const char *str);

/**
 * @brief  阻塞接收单字节（带超时）
 * @param  usart    USART实例
 * @param  byte     接收到的字节（输出）
 * @param  timeout  超时循环次数（0 = 永久等待）
 * @retval DRV_USART_OK 成功, DRV_USART_TIMEOUT 超时
 */
int DRV_USART_ReceiveByte(USART_TypeDef *usart, uint8_t *byte, uint32_t timeout);

/**
 * @brief  查询USART状态寄存器标志
 * @param  usart  USART实例
 * @param  flag   标志位，如 USART_FLAG_TXE, USART_FLAG_RXNE
 * @retval SET / RESET
 */
FlagStatus DRV_USART_GetFlag(USART_TypeDef *usart, uint16_t flag);

/**
 * @brief  清除USART状态标志
 * @param  usart  USART实例
 * @param  flag   标志位
 */
void DRV_USART_ClearFlag(USART_TypeDef *usart, uint16_t flag);

/**
 * @brief  使能/禁止USART中断
 * @param  usart   USART实例
 * @param  itFlag  中断源，如 USART_IT_RXNE
 * @param  en      ENABLE / DISABLE
 */
void DRV_USART_ITConfig(USART_TypeDef *usart, uint16_t itFlag, FunctionalState en);

#endif /* __DRV_USART_H */
