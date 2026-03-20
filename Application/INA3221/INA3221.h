#ifndef __INA3221_H
#define __INA3221_H

#include "stdint.h"
#include "Power.h"
#define MIN_POWER_VOLTAGE 23.0f //最小电池电压,低于此电压将会断电且报警
#define MAX_POWER_VOLTAGE 26.0f //最大电池电压,高于此电压将会断电且报警
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
  uint8_t address;//寄存器地址
  uint16_t send_data_buffer;//发送数据缓冲区
  uint16_t read_data_buffer;//读取数据缓冲区
  Power_DataTypeDef Power_Data;//电压电流数据
  Power_State channel_state;//通道状态
  uint8_t index;//当前正在读取的通道索引,0-2分别对应3个通道
  /*成员函数*/
  INA3221_STATE (*Init)(struct INA3221 *self);
  INA3221_STATE (*Read_Loop)(struct INA3221 *self,Power_Control *power);
  Power_State (*Control_Loop)(struct INA3221 *self,Power_Control *power);
}INA3221;

void Init_Power_Read(INA3221 *ina3221);


#endif
