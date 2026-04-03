/**
  ******************************************************************************
  * @file    drv_bkp.h
  * @brief   备份寄存器(BKP)驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - STM32F103C8T6 提供10个16位备份数据寄存器（BKP_DR1~DR10）
  *   - 由VBAT供电维持（系统断电后数据保留）
  *   - 写入需先通过PWR解除写保护
  *   - RTC校准输出和防篡改事件配置
  *
  * 使用示例:
  *   DRV_BKP_Write(BKP_DR1, 0xABCD);
  *   uint16_t val = DRV_BKP_Read(BKP_DR1);
  ******************************************************************************
  */

#ifndef __DRV_BKP_H
#define __DRV_BKP_H

#include "stm32f10x.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"

/*===========================================================================
 * 宏定义
 *===========================================================================*/
/** BKP寄存器数量（STM32F103C8T6中密度为10个，互联型42个）*/
#define DRV_BKP_REG_COUNT   (10U)

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  向备份数据寄存器写入16位值
 * @param  reg    寄存器：BKP_DR1 ~ BKP_DR10
 * @param  value  待写入值
 */
void DRV_BKP_Write(uint16_t reg, uint16_t value);

/**
 * @brief  从备份数据寄存器读取16位值
 * @param  reg  寄存器：BKP_DR1 ~ BKP_DR10
 * @retval 读取的16位值
 */
uint16_t DRV_BKP_Read(uint16_t reg);

/**
 * @brief  清零所有备份数据寄存器（BKP_DR1~DR10）
 */
void DRV_BKP_ResetAll(void);

/**
 * @brief  使能/禁止RTC校准输出（TAMPER引脚PC13输出512Hz或1Hz）
 * @param  en        ENABLE / DISABLE
 * @param  calib512  ENABLE=512Hz / DISABLE=1Hz
 */
void DRV_BKP_RTCOutputConfig(FunctionalState en, FunctionalState calib512);

/**
 * @brief  使能/禁止侵入检测（TAMPER引脚，PC13）
 * @param  activeLevel  触发电平：BKP_TamperPinLevel_High / _Low
 * @param  en           ENABLE / DISABLE
 */
void DRV_BKP_TamperConfig(uint16_t activeLevel, FunctionalState en);

#endif /* __DRV_BKP_H */
