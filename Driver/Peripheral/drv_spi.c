/**
  ******************************************************************************
  * @file    drv_spi.c
  * @brief   SPI外设驱动实现 - STM32F103C8T6
  *
  * 引脚映射（默认，无重映射）:
  *   SPI1: SCK=PA5, MISO=PA6, MOSI=PA7  （NSS=PA4，由用户GPIO控制）
  *   SPI2: SCK=PB13, MISO=PB14, MOSI=PB15 （NSS=PB12，由用户GPIO控制）
  ******************************************************************************
  */

#include "drv_spi.h"
#include <stddef.h>

/*---------------------------------------------------------------------------
 * 内部辅助：配置SPI对应GPIO
 *---------------------------------------------------------------------------*/
static void spi_gpio_init(SPI_TypeDef *spi)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);

    if (spi == SPI1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        /* SCK(PA5), MOSI(PA7): 复用推挽输出 */
        gpio.GPIO_Pin   = GPIO_Pin_5 | GPIO_Pin_7;
        gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
        gpio.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOA, &gpio);
        /* MISO(PA6): 浮空输入 */
        gpio.GPIO_Pin  = GPIO_Pin_6;
        gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOA, &gpio);
    }
    else if (spi == SPI2)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        /* SCK(PB13), MOSI(PB15): 复用推挽输出 */
        gpio.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_15;
        gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
        gpio.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(GPIOB, &gpio);
        /* MISO(PB14): 浮空输入 */
        gpio.GPIO_Pin  = GPIO_Pin_14;
        gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
        GPIO_Init(GPIOB, &gpio);
    }
}

/*---------------------------------------------------------------------------
 * 内部辅助：使能SPI总线时钟
 *---------------------------------------------------------------------------*/
static void spi_rcc_enable(SPI_TypeDef *spi)
{
    if      (spi == SPI1) { RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); }
    else if (spi == SPI2) { RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); }
}

/*===========================================================================
 * API 实现
 *===========================================================================*/

void DRV_SPI_Init(const SPI_Cfg_t *cfg)
{
    SPI_InitTypeDef init;

    if (cfg == NULL) { return; }

    spi_rcc_enable(cfg->spi);
    spi_gpio_init(cfg->spi);

    SPI_I2S_DeInit(cfg->spi);

    init.SPI_Direction         = SPI_Direction_2Lines_FullDuplex;
    init.SPI_Mode              = SPI_Mode_Master;
    init.SPI_DataSize          = cfg->dataSize;
    init.SPI_CPOL              = cfg->cpol;
    init.SPI_CPHA              = cfg->cpha;
    init.SPI_NSS               = SPI_NSS_Soft;  /* NSS软件控制，由用户GPIO实现 */
    init.SPI_BaudRatePrescaler = cfg->prescaler;
    init.SPI_FirstBit          = cfg->firstBit;
    init.SPI_CRCPolynomial     = 7U;

    SPI_Init(cfg->spi, &init);
    SPI_Cmd(cfg->spi, ENABLE);
}

void DRV_SPI_DeInit(SPI_TypeDef *spi)
{
    SPI_Cmd(spi, DISABLE);
    SPI_I2S_DeInit(spi);
}

uint8_t DRV_SPI_TransmitReceive(SPI_TypeDef *spi, uint8_t txData)
{
    while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_TXE)  == RESET) {}
    SPI_I2S_SendData(spi, (uint16_t)txData);
    while (SPI_I2S_GetFlagStatus(spi, SPI_I2S_FLAG_RXNE) == RESET) {}
    return (uint8_t)SPI_I2S_ReceiveData(spi);
}

void DRV_SPI_TransmitReceiveBuffer(SPI_TypeDef    *spi,
                                   const uint8_t  *txBuf,
                                   uint8_t        *rxBuf,
                                   uint16_t        len)
{
    uint16_t i;
    for (i = 0U; i < len; i++)
    {
        uint8_t tx   = (txBuf != NULL) ? txBuf[i] : 0xFFU;
        uint8_t rx   = DRV_SPI_TransmitReceive(spi, tx);
        if (rxBuf != NULL) { rxBuf[i] = rx; }
    }
}

void DRV_SPI_Transmit(SPI_TypeDef *spi, const uint8_t *buf, uint16_t len)
{
    DRV_SPI_TransmitReceiveBuffer(spi, buf, NULL, len);
}

void DRV_SPI_Receive(SPI_TypeDef *spi, uint8_t *buf, uint16_t len)
{
    DRV_SPI_TransmitReceiveBuffer(spi, NULL, buf, len);
}

FlagStatus DRV_SPI_GetFlag(SPI_TypeDef *spi, uint16_t flag)
{
    return SPI_I2S_GetFlagStatus(spi, flag);
}
