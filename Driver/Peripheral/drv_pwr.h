/**
  ******************************************************************************
  * @file    drv_pwr.h
  * @brief   电源管理(PWR)驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - 睡眠模式（Sleep）：CPU停止，外设继续运行，任意中断唤醒
  *   - 停止模式（Stop）：CPU+大部分外设停止，保留SRAM和寄存器
  *     唤醒源：EXTI线（GPIO/RTC/USART/I2C唤醒）
  *   - 待机模式（Standby）：最低功耗，仅备份域和看门狗运行
  *     唤醒源：NRST/WKUP引脚(PA0)/RTC报警/IWDG
  *   - PVD电源电压检测配置
  *
  * 使用示例:
  *   DRV_PWR_EnterSleepMode();              // 等待下一个中断唤醒
  *   DRV_PWR_EnterStopMode(PWR_Regulator_ON, PWR_STOPEntry_WFI);
  *   DRV_PWR_EnterStandbyMode();            // 最低功耗，复位后重新运行
  ******************************************************************************
  */

#ifndef __DRV_PWR_H
#define __DRV_PWR_H

#include "stm32f10x.h"
#include "stm32f10x_pwr.h"

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  进入睡眠模式（WFI，等待中断唤醒）
 *         唤醒后CPU继续执行，外设时钟不中断
 */
void DRV_PWR_EnterSleepMode(void);

/**
 * @brief  进入停止模式
 *         需要EXTI中断唤醒（唤醒后HSI作为系统时钟，需重新切换至PLL/HSE）
 * @param  regulator  电压调节器状态：PWR_Regulator_ON（正常）/ PWR_Regulator_LowPower（低功耗）
 * @param  entry      进入方式：PWR_STOPEntry_WFI / PWR_STOPEntry_WFE
 */
void DRV_PWR_EnterStopMode(uint32_t regulator, uint8_t entry);

/**
 * @brief  进入待机模式（最低功耗，唤醒后等同系统复位）
 */
void DRV_PWR_EnterStandbyMode(void);

/**
 * @brief  从停止模式唤醒后恢复系统时钟至72MHz（重新切换至PLL）
 *         停止模式唤醒后默认使用HSI，调用本函数恢复HSE/PLL
 */
void DRV_PWR_RestoreClockAfterStop(void);

/**
 * @brief  使能/禁止电源电压检测器（PVD）
 * @param  level  PVD阈值：PWR_PVDLevel_2V2 ~ PWR_PVDLevel_2V9
 * @param  en     ENABLE / DISABLE
 */
void DRV_PWR_PVDConfig(uint32_t level, FunctionalState en);

/**
 * @brief  获取PVD输出状态
 * @retval SET（Vdd < PVD阈值，即低压警告）/ RESET（正常）
 */
FlagStatus DRV_PWR_GetPVDOutput(void);

/**
 * @brief  清除待机标志（进入Standby前建议先清除）
 */
void DRV_PWR_ClearStandbyFlag(void);

/**
 * @brief  查询系统是否从待机模式唤醒
 * @retval SET（是）/ RESET（否）
 */
FlagStatus DRV_PWR_GetStandbyFlag(void);

/**
 * @brief  使能/禁止WKUP引脚（PA0）唤醒功能
 * @param  en  ENABLE / DISABLE
 */
void DRV_PWR_WakeUpPinCmd(FunctionalState en);

#endif /* __DRV_PWR_H */
