    .section .text.Reset_Handler,"ax",%progbits
    .weak   Reset_Handler
    .type   Reset_Handler, %function
Reset_Handler:
    ldr     r0, =_estack
    mov     sp, r0

    ldr     r0, =_sidata
    ldr     r1, =_sdata
    ldr     r2, =_edata
1:  cmp     r1, r2
    ittt    lt
    ldrlt   r3, [r0], #4
    strlt   r3, [r1], #4
    blt     1b

    ldr     r0, =_sbss
    ldr     r1, =_ebss
    movs    r2, #0
2:  cmp     r0, r1
    itt     lt
    strlt   r2, [r0], #4
    blt     2b

    bl      SystemInit
    bl      main
    b       .
    .size   Reset_Handler, .-Reset_Handler

    .section .text.Default_Handler,"ax",%progbits
    .thumb_func
Default_Handler:
    b   .

    .macro  DEF_IRQ name
    .weak   \name
    .thumb_set \name, Default_Handler
    .endm

    DEF_IRQ NMI_Handler
    DEF_IRQ HardFault_Handler
    DEF_IRQ MemManage_Handler
    DEF_IRQ BusFault_Handler
    DEF_IRQ UsageFault_Handler
    DEF_IRQ SVC_Handler
    DEF_IRQ DebugMon_Handler
    DEF_IRQ PendSV_Handler
    DEF_IRQ SysTick_Handler
    DEF_IRQ WWDG_IRQHandler
    DEF_IRQ PVD_IRQHandler
    DEF_IRQ TAMPER_IRQHandler
    DEF_IRQ RTC_IRQHandler
    DEF_IRQ FLASH_IRQHandler
    DEF_IRQ RCC_IRQHandler
    DEF_IRQ EXTI0_IRQHandler
    DEF_IRQ EXTI1_IRQHandler
    DEF_IRQ EXTI2_IRQHandler
    DEF_IRQ EXTI3_IRQHandler
    DEF_IRQ EXTI4_IRQHandler
    DEF_IRQ DMA1_Channel1_IRQHandler
    DEF_IRQ DMA1_Channel2_IRQHandler
    DEF_IRQ DMA1_Channel3_IRQHandler
    DEF_IRQ DMA1_Channel4_IRQHandler
    DEF_IRQ DMA1_Channel5_IRQHandler
    DEF_IRQ DMA1_Channel6_IRQHandler
    DEF_IRQ DMA1_Channel7_IRQHandler
    DEF_IRQ ADC1_IRQHandler
    DEF_IRQ ADC1_2_IRQHandler
    DEF_IRQ ADC3_IRQHandler
    DEF_IRQ USB_HP_CAN1_TX_IRQHandler
    DEF_IRQ USB_LP_CAN1_RX0_IRQHandler
    DEF_IRQ CAN1_TX_IRQHandler
    DEF_IRQ CAN1_RX0_IRQHandler
    DEF_IRQ CAN1_RX1_IRQHandler
    DEF_IRQ CAN1_SCE_IRQHandler
    DEF_IRQ EXTI9_5_IRQHandler
    DEF_IRQ TIM1_BRK_IRQHandler
    DEF_IRQ TIM1_UP_IRQHandler
    DEF_IRQ TIM1_TRG_COM_IRQHandler
    DEF_IRQ TIM1_CC_IRQHandler
    DEF_IRQ TIM1_BRK_TIM15_IRQHandler
    DEF_IRQ TIM1_UP_TIM16_IRQHandler
    DEF_IRQ TIM1_TRG_COM_TIM17_IRQHandler
    DEF_IRQ TIM1_BRK_TIM9_IRQHandler
    DEF_IRQ TIM1_UP_TIM10_IRQHandler
    DEF_IRQ TIM1_TRG_COM_TIM11_IRQHandler
    DEF_IRQ TIM2_IRQHandler
    DEF_IRQ TIM3_IRQHandler
    DEF_IRQ TIM4_IRQHandler
    DEF_IRQ TIM5_IRQHandler
    DEF_IRQ TIM6_IRQHandler
    DEF_IRQ TIM6_DAC_IRQHandler
    DEF_IRQ TIM7_IRQHandler
    DEF_IRQ TIM8_BRK_IRQHandler
    DEF_IRQ TIM8_UP_IRQHandler
    DEF_IRQ TIM8_TRG_COM_IRQHandler
    DEF_IRQ TIM8_CC_IRQHandler
    DEF_IRQ TIM8_BRK_TIM12_IRQHandler
    DEF_IRQ TIM8_UP_TIM13_IRQHandler
    DEF_IRQ TIM8_TRG_COM_TIM14_IRQHandler
    DEF_IRQ TIM12_IRQHandler
    DEF_IRQ TIM13_IRQHandler
    DEF_IRQ TIM14_IRQHandler
    DEF_IRQ I2C1_EV_IRQHandler
    DEF_IRQ I2C1_ER_IRQHandler
    DEF_IRQ I2C2_EV_IRQHandler
    DEF_IRQ I2C2_ER_IRQHandler
    DEF_IRQ SPI1_IRQHandler
    DEF_IRQ SPI2_IRQHandler
    DEF_IRQ SPI3_IRQHandler
    DEF_IRQ USART1_IRQHandler
    DEF_IRQ USART2_IRQHandler
    DEF_IRQ USART3_IRQHandler
    DEF_IRQ UART4_IRQHandler
    DEF_IRQ UART5_IRQHandler
    DEF_IRQ EXTI15_10_IRQHandler
    DEF_IRQ RTCAlarm_IRQHandler
    DEF_IRQ USBWakeUp_IRQHandler
    DEF_IRQ OTG_FS_WKUP_IRQHandler
    DEF_IRQ OTG_FS_IRQHandler
    DEF_IRQ CEC_IRQHandler
    DEF_IRQ FSMC_IRQHandler
    DEF_IRQ SDIO_IRQHandler
    DEF_IRQ DMA2_Channel1_IRQHandler
    DEF_IRQ DMA2_Channel2_IRQHandler
    DEF_IRQ DMA2_Channel3_IRQHandler
    DEF_IRQ DMA2_Channel4_IRQHandler
    DEF_IRQ DMA2_Channel4_5_IRQHandler
    DEF_IRQ DMA2_Channel5_IRQHandler
    DEF_IRQ ETH_IRQHandler
    DEF_IRQ ETH_WKUP_IRQHandler
    DEF_IRQ CAN2_TX_IRQHandler
    DEF_IRQ CAN2_RX0_IRQHandler
    DEF_IRQ CAN2_RX1_IRQHandler
    DEF_IRQ CAN2_SCE_IRQHandler

    .end
