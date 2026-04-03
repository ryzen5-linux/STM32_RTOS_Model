/**
  ******************************************************************************
  * @file    drv_exti.c
  * @brief   EXTI外部中断驱动实现 - STM32F103C8T6
  *
  * EXTI线与IRQ通道映射:
  *   EXTI_Line0          -> EXTI0_IRQn
  *   EXTI_Line1          -> EXTI1_IRQn
  *   EXTI_Line2          -> EXTI2_IRQn
  *   EXTI_Line3          -> EXTI3_IRQn
  *   EXTI_Line4          -> EXTI4_IRQn
  *   EXTI_Line5~Line9    -> EXTI9_5_IRQn
  *   EXTI_Line10~Line15  -> EXTI15_10_IRQn
  ******************************************************************************
  */

#include "drv_exti.h"
#include <stddef.h>

/*---------------------------------------------------------------------------
 * 内部辅助：根据引脚号获取AFIO端口来源
 *---------------------------------------------------------------------------*/
static uint8_t get_port_source(GPIO_TypeDef *port)
{
    if      (port == GPIOA) { return GPIO_PortSourceGPIOA; }
    else if (port == GPIOB) { return GPIO_PortSourceGPIOB; }
    else if (port == GPIOC) { return GPIO_PortSourceGPIOC; }
    else if (port == GPIOD) { return GPIO_PortSourceGPIOD; }
    return GPIO_PortSourceGPIOA;
}

/*---------------------------------------------------------------------------
 * 内部辅助：根据EXTI线号获取IRQ通道
 *---------------------------------------------------------------------------*/
static IRQn_Type get_irqn(uint32_t line)
{
    if      (line == EXTI_Line0)  { return EXTI0_IRQn;     }
    else if (line == EXTI_Line1)  { return EXTI1_IRQn;     }
    else if (line == EXTI_Line2)  { return EXTI2_IRQn;     }
    else if (line == EXTI_Line3)  { return EXTI3_IRQn;     }
    else if (line == EXTI_Line4)  { return EXTI4_IRQn;     }
    else if (line <= EXTI_Line9)  { return EXTI9_5_IRQn;   }
    else                          { return EXTI15_10_IRQn; }
}

/*---------------------------------------------------------------------------
 * 内部辅助：使能GPIO端口时钟
 *---------------------------------------------------------------------------*/
static void exti_gpio_rcc_enable(GPIO_TypeDef *port)
{
    if      (port == GPIOA) { RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); }
    else if (port == GPIOB) { RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); }
    else if (port == GPIOC) { RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); }
    else if (port == GPIOD) { RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); }
}

/*===========================================================================
 * API 实现
 *===========================================================================*/

void DRV_EXTI_Init(const EXTI_PinCfg_t *cfg)
{
    GPIO_InitTypeDef  gpio;
    EXTI_InitTypeDef  exti;
    NVIC_InitTypeDef  nvic;
    uint32_t          line;

    if (cfg == NULL) { return; }

    line = DRV_EXTI_GetLine(cfg->pin);

    /* 1. 使能时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    exti_gpio_rcc_enable(cfg->port);

    /* 2. 配置GPIO为输入（上拉输入，抗干扰） */
    gpio.GPIO_Pin  = cfg->pin;
    gpio.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(cfg->port, &gpio);

    /* 3. AFIO外部中断引脚映射 */
    GPIO_EXTILineConfig(get_port_source(cfg->port), cfg->pinSource);

    /* 4. 配置EXTI线 */
    exti.EXTI_Line    = line;
    exti.EXTI_Mode    = EXTI_Mode_Interrupt;
    exti.EXTI_Trigger = cfg->trigger;
    exti.EXTI_LineCmd = cfg->lineCmd;
    EXTI_Init(&exti);

    /* 5. 配置NVIC */
    nvic.NVIC_IRQChannel                   = get_irqn(line);
    nvic.NVIC_IRQChannelPreemptionPriority = cfg->preemptPri;
    nvic.NVIC_IRQChannelSubPriority        = cfg->subPri;
    nvic.NVIC_IRQChannelCmd                = ENABLE;
    NVIC_Init(&nvic);
}

void DRV_EXTI_Enable(uint32_t line)
{
    EXTI->IMR |= line;
}

void DRV_EXTI_Disable(uint32_t line)
{
    EXTI->IMR &= ~line;
}

FlagStatus DRV_EXTI_GetFlag(uint32_t line)
{
    return EXTI_GetFlagStatus(line);
}

void DRV_EXTI_ClearFlag(uint32_t line)
{
    EXTI_ClearITPendingBit(line);
}

void DRV_EXTI_SoftwareTrigger(uint32_t line)
{
    EXTI->SWIER |= line;
}

uint32_t DRV_EXTI_GetLine(uint16_t pin)
{
    uint32_t line = 1U;
    uint16_t p   = pin;
    while (p > 1U) { p >>= 1U; line <<= 1U; }
    return line;
}
