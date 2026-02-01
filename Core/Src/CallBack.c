#include "CallBack.h"
#include "tim.h"
#include "Button.h"
#include "LED.h"
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /*10Hz*/
  if(htim->Instance == TIM1)
  {

  }
  /*100Hz*/
  else if(htim->Instance == TIM3)
  {

  }
  /*1kHz*/
  else if(htim->Instance == TIM4)
  {
    /*BUTTON_1*/
    /*进入等待长按计时状态*/
    if(button_1_counter_state==COUNTER_START&&button_1_state==BUTTON_STATE_WAIT_LONG) 
    {
      button_1_counter++;
      if(button_1_counter%500==0) LED_Switch(LED_RUN_1, LED_TOGGLING);
      if(button_1_counter>=5000)
      {
        /*达到等待最长时间5s*/
        button_1_counter_state=COUNTER_STOP;
        button_1_counter=0;
        button_1_state=BUTTON_STATE_IDLE;
      }
    }
    else if (button_1_counter_state==COUNTER_LONG_START) 
    {
      /*进入长按计时状态,并且LED切换为流水灯*/
      button_1_counter++;
      if(button_1_counter>3000)
      {
        button_1_counter_state=COUNTER_LONG_STOP;
        button_1_counter=0;
        button_1_state=BUTTON_STATE_IDLE;
      }
    }
    /*BUTTON_2*/
    /*进入等待长按计时状态*/
    if(button_2_counter_state==COUNTER_START&&button_2_state==BUTTON_STATE_WAIT_LONG) 
    {
      button_2_counter++;
      if(button_2_counter%500==0) LED_Switch(LED_RUN_2, LED_TOGGLING);
      if(button_2_counter>=5000)
      {
        /*达到等待最长时间5s*/
        button_2_counter_state=COUNTER_STOP;
        button_2_counter=0;
        button_2_state=BUTTON_STATE_IDLE;
      }
    }
    else if (button_2_counter_state==COUNTER_LONG_START) 
    {
      /*进入长按计时状态,并且LED切换为流水灯*/
      button_2_counter++;
      if(button_2_counter>3000)
      {
        button_2_counter_state=COUNTER_LONG_STOP;
        button_2_counter=0;
        button_2_state=BUTTON_STATE_IDLE;
      }
    }
    /*BUTTON_3*/
    /*进入等待长按计时状态*/
    if(button_3_counter_state==COUNTER_START&&button_3_state==BUTTON_STATE_WAIT_LONG) 
    {
      button_3_counter++;
      if(button_3_counter%500==0) LED_Switch(LED_RUN_3, LED_TOGGLING);
      if(button_3_counter>=5000)
      {
        /*达到等待最长时间5s*/
        button_3_counter_state=COUNTER_STOP;
        button_3_counter=0;
        button_3_state=BUTTON_STATE_IDLE;
      }
    }
    else if (button_3_counter_state==COUNTER_LONG_START) 
    {
      /*进入长按计时状态,并且LED切换为流水灯*/
      button_3_counter++;
      if(button_3_counter>3000)
      {
        button_3_counter_state=COUNTER_LONG_STOP;
        button_3_counter=0;
        button_3_state=BUTTON_STATE_IDLE;
      }
    }
  }
}