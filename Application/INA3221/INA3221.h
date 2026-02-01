#ifndef __INA3221_H
#define __INA3221_H

#include "stdint.h"
#include "Power.h"
#define MIN_POWER_VOLTAGE 23.0f //最小电池电压,低于此电压将会断电且报警
#define MAX_POWER_CURRENT 30.0f  //最大电流,高于此电流将会断电且报警
typedef enum
{
  INA3221_STATE_READING = 0,
  INA3221_STATE_READ_OK,
  INA3221_STATE_IDLE,
  INA3221_STATE_ERROR
}INA3221_STATE;

typedef struct
{
  float voltage[3];
  float current[3];
}Power_DataTypeDef;

typedef struct INA3221
{
  uint8_t address;
  uint16_t send_data_buffer;
  uint16_t read_data_buffer;
  Power_DataTypeDef Power_Data;
  Power_State channel_state;
  uint8_t index;
  /*成员函数*/
  INA3221_STATE (*Init)(struct INA3221 *self);
  INA3221_STATE (*Read_Loop)(struct INA3221 *self,Power_Control *power);
  Power_State (*Control_Loop)(struct INA3221 *self,Power_Control *power);
}INA3221;

void Init_Power_Read(INA3221 *ina3221);


#endif
