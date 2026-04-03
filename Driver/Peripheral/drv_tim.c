/**
  ******************************************************************************
  * @file    drv_tim.c
  * @brief   定时器外设驱动实现 - STM32F103C8T6
  *
  * PWM引脚映射（默认，无重映射）:
  *   TIM1: CH1=PA8,  CH2=PA9,  CH3=PA10, CH4=PA11
  *   TIM2: CH1=PA0,  CH2=PA1,  CH3=PA2,  CH4=PA3
  *   TIM3: CH1=PA6,  CH2=PA7,  CH3=PB0,  CH4=PB1
  *   TIM4: CH1=PB6,  CH2=PB7,  CH3=PB8,  CH4=PB9
  ******************************************************************************
  */

#include "drv_tim.h"
#include <stddef.h>

/*---------------------------------------------------------------------------
 * 内部辅助：使能定时器时钟
 *---------------------------------------------------------------------------*/
static void tim_rcc_enable(TIM_TypeDef *tim)
{
    if      (tim == TIM1) { RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE); }
    else if (tim == TIM2) { RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); }
    else if (tim == TIM3) { RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); }
    else if (tim == TIM4) { RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); }
}

/*---------------------------------------------------------------------------
 * 内部辅助：配置PWM输出GPIO（复用推挽输出）
 *---------------------------------------------------------------------------*/
static void tim_pwm_gpio_init(TIM_TypeDef *tim, uint16_t channel)
{
    GPIO_InitTypeDef gpio;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;

    if (tim == TIM1)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        if      (channel == TIM_Channel_1) { gpio.GPIO_Pin = GPIO_Pin_8;  }
        else if (channel == TIM_Channel_2) { gpio.GPIO_Pin = GPIO_Pin_9;  }
        else if (channel == TIM_Channel_3) { gpio.GPIO_Pin = GPIO_Pin_10; }
        else                               { gpio.GPIO_Pin = GPIO_Pin_11; }
        GPIO_Init(GPIOA, &gpio);
    }
    else if (tim == TIM2)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        if      (channel == TIM_Channel_1) { gpio.GPIO_Pin = GPIO_Pin_0; }
        else if (channel == TIM_Channel_2) { gpio.GPIO_Pin = GPIO_Pin_1; }
        else if (channel == TIM_Channel_3) { gpio.GPIO_Pin = GPIO_Pin_2; }
        else                               { gpio.GPIO_Pin = GPIO_Pin_3; }
        GPIO_Init(GPIOA, &gpio);
    }
    else if (tim == TIM3)
    {
        if (channel == TIM_Channel_1 || channel == TIM_Channel_2)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
            gpio.GPIO_Pin = (channel == TIM_Channel_1) ? GPIO_Pin_6 : GPIO_Pin_7;
            GPIO_Init(GPIOA, &gpio);
        }
        else
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
            gpio.GPIO_Pin = (channel == TIM_Channel_3) ? GPIO_Pin_0 : GPIO_Pin_1;
            GPIO_Init(GPIOB, &gpio);
        }
    }
    else if (tim == TIM4)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
        if      (channel == TIM_Channel_1) { gpio.GPIO_Pin = GPIO_Pin_6; }
        else if (channel == TIM_Channel_2) { gpio.GPIO_Pin = GPIO_Pin_7; }
        else if (channel == TIM_Channel_3) { gpio.GPIO_Pin = GPIO_Pin_8; }
        else                               { gpio.GPIO_Pin = GPIO_Pin_9; }
        GPIO_Init(GPIOB, &gpio);
    }
}

/*---------------------------------------------------------------------------
 * 内部辅助：配置输入捕获GPIO（浮空输入）
 *---------------------------------------------------------------------------*/
static void tim_ic_gpio_init(TIM_TypeDef *tim, uint16_t channel)
{
    GPIO_InitTypeDef gpio;

    gpio.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;

    if (tim == TIM2)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        if      (channel == TIM_Channel_1) { gpio.GPIO_Pin = GPIO_Pin_0; }
        else if (channel == TIM_Channel_2) { gpio.GPIO_Pin = GPIO_Pin_1; }
        else if (channel == TIM_Channel_3) { gpio.GPIO_Pin = GPIO_Pin_2; }
        else                               { gpio.GPIO_Pin = GPIO_Pin_3; }
        GPIO_Init(GPIOA, &gpio);
    }
    else if (tim == TIM3)
    {
        if (channel == TIM_Channel_1 || channel == TIM_Channel_2)
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
            gpio.GPIO_Pin = (channel == TIM_Channel_1) ? GPIO_Pin_6 : GPIO_Pin_7;
            GPIO_Init(GPIOA, &gpio);
        }
        else
        {
            RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
            gpio.GPIO_Pin = (channel == TIM_Channel_3) ? GPIO_Pin_0 : GPIO_Pin_1;
            GPIO_Init(GPIOB, &gpio);
        }
    }
}

/*===========================================================================
 * API 实现
 *===========================================================================*/

void DRV_TIM_Base_Init(const TIM_Base_Cfg_t *cfg)
{
    TIM_TimeBaseInitTypeDef init;

    if (cfg == NULL) { return; }

    tim_rcc_enable(cfg->tim);
    TIM_DeInit(cfg->tim);

    init.TIM_Prescaler         = cfg->prescaler;
    init.TIM_CounterMode       = TIM_CounterMode_Up;
    init.TIM_Period            = cfg->period;
    init.TIM_ClockDivision     = TIM_CKD_DIV1;
    init.TIM_RepetitionCounter = 0U;

    TIM_TimeBaseInit(cfg->tim, &init);
}

void DRV_TIM_PWM_Init(const TIM_PWM_Cfg_t *cfg)
{
    TIM_TimeBaseInitTypeDef base;
    TIM_OCInitTypeDef       oc;
    uint32_t sysClk, arr, psc, compare;

    if (cfg == NULL || cfg->freqHz == 0U) { return; }

    sysClk = cfg->sysClkHz;
    /* 简单分频策略：PSC尽量小，ARR尽量大，以提高分辨率 */
    psc = 0U;
    arr = (sysClk / cfg->freqHz) - 1U;
    /* 若arr超过65535，则增大PSC */
    while (arr > 65535U)
    {
        psc++;
        arr = (sysClk / (psc + 1U) / cfg->freqHz) - 1U;
    }
    compare = (arr + 1U) * cfg->dutyPct / 100U;
    if (compare > 0U) { compare--; }

    tim_rcc_enable(cfg->tim);
    tim_pwm_gpio_init(cfg->tim, cfg->channel);
    TIM_DeInit(cfg->tim);

    base.TIM_Prescaler         = (uint16_t)psc;
    base.TIM_CounterMode       = TIM_CounterMode_Up;
    base.TIM_Period            = (uint16_t)arr;
    base.TIM_ClockDivision     = TIM_CKD_DIV1;
    base.TIM_RepetitionCounter = 0U;
    TIM_TimeBaseInit(cfg->tim, &base);

    TIM_OCStructInit(&oc);
    oc.TIM_OCMode      = TIM_OCMode_PWM1;
    oc.TIM_OutputState = TIM_OutputState_Enable;
    oc.TIM_Pulse       = (uint16_t)compare;
    oc.TIM_OCPolarity  = TIM_OCPolarity_High;

    switch (cfg->channel)
    {
        case TIM_Channel_1: TIM_OC1Init(cfg->tim, &oc); TIM_OC1PreloadConfig(cfg->tim, TIM_OCPreload_Enable); break;
        case TIM_Channel_2: TIM_OC2Init(cfg->tim, &oc); TIM_OC2PreloadConfig(cfg->tim, TIM_OCPreload_Enable); break;
        case TIM_Channel_3: TIM_OC3Init(cfg->tim, &oc); TIM_OC3PreloadConfig(cfg->tim, TIM_OCPreload_Enable); break;
        case TIM_Channel_4: TIM_OC4Init(cfg->tim, &oc); TIM_OC4PreloadConfig(cfg->tim, TIM_OCPreload_Enable); break;
        default: break;
    }

    TIM_ARRPreloadConfig(cfg->tim, ENABLE);
    /* TIM1高级定时器需要额外使能主输出 */
    if (cfg->tim == TIM1) { TIM_CtrlPWMOutputs(cfg->tim, ENABLE); }
}

void DRV_TIM_IC_Init(const TIM_IC_Cfg_t *cfg)
{
    TIM_TimeBaseInitTypeDef base;
    TIM_ICInitTypeDef       ic;

    if (cfg == NULL) { return; }

    tim_rcc_enable(cfg->tim);
    tim_ic_gpio_init(cfg->tim, cfg->channel);
    TIM_DeInit(cfg->tim);

    base.TIM_Prescaler         = cfg->prescaler;
    base.TIM_CounterMode       = TIM_CounterMode_Up;
    base.TIM_Period            = 0xFFFFU;
    base.TIM_ClockDivision     = TIM_CKD_DIV1;
    base.TIM_RepetitionCounter = 0U;
    TIM_TimeBaseInit(cfg->tim, &base);

    ic.TIM_Channel     = cfg->channel;
    ic.TIM_ICPolarity  = cfg->polarity;
    ic.TIM_ICSelection = TIM_ICSelection_DirectTI;
    ic.TIM_ICPrescaler = TIM_ICPSC_DIV1;
    ic.TIM_ICFilter    = 0x00U;
    TIM_ICInit(cfg->tim, &ic);
}

void DRV_TIM_Start(TIM_TypeDef *tim)
{
    TIM_Cmd(tim, ENABLE);
}

void DRV_TIM_Stop(TIM_TypeDef *tim)
{
    TIM_Cmd(tim, DISABLE);
}

void DRV_TIM_ITConfig(TIM_TypeDef *tim, uint16_t it, FunctionalState en)
{
    TIM_ITConfig(tim, it, en);
}

FlagStatus DRV_TIM_GetFlag(TIM_TypeDef *tim, uint16_t flag)
{
    return TIM_GetFlagStatus(tim, flag);
}

void DRV_TIM_ClearFlag(TIM_TypeDef *tim, uint16_t flag)
{
    TIM_ClearFlag(tim, flag);
}

uint16_t DRV_TIM_GetCounter(TIM_TypeDef *tim)
{
    return TIM_GetCounter(tim);
}

void DRV_TIM_SetCounter(TIM_TypeDef *tim, uint16_t count)
{
    TIM_SetCounter(tim, count);
}

void DRV_TIM_SetDuty(TIM_TypeDef *tim, uint16_t channel, uint32_t dutyPct)
{
    uint32_t arr     = (uint32_t)(tim->ARR);
    uint32_t compare = (arr + 1U) * dutyPct / 100U;
    if (compare > 0U) { compare--; }

    switch (channel)
    {
        case TIM_Channel_1: TIM_SetCompare1(tim, (uint16_t)compare); break;
        case TIM_Channel_2: TIM_SetCompare2(tim, (uint16_t)compare); break;
        case TIM_Channel_3: TIM_SetCompare3(tim, (uint16_t)compare); break;
        case TIM_Channel_4: TIM_SetCompare4(tim, (uint16_t)compare); break;
        default: break;
    }
}

uint16_t DRV_TIM_GetCapture(TIM_TypeDef *tim, uint16_t channel)
{
    switch (channel)
    {
        case TIM_Channel_1: return TIM_GetCapture1(tim);
        case TIM_Channel_2: return TIM_GetCapture2(tim);
        case TIM_Channel_3: return TIM_GetCapture3(tim);
        case TIM_Channel_4: return TIM_GetCapture4(tim);
        default:            return 0U;
    }
}

void DRV_TIM_DeInit(TIM_TypeDef *tim)
{
    TIM_Cmd(tim, DISABLE);
    TIM_DeInit(tim);
}
