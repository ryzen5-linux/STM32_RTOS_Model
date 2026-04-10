/**
    ******************************************************************************
    * @file    drv_rcc.c
    * @brief   复位与时钟控制(RCC)驱动实现
    ******************************************************************************
    */

#include "drv_rcc.h"
#include "stm32f10x_gpio.h"
#include <stddef.h>

void DRV_RCC_SystemClockConfig_72MHz(void)
{
    ErrorStatus hse_status;

    /* 复位RCC到默认复位值 */
    RCC_DeInit();

    /* 使能HSE */
    RCC_HSEConfig(RCC_HSE_ON);
    hse_status = RCC_WaitForHSEStartUp();

    if (hse_status == SUCCESS)
    {
#if defined(STM32F10X_CL)
        FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
        FLASH_SetLatency(FLASH_Latency_2);

        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        RCC_PCLK2Config(RCC_HCLK_Div1);
        RCC_PCLK1Config(RCC_HCLK_Div2);

        /* F105/F107: 25 MHz HSE -> PLL2 -> PREDIV1 -> PLL -> 72 MHz */
        RCC->CFGR2 &= (uint32_t)~(RCC_CFGR2_PREDIV2 | RCC_CFGR2_PLL2MUL |
                      RCC_CFGR2_PREDIV1 | RCC_CFGR2_PREDIV1SRC);
        RCC->CFGR2 |= (uint32_t)(RCC_CFGR2_PREDIV2_DIV5 | RCC_CFGR2_PLL2MUL8 |
                     RCC_CFGR2_PREDIV1SRC_PLL2 | RCC_CFGR2_PREDIV1_DIV5);

        RCC->CR |= RCC_CR_PLL2ON;
        while ((RCC->CR & RCC_CR_PLL2RDY) == 0U) {}

        RCC->CFGR &= (uint32_t)~(RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLSRC | RCC_CFGR_PLLMULL);
        RCC->CFGR |= (uint32_t)(RCC_CFGR_PLLXTPRE_PREDIV1 |
                    RCC_CFGR_PLLSRC_PREDIV1 |
                    RCC_CFGR_PLLMULL9);
#elif defined(STM32F10X_LD_VL) || defined(STM32F10X_MD_VL) || defined(STM32F10X_HD_VL)
    /* Value Line 典型最高主频为 24 MHz，使用 HSE/2 * 6。 */
    FLASH_SetLatency(FLASH_Latency_0);

    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK2Config(RCC_HCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div1);

    RCC_PREDIV1Config(RCC_PREDIV1_Source_HSE, RCC_PREDIV1_Div2);
    RCC_PLLConfig(RCC_PLLSource_PREDIV1, RCC_PLLMul_6);
#else
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    FLASH_SetLatency(FLASH_Latency_2);

    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK2Config(RCC_HCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div2);

        /* F100/F101/F102/F103: HSE * 9 = 8 MHz * 9 = 72 MHz */
        RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9);
#endif

        RCC_PLLCmd(ENABLE);
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {}

        /* 切换系统时钟至PLL */
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
        while (RCC_GetSYSCLKSource() != 0x08U) {}
    }
    else
    {
        /* HSE启动失败处理：悬停（实际项目中可切换至HSI保底运行） */
        while (1) {}
    }
}

void DRV_RCC_GetClockFreq(RCC_ClockFreq_t *freq)
{
    RCC_ClocksTypeDef clk;
    if (freq == NULL) { return; }
    RCC_GetClocksFreq(&clk);
    freq->sysclk = clk.SYSCLK_Frequency;
    freq->hclk   = clk.HCLK_Frequency;
    freq->pclk1  = clk.PCLK1_Frequency;
    freq->pclk2  = clk.PCLK2_Frequency;
    freq->adcclk = clk.ADCCLK_Frequency;
}

uint32_t DRV_RCC_GetSysClockFreq(void)
{
    return SystemCoreClock;
}

void DRV_RCC_APB2ClockEnable(uint32_t periph)
{
    RCC_APB2PeriphClockCmd(periph, ENABLE);
}

void DRV_RCC_APB2ClockDisable(uint32_t periph)
{
    RCC_APB2PeriphClockCmd(periph, DISABLE);
}

void DRV_RCC_APB1ClockEnable(uint32_t periph)
{
    RCC_APB1PeriphClockCmd(periph, ENABLE);
}

void DRV_RCC_APB1ClockDisable(uint32_t periph)
{
    RCC_APB1PeriphClockCmd(periph, DISABLE);
}

void DRV_RCC_AHBClockEnable(uint32_t periph)
{
    RCC_AHBPeriphClockCmd(periph, ENABLE);
}

void DRV_RCC_AHBClockDisable(uint32_t periph)
{
    RCC_AHBPeriphClockCmd(periph, DISABLE);
}

void DRV_RCC_APB2Reset(uint32_t periph)
{
    RCC_APB2PeriphResetCmd(periph, ENABLE);
    RCC_APB2PeriphResetCmd(periph, DISABLE);
}

void DRV_RCC_APB1Reset(uint32_t periph)
{
    RCC_APB1PeriphResetCmd(periph, ENABLE);
    RCC_APB1PeriphResetCmd(periph, DISABLE);
}

void DRV_RCC_MCOConfig(uint8_t src)
{
    GPIO_InitTypeDef gpio;

    /* MCO输出引脚PA8需配置为复用推挽输出 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    gpio.GPIO_Pin   = GPIO_Pin_8;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    RCC_MCOConfig(src);
}

uint32_t DRV_RCC_GetResetFlags(void)
{
    return RCC->CSR;
}

void DRV_RCC_ClearResetFlags(void)
{
    RCC_ClearFlag();
}
