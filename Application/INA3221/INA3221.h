#ifndef __INA3221_H
#define __INA3221_H

#include "Power.h"
#include "stdbool.h"
#include "stdint.h"


#define MIN_POWER_VOLTAGE 23.0f  //最小电池电压,低于此电压将会断电且报警
#define MAX_POWER_VOLTAGE 26.0f  //最大电池电压,高于此电压将会断电且报警
#define MAX_POWER_CURRENT 31.0f  //最大电流,高于此电流将会断电且报警
#define MAX_HOLDING_TIME 20      //最大持续时间,单位为秒,超过此时间将会断电且报警(最大值为25.5s)
typedef enum
{
  INA3221_STATE_READING = 0,
  INA3221_STATE_READ_OK,
  INA3221_STATE_IDLE,
  INA3221_STATE_ERROR
} INA3221_STATE;

typedef struct
{
  float voltage[3];
  float current[3];
} Power_DataTypeDef;

typedef struct INA3221
{
  uint8_t address;               //寄存器地址
  uint16_t send_data_buffer;     //发送数据缓冲区
  uint16_t read_data_buffer;     //读取数据缓冲区
  Power_DataTypeDef Power_Data;  //电压电流数据
  bool read_data_mutex;          //读取数据互斥锁,防止在读取过程中被其他函数修改数据
  Power_State channel_state;     //通道状态
  uint8_t index;                 //当前正在读取的通道索引,0-2分别对应3个通道
  uint8_t holding_time;          //监测持续时间,单位为秒
  /*成员函数*/
  INA3221_STATE (*Init)(struct INA3221 *self);
  INA3221_STATE (*Read_Loop)(struct INA3221 *self, Power_Control *power);
  Power_State (*Voltage_Control_Loop)(struct INA3221 *self, Power_Control *power);
  Power_State (*Current_Control_Loop)(struct INA3221 *self, Power_Control *power);
} INA3221;

void Init_Power_Read(INA3221 *ina3221);

#endif
