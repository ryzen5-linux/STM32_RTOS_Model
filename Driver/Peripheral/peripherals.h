/**
  ******************************************************************************
  * @file    peripherals.h
  * @brief   外设驱动库总索引头文件 - STM32F103C8T6
  *
  * 用法：
  *   在应用层或任务文件中包含本头文件即可使用全部外设驱动API。
  *   也可单独包含所需驱动的头文件以减小编译依赖。
  *
  * 外设驱动列表：
  * ┌──────────────────────────────────────────────┐
  * │ 外设   │ 头文件          │ 源文件            │
  * ├──────────────────────────────────────────────┤
  * │ GPIO   │ drv_gpio.h      │ drv_gpio.c        │ 引脚配置/读写/翻转
  * │ USART  │ drv_usart.h     │ drv_usart.c       │ 串口收发（USART1/2/3）
  * │ SPI    │ drv_spi.h       │ drv_spi.c         │ SPI主模式全双工（SPI1/2）
  * │ I2C    │ drv_i2c.h       │ drv_i2c.c         │ I2C主模式寄存器读写（I2C1/2）
  * │ TIM    │ drv_tim.h       │ drv_tim.c         │ 基础定时/PWM/输入捕获（TIM1~4）
  * │ ADC    │ drv_adc.h       │ drv_adc.c         │ 单通道采样/温度/VREF（ADC1/2）
  * │ DMA    │ drv_dma.h       │ drv_dma.c         │ DMA1通道传输控制
  * │ EXTI   │ drv_exti.h      │ drv_exti.c        │ GPIO外部中断（EXTI0~15）
  * │ RTC    │ drv_rtc.h       │ drv_rtc.c         │ 实时时钟/日期时间/报警
  * │ IWDG   │ drv_iwdg.h      │ drv_iwdg.c        │ 独立看门狗
  * │ WWDG   │ drv_wwdg.h      │ drv_wwdg.c        │ 窗口看门狗
  * │ PWR    │ drv_pwr.h       │ drv_pwr.c         │ 睡眠/停止/待机电源模式
  * │ RCC    │ drv_rcc.h       │ drv_rcc.c         │ 时钟配置/外设时钟使能
  * │ CAN    │ drv_can.h       │ drv_can.c         │ CAN1帧收发/过滤器
  * │ BKP    │ drv_bkp.h       │ drv_bkp.c         │ 备份寄存器读写
  * │ CRC    │ drv_crc.h       │ drv_crc.c         │ 硬件CRC32计算
  * │ FLASH  │ drv_flash.h     │ drv_flash.c       │ 内部Flash擦除/编程/读取
  * └──────────────────────────────────────────────┘
  *
  * 构建系统说明（CMake/Makefile）:
  *   将 Driver/Peripheral 目录下的源文件加入源文件列表。
  *   将 Driver/Peripheral 目录加入头文件搜索路径即可。
  ******************************************************************************
  */

#ifndef __PERIPHERALS_H
#define __PERIPHERALS_H

#include "drv_gpio.h"
#include "drv_usart.h"
#include "drv_spi.h"
#include "drv_i2c.h"
#include "drv_tim.h"
#include "drv_adc.h"
#include "drv_dma.h"
#include "drv_exti.h"
#include "drv_rtc.h"
#include "drv_iwdg.h"
#include "drv_wwdg.h"
#include "drv_pwr.h"
#include "drv_rcc.h"
#include "drv_can.h"
#include "drv_bkp.h"
#include "drv_crc.h"
#include "drv_flash.h"

#endif /* __PERIPHERALS_H */
