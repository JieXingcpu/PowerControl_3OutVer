#ifndef __LOG_H
#define __LOG_H

#include "Power.h"
#include "INA3221.h"
typedef enum
{
  LOG_CHECK,
  LOG_ON,
  LOG_OFF
} Log;

typedef enum
{
  LOG_MUTEX_UNLOCKED,
  LOG_MUTEX_LOCKED
} Log_Mutex;

extern Log Log_State;
void Log_Init(void);
void Log_Check(void);
void Log_printf(Power_Control *power, INA3221 *ina3221);

#endif