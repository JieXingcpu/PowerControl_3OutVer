#include "Message.h"

#include "main.h"
#include "tim.h"

extern CAN_HandleTypeDef hcan;

void Send_Period_Init()
{
  HAL_TIM_Base_Start_IT(&htim1);
}

void Send_CAN_Message(uint8_t (*data)[8])
{
  CAN_TxHeaderTypeDef txHeader;
  uint32_t TxMailbox;
  txHeader.StdId = POWER_CONTROL_ID;
  txHeader.ExtId = 0;
  txHeader.RTR = CAN_RTR_DATA;
  txHeader.IDE = CAN_ID_STD;
  txHeader.DLC = 8;
  txHeader.TransmitGlobalTime = DISABLE;

  HAL_CAN_AddTxMessage(&hcan, &txHeader, *data, &TxMailbox);
}
void CAN_Connect_Init()
{
  CAN_FilterTypeDef canfilterconfig;

  canfilterconfig.FilterActivation = ENABLE;
  canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
  canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;
  canfilterconfig.FilterIdHigh = 0x0000;
  canfilterconfig.FilterIdLow = 0x0000;
  canfilterconfig.FilterMaskIdHigh = 0x0000;
  canfilterconfig.FilterMaskIdLow = 0x0000;
  canfilterconfig.FilterBank = 0;
  canfilterconfig.FilterFIFOAssignment = CAN_RX_FIFO0;
  HAL_CAN_ConfigFilter(&hcan, &canfilterconfig);
  HAL_CAN_Start(&hcan);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO0_MSG_PENDING);

  canfilterconfig.FilterFIFOAssignment = CAN_RX_FIFO1;
  HAL_CAN_ConfigFilter(&hcan, &canfilterconfig);
  HAL_CAN_Start(&hcan);
  HAL_CAN_ActivateNotification(&hcan, CAN_IT_RX_FIFO1_MSG_PENDING);
}
