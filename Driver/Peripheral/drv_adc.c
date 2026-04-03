/**
  ******************************************************************************
  * @file    drv_adc.c
  * @brief   ADC外设驱动实现 - STM32F103C8T6
  ******************************************************************************
  */

#include "drv_adc.h"
#include <stddef.h>

/*---------------------------------------------------------------------------
 * 内部辅助：根据通道号配置对应GPIO为模拟输入
 *---------------------------------------------------------------------------*/
static void adc_gpio_init(uint8_t channel)
{
    GPIO_InitTypeDef gpio;
    gpio.GPIO_Mode  = GPIO_Mode_AIN;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;

    if (channel <= 7U) /* PA0~PA7 */
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        gpio.GPIO_Pin = (uint16_t)(1U << channel);
        GPIO_Init(GPIOA, &gpio);
    }
    else if (channel <= 9U) /* PB0~PB1 */
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        gpio.GPIO_Pin = (uint16_t)(1U << (channel - 8U));
        GPIO_Init(GPIOB, &gpio);
    }
    else if (channel <= 15U) /* PC0~PC5 */
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
        gpio.GPIO_Pin = (uint16_t)(1U << (channel - 10U));
        GPIO_Init(GPIOC, &gpio);
    }
    /* 通道16/17为内部通道，无需配置GPIO */
}

/*===========================================================================
 * API 实现
 *===========================================================================*/

void DRV_ADC_Init(const ADC_Cfg_t *cfg)
{
    ADC_InitTypeDef init;

    if (cfg == NULL) { return; }

    if      (cfg->adc == ADC1) { RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); }
    else if (cfg->adc == ADC2) { RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); }

    /* ADC时钟 = PCLK2 / 6 = 72MHz/6 = 12MHz（≤14MHz） */
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);

    ADC_DeInit(cfg->adc);

    init.ADC_Mode               = ADC_Mode_Independent;
    init.ADC_ScanConvMode       = DISABLE;
    init.ADC_ContinuousConvMode = DISABLE;
    init.ADC_ExternalTrigConv   = ADC_ExternalTrigConv_None;
    init.ADC_DataAlign          = ADC_DataAlign_Right;
    init.ADC_NbrOfChannel       = 1U;

    ADC_Init(cfg->adc, &init);
    ADC_Cmd(cfg->adc, ENABLE);

    /* ADC校准 */
    ADC_ResetCalibration(cfg->adc);
    while (ADC_GetResetCalibrationStatus(cfg->adc) != RESET) {}
    ADC_StartCalibration(cfg->adc);
    while (ADC_GetCalibrationStatus(cfg->adc) != RESET) {}
}

void DRV_ADC_DeInit(ADC_TypeDef *adc)
{
    ADC_Cmd(adc, DISABLE);
    ADC_DeInit(adc);
}

uint16_t DRV_ADC_ReadChannel(ADC_TypeDef *adc, uint8_t channel, uint8_t sampleTime)
{
    adc_gpio_init(channel);

    /* 使能内部温度传感器/VREFINT（通道16/17） */
    if (channel == ADC_Channel_16 || channel == ADC_Channel_17)
    {
        ADC_TempSensorVrefintCmd(ENABLE);
    }

    ADC_RegularChannelConfig(adc, channel, 1U, sampleTime);
    ADC_SoftwareStartConvCmd(adc, ENABLE);
    while (ADC_GetFlagStatus(adc, ADC_FLAG_EOC) == RESET) {}
    return ADC_GetConversionValue(adc);
}

uint16_t DRV_ADC_GetVoltage_mV(ADC_TypeDef *adc, uint8_t channel,
                                uint8_t sampleTime, uint16_t vrefMv)
{
    uint32_t raw = (uint32_t)DRV_ADC_ReadChannel(adc, channel, sampleTime);
    return (uint16_t)((raw * (uint32_t)vrefMv) / 4095U);
}

int16_t DRV_ADC_ReadTemperature(void)
{
    /* 内部温度传感器转换公式（参考RM0008）:
     * T = (V25 - Vsense) / Avg_Slope + 25
     * V25 = 1430mV, Avg_Slope = 4.3mV/°C
     * Vsense = ADC_raw * 3300mV / 4095
     */
    uint16_t raw   = DRV_ADC_ReadChannel(ADC1, ADC_Channel_16, ADC_SampleTime_239Cycles5);
    int32_t  mv    = (int32_t)((uint32_t)raw * 3300U / 4095U);
    int32_t  temp  = ((1430L - mv) * 10L) / 43L + 250L; /* *10 保留一位小数 */
    return (int16_t)temp;
}

uint16_t DRV_ADC_ReadVrefint_mV(void)
{
    uint16_t raw = DRV_ADC_ReadChannel(ADC1, ADC_Channel_17, ADC_SampleTime_239Cycles5);
    /* VREFINT典型值1200mV，用以上ADC值反推实际VDDA */
    return (uint16_t)((uint32_t)raw * 3300U / 4095U);
}

void DRV_ADC_StartConversion(ADC_TypeDef *adc)
{
    ADC_SoftwareStartConvCmd(adc, ENABLE);
}

uint16_t DRV_ADC_GetValue(ADC_TypeDef *adc)
{
    while (ADC_GetFlagStatus(adc, ADC_FLAG_EOC) == RESET) {}
    return ADC_GetConversionValue(adc);
}
