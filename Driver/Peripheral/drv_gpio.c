/**
  ******************************************************************************
  * @file    drv_gpio.c
  * @brief   GPIO外设驱动实现 - STM32F103C8T6
  ******************************************************************************
  */

#include "drv_gpio.h"
#include <stddef.h>

/* 各GPIO端口时钟使能辅助函数 */
static void gpio_rcc_enable(GPIO_TypeDef *port)
{
    if      (port == GPIOA) { RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); }
    else if (port == GPIOB) { RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); }
    else if (port == GPIOC) { RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE); }
    else if (port == GPIOD) { RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE); }
}

void DRV_GPIO_Init(const GPIO_PinCfg_t *cfg)
{
    GPIO_InitTypeDef init;

    if (cfg == NULL) { return; }

    gpio_rcc_enable(cfg->port);

    init.GPIO_Pin   = cfg->pin;
    init.GPIO_Mode  = cfg->mode;
    init.GPIO_Speed = cfg->speed;
    GPIO_Init(cfg->port, &init);
}

void DRV_GPIO_SetPin(GPIO_TypeDef *port, uint16_t pin)
{
    GPIO_SetBits(port, pin);
}

void DRV_GPIO_ResetPin(GPIO_TypeDef *port, uint16_t pin)
{
    GPIO_ResetBits(port, pin);
}

void DRV_GPIO_TogglePin(GPIO_TypeDef *port, uint16_t pin)
{
    port->ODR ^= pin;
}

uint8_t DRV_GPIO_ReadPin(GPIO_TypeDef *port, uint16_t pin)
{
    return (GPIO_ReadInputDataBit(port, pin) == Bit_SET) ? 1U : 0U;
}

uint16_t DRV_GPIO_ReadPort(GPIO_TypeDef *port)
{
    return GPIO_ReadInputData(port);
}

void DRV_GPIO_WritePort(GPIO_TypeDef *port, uint16_t value)
{
    GPIO_Write(port, value);
}

void DRV_GPIO_DeInit(GPIO_TypeDef *port, uint16_t pin)
{
    GPIO_InitTypeDef init;
    init.GPIO_Pin   = pin;
    init.GPIO_Mode  = GPIO_Mode_AIN; /* 模拟输入（低功耗默认） */
    init.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(port, &init);
}
