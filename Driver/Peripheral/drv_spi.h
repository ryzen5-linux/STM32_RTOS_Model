/**
  ******************************************************************************
  * @file    drv_spi.h
  * @brief   SPI外设驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - 支持 SPI1 / SPI2，主模式全双工
  *   - 初始化（极性、相位、数据宽度、分频、LSB/MSB）
  *   - 单字节/缓冲收发
  *   - 软件片选辅助函数（NSS由用户GPIO控制）
  *
  * 使用示例:
  *   SPI_Cfg_t cfg = { SPI1, SPI_BaudRatePrescaler_8,
  *                     SPI_CPOL_Low, SPI_CPHA_1Edge,
  *                     SPI_DataSize_8b, SPI_FirstBit_MSB };
  *   DRV_SPI_Init(&cfg);
  *   uint8_t rx = DRV_SPI_TransmitReceive(SPI1, 0xAA);
  ******************************************************************************
  */

#ifndef __DRV_SPI_H
#define __DRV_SPI_H

#include "stm32f10x.h"
#include "stm32f10x_spi.h"

/*===========================================================================
 * 数据结构
 *===========================================================================*/

/** SPI初始化配置结构体 */
typedef struct
{
    SPI_TypeDef   *spi;          /**< SPI实例：SPI1 或 SPI2 */
    uint16_t       prescaler;    /**< 波特率分频：SPI_BaudRatePrescaler_2 ~ _256 */
    uint16_t       cpol;         /**< 时钟极性：SPI_CPOL_Low / SPI_CPOL_High */
    uint16_t       cpha;         /**< 时钟相位：SPI_CPHA_1Edge / SPI_CPHA_2Edge */
    uint16_t       dataSize;     /**< 数据宽度：SPI_DataSize_8b / _16b */
    uint16_t       firstBit;     /**< 位序：SPI_FirstBit_MSB / SPI_FirstBit_LSB */
} SPI_Cfg_t;

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  初始化SPI外设（主模式，软件NSS）
 * @param  cfg  配置结构体指针
 */
void DRV_SPI_Init(const SPI_Cfg_t *cfg);

/**
 * @brief  关闭并复位SPI外设
 * @param  spi  SPI实例
 */
void DRV_SPI_DeInit(SPI_TypeDef *spi);

/**
 * @brief  全双工收发单字节（先发后收，阻塞等待）
 * @param  spi   SPI实例
 * @param  txData 发送字节
 * @retval 接收到的字节
 */
uint8_t DRV_SPI_TransmitReceive(SPI_TypeDef *spi, uint8_t txData);

/**
 * @brief  全双工收发数据缓冲（txBuf与rxBuf可相同）
 * @param  spi    SPI实例
 * @param  txBuf  发送缓冲指针（NULL则发0xFF）
 * @param  rxBuf  接收缓冲指针（NULL则丢弃）
 * @param  len    数据长度（字节）
 */
void DRV_SPI_TransmitReceiveBuffer(SPI_TypeDef *spi,
                                   const uint8_t *txBuf,
                                   uint8_t       *rxBuf,
                                   uint16_t       len);

/**
 * @brief  仅发送缓冲（忽略接收）
 * @param  spi   SPI实例
 * @param  buf   发送缓冲指针
 * @param  len   数据长度
 */
void DRV_SPI_Transmit(SPI_TypeDef *spi, const uint8_t *buf, uint16_t len);

/**
 * @brief  仅接收缓冲（发送哑字节0xFF）
 * @param  spi   SPI实例
 * @param  buf   接收缓冲指针
 * @param  len   数据长度
 */
void DRV_SPI_Receive(SPI_TypeDef *spi, uint8_t *buf, uint16_t len);

/**
 * @brief  查询SPI状态标志
 * @param  spi   SPI实例
 * @param  flag  标志位，如 SPI_I2S_FLAG_TXE, SPI_I2S_FLAG_RXNE, SPI_I2S_FLAG_BSY
 * @retval SET / RESET
 */
FlagStatus DRV_SPI_GetFlag(SPI_TypeDef *spi, uint16_t flag);

#endif /* __DRV_SPI_H */
