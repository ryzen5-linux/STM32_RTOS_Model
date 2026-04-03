/**
  ******************************************************************************
  * @file    drv_tim.h
  * @brief   定时器外设驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - 支持 TIM1（高级）/ TIM2 / TIM3 / TIM4（通用）
  *   - 基础定时模式（周期性中断）
  *   - PWM输出模式（可设置频率和占空比）
  *   - 输入捕获模式（测量脉宽/频率）
  *   - 启动/停止/获取计数
  *
  * 使用示例（PWM输出）:
  *   TIM_PWM_Cfg_t pwm = { TIM3, TIM_Channel_1, 72000000, 1000, 500 };
  *   // 频率 = 72MHz / (PSC+1) / (ARR+1)，配置时内部计算
  *   DRV_TIM_PWM_Init(&pwm);  // 1kHz, 50%占空比
  *   DRV_TIM_Start(TIM3);
  *
  * 使用示例（基础定时中断）:
  *   TIM_Base_Cfg_t base = { TIM2, 7199, 999 };  // 72MHz/(7199+1)/(999+1) = 10Hz
  *   DRV_TIM_Base_Init(&base);
  *   DRV_TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
  *   DRV_TIM_Start(TIM2);
  ******************************************************************************
  */

#ifndef __DRV_TIM_H
#define __DRV_TIM_H

#include "stm32f10x.h"
#include "stm32f10x_tim.h"

/*===========================================================================
 * 数据结构
 *===========================================================================*/

/** 定时器基础配置（基础定时 / 上溢中断）*/
typedef struct
{
    TIM_TypeDef *tim;        /**< 定时器实例：TIM1~TIM4 */
    uint16_t     prescaler;  /**< 预分频值（0 = 不分频），实际 = prescaler+1 */
    uint16_t     period;     /**< 自动重装值，实际计数 = period+1 */
} TIM_Base_Cfg_t;

/** PWM输出配置 */
typedef struct
{
    TIM_TypeDef *tim;        /**< 定时器实例 */
    uint16_t     channel;    /**< 通道：TIM_Channel_1 ~ TIM_Channel_4 */
    uint32_t     sysClkHz;   /**< 系统时钟（Hz），用于计算分频，通常为 72000000 */
    uint32_t     freqHz;     /**< PWM频率（Hz）*/
    uint32_t     dutyPct;    /**< 初始占空比百分比（0~100）*/
} TIM_PWM_Cfg_t;

/** 输入捕获配置 */
typedef struct
{
    TIM_TypeDef *tim;         /**< 定时器实例 */
    uint16_t     channel;     /**< 通道：TIM_Channel_1 ~ TIM_Channel_4 */
    uint16_t     prescaler;   /**< 预分频 */
    uint16_t     polarity;    /**< 捕获边沿：TIM_ICPolarity_Rising / _Falling / _BothEdge */
} TIM_IC_Cfg_t;

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  初始化定时器基础/上溢模式
 * @param  cfg  基础配置结构体指针
 */
void DRV_TIM_Base_Init(const TIM_Base_Cfg_t *cfg);

/**
 * @brief  初始化定时器PWM输出模式
 *         自动计算PSC和ARR以产生目标频率，PWM模式1
 * @param  cfg  PWM配置结构体指针
 */
void DRV_TIM_PWM_Init(const TIM_PWM_Cfg_t *cfg);

/**
 * @brief  初始化定时器输入捕获模式
 * @param  cfg  输入捕获配置结构体指针
 */
void DRV_TIM_IC_Init(const TIM_IC_Cfg_t *cfg);

/**
 * @brief  启动定时器（使能计数器）
 * @param  tim  定时器实例
 */
void DRV_TIM_Start(TIM_TypeDef *tim);

/**
 * @brief  停止定时器（禁止计数器）
 * @param  tim  定时器实例
 */
void DRV_TIM_Stop(TIM_TypeDef *tim);

/**
 * @brief  使能/禁止定时器中断
 * @param  tim   定时器实例
 * @param  it    中断源，如 TIM_IT_Update, TIM_IT_CC1
 * @param  en    ENABLE / DISABLE
 */
void DRV_TIM_ITConfig(TIM_TypeDef *tim, uint16_t it, FunctionalState en);

/**
 * @brief  获取并清除定时器中断标志
 * @param  tim   定时器实例
 * @param  flag  标志，如 TIM_FLAG_Update, TIM_FLAG_CC1
 * @retval SET / RESET
 */
FlagStatus DRV_TIM_GetFlag(TIM_TypeDef *tim, uint16_t flag);

/**
 * @brief  清除定时器标志
 * @param  tim   定时器实例
 * @param  flag  标志
 */
void DRV_TIM_ClearFlag(TIM_TypeDef *tim, uint16_t flag);

/**
 * @brief  获取当前计数值
 * @param  tim  定时器实例
 * @retval 16位计数值
 */
uint16_t DRV_TIM_GetCounter(TIM_TypeDef *tim);

/**
 * @brief  设置计数器初值
 * @param  tim    定时器实例
 * @param  count  计数初值
 */
void DRV_TIM_SetCounter(TIM_TypeDef *tim, uint16_t count);

/**
 * @brief  设置PWM占空比（运行时更新）
 * @param  tim      定时器实例
 * @param  channel  通道
 * @param  dutyPct  占空比（0~100）
 */
void DRV_TIM_SetDuty(TIM_TypeDef *tim, uint16_t channel, uint32_t dutyPct);

/**
 * @brief  获取输入捕获值
 * @param  tim      定时器实例
 * @param  channel  通道
 * @retval 捕获寄存器值
 */
uint16_t DRV_TIM_GetCapture(TIM_TypeDef *tim, uint16_t channel);

/**
 * @brief  关闭并复位定时器
 * @param  tim  定时器实例
 */
void DRV_TIM_DeInit(TIM_TypeDef *tim);

#endif /* __DRV_TIM_H */
