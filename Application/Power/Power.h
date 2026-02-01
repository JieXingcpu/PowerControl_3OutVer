#ifndef __POWER_H
#define __POWER_H

typedef enum
{
  POWER_OUT_1 = 0x01,
  POWER_OUT_2 = 0x02,
  POWER_OUT_1_AND_2 = 0x03,
  POWER_OUT_3 = 0x04,
  POWER_OUT_1_AND_3 = 0x05,
  POWER_OUT_2_AND_3 = 0x06,
  POWER_OUT_ALL = 0x07
} Power_Output_Channel;

typedef enum
{
  POWER_ON = 0,
  POWER_OFF
} Channel_State;

typedef enum
{
  POWER_ALL_OFF = 0x00,
  POWER_ON_1 = 0x01,
  POWER_ON_2 = 0x02,
  POWER_ON_1_AND_2 = 0x03,
  POWER_ON_3 = 0x04,
  POWER_ON_1_AND_3 = 0x05,
  POWER_ON_2_AND_3 = 0x06,
  POWER_ON_ALL = 0x07,
  POWER_ERROR
} Power_State;

typedef struct Power_Control
{
  Power_State Power_Channel_State;// 当前输出通道状态，按位表示
  
  Power_State (*Init)(struct Power_Control *self);
  Power_State (*Switch)(struct Power_Control *self, Power_Output_Channel channel, Channel_State state);
  Power_State (*Read_Channel_State)(struct Power_Control *self);
} Power_Control;

void Init_Power_Control(Power_Control *power);

#endif
