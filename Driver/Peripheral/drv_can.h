/**
  ******************************************************************************
  * @file    drv_can.h
  * @brief   CAN总线驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - CAN1初始化（波特率配置）
  *   - 标准帧/扩展帧、数据帧/远程帧发送
  *   - 接收过滤器组配置（掩码模式/ ID列表模式）
  *   - 阻塞发送与状态查询
  *   - 接收FIFO读取
  *   - 中断配置
  *
  * 引脚映射（默认，无重映射）:
  *   CAN1: TX=PA12, RX=PA11
  *   重映射: TX=PB9,  RX=PB8（需调用GPIO_PinRemapConfig）
  *
  * 波特率计算（tq = 1 / (CAN_CLK / Prescaler)）:
  *   Bit time = (1 + BS1 + BS2) * tq，CAN_CLK = APB1 = 36MHz
  *   例：Prescaler=4, BS1=9, BS2=8 -> tq=111ns, BitTime=2000ns -> 500kbps
  *
  * 使用示例:
  *   CAN_Cfg_t cfg = { 4, CAN_SJW_1tq, CAN_BS1_9tq, CAN_BS2_8tq, DISABLE };
  *   DRV_CAN_Init(&cfg);
  *   CAN_Frame_t frame = { 0x123, 0, 0, 8, {1,2,3,4,5,6,7,8} };
  *   DRV_CAN_Send(&frame, 100);
  ******************************************************************************
  */

#ifndef __DRV_CAN_H
#define __DRV_CAN_H

#include "stm32f10x.h"
#include "stm32f10x_can.h"

/*===========================================================================
 * 数据结构
 *===========================================================================*/

/** CAN初始化配置结构体 */
typedef struct
{
    uint16_t        prescaler; /**< 分频值（1~1024），决定tq时间 */
    uint8_t         sjw;       /**< 同步跳转宽度：CAN_SJW_1tq ~ CAN_SJW_4tq */
    uint8_t         bs1;       /**< 位段1：CAN_BS1_1tq ~ CAN_BS1_16tq */
    uint8_t         bs2;       /**< 位段2：CAN_BS2_1tq ~ CAN_BS2_8tq */
    FunctionalState loopback;  /**< 回环模式（用于自测）：ENABLE / DISABLE */
} CAN_Cfg_t;

/** CAN帧结构体 */
typedef struct
{
    uint32_t id;          /**< 帧ID（标准帧11位，扩展帧29位）*/
    uint8_t  ide;         /**< ID类型：0=标准帧(11bit), 1=扩展帧(29bit) */
    uint8_t  rtr;         /**< 帧类型：0=数据帧, 1=远程帧 */
    uint8_t  dlc;         /**< 数据长度（0~8字节）*/
    uint8_t  data[8];     /**< 数据内容（远程帧ignored）*/
} CAN_Frame_t;

/** CAN过滤器配置结构体 */
typedef struct
{
    uint8_t  filterNum;   /**< 过滤器组号（0~13）*/
    uint8_t  fifo;        /**< 关联FIFO：CAN_Filter_FIFO0 / CAN_Filter_FIFO1 */
    uint8_t  mode;        /**< 过滤模式：CAN_FilterMode_IdMask / CAN_FilterMode_IdList */
    uint8_t  scale;       /**< 过滤位宽：CAN_FilterScale_32bit / CAN_FilterScale_16bit */
    uint32_t idHigh;      /**< 过滤器ID高16位（32bit模式时为高半部分）*/
    uint32_t idLow;       /**< 过滤器ID低16位 */
    uint32_t maskHigh;    /**< 掩码高16位（掩码模式有效）*/
    uint32_t maskLow;     /**< 掩码低16位 */
} CAN_FilterCfg_t;

/*===========================================================================
 * 返回状态码
 *===========================================================================*/
#define DRV_CAN_OK       (0)
#define DRV_CAN_TIMEOUT  (-1)
#define DRV_CAN_ERROR    (-2)

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  初始化CAN1外设
 * @param  cfg  配置结构体指针
 * @retval DRV_CAN_OK / DRV_CAN_ERROR
 */
int DRV_CAN_Init(const CAN_Cfg_t *cfg);

/**
 * @brief  关闭并复位CAN1
 */
void DRV_CAN_DeInit(void);

/**
 * @brief  配置CAN接收过滤器
 * @param  cfg  过滤器配置结构体指针
 */
void DRV_CAN_FilterConfig(const CAN_FilterCfg_t *cfg);

/**
 * @brief  阻塞发送一帧CAN数据
 * @param  frame    帧结构体指针
 * @param  timeout  超时循环次数（0=永久等待）
 * @retval DRV_CAN_OK / DRV_CAN_TIMEOUT / DRV_CAN_ERROR
 */
int DRV_CAN_Send(const CAN_Frame_t *frame, uint32_t timeout);

/**
 * @brief  从接收FIFO读取一帧数据
 * @param  fifox  FIFO编号：CAN_FIFO0 / CAN_FIFO1
 * @param  frame  帧结构体指针（输出）
 * @retval DRV_CAN_OK / DRV_CAN_ERROR（FIFO空）
 */
int DRV_CAN_Receive(uint8_t fifox, CAN_Frame_t *frame);

/**
 * @brief  查询接收FIFO挂起帧数量
 * @param  fifox  FIFO编号
 * @retval 待读取帧数（0~3）
 */
uint8_t DRV_CAN_GetRxPendingCount(uint8_t fifox);

/**
 * @brief  使能/禁止CAN中断
 * @param  it  中断源，如 CAN_IT_FMP0 / CAN_IT_TME 等
 * @param  en  ENABLE / DISABLE
 */
void DRV_CAN_ITConfig(uint32_t it, FunctionalState en);

#endif /* __DRV_CAN_H */
