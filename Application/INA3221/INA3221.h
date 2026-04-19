#ifndef __INA3221_H
#define __INA3221_H

#include "Power.h"
#include "stdbool.h"
#include "stdint.h"

#define MIN_POWER_VOLTAGE 23.0f        //最小电池电压,低于此电压将会断电且报警
#define MAX_POWER_VOLTAGE 26.0f        //最大电池电压,高于此电压将会断电且报警
#define MAX_POWER_CURRENT 31.0f        //最大电流,持续高于此电流将会断电且报警
#define MAX_CURRENT_SUDDEN 50.0f       //电流突变保护,如果电流突然(SUDDEN_CURRENT_HOLDING_TIME)超过此值,则断电
#define MAX_HOLDING_TIME 20            //最大持续时间,单位为秒,超过此时间将会断电且报警(最大值为25.5s)
#define SUDDEN_CURRENT_HOLDING_TIME 3  //电流异常持续时间,单位为0.1秒,超过此时间将会断电
#define FILTER_BUFFER_SIZE 11          //滤波缓冲区大小

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
  uint8_t address;                             //寄存器地址
  uint16_t send_data_buffer;                   //发送数据缓冲区
  uint16_t read_data_buffer;                   //读取数据缓冲区
  Power_DataTypeDef Power_Data;                //电压电流数据
  volatile bool read_data_mutex;               //读取数据互斥锁,防止在读取过程中被其他函数修改数据
  Power_State channel_state;                   //通道状态
  uint8_t index;                               //当前正在读取的通道索引,0-2分别对应3个通道
  volatile uint8_t voltage_counter[3];         //电压异常计数器
  volatile uint8_t current_counter[3];         //电流异常计数器
  volatile uint8_t sudden_current_counter[3];  //电流突变计数器
  volatile bool voltage_count_flag[3];         //电压异常报警标志
  volatile bool current_count_flag[3];         //电流异常报警标志
  volatile bool sudden_current_count_flag[3];  //电流突变报警标志
  volatile bool filter_init;                   //滤波初始化标志,为true时表示滤波初始值已经准备好,可以进行滤波计算
  /*成员函数*/
  INA3221_STATE (*Init)(struct INA3221 *self);
  INA3221_STATE (*Read_Loop)(struct INA3221 *self, Power_Control *power);
  Power_State (*Voltage_Control_Loop)(struct INA3221 *self, Power_Control *power);
  Power_State (*Current_Control_Loop)(struct INA3221 *self, Power_Control *power);
  INA3221_STATE (*Median_Filter)(struct INA3221 *self, Power_Control *power);
} INA3221;

void Init_Power_Read(INA3221 *ina3221);

#endif
