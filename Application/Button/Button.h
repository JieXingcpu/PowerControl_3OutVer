#ifndef __BUTTON_H
#define __BUTTON_H

#include "stdint.h"

typedef enum
{
  BUTTON_STATE_IDLE,         // 空闲状态
  BUTTON_STATE_PRESSED,      // 按键按下
  BUTTON_STATE_WAIT_LONG,    // 等待长按
  BUTTON_STATE_LONG_PRESS,   // 长按状态
  BUTTON_STATE_CHANGE          // 按键打开输出状态
} ButtonState;

typedef enum 
{
  COUNTER_START = 0,
  COUNTER_STOP,
  COUNTER_LONG_START,
  COUNTER_LONG_STOP
} CounterState;

extern volatile uint16_t button_1_counter;
extern volatile CounterState button_1_counter_state;
extern volatile ButtonState button_1_state;

extern volatile uint16_t button_2_counter;
extern volatile CounterState button_2_counter_state;
extern volatile ButtonState button_2_state;

extern volatile uint16_t button_3_counter;
extern volatile CounterState button_3_counter_state;
extern volatile ButtonState button_3_state;

void Button_Init();


#endif
