/**
  ******************************************************************************
  * @file    drv_rtc.c
  * @brief   RTC实时时钟驱动实现 - STM32F103C8T6
  *
  * 时间戳基准：1970-01-01 00:00:00 UTC（Unix Epoch）
  ******************************************************************************
  */

#include "drv_rtc.h"
#include <stddef.h>

#define RTC_WAIT_CNT  (0x800000U)

static int rtc_wait_for_last_task(void)
{
    uint32_t cnt = 0U;

    while ((RTC->CRL & RTC_FLAG_RTOFF) == (uint16_t)RESET)
    {
        if (++cnt >= RTC_WAIT_CNT)
        {
            return DRV_RTC_TIMEOUT;
        }
    }

    return DRV_RTC_OK;
}

static int rtc_wait_for_synchro(void)
{
    uint32_t cnt = 0U;

    RTC->CRL &= (uint16_t)~RTC_FLAG_RSF;
    while ((RTC->CRL & RTC_FLAG_RSF) == (uint16_t)RESET)
    {
        if (++cnt >= RTC_WAIT_CNT)
        {
            return DRV_RTC_TIMEOUT;
        }
    }

    return DRV_RTC_OK;
}

/*---------------------------------------------------------------------------
 * 内部辅助：判断是否为闰年
 *---------------------------------------------------------------------------*/
static uint8_t is_leap_year(uint16_t year)
{
    return ((year % 4U == 0U && year % 100U != 0U) || (year % 400U == 0U)) ? 1U : 0U;
}

static const uint8_t days_in_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};

/*---------------------------------------------------------------------------
 * 内部辅助：日期时间 <-> Unix时间戳 转换
 *---------------------------------------------------------------------------*/
static uint32_t datetime_to_timestamp(const RTC_DateTime_t *dt)
{
    uint32_t days = 0U;
    uint16_t y;
    uint8_t  m;

    /* 统计1970年至(year-1)年的天数 */
    for (y = 1970U; y < dt->year; y++)
    {
        days += is_leap_year(y) ? 366U : 365U;
    }
    /* 统计本年1月至(month-1)月的天数 */
    for (m = 0U; m < (uint8_t)(dt->month - 1U); m++)
    {
        days += days_in_month[m];
        if (m == 1U && is_leap_year(dt->year)) { days++; } /* 2月闰年补1天 */
    }
    days += (uint32_t)(dt->day - 1U);

    return days * 86400U + (uint32_t)dt->hour * 3600U
           + (uint32_t)dt->minute * 60U + (uint32_t)dt->second;
}

static void timestamp_to_datetime(uint32_t ts, RTC_DateTime_t *dt)
{
    uint32_t remaining;
    uint16_t y;
    uint8_t  m;
    uint32_t days_year;

    dt->second = (uint8_t)(ts % 60U); ts /= 60U;
    dt->minute = (uint8_t)(ts % 60U); ts /= 60U;
    dt->hour   = (uint8_t)(ts % 24U); ts /= 24U;

    /* ts现在是从1970-01-01起的天数 */
    y = 1970U;
    while (1)
    {
        days_year = is_leap_year(y) ? 366U : 365U;
        if (ts < days_year) { break; }
        ts -= days_year;
        y++;
    }
    dt->year = y;

    /* 月份 */
    remaining = ts;
    for (m = 0U; m < 12U; m++)
    {
        uint32_t dim = days_in_month[m];
        if (m == 1U && is_leap_year(y)) { dim++; }
        if (remaining < dim) { break; }
        remaining -= dim;
    }
    dt->month = (uint8_t)(m + 1U);
    dt->day   = (uint8_t)(remaining + 1U);
}

/*===========================================================================
 * API 实现
 *===========================================================================*/

int DRV_RTC_Init(RTC_ClockSrc_t src)
{
    uint32_t cnt;
    uint32_t prescaler;
    uint8_t hadInitFlag;

    /* 使能电源和备份接口时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    /* 若BKP标志已存在，说明RTC已配置，无需重新初始化 */
    hadInitFlag = (BKP_ReadBackupRegister(RTC_BKP_REG) == RTC_BKP_INIT_FLAG) ? 1U : 0U;
    if (hadInitFlag != 0U)
    {
        if (rtc_wait_for_synchro() == DRV_RTC_OK)
        {
            return DRV_RTC_OK;
        }

        /* 备份域中可能残留了初始化标志，但RTC时钟并未真正工作，转为完整重建。 */
        BKP_WriteBackupRegister(RTC_BKP_REG, 0U);
    }

    /* 重置备份域，重新配置RTC时钟源 */
    BKP_DeInit();
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);

    if (src == RTC_CLOCK_LSE)
    {
        RCC_LSEConfig(RCC_LSE_ON);
        cnt = 0U;
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
        {
            if (++cnt >= RTC_WAIT_CNT) { return DRV_RTC_TIMEOUT; }
        }
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
        prescaler = 32767U;
    }
    else if (src == RTC_CLOCK_LSI)
    {
        RCC_LSICmd(ENABLE);
        cnt = 0U;
        while (RCC_GetFlagStatus(RCC_FLAG_LSIRDY) == RESET)
        {
            if (++cnt >= RTC_WAIT_CNT) { return DRV_RTC_TIMEOUT; }
        }
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
        prescaler = 39999U;
    }
    else
    {
        if (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
        {
            RCC_HSEConfig(RCC_HSE_ON);
            cnt = 0U;
            while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET)
            {
                if (++cnt >= RTC_WAIT_CNT) { return DRV_RTC_TIMEOUT; }
            }
        }

        RCC_RTCCLKConfig(RCC_RTCCLKSource_HSE_Div128);
        prescaler = (HSE_VALUE / 128U) - 1U;
    }

    RCC_RTCCLKCmd(ENABLE);
    if (rtc_wait_for_synchro() != DRV_RTC_OK) { return DRV_RTC_TIMEOUT; }
    if (rtc_wait_for_last_task() != DRV_RTC_OK) { return DRV_RTC_TIMEOUT; }

    /* 设置分频到1Hz。 */
    RTC_SetPrescaler(prescaler);
    if (rtc_wait_for_last_task() != DRV_RTC_OK) { return DRV_RTC_TIMEOUT; }

    /* 写入初始化完成标志到BKP寄存器 */
    BKP_WriteBackupRegister(RTC_BKP_REG, RTC_BKP_INIT_FLAG);

    return DRV_RTC_OK;
}

void DRV_RTC_SetDateTime(const RTC_DateTime_t *dt)
{
    if (dt == NULL) { return; }
    PWR_BackupAccessCmd(ENABLE);
    if (rtc_wait_for_last_task() != DRV_RTC_OK) { return; }
    RTC_SetCounter(datetime_to_timestamp(dt));
    (void)rtc_wait_for_last_task();
}

void DRV_RTC_GetDateTime(RTC_DateTime_t *dt)
{
    if (dt == NULL) { return; }
    timestamp_to_datetime(RTC_GetCounter(), dt);
}

void DRV_RTC_SetTimestamp(uint32_t timestamp)
{
    PWR_BackupAccessCmd(ENABLE);
    if (rtc_wait_for_last_task() != DRV_RTC_OK) { return; }
    RTC_SetCounter(timestamp);
    (void)rtc_wait_for_last_task();
}

uint32_t DRV_RTC_GetTimestamp(void)
{
    return RTC_GetCounter();
}

void DRV_RTC_SetAlarm(uint32_t timestamp)
{
    PWR_BackupAccessCmd(ENABLE);
    if (rtc_wait_for_last_task() != DRV_RTC_OK) { return; }
    RTC_SetAlarm(timestamp);
    (void)rtc_wait_for_last_task();
}

void DRV_RTC_SecondITConfig(FunctionalState en)
{
    if (rtc_wait_for_last_task() != DRV_RTC_OK) { return; }
    RTC_ITConfig(RTC_IT_SEC, en);
    (void)rtc_wait_for_last_task();
}

void DRV_RTC_AlarmITConfig(FunctionalState en)
{
    if (rtc_wait_for_last_task() != DRV_RTC_OK) { return; }
    RTC_ITConfig(RTC_IT_ALR, en);
    (void)rtc_wait_for_last_task();
}

void DRV_RTC_ClearITFlag(uint16_t flag)
{
    RTC_ClearITPendingBit(flag);
    (void)rtc_wait_for_last_task();
}
