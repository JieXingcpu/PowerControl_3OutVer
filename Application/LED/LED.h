#ifndef __LED_H
#define __LED_H

typedef enum
{
  LED_RUN_1 = 1,
  LED_RUN_2,
  LED_RUN_3,
  LED_ERR_1,
  LED_ERR_2,
  LED_ERR_3
} LED_ID;

typedef enum 
{
  LED_OFF = 0,
  LED_ON,
  LED_TOGGLING,
  LED_FLOWING_1,
  LED_FLOWING_2,
  LED_FLOWING_3,
} LED_State;

extern LED_State led_state;
void LED_Init(void);
void LED_Switch(LED_ID led_id, LED_State state);

#endif
