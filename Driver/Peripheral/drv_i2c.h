/**
  ******************************************************************************
  * @file    drv_i2c.h
  * @brief   I2C外设驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - 支持 I2C1 / I2C2，主模式（标准100kHz / 快速400kHz）
  *   - 7位从机地址寻址
  *   - 写寄存器、读寄存器、连续读写封装
  *   - 错误/超时处理
  *
  * 使用示例:
  *   I2C_Cfg_t cfg = { I2C1, 400000 };
  *   DRV_I2C_Init(&cfg);
  *   uint8_t val;
  *   DRV_I2C_ReadReg(I2C1, 0x68, 0x00, &val, 1);  // MPU6050读寄存器
  ******************************************************************************
  */

#ifndef __DRV_I2C_H
#define __DRV_I2C_H

#include "stm32f10x.h"
#include "stm32f10x_i2c.h"

/*===========================================================================
 * 数据结构
 *===========================================================================*/

/** I2C初始化配置结构体 */
typedef struct
{
    I2C_TypeDef *i2c;        /**< I2C实例：I2C1 或 I2C2 */
    uint32_t     clockSpeed; /**< 时钟频率（Hz），如 100000, 400000 */
} I2C_Cfg_t;

/*===========================================================================
 * 返回状态码
 *===========================================================================*/
#define DRV_I2C_OK       (0)   /**< 操作成功 */
#define DRV_I2C_TIMEOUT  (-1)  /**< 等待超时 */
#define DRV_I2C_ERROR    (-2)  /**< 总线错误 */

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  初始化I2C外设（主模式，7位地址）
 * @param  cfg  配置结构体指针
 */
void DRV_I2C_Init(const I2C_Cfg_t *cfg);

/**
 * @brief  关闭并复位I2C外设
 * @param  i2c  I2C实例
 */
void DRV_I2C_DeInit(I2C_TypeDef *i2c);

/**
 * @brief  向从机指定寄存器写多字节
 * @param  i2c      I2C实例
 * @param  devAddr  7位从机地址（未左移）
 * @param  regAddr  寄存器地址
 * @param  buf      数据缓冲指针
 * @param  len      字节数
 * @retval DRV_I2C_OK / DRV_I2C_TIMEOUT / DRV_I2C_ERROR
 */
int DRV_I2C_WriteReg(I2C_TypeDef *i2c, uint8_t devAddr,
                     uint8_t regAddr, const uint8_t *buf, uint16_t len);

/**
 * @brief  从从机指定寄存器读多字节
 * @param  i2c      I2C实例
 * @param  devAddr  7位从机地址（未左移）
 * @param  regAddr  寄存器地址
 * @param  buf      接收缓冲指针
 * @param  len      字节数
 * @retval DRV_I2C_OK / DRV_I2C_TIMEOUT / DRV_I2C_ERROR
 */
int DRV_I2C_ReadReg(I2C_TypeDef *i2c, uint8_t devAddr,
                    uint8_t regAddr, uint8_t *buf, uint16_t len);

/**
 * @brief  向从机发送原始字节流（不含寄存器地址）
 * @param  i2c      I2C实例
 * @param  devAddr  7位从机地址（未左移）
 * @param  buf      发送缓冲指针
 * @param  len      字节数
 * @retval DRV_I2C_OK / DRV_I2C_TIMEOUT / DRV_I2C_ERROR
 */
int DRV_I2C_Write(I2C_TypeDef *i2c, uint8_t devAddr,
                  const uint8_t *buf, uint16_t len);

/**
 * @brief  从从机接收原始字节流（不含寄存器地址）
 * @param  i2c      I2C实例
 * @param  devAddr  7位从机地址（未左移）
 * @param  buf      接收缓冲指针
 * @param  len      字节数
 * @retval DRV_I2C_OK / DRV_I2C_TIMEOUT / DRV_I2C_ERROR
 */
int DRV_I2C_Read(I2C_TypeDef *i2c, uint8_t devAddr,
                 uint8_t *buf, uint16_t len);

/**
 * @brief  检测从机设备是否响应（ACK探测）
 * @param  i2c      I2C实例
 * @param  devAddr  7位从机地址（未左移）
 * @retval DRV_I2C_OK（存在）/ DRV_I2C_TIMEOUT（无响应）
 */
int DRV_I2C_IsDeviceReady(I2C_TypeDef *i2c, uint8_t devAddr);

#endif /* __DRV_I2C_H */
