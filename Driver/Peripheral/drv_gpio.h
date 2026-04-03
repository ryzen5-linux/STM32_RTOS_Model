/**
  ******************************************************************************
  * @file    drv_gpio.h
  * @brief   GPIO外设驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - GPIO引脚初始化（输出推挽/开漏，输入浮空/上下拉，模拟，复用）
  *   - 引脚置位/复位/翻转
  *   - 引脚/端口读写
  *
  * 使用示例:
  *   GPIO_PinCfg_t cfg = { GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP, GPIO_Speed_50MHz };
  *   DRV_GPIO_Init(&cfg);
  *   DRV_GPIO_SetPin(GPIOC, GPIO_Pin_13);
  ******************************************************************************
  */

#ifndef __DRV_GPIO_H
#define __DRV_GPIO_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

/*===========================================================================
 * 数据结构
 *===========================================================================*/

/** GPIO引脚配置结构体 */
typedef struct
{
    GPIO_TypeDef   *port;   /**< GPIO端口，如 GPIOA, GPIOB, GPIOC */
    uint16_t        pin;    /**< 引脚号，如 GPIO_Pin_0 ~ GPIO_Pin_15，可用 | 组合 */
    GPIOMode_TypeDef mode;  /**< 引脚模式，如 GPIO_Mode_Out_PP */
    GPIOSpeed_TypeDef speed;/**< 引脚速度（输出有效），如 GPIO_Speed_50MHz */
} GPIO_PinCfg_t;

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  初始化GPIO引脚
 * @param  cfg  引脚配置结构体指针
 */
void DRV_GPIO_Init(const GPIO_PinCfg_t *cfg);

/**
 * @brief  将指定引脚输出高电平
 * @param  port  GPIO端口
 * @param  pin   引脚号（可用 | 组合多个）
 */
void DRV_GPIO_SetPin(GPIO_TypeDef *port, uint16_t pin);

/**
 * @brief  将指定引脚输出低电平
 * @param  port  GPIO端口
 * @param  pin   引脚号
 */
void DRV_GPIO_ResetPin(GPIO_TypeDef *port, uint16_t pin);

/**
 * @brief  翻转指定引脚输出电平
 * @param  port  GPIO端口
 * @param  pin   引脚号
 */
void DRV_GPIO_TogglePin(GPIO_TypeDef *port, uint16_t pin);

/**
 * @brief  读取指定输入引脚电平
 * @param  port  GPIO端口
 * @param  pin   引脚号（单个）
 * @retval 0 低电平，1 高电平
 */
uint8_t DRV_GPIO_ReadPin(GPIO_TypeDef *port, uint16_t pin);

/**
 * @brief  读取整个端口输入值
 * @param  port  GPIO端口
 * @retval 端口16位输入寄存器值
 */
uint16_t DRV_GPIO_ReadPort(GPIO_TypeDef *port);

/**
 * @brief  写整个端口输出值
 * @param  port   GPIO端口
 * @param  value  16位输出值
 */
void DRV_GPIO_WritePort(GPIO_TypeDef *port, uint16_t value);

/**
 * @brief  将GPIO引脚复位为模拟输入（低功耗默认态）
 * @param  port  GPIO端口
 * @param  pin   引脚号
 */
void DRV_GPIO_DeInit(GPIO_TypeDef *port, uint16_t pin);

#endif /* __DRV_GPIO_H */
