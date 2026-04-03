/**
  ******************************************************************************
  * @file    drv_wwdg.h
  * @brief   窗口看门狗(WWDG)驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - 基于APB1时钟（36MHz@72MHz系统频率）的窗口看门狗
  *   - 窗口值和计数器值配置
  *   - 提前唤醒中断（EWI）
  *   - 必须在窗口期内喂狗（太早或太晚都会复位）
  *
  * 超时时间参考（PCLK1=36MHz）:
  *   WWDG时钟 = PCLK1/4096/预分频
  *   T[6:0]计数范围：0x40（64）到0x7F（127）
  *   超时时间 = (4096 * psc * (counter - window)) / PCLK1
  *
  * 使用示例:
  *   DRV_WWDG_Init(WWDG_Prescaler_8, 0x7F, 0x50);
  *   // counter=0x7F, window=0x50: 喂狗必须在计数器降到0x50~0x40区间内
  *   // 在合适的时机调用:
  *   DRV_WWDG_Feed(0x7F);
  ******************************************************************************
  */

#ifndef __DRV_WWDG_H
#define __DRV_WWDG_H

#include "stm32f10x.h"
#include "stm32f10x_wwdg.h"

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  初始化并启动WWDG
 * @param  prescaler  预分频：WWDG_Prescaler_1 / _2 / _4 / _8
 * @param  counter    计数器初值（7位，范围0x40~0x7F，0x40=复位阈值）
 * @param  windowVal  窗口值（喂狗必须在counter降到windowVal到0x40之间）
 */
void DRV_WWDG_Init(uint8_t prescaler, uint8_t counter, uint8_t windowVal);

/**
 * @brief  喂狗（设置新计数器值，必须在窗口期内调用）
 * @param  counter  新计数器值（0x40~0x7F）
 */
void DRV_WWDG_Feed(uint8_t counter);

/**
 * @brief  使能/禁止WWDG提前唤醒中断（EWI）
 * @param  en  ENABLE / DISABLE
 */
void DRV_WWDG_EWIConfig(FunctionalState en);

/**
 * @brief  清除WWDG提前唤醒中断标志（在EWI中断服务中调用）
 */
void DRV_WWDG_ClearEWIFlag(void);

#endif /* __DRV_WWDG_H */
