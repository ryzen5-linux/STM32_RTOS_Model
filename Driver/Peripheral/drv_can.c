/**
  ******************************************************************************
  * @file    drv_can.c
  * @brief   CAN总线驱动实现 - STM32F103C8T6
  ******************************************************************************
  */

#include "drv_can.h"
#include <stddef.h>

#define CAN_INIT_TIMEOUT  (0x0000FFFFU)

int DRV_CAN_Init(const CAN_Cfg_t *cfg)
{
    CAN_InitTypeDef  init;
    GPIO_InitTypeDef gpio;
    uint8_t          status;

    if (cfg == NULL) { return DRV_CAN_ERROR; }

    /* 时钟使能 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);

    /* GPIO: TX=PA12 复用推挽，RX=PA11 浮空输入 */
    gpio.GPIO_Pin   = GPIO_Pin_12;
    gpio.GPIO_Mode  = GPIO_Mode_AF_PP;
    gpio.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &gpio);

    gpio.GPIO_Pin  = GPIO_Pin_11;
    gpio.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpio);

    CAN_DeInit(CAN1);
    CAN_StructInit(&init);

    init.CAN_TTCM = DISABLE;
    init.CAN_ABOM = ENABLE;   /* 自动离线恢复 */
    init.CAN_AWUM = DISABLE;
    init.CAN_NART = DISABLE;  /* 自动重传 */
    init.CAN_RFLM = DISABLE;
    init.CAN_TXFP = DISABLE;
    init.CAN_Mode       = (cfg->loopback == ENABLE) ? CAN_Mode_LoopBack : CAN_Mode_Normal;
    init.CAN_SJW        = cfg->sjw;
    init.CAN_BS1        = cfg->bs1;
    init.CAN_BS2        = cfg->bs2;
    init.CAN_Prescaler  = cfg->prescaler;

    status = CAN_Init(CAN1, &init);
    if (status == CAN_InitStatus_Failed) { return DRV_CAN_ERROR; }

    return DRV_CAN_OK;
}

void DRV_CAN_DeInit(void)
{
    CAN_DeInit(CAN1);
}

void DRV_CAN_FilterConfig(const CAN_FilterCfg_t *cfg)
{
    CAN_FilterInitTypeDef flt;

    if (cfg == NULL) { return; }

    flt.CAN_FilterNumber         = cfg->filterNum;
    flt.CAN_FilterFIFOAssignment = cfg->fifo;
    flt.CAN_FilterMode           = cfg->mode;
    flt.CAN_FilterScale          = cfg->scale;
    flt.CAN_FilterIdHigh         = (uint16_t)cfg->idHigh;
    flt.CAN_FilterIdLow          = (uint16_t)cfg->idLow;
    flt.CAN_FilterMaskIdHigh     = (uint16_t)cfg->maskHigh;
    flt.CAN_FilterMaskIdLow      = (uint16_t)cfg->maskLow;
    flt.CAN_FilterActivation     = ENABLE;

    CAN_FilterInit(&flt);
}

int DRV_CAN_Send(const CAN_Frame_t *frame, uint32_t timeout)
{
    CanTxMsg msg;
    uint8_t  mailbox;
    uint32_t cnt = 0U;

    if (frame == NULL) { return DRV_CAN_ERROR; }

    msg.DLC = frame->dlc;
    msg.RTR = (frame->rtr != 0U) ? CAN_RTR_Remote : CAN_RTR_Data;

    if (frame->ide != 0U)
    {
        msg.IDE   = CAN_Id_Extended;
        msg.ExtId = frame->id;
        msg.StdId = 0U;
    }
    else
    {
        msg.IDE   = CAN_Id_Standard;
        msg.StdId = frame->id & 0x7FFU;
        msg.ExtId = 0U;
    }

    for (uint8_t i = 0U; i < frame->dlc && i < 8U; i++)
    {
        msg.Data[i] = frame->data[i];
    }

    mailbox = CAN_Transmit(CAN1, &msg);
    if (mailbox == CAN_TxStatus_NoMailBox) { return DRV_CAN_ERROR; }

    while (CAN_TransmitStatus(CAN1, mailbox) != CAN_TxStatus_Ok)
    {
        if (timeout != 0U)
        {
            cnt++;
            if (cnt >= timeout) { return DRV_CAN_TIMEOUT; }
        }
    }

    return DRV_CAN_OK;
}

int DRV_CAN_Receive(uint8_t fifox, CAN_Frame_t *frame)
{
    CanRxMsg msg;
    uint8_t  i;

    if (frame == NULL) { return DRV_CAN_ERROR; }
    if (CAN_MessagePending(CAN1, fifox) == 0U) { return DRV_CAN_ERROR; }

    CAN_Receive(CAN1, fifox, &msg);

    frame->ide = (msg.IDE == CAN_Id_Extended) ? 1U : 0U;
    frame->rtr = (msg.RTR == CAN_RTR_Remote)  ? 1U : 0U;
    frame->dlc = msg.DLC;
    frame->id  = (frame->ide != 0U) ? msg.ExtId : msg.StdId;

    for (i = 0U; i < msg.DLC && i < 8U; i++)
    {
        frame->data[i] = msg.Data[i];
    }

    return DRV_CAN_OK;
}

uint8_t DRV_CAN_GetRxPendingCount(uint8_t fifox)
{
    return (uint8_t)CAN_MessagePending(CAN1, fifox);
}

void DRV_CAN_ITConfig(uint32_t it, FunctionalState en)
{
    CAN_ITConfig(CAN1, it, en);
}
