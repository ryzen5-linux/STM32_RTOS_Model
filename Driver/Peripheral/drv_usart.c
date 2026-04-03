/**
  ******************************************************************************
  * @file    drv_usart.c
  * @brief   USART外设驱动实现 - STM32F103C8T6
  *
  * 引脚映射（默认，无重映射）:
  *   USART1: TX=PA9,  RX=PA10
  *   USART2: TX=PA2,  RX=PA3
  *   USART3: TX=PB10, RX=PB11
  ******************************************************************************
  */

#include "drv_usart.h"
#include <stddef.h>

/*---------------------------------------------------------------------------
 * 内部辅助：配置USART对应GPIO引脚
 *---------------------------------------------------------------------------*/
static void usart_gpio_init(USART_TypeDef *usart, uint16_t mode)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    if (usart == USART1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        if (mode & USART_Mode_Tx)
        {
            gpio.GPIO_Pin   = GPIO_Pin_9;
            gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
            gpio.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(GPIOA, &gpio);
        }
        if (mode & USART_Mode_Rx)
        {
            gpio.GPIO_Pin  = GPIO_Pin_10;
            gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_Init(GPIOA, &gpio);
        }
    }
    else if (usart == USART2)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        if (mode & USART_Mode_Tx)
        {
            gpio.GPIO_Pin   = GPIO_Pin_2;
            gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
            gpio.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(GPIOA, &gpio);
        }
        if (mode & USART_Mode_Rx)
        {
            gpio.GPIO_Pin  = GPIO_Pin_3;
            gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_Init(GPIOA, &gpio);
        }
    }
    else if (usart == USART3)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        if (mode & USART_Mode_Tx)
        {
            gpio.GPIO_Pin   = GPIO_Pin_10;
            gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
            gpio.GPIO_Speed = GPIO_Speed_50MHz;
            GPIO_Init(GPIOB, &gpio);
        }
        if (mode & USART_Mode_Rx)
        {
            gpio.GPIO_Pin  = GPIO_Pin_11;
            gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            GPIO_Init(GPIOB, &gpio);
        }
    }
}

/*---------------------------------------------------------------------------
 * 内部辅助：使能USART总线时钟
 *---------------------------------------------------------------------------*/
static void usart_rcc_enable(USART_TypeDef *usart)
{
    if      (usart == USART1) { RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE); }
    else if (usart == USART2) { RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE); }
    else if (usart == USART3) { RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); }
}

/*===========================================================================
 * API 实现
 *===========================================================================*/

void DRV_USART_Init(const USART_Cfg_t *cfg)
{
    USART_InitTypeDef init;

    if (cfg == NULL) { return; }

    usart_rcc_enable(cfg->usart);
    usart_gpio_init(cfg->usart, cfg->mode);

    init.USART_BaudRate            = cfg->baudRate;
    init.USART_WordLength          = cfg->wordLength;
    init.USART_StopBits            = cfg->stopBits;
    init.USART_Parity              = cfg->parity;
    init.USART_Mode                = cfg->mode;
    init.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

    USART_Init(cfg->usart, &init);
    USART_Cmd(cfg->usart, ENABLE);
}

void DRV_USART_DeInit(USART_TypeDef *usart)
{
    USART_Cmd(usart, DISABLE);
    USART_DeInit(usart);
}

void DRV_USART_SendByte(USART_TypeDef *usart, uint8_t byte)
{
    while (USART_GetFlagStatus(usart, USART_FLAG_TXE) == RESET) {}
    USART_SendData(usart, (uint16_t)byte);
    while (USART_GetFlagStatus(usart, USART_FLAG_TC)  == RESET) {}
}

void DRV_USART_SendBuffer(USART_TypeDef *usart, const uint8_t *buf, uint16_t len)
{
    uint16_t i;
    for (i = 0U; i < len; i++)
    {
        DRV_USART_SendByte(usart, buf[i]);
    }
}

void DRV_USART_SendString(USART_TypeDef *usart, const char *str)
{
    while (str != NULL && *str != '\0')
    {
        DRV_USART_SendByte(usart, (uint8_t)(*str));
        str++;
    }
}

int DRV_USART_ReceiveByte(USART_TypeDef *usart, uint8_t *byte, uint32_t timeout)
{
    uint32_t cnt = 0U;

    while (USART_GetFlagStatus(usart, USART_FLAG_RXNE) == RESET)
    {
        if (timeout != 0U)
        {
            cnt++;
            if (cnt >= timeout) { return DRV_USART_TIMEOUT; }
        }
    }
    *byte = (uint8_t)USART_ReceiveData(usart);
    return DRV_USART_OK;
}

FlagStatus DRV_USART_GetFlag(USART_TypeDef *usart, uint16_t flag)
{
    return USART_GetFlagStatus(usart, flag);
}

void DRV_USART_ClearFlag(USART_TypeDef *usart, uint16_t flag)
{
    USART_ClearFlag(usart, flag);
}

void DRV_USART_ITConfig(USART_TypeDef *usart, uint16_t itFlag, FunctionalState en)
{
    USART_ITConfig(usart, itFlag, en);
}
