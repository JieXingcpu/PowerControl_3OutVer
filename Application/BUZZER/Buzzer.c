#include "Buzzer.h"

#include "tim.h"


void Buzzer_Init(void)
{
  //初始化关闭蜂鸣器
  __HAL_TIM_SET_AUTORELOAD(&htim2, tone1_2kHz_arr);
  __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
}

void Buzzer_Switch(Buzzer_State state)
{
  switch(state)
  {
    case BUZZER_OFF:
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
      HAL_TIM_PWM_Stop(&htim2, TIM_CHANNEL_2);
      break;
    case BUZZER_SYSTEM_INIT:
      if(HAL_TIM_GetChannelState(&htim2, TIM_CHANNEL_2) == HAL_TIM_CHANNEL_STATE_READY)
      {
        /*如果没有启动PWM，则启动*/
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
        __HAL_TIM_SET_AUTORELOAD(&htim2, 0);
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
      }
      __HAL_TIM_SET_AUTORELOAD(&htim2, tone1_2kHz_arr);
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, tone1_2kHz_ccr);
      HAL_Delay(300);
      __HAL_TIM_SET_AUTORELOAD(&htim2, tone2_2_2kHz_arr);
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, tone2_2_2kHz_ccr);
      HAL_Delay(300);
      __HAL_TIM_SET_AUTORELOAD(&htim2, tone3_2_4kHz_arr);
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, tone3_2_4kHz_ccr);
      HAL_Delay(300);
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
      break;
    case BUZZER_WARNING:
      if(HAL_TIM_GetChannelState(&htim2, TIM_CHANNEL_2) == HAL_TIM_CHANNEL_STATE_READY)
      {
        /*如果没有启动PWM，则启动*/
        __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, 0);
        __HAL_TIM_SET_AUTORELOAD(&htim2, 0);
        HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
      }
      __HAL_TIM_SET_AUTORELOAD(&htim2, tone2_2_2kHz_arr);
      __HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_2, tone2_2_2kHz_ccr);
      break;
  }
}