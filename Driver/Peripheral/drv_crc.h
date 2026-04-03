/**
  ******************************************************************************
  * @file    drv_crc.h
  * @brief   CRC计算单元驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - 硬件CRC32计算（多项式 0x04C11DB7，与以太网CRC相同）
  *   - 支持连续计算（喂数据无需重置）
  *   - 独立数据寄存器（8位）
  *
  * 使用示例:
  *   uint32_t crc = DRV_CRC_Calculate(data, length);
  *   // 也可分段计算:
  *   DRV_CRC_Reset();
  *   DRV_CRC_Accumulate(buf1, len1);
  *   uint32_t result = DRV_CRC_Accumulate(buf2, len2);
  ******************************************************************************
  */

#ifndef __DRV_CRC_H
#define __DRV_CRC_H

#include "stm32f10x.h"
#include "stm32f10x_crc.h"

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  使能CRC时钟（使用CRC前必须调用一次）
 */
void DRV_CRC_Init(void);

/**
 * @brief  复位CRC数据寄存器（清零中间结果，开始新的计算序列）
 */
void DRV_CRC_Reset(void);

/**
 * @brief  对数据缓冲区进行完整CRC32计算（先复位再计算）
 * @param  buf  数据指针（32位对齐数组）
 * @param  len  32位字的数量（字节数/4）
 * @retval CRC32结果
 */
uint32_t DRV_CRC_Calculate(const uint32_t *buf, uint32_t len);

/**
 * @brief  在当前状态基础上累加计算CRC（不复位，用于分段计算）
 * @param  buf  数据指针（32位对齐数组）
 * @param  len  32位字的数量
 * @retval 当前累积CRC32结果
 */
uint32_t DRV_CRC_Accumulate(const uint32_t *buf, uint32_t len);

/**
 * @brief  读取当前CRC计算结果（不触发新计算）
 * @retval 当前CRC32值
 */
uint32_t DRV_CRC_GetResult(void);

/**
 * @brief  写入8位独立数据寄存器（IDR，用于临时存储1字节）
 * @param  data  8位数据
 */
void DRV_CRC_WriteIDR(uint8_t data);

/**
 * @brief  读取8位独立数据寄存器
 * @retval 8位IDR值
 */
uint8_t DRV_CRC_ReadIDR(void);

#endif /* __DRV_CRC_H */
