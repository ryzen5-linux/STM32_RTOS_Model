/**
  ******************************************************************************
  * @file    drv_exti.h
  * @brief   EXTI外部中断驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - 配置GPIO引脚触发外部中断（上升沿/下降沿/双边沿）
  *   - 自动配置AFIO、GPIO输入模式和NVIC
  *   - 标志位查询与清除
  *   - 软件中断触发
  *
  * 使用示例:
  *   EXTI_PinCfg_t cfg = { GPIOA, GPIO_Pin_0, 0,
  *                         EXTI_Trigger_Rising, ENABLE,
  *                         EXTI_IRQ_PRIORITY_DEFAULT, EXTI_IRQ_PRIORITY_DEFAULT };
  *   DRV_EXTI_Init(&cfg);
  *   // 在 EXTI0_IRQHandler 中调用 DRV_EXTI_ClearFlag(EXTI_Line0);
  ******************************************************************************
  */

#ifndef __DRV_EXTI_H
#define __DRV_EXTI_H

#include "stm32f10x.h"
#include "stm32f10x_exti.h"
#include "misc.h"

/*===========================================================================
 * 宏定义
 *===========================================================================*/
#define EXTI_IRQ_PRIORITY_DEFAULT  (2U)  /**< 默认EXTI中断抢占优先级 */
#define EXTI_IRQ_SUBPRI_DEFAULT    (0U)  /**< 默认EXTI中断响应优先级 */

/*===========================================================================
 * 数据结构
 *===========================================================================*/

/** EXTI引脚中断配置结构体 */
typedef struct
{
    GPIO_TypeDef       *port;          /**< GPIO端口，如 GPIOA */
    uint16_t            pin;           /**< GPIO引脚号（仅单个），如 GPIO_Pin_0 */
    uint8_t             pinSource;     /**< GPIO_PinSource0 ~ GPIO_PinSource15 */
    EXTITrigger_TypeDef trigger;       /**< 触发方式：EXTI_Trigger_Rising/Falling/Rising_Falling */
    FunctionalState     lineCmd;       /**< 使能/禁止该EXTI线 */
    uint8_t             preemptPri;    /**< NVIC抢占优先级 */
    uint8_t             subPri;        /**< NVIC响应优先级 */
} EXTI_PinCfg_t;

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  初始化EXTI（配置GPIO输入、AFIO映射、EXTI线、NVIC）
 * @param  cfg  配置结构体指针
 */
void DRV_EXTI_Init(const EXTI_PinCfg_t *cfg);

/**
 * @brief  使能指定EXTI线
 * @param  line  EXTI线，如 EXTI_Line0
 */
void DRV_EXTI_Enable(uint32_t line);

/**
 * @brief  禁止指定EXTI线
 * @param  line  EXTI线
 */
void DRV_EXTI_Disable(uint32_t line);

/**
 * @brief  查询EXTI线中断/事件标志
 * @param  line  EXTI线
 * @retval SET / RESET
 */
FlagStatus DRV_EXTI_GetFlag(uint32_t line);

/**
 * @brief  清除EXTI线中断挂起标志（中断服务函数中调用）
 * @param  line  EXTI线
 */
void DRV_EXTI_ClearFlag(uint32_t line);

/**
 * @brief  软件产生EXTI事件（用于测试或同步）
 * @param  line  EXTI线
 */
void DRV_EXTI_SoftwareTrigger(uint32_t line);

/**
 * @brief  获取GPIO引脚对应的EXTI线号（掩码形式）
 * @param  pin  GPIO_Pin_0 ~ GPIO_Pin_15
 * @retval 对应的EXTI_Line
 */
uint32_t DRV_EXTI_GetLine(uint16_t pin);

#endif /* __DRV_EXTI_H */
