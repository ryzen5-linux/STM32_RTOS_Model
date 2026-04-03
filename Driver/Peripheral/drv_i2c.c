/**
  ******************************************************************************
  * @file    drv_i2c.c
  * @brief   I2C外设驱动实现 - STM32F103C8T6
  *
  * 引脚映射（默认，无重映射）:
  *   I2C1: SCL=PB6, SDA=PB7
  *   I2C2: SCL=PB10, SDA=PB11
  ******************************************************************************
  */

#include "drv_i2c.h"
#include <stddef.h>

#define I2C_TIMEOUT_CNT  (100000U)  /* 超时计数上限 */

/*---------------------------------------------------------------------------
 * 内部辅助：等待事件（带超时）
 *---------------------------------------------------------------------------*/
static int i2c_wait_event(I2C_TypeDef *i2c, uint32_t event)
{
    uint32_t cnt = 0U;
    while (!I2C_CheckEvent(i2c, event))
    {
        if (++cnt >= I2C_TIMEOUT_CNT) { return DRV_I2C_TIMEOUT; }
    }
    return DRV_I2C_OK;
}

/*---------------------------------------------------------------------------
 * 内部辅助：等待标志位（带超时）
 *---------------------------------------------------------------------------*/
static int i2c_wait_flag(I2C_TypeDef *i2c, uint32_t flag, FlagStatus state)
{
    uint32_t cnt = 0U;
    while (I2C_GetFlagStatus(i2c, flag) != state)
    {
        if (++cnt >= I2C_TIMEOUT_CNT) { return DRV_I2C_TIMEOUT; }
    }
    return DRV_I2C_OK;
}

/*---------------------------------------------------------------------------
 * 内部辅助：GPIO和时钟初始化
 *---------------------------------------------------------------------------*/
static void i2c_gpio_init(I2C_TypeDef *i2c)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOB, ENABLE);

    /* I2C引脚需配置为开漏复用输出，外部需上拉电阻 */
    gpio.GPIO_Mode  = GPIO_Mode_AF_OD;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;

    if (i2c == I2C1)
    {
        gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; /* SCL=PB6, SDA=PB7 */
        GPIO_Init(GPIOB, &gpio);
    }
    else if (i2c == I2C2)
    {
        gpio.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; /* SCL=PB10, SDA=PB11 */
        GPIO_Init(GPIOB, &gpio);
    }
}

static void i2c_rcc_enable(I2C_TypeDef *i2c)
{
    if      (i2c == I2C1) { RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE); }
    else if (i2c == I2C2) { RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE); }
}

/*===========================================================================
 * API 实现
 *===========================================================================*/

void DRV_I2C_Init(const I2C_Cfg_t *cfg)
{
    I2C_InitTypeDef init;

    if (cfg == NULL) { return; }

    i2c_rcc_enable(cfg->i2c);
    i2c_gpio_init(cfg->i2c);

    I2C_DeInit(cfg->i2c);

    init.I2C_Mode                = I2C_Mode_I2C;
    init.I2C_DutyCycle           = I2C_DutyCycle_2;
    init.I2C_OwnAddress1         = 0x00U;
    init.I2C_Ack                 = I2C_Ack_Enable;
    init.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    init.I2C_ClockSpeed          = cfg->clockSpeed;

    I2C_Init(cfg->i2c, &init);
    I2C_Cmd(cfg->i2c, ENABLE);
}

void DRV_I2C_DeInit(I2C_TypeDef *i2c)
{
    I2C_Cmd(i2c, DISABLE);
    I2C_DeInit(i2c);
}

int DRV_I2C_Write(I2C_TypeDef *i2c, uint8_t devAddr,
                  const uint8_t *buf, uint16_t len)
{
    uint16_t i;
    int ret;

    I2C_GenerateSTART(i2c, ENABLE);
    ret = i2c_wait_event(i2c, I2C_EVENT_MASTER_MODE_SELECT);
    if (ret != DRV_I2C_OK) { I2C_GenerateSTOP(i2c, ENABLE); return ret; }

    I2C_Send7bitAddress(i2c, (uint8_t)(devAddr << 1U), I2C_Direction_Transmitter);
    ret = i2c_wait_event(i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
    if (ret != DRV_I2C_OK) { I2C_GenerateSTOP(i2c, ENABLE); return ret; }

    for (i = 0U; i < len; i++)
    {
        I2C_SendData(i2c, buf[i]);
        ret = i2c_wait_event(i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
        if (ret != DRV_I2C_OK) { I2C_GenerateSTOP(i2c, ENABLE); return ret; }
    }

    I2C_GenerateSTOP(i2c, ENABLE);
    ret = i2c_wait_flag(i2c, I2C_FLAG_BUSY, RESET);
    return ret;
}

int DRV_I2C_Read(I2C_TypeDef *i2c, uint8_t devAddr,
                 uint8_t *buf, uint16_t len)
{
    uint16_t i;
    int ret;

    I2C_AcknowledgeConfig(i2c, ENABLE);
    I2C_GenerateSTART(i2c, ENABLE);
    ret = i2c_wait_event(i2c, I2C_EVENT_MASTER_MODE_SELECT);
    if (ret != DRV_I2C_OK) { I2C_GenerateSTOP(i2c, ENABLE); return ret; }

    I2C_Send7bitAddress(i2c, (uint8_t)(devAddr << 1U), I2C_Direction_Receiver);
    ret = i2c_wait_event(i2c, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED);
    if (ret != DRV_I2C_OK) { I2C_GenerateSTOP(i2c, ENABLE); return ret; }

    for (i = 0U; i < len; i++)
    {
        if (i == (uint16_t)(len - 1U))
        {
            I2C_AcknowledgeConfig(i2c, DISABLE); /* 最后一字节发NACK */
            I2C_GenerateSTOP(i2c, ENABLE);
        }
        ret = i2c_wait_event(i2c, I2C_EVENT_MASTER_BYTE_RECEIVED);
        if (ret != DRV_I2C_OK) { return ret; }
        buf[i] = I2C_ReceiveData(i2c);
    }

    I2C_AcknowledgeConfig(i2c, ENABLE);
    return DRV_I2C_OK;
}

int DRV_I2C_WriteReg(I2C_TypeDef *i2c, uint8_t devAddr,
                      uint8_t regAddr, const uint8_t *buf, uint16_t len)
{
    uint16_t i;
    int ret;

    I2C_GenerateSTART(i2c, ENABLE);
    ret = i2c_wait_event(i2c, I2C_EVENT_MASTER_MODE_SELECT);
    if (ret != DRV_I2C_OK) { I2C_GenerateSTOP(i2c, ENABLE); return ret; }

    I2C_Send7bitAddress(i2c, (uint8_t)(devAddr << 1U), I2C_Direction_Transmitter);
    ret = i2c_wait_event(i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
    if (ret != DRV_I2C_OK) { I2C_GenerateSTOP(i2c, ENABLE); return ret; }

    /* 发送寄存器地址 */
    I2C_SendData(i2c, regAddr);
    ret = i2c_wait_event(i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
    if (ret != DRV_I2C_OK) { I2C_GenerateSTOP(i2c, ENABLE); return ret; }

    for (i = 0U; i < len; i++)
    {
        I2C_SendData(i2c, buf[i]);
        ret = i2c_wait_event(i2c, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
        if (ret != DRV_I2C_OK) { I2C_GenerateSTOP(i2c, ENABLE); return ret; }
    }

    I2C_GenerateSTOP(i2c, ENABLE);
    ret = i2c_wait_flag(i2c, I2C_FLAG_BUSY, RESET);
    return ret;
}

int DRV_I2C_ReadReg(I2C_TypeDef *i2c, uint8_t devAddr,
                     uint8_t regAddr, uint8_t *buf, uint16_t len)
{
    int ret;
    uint8_t reg = regAddr;
    ret = DRV_I2C_Write(i2c, devAddr, &reg, 1U);
    if (ret != DRV_I2C_OK) { return ret; }
    return DRV_I2C_Read(i2c, devAddr, buf, len);
}

int DRV_I2C_IsDeviceReady(I2C_TypeDef *i2c, uint8_t devAddr)
{
    uint32_t cnt = 0U;

    I2C_GenerateSTART(i2c, ENABLE);
    while (I2C_GetFlagStatus(i2c, I2C_FLAG_SB) == RESET)
    {
        if (++cnt >= I2C_TIMEOUT_CNT) { I2C_GenerateSTOP(i2c, ENABLE); return DRV_I2C_TIMEOUT; }
    }

    I2C_Send7bitAddress(i2c, (uint8_t)(devAddr << 1U), I2C_Direction_Transmitter);
    cnt = 0U;
    while (!I2C_CheckEvent(i2c, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
    {
        if (I2C_GetFlagStatus(i2c, I2C_FLAG_AF) != RESET)
        {
            I2C_ClearFlag(i2c, I2C_FLAG_AF);
            I2C_GenerateSTOP(i2c, ENABLE);
            return DRV_I2C_TIMEOUT;
        }
        if (++cnt >= I2C_TIMEOUT_CNT) { I2C_GenerateSTOP(i2c, ENABLE); return DRV_I2C_TIMEOUT; }
    }

    I2C_GenerateSTOP(i2c, ENABLE);
    return DRV_I2C_OK;
}
