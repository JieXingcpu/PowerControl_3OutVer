#ifndef __MESSAGE_H
#define __MESSAGE_H
#include "main.h"
#define POWER_CONTROL_ID  0x91

void CAN_Connect_Init();
void Send_CAN_Message(uint8_t (*data)[8]);
void Send_Period_Init();

#endif
