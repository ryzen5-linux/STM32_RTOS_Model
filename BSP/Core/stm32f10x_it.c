/**
  ******************************************************************************
  * @file    GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and peripherals
  *          interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h" 
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_exti.h"

extern volatile uint32_t g_exti0_irq_seen;


static void Fault_LED_Init(void)
{
  GPIO_InitTypeDef gpio;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  gpio.GPIO_Pin = GPIO_Pin_13;
  gpio.GPIO_Speed = GPIO_Speed_2MHz;
  gpio.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &gpio);
}

static void Fault_LED_Set(uint8_t on)
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

static void Fault_Delay(volatile uint32_t count)
{
  while (count-- != 0U)
  {
  }
}

static void Fault_USART2_Init(void)
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

  usart.USART_BaudRate = 115200U;
  usart.USART_WordLength = USART_WordLength_8b;
  usart.USART_StopBits = USART_StopBits_1;
  usart.USART_Parity = USART_Parity_No;
  usart.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  usart.USART_Mode = USART_Mode_Tx;
  USART_Init(USART2, &usart);
  USART_Cmd(USART2, ENABLE);
}

static void Fault_USART2_SendChar(char ch)
{
  while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET)
  {
  }

  USART_SendData(USART2, (uint16_t)ch);
}

static void Fault_USART2_SendString(const char *str)
{
  while ((str != 0) && (*str != '\0'))
  {
    Fault_USART2_SendChar(*str++);
  }
}

static void Fault_USART2_SendHex32(uint32_t value)
{
  int shift;

  Fault_USART2_SendString("0x");
  for (shift = 28; shift >= 0; shift -= 4)
  {
    uint32_t nibble = (value >> (uint32_t)shift) & 0xFU;
    Fault_USART2_SendChar((char)(nibble < 10U ? ('0' + nibble) : ('A' + (nibble - 10U))));
  }
}

static void Fault_Report(const char *name)
{
  Fault_LED_Init();
  Fault_USART2_Init();

  Fault_USART2_SendString("\r\nFAULT: ");
  Fault_USART2_SendString(name);
  Fault_USART2_SendString(" CFSR=");
  Fault_USART2_SendHex32(SCB->CFSR);
  Fault_USART2_SendString(" HFSR=");
  Fault_USART2_SendHex32(SCB->HFSR);
  Fault_USART2_SendString(" BFAR=");
  Fault_USART2_SendHex32(SCB->BFAR);
  Fault_USART2_SendString(" MMFAR=");
  Fault_USART2_SendHex32(SCB->MMFAR);
  Fault_USART2_SendString("\r\n");

  while (1)
  {
    Fault_LED_Set(1U);
    Fault_Delay(800000U);
    Fault_LED_Set(0U);
    Fault_Delay(800000U);
  }
}

 
void NMI_Handler(void)
{
}
 
void HardFault_Handler(void)
{
  Fault_Report("HardFault");
}
 
void MemManage_Handler(void)
{
  Fault_Report("MemManage");
}

 
void BusFault_Handler(void)
{
  Fault_Report("BusFault");
}
 
void UsageFault_Handler(void)
{
  Fault_Report("UsageFault");
}
 
void DebugMon_Handler(void)
{
}

/* FreeRTOS supplies SVC/PendSV/SysTick, so the CMSIS stubs are removed to avoid
 * duplicate definitions during linking. */

void TIM2_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
  {
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}

void EXTI0_IRQHandler(void)
{
  if (EXTI_GetITStatus(EXTI_Line0) != RESET)
  {
    g_exti0_irq_seen = 1U;
    EXTI_ClearITPendingBit(EXTI_Line0);
  }
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/
