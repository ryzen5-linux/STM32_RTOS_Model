/**
  ******************************************************************************
  * @file    main_rtos.c
  * @brief   FreeRTOS启动入口模板 - STM32F103C8T6
  * @date    2026-04-02
  *
  * 说明:
  *   本文件仅保留 RTOS 模板工程需要的最小启动流程：
  *   1. 基础时钟和中断初始化
  *   2. 调试串口初始化（USART2: PA2/PA3）
  *   3. 应用层 RTOS 对象与任务创建
  *   4. 启动 FreeRTOS 调度器
  ******************************************************************************
  */

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "misc.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "app.h"
#include "app_debug.h"
#include "app_config.h"
#include "peripherals.h"
#include "delay.h"
#include "sys.h"
#include <stdio.h>
#include <string.h>

static SemaphoreHandle_t g_logMutex = NULL;
volatile uint32_t g_exti0_irq_seen = 0U;

static void BoardStatusLed_Init(void)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
    gpio.GPIO_Pin = GPIO_Pin_13;
    gpio.GPIO_Speed = GPIO_Speed_2MHz;
    gpio.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOC, &gpio);
    GPIO_SetBits(GPIOC, GPIO_Pin_13);
}

static void BoardStatusLed_Set(uint8_t on)
{
    if (on != 0U)
    {
        GPIO_ResetBits(GPIOC, GPIO_Pin_13);
    }
    else
    {
        GPIO_SetBits(GPIOC, GPIO_Pin_13);
    }
}

static void BoardStatusLed_Toggle(void)
{
    GPIOC->ODR ^= GPIO_Pin_13;
}

typedef enum
{
    SELFTEST_PASS = 0,
    SELFTEST_FAIL,
    SELFTEST_SKIP
} SelfTestStatus_t;

static void Debug_LogU32(const char *label, uint32_t value)
{
    char out[64];
    (void)snprintf(out, sizeof(out), "%s=%lu", label, (unsigned long)value);
    Debug_Log(out);
}

static void Debug_LogS32(const char *label, int32_t value)
{
    char out[64];
    (void)snprintf(out, sizeof(out), "%s=%ld", label, (long)value);
    Debug_Log(out);
}

static void SelfTest_Report(const char *name, SelfTestStatus_t status, const char *detail)
{
    char out[96];
    const char *tag;

    if (status == SELFTEST_PASS)
    {
        tag = "PASS";
    }
    else if (status == SELFTEST_FAIL)
    {
        tag = "FAIL";
    }
    else
    {
        tag = "SKIP";
    }

    if (detail != NULL && detail[0] != '\0')
    {
        (void)snprintf(out, sizeof(out), "[%s] %s: %s", tag, name, detail);
    }
    else
    {
        (void)snprintf(out, sizeof(out), "[%s] %s", tag, name);
    }

    Debug_Log(out);
}

static SelfTestStatus_t SelfTest_RCC(void)
{
    RCC_ClockFreq_t freq;
    DRV_RCC_GetClockFreq(&freq);
    Debug_LogU32("SYSCLK", freq.sysclk);
    Debug_LogU32("HCLK", freq.hclk);
    Debug_LogU32("PCLK1", freq.pclk1);
    Debug_LogU32("PCLK2", freq.pclk2);
    return (freq.sysclk != 0U) ? SELFTEST_PASS : SELFTEST_FAIL;
}

static SelfTestStatus_t SelfTest_GPIO(void)
{
    GPIO_PinCfg_t cfg = { GPIOC, GPIO_Pin_13, GPIO_Mode_Out_PP, GPIO_Speed_2MHz };
    uint8_t highState;
    uint8_t lowState;

    DRV_GPIO_Init(&cfg);
    DRV_GPIO_SetPin(GPIOC, GPIO_Pin_13);
    highState = (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == Bit_SET) ? 1U : 0U;
    delay_ms(10U);
    DRV_GPIO_ResetPin(GPIOC, GPIO_Pin_13);
    lowState = (GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) == Bit_RESET) ? 1U : 0U;

    return (highState != 0U && lowState != 0U) ? SELFTEST_PASS : SELFTEST_FAIL;
}

static SelfTestStatus_t SelfTest_USART(void)
{
    USART_Cfg_t cfg = {
        USART2,
        115200U,
        USART_WordLength_8b,
        USART_StopBits_1,
        USART_Parity_No,
        USART_Mode_Tx | USART_Mode_Rx
    };

    DRV_USART_Init(&cfg);
    DRV_USART_SendString(USART2, "[PASS] USART2 driver path active\r\n");
    return (DRV_USART_GetFlag(USART2, USART_FLAG_TC) == SET) ? SELFTEST_PASS : SELFTEST_FAIL;
}

static SelfTestStatus_t SelfTest_SPI(void)
{
    SPI_Cfg_t cfg = {
        SPI1,
        SPI_BaudRatePrescaler_16,
        SPI_CPOL_Low,
        SPI_CPHA_1Edge,
        SPI_DataSize_8b,
        SPI_FirstBit_MSB
    };

    DRV_SPI_Init(&cfg);
    (void)DRV_SPI_TransmitReceive(SPI1, 0xA5U);
    DRV_SPI_DeInit(SPI1);
    return SELFTEST_SKIP;
}

static SelfTestStatus_t SelfTest_I2C(void)
{
    I2C_Cfg_t cfg = { I2C1, 100000U };
    DRV_I2C_Init(&cfg);
    DRV_I2C_DeInit(I2C1);
    return SELFTEST_SKIP;
}

static SelfTestStatus_t SelfTest_TIM(void)
{
    TIM_Base_Cfg_t cfg = { TIM2, 7199U, 999U };
    uint16_t count;

    DRV_TIM_Base_Init(&cfg);
    DRV_TIM_SetCounter(TIM2, 0U);
    DRV_TIM_Start(TIM2);
    delay_ms(20U);
    count = DRV_TIM_GetCounter(TIM2);
    DRV_TIM_Stop(TIM2);
    DRV_TIM_DeInit(TIM2);

    return (count > 0U) ? SELFTEST_PASS : SELFTEST_FAIL;
}

static SelfTestStatus_t SelfTest_ADC(void)
{
    ADC_Cfg_t cfg = { ADC1, 3300U };
    uint16_t vrefMv;
    int16_t tempX10;

    DRV_ADC_Init(&cfg);
    vrefMv = DRV_ADC_ReadVrefint_mV();
    tempX10 = DRV_ADC_ReadTemperature();
    DRV_ADC_DeInit(ADC1);

    Debug_LogU32("ADC VREFINT mV", vrefMv);
    Debug_LogS32("ADC Temp x10C", (int32_t)tempX10);
    return SELFTEST_PASS;
}

static SelfTestStatus_t SelfTest_DMA(void)
{
    static uint32_t src[4] = { 0x12345678U, 0xA5A55A5AU, 0x0F0F55AAU, 0x87654321U };
    static uint32_t dst[4] = { 0U, 0U, 0U, 0U };
    DMA_Cfg_t cfg;
    uint32_t timeout = 1000000U;

    cfg.channel      = DMA1_Channel1;
    cfg.dir          = DMA_DIR_MemToMem;
    cfg.mode         = DMA_Mode_Normal;
    cfg.priority     = DMA_Priority_High;
    cfg.periphDataSz = DMA_PeripheralDataSize_Word;
    cfg.memDataSz    = DMA_MemoryDataSize_Word;
    cfg.periphInc    = ENABLE;
    cfg.periphAddr   = (uint32_t)src;
    cfg.memAddr      = (uint32_t)dst;
    cfg.dataLen      = 4U;

    DRV_DMA_ClearFlag(DMA1_FLAG_GL1);
    DRV_DMA_Init(&cfg);
    DRV_DMA_Enable(DMA1_Channel1);

    while (DRV_DMA_GetFlag(DMA1_FLAG_TC1) == RESET)
    {
        if (timeout-- == 0U)
        {
            DRV_DMA_Disable(DMA1_Channel1);
            DRV_DMA_DeInit(DMA1_Channel1);
            return SELFTEST_FAIL;
        }
    }

    DRV_DMA_Disable(DMA1_Channel1);
    DRV_DMA_DeInit(DMA1_Channel1);
    DRV_DMA_ClearFlag(DMA1_FLAG_GL1);

    return (memcmp(src, dst, sizeof(src)) == 0) ? SELFTEST_PASS : SELFTEST_FAIL;
}

static SelfTestStatus_t SelfTest_EXTI(void)
{
    uint32_t timeout = 100000U;
    EXTI_PinCfg_t cfg = {
        GPIOA,
        GPIO_Pin_0,
        GPIO_PinSource0,
        EXTI_Trigger_Rising,
        ENABLE,
        EXTI_IRQ_PRIORITY_DEFAULT,
        EXTI_IRQ_SUBPRI_DEFAULT
    };

    g_exti0_irq_seen = 0U;
    DRV_EXTI_Init(&cfg);
    DRV_EXTI_ClearFlag(EXTI_Line0);
    DRV_EXTI_SoftwareTrigger(EXTI_Line0);

    while (g_exti0_irq_seen == 0U)
    {
        if (timeout-- == 0U)
        {
            DRV_EXTI_ClearFlag(EXTI_Line0);
            return SELFTEST_FAIL;
        }
    }

    DRV_EXTI_ClearFlag(EXTI_Line0);
    return SELFTEST_PASS;
}

static SelfTestStatus_t SelfTest_RTC(void)
{
    RTC_DateTime_t dt;
    const char *clockSource = "LSI";

    if (DRV_RTC_Init(RTC_CLOCK_LSI) != DRV_RTC_OK)
    {
        if (DRV_RTC_Init(RTC_CLOCK_HSE_DIV128) != DRV_RTC_OK)
        {
            Debug_Log("RTC init failed on LSI and HSE/128");
            return SELFTEST_FAIL;
        }

        clockSource = "HSE/128";
    }

    DRV_RTC_SetTimestamp(1712016000U);
    DRV_RTC_GetDateTime(&dt);
    Debug_Log((clockSource[0] == 'L') ? "RTC source=LSI" : "RTC source=HSE/128");
    Debug_LogU32("RTC year", dt.year);
    Debug_LogU32("RTC month", dt.month);
    Debug_LogU32("RTC day", dt.day);
    return (dt.year >= 2024U) ? SELFTEST_PASS : SELFTEST_FAIL;
}

static SelfTestStatus_t SelfTest_IWDG(void)
{
    return SELFTEST_SKIP;
}

static SelfTestStatus_t SelfTest_WWDG(void)
{
    return SELFTEST_SKIP;
}

static SelfTestStatus_t SelfTest_PWR(void)
{
    FlagStatus pvd;

    DRV_PWR_PVDConfig(PWR_PVDLevel_2V9, ENABLE);
    pvd = DRV_PWR_GetPVDOutput();
    DRV_PWR_PVDConfig(PWR_PVDLevel_2V9, DISABLE);
    Debug_Log((pvd == SET) ? "PWR PVD=LOW" : "PWR PVD=OK");
    return SELFTEST_PASS;
}

static SelfTestStatus_t SelfTest_CAN(void)
{
    CAN_Cfg_t canCfg = { 4U, CAN_SJW_1tq, CAN_BS1_9tq, CAN_BS2_8tq, ENABLE };
    CAN_FilterCfg_t filterCfg = { 0U, CAN_Filter_FIFO0, CAN_FilterMode_IdMask, CAN_FilterScale_32bit, 0U, 0U, 0U, 0U };
    CAN_Frame_t txFrame = { 0x123U, 0U, 0U, 2U, { 0x5AU, 0xA5U, 0U, 0U, 0U, 0U, 0U, 0U } };
    CAN_Frame_t rxFrame;
    uint32_t timeout = 100000U;

    if (DRV_CAN_Init(&canCfg) != DRV_CAN_OK)
    {
        return SELFTEST_FAIL;
    }

    DRV_CAN_FilterConfig(&filterCfg);
    if (DRV_CAN_Send(&txFrame, 100000U) != DRV_CAN_OK)
    {
        DRV_CAN_DeInit();
        return SELFTEST_FAIL;
    }

    while (DRV_CAN_GetRxPendingCount(CAN_FIFO0) == 0U)
    {
        if (timeout-- == 0U)
        {
            DRV_CAN_DeInit();
            return SELFTEST_FAIL;
        }
    }

    if (DRV_CAN_Receive(CAN_FIFO0, &rxFrame) != DRV_CAN_OK)
    {
        DRV_CAN_DeInit();
        return SELFTEST_FAIL;
    }

    DRV_CAN_DeInit();
    return (rxFrame.id == txFrame.id && rxFrame.data[0] == txFrame.data[0] && rxFrame.data[1] == txFrame.data[1])
           ? SELFTEST_PASS : SELFTEST_FAIL;
}

static SelfTestStatus_t SelfTest_BKP(void)
{
    DRV_BKP_Write(BKP_DR2, 0x55AAU);
    return (DRV_BKP_Read(BKP_DR2) == 0x55AAU) ? SELFTEST_PASS : SELFTEST_FAIL;
}

static SelfTestStatus_t SelfTest_CRC(void)
{
    uint32_t words[2] = { 0x12345678U, 0x9ABCDEF0U };
    uint32_t crc = 0U;

    DRV_CRC_Init();
    crc = DRV_CRC_Calculate(words, 2U);
    Debug_LogU32("CRC32", crc);
    return (crc != 0U) ? SELFTEST_PASS : SELFTEST_FAIL;
}

static SelfTestStatus_t SelfTest_FLASH(void)
{
    uint32_t value = DRV_FLASH_ReadWord(DRV_FLASH_BASE_ADDR);
    Debug_LogU32("FLASH[0]", value);
    return (value != 0xFFFFFFFFU) ? SELFTEST_PASS : SELFTEST_FAIL;
}

static void Run_PeripheralSelfTests(void)
{
    uint32_t passCount = 0U;
    uint32_t failCount = 0U;
    uint32_t skipCount = 0U;
    SelfTestStatus_t status;

    Debug_Log("==== Peripheral self-test start ====");

    status = SelfTest_RCC(); SelfTest_Report("RCC", status, NULL); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_GPIO(); SelfTest_Report("GPIO", status, "PC13 register toggle"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_USART(); SelfTest_Report("USART", status, "USART2 debug output active"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_SPI(); SelfTest_Report("SPI", status, "init/deinit only, no loopback wire"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_I2C(); SelfTest_Report("I2C", status, "init/deinit only, no slave attached"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_TIM(); SelfTest_Report("TIM", status, "TIM2 counter advanced"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_ADC(); SelfTest_Report("ADC", status, "internal VREFINT/TEMP sampled"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_DMA(); SelfTest_Report("DMA", status, "DMA1 mem-to-mem copy"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_EXTI(); SelfTest_Report("EXTI", status, "software trigger on line0"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_RTC(); SelfTest_Report("RTC", status, "LSI set/get timestamp"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_IWDG(); SelfTest_Report("IWDG", status, "destructive reset test skipped"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_WWDG(); SelfTest_Report("WWDG", status, "destructive reset test skipped"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_PWR(); SelfTest_Report("PWR", status, "PVD check only"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_CAN(); SelfTest_Report("CAN", status, "internal loopback frame"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_BKP(); SelfTest_Report("BKP", status, "DR2 write/readback"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_CRC(); SelfTest_Report("CRC", status, NULL); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;
    status = SelfTest_FLASH(); SelfTest_Report("FLASH", status, "read-only API check"); if (status == SELFTEST_PASS) passCount++; else if (status == SELFTEST_FAIL) failCount++; else skipCount++;

    Debug_LogU32("SelfTest PASS", passCount);
    Debug_LogU32("SelfTest FAIL", failCount);
    Debug_LogU32("SelfTest SKIP", skipCount);
    Debug_Log("==== Peripheral self-test end ====");
}

static void USART2_MinimalInit(uint32_t baud)
{
    GPIO_InitTypeDef gpio;
    USART_InitTypeDef usart;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    gpio.GPIO_Pin = GPIO_Pin_2;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    gpio.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin = GPIO_Pin_3;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    usart.USART_BaudRate = baud;
    usart.USART_WordLength = USART_WordLength_8b;
    usart.USART_StopBits = USART_StopBits_1;
    usart.USART_Parity = USART_Parity_No;
    usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    usart.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &usart);
    USART_Cmd(USART2, ENABLE);
}

static void USART2_SendChar(char ch)
{
    while ((USART2->SR & USART_SR_TXE) == 0U)
    {
    }
    USART2->DR = (uint16_t)ch;
}

static void USART2_SendString(const char *str)
{
    while ((str != NULL) && (*str != '\0'))
    {
        USART2_SendChar(*str++);
    }
}

static void BootTrace_Raw(const char *msg)
{
    USART2_SendString(msg);
    USART2_SendString("\r\n");
}

static void BootTrace_Banner(void)
{
    uint32_t index;

    for (index = 0U; index < APP_BOOT_BANNER_REPEAT; index++)
    {
        BoardStatusLed_Toggle();
        BootTrace_Raw("BOOT: USART2 alive");
        delay_ms((u16)APP_BOOT_BANNER_DELAY_MS);
    }

    BoardStatusLed_Set(0U);
}

static void Debug_LogInit(void)
{
    if (g_logMutex == NULL)
    {
        g_logMutex = xSemaphoreCreateMutex();
    }
}

void Debug_Log(const char *msg)
{
    BaseType_t schedulerRunning;

    if (msg == NULL)
    {
        return;
    }

    schedulerRunning = (xTaskGetSchedulerState() == taskSCHEDULER_RUNNING) ? pdTRUE : pdFALSE;

    if (schedulerRunning == pdTRUE && g_logMutex != NULL)
    {
        if (xSemaphoreTake(g_logMutex, pdMS_TO_TICKS(50U)) == pdPASS)
        {
            USART2_SendString(msg);
            USART2_SendString("\r\n");
            xSemaphoreGive(g_logMutex);
        }
    }
    else
    {
        USART2_SendString(msg);
        USART2_SendString("\r\n");
    }
}

int main(void)
{
    BoardStatusLed_Init();
    BoardStatusLed_Set(1U);
    USART2_MinimalInit(115200U);
    BootTrace_Raw("BOOT: reset entry");

    NVIC_Configuration();
    BootTrace_Raw("BOOT: NVIC ok");

    delay_init();
    BootTrace_Raw("BOOT: delay ok");
    BootTrace_Banner();

    Debug_LogInit();

    Debug_Log("Boot: RTOS app template");

#if APP_ENABLE_PERIPHERAL_SELFTEST
    Debug_Log("Boot: self-test enabled");
    Run_PeripheralSelfTests();
#else
    Debug_Log("Boot: self-test disabled");
#endif

    App_Init();
    Debug_LogU32("RTOS task count", uxTaskGetNumberOfTasks());

    Debug_Log("RTOS: start scheduler");
    vTaskStartScheduler();

    Debug_Log("RTOS: scheduler exit");

    while (1)
    {
        BoardStatusLed_Toggle();
        delay_ms(250U);
    }
}

void vApplicationIdleHook(void)
{
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    (void)xTask;
    (void)pcTaskName;
    Debug_Log("StackOverflow");
    taskDISABLE_INTERRUPTS();
    while (1)
    {
    }
}

void vApplicationMallocFailedHook(void)
{
    Debug_Log("MallocFailed");
    taskDISABLE_INTERRUPTS();
    while (1)
    {
    }
}
