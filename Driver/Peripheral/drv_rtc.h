/**
  ******************************************************************************
  * @file    drv_rtc.h
  * @brief   RTC实时时钟驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - RTC初始化（LSE/LSI时钟源可选）
  *   - 以Unix时间戳形式读写时间（秒计数器）
  *   - 以年月日时分秒结构体读写时间（需应用层处理时间与时间戳转换）
  *   - 报警功能配置
  *   - 秒中断使能
  *   - 备份寄存器标志判断（防止重复初始化）
  *
  * 说明:
  *   STM32F103 RTC底层为32位秒计数器，本驱动将时间格式化为结构体供上层使用。
  *   要求外部32.768kHz LSE晶振或内部LSI（精度较低）。
  *
  * 使用示例:
  *   RTC_DateTime_t dt = { 2026, 4, 2, 12, 30, 0 };
  *   DRV_RTC_Init(RTC_CLOCK_LSE);
  *   DRV_RTC_SetDateTime(&dt);
  *   DRV_RTC_GetDateTime(&dt);  // 读回时间
  ******************************************************************************
  */

#ifndef __DRV_RTC_H
#define __DRV_RTC_H

#include "stm32f10x.h"
#include "stm32f10x_rtc.h"
#include "stm32f10x_bkp.h"
#include "stm32f10x_pwr.h"
#include "stm32f10x_rcc.h"

/*===========================================================================
 * 枚举与宏
 *===========================================================================*/

/** RTC时钟源选择 */
typedef enum
{
    RTC_CLOCK_LSE = 0U, /**< 外部低速32.768kHz晶振（精度高，推荐） */
  RTC_CLOCK_LSI = 1U, /**< 内部低速约40kHz RC振荡器（无需外部晶振） */
  RTC_CLOCK_HSE_DIV128 = 2U /**< HSE/128 作为RTC时钟，适合无LSE且LSI不稳定场景 */
} RTC_ClockSrc_t;

/** BKP寄存器用于标记RTC是否已初始化 */
#define RTC_BKP_REG          BKP_DR1
#define RTC_BKP_INIT_FLAG    (0xA5A5U)

/*===========================================================================
 * 数据结构
 *===========================================================================*/

/** 日期时间结构体 */
typedef struct
{
    uint16_t year;   /**< 年，如 2026 */
    uint8_t  month;  /**< 月，1~12 */
    uint8_t  day;    /**< 日，1~31 */
    uint8_t  hour;   /**< 时，0~23 */
    uint8_t  minute; /**< 分，0~59 */
    uint8_t  second; /**< 秒，0~59 */
} RTC_DateTime_t;

/*===========================================================================
 * 返回状态码
 *===========================================================================*/
#define DRV_RTC_OK       (0)
#define DRV_RTC_TIMEOUT  (-1)

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  初始化RTC（若BKP标志存在则跳过重置，仅重新使能）
 * @param  src  时钟源：RTC_CLOCK_LSE 或 RTC_CLOCK_LSI
 * @retval DRV_RTC_OK 成功，DRV_RTC_TIMEOUT LSE超时（可换用LSI）
 */
int DRV_RTC_Init(RTC_ClockSrc_t src);

/**
 * @brief  将日期时间结构体转换为时间戳并写入RTC计数器
 * @param  dt  日期时间结构体指针
 */
void DRV_RTC_SetDateTime(const RTC_DateTime_t *dt);

/**
 * @brief  从RTC计数器读取时间戳并转换为日期时间结构体
 * @param  dt  日期时间结构体指针（输出）
 */
void DRV_RTC_GetDateTime(RTC_DateTime_t *dt);

/**
 * @brief  直接写入RTC秒计数器（Unix时间戳）
 * @param  timestamp  秒数（从1970-01-01起）
 */
void DRV_RTC_SetTimestamp(uint32_t timestamp);

/**
 * @brief  读取RTC秒计数器
 * @retval Unix时间戳（秒）
 */
uint32_t DRV_RTC_GetTimestamp(void);

/**
 * @brief  设置RTC报警时间（时间戳形式）
 * @param  timestamp  报警时刻的Unix时间戳
 */
void DRV_RTC_SetAlarm(uint32_t timestamp);

/**
 * @brief  使能/禁止RTC秒中断
 * @param  en  ENABLE / DISABLE
 */
void DRV_RTC_SecondITConfig(FunctionalState en);

/**
 * @brief  使能/禁止RTC报警中断
 * @param  en  ENABLE / DISABLE
 */
void DRV_RTC_AlarmITConfig(FunctionalState en);

/**
 * @brief  清除RTC中断标志（在中断服务函数中调用）
 * @param  flag  RTC_IT_SEC / RTC_IT_ALR / RTC_IT_OW
 */
void DRV_RTC_ClearITFlag(uint16_t flag);

#endif /* __DRV_RTC_H */
