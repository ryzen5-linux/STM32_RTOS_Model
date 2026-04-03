/**
  ******************************************************************************
  * @file    drv_adc.h
  * @brief   ADC外设驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - 支持 ADC1 / ADC2
  *   - 单通道单次/连续转换
  *   - 读取原始值（0~4095）或毫伏值（基于VREF+）
  *   - 内部温度传感器和VREFINT通道读取
  *
  * ADC通道与引脚映射:
  *   ADC_Channel_0  ~ ADC_Channel_7  : PA0~PA7
  *   ADC_Channel_8  ~ ADC_Channel_9  : PB0~PB1
  *   ADC_Channel_10 ~ ADC_Channel_15 : PC0~PC5
  *   ADC_Channel_16: 内部温度传感器（仅ADC1）
  *   ADC_Channel_17: 内部参考电压VREFINT（仅ADC1）
  *
  * 使用示例:
  *   ADC_Cfg_t cfg = { ADC1, 3300 };
  *   DRV_ADC_Init(&cfg);
  *   uint16_t raw_val = DRV_ADC_ReadChannel(ADC1, ADC_Channel_0, ADC_SampleTime_55Cycles5);
  *   uint16_t mv_val  = DRV_ADC_GetVoltage_mV(ADC1, ADC_Channel_0, ADC_SampleTime_55Cycles5, 3300);
  ******************************************************************************
  */

#ifndef __DRV_ADC_H
#define __DRV_ADC_H

#include "stm32f10x.h"
#include "stm32f10x_adc.h"

/*===========================================================================
 * 数据结构
 *===========================================================================*/

/** ADC初始化配置结构体 */
typedef struct
{
    ADC_TypeDef *adc;       /**< ADC实例：ADC1 或 ADC2 */
    uint16_t     vrefMv;    /**< 参考电压（mV），典型值 3300 */
} ADC_Cfg_t;

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  初始化ADC外设（独立模式，单次转换）
 * @param  cfg  配置结构体指针
 */
void DRV_ADC_Init(const ADC_Cfg_t *cfg);

/**
 * @brief  关闭并复位ADC外设
 * @param  adc  ADC实例
 */
void DRV_ADC_DeInit(ADC_TypeDef *adc);

/**
 * @brief  读取指定通道的ADC原始值（阻塞单次转换）
 * @param  adc         ADC实例
 * @param  channel     通道号，如 ADC_Channel_0 ~ ADC_Channel_17
 * @param  sampleTime  采样时间，如 ADC_SampleTime_55Cycles5
 * @retval 12位ADC原始值（0~4095）
 */
uint16_t DRV_ADC_ReadChannel(ADC_TypeDef *adc, uint8_t channel, uint8_t sampleTime);

/**
 * @brief  读取指定通道并转换为毫伏电压值
 * @param  adc         ADC实例
 * @param  channel     通道号
 * @param  sampleTime  采样时间
 * @param  vrefMv      参考电压（mV）
 * @retval 电压值（mV）
 */
uint16_t DRV_ADC_GetVoltage_mV(ADC_TypeDef *adc, uint8_t channel,
                                uint8_t sampleTime, uint16_t vrefMv);

/**
 * @brief  读取内部温度传感器（摄氏度 * 10，如 250 = 25.0°C）
 *         仅ADC1支持，采样时间建议 >= ADC_SampleTime_239Cycles5
 * @retval 温度 * 10 (°C)，精度约 ±1.5°C
 */
int16_t DRV_ADC_ReadTemperature(void);

/**
 * @brief  读取内部参考电压VREFINT（mV）
 *         仅ADC1支持，典型值 1200mV
 * @retval VREFINT电压（mV）
 */
uint16_t DRV_ADC_ReadVrefint_mV(void);

/**
 * @brief  启动ADC多通道扫描（注入/规则序列），调用前需完整配置
 *         此函数仅触发软件启动，结合中断/DMA使用
 * @param  adc  ADC实例
 */
void DRV_ADC_StartConversion(ADC_TypeDef *adc);

/**
 * @brief  等待ADC转换完成并返回数据寄存器值
 * @param  adc  ADC实例
 * @retval DR寄存器值
 */
uint16_t DRV_ADC_GetValue(ADC_TypeDef *adc);

#endif /* __DRV_ADC_H */
