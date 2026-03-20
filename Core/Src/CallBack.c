#include "CallBack.h"
#include "tim.h"
#include "Button.h"
#include "LED.h"
#include "Power.h"
#include "INA3221.h"
#include <stdio.h>
extern Power_Control power;
extern INA3221 power_read;

#define LONG_PRESS_TIME 2000
#define MAX_WAITING_TIME 5000
void print_callback(Power_Control *power,INA3221 *ina3221);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /*10Hz*/
  if(htim->Instance == TIM1)
  {
    print_callback(&power, &power_read);
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
      if(button_1_counter>=MAX_WAITING_TIME)
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
      if(button_1_counter>LONG_PRESS_TIME)//达到长按时间3s,开启对应输出口
      {
        button_1_counter_state=COUNTER_LONG_STOP;
        button_1_counter=0;
        // button_1_state=BUTTON_STATE_IDLE;
        button_1_state=BUTTON_STATE_OPEN;
      }
    }
    /*BUTTON_2*/
    /*进入等待长按计时状态*/
    if(button_2_counter_state==COUNTER_START&&button_2_state==BUTTON_STATE_WAIT_LONG) 
    {
      button_2_counter++;
      if(button_2_counter%500==0) LED_Switch(LED_RUN_2, LED_TOGGLING);
      if(button_2_counter>=MAX_WAITING_TIME)
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
      if(button_2_counter>LONG_PRESS_TIME)//达到长按时间3s,开启对应输出口
      {
        button_2_counter_state=COUNTER_LONG_STOP;
        button_2_counter=0;
        button_2_state=BUTTON_STATE_OPEN;
      }
    }
    /*BUTTON_3*/
    /*进入等待长按计时状态*/
    if(button_3_counter_state==COUNTER_START&&button_3_state==BUTTON_STATE_WAIT_LONG) 
    {
      button_3_counter++;
      if(button_3_counter%500==0) LED_Switch(LED_RUN_3, LED_TOGGLING);
      if(button_3_counter>=MAX_WAITING_TIME)
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
      if(button_3_counter>LONG_PRESS_TIME)
      {
        button_3_counter_state=COUNTER_LONG_STOP;
        button_3_counter=0;
        button_3_state=BUTTON_STATE_OPEN;
      }
    }
  }
}
void print_callback(Power_Control *power,INA3221 *ina3221)
{
  printf("Power State: %d, Voltage: %.2fV, Current: %.2fA\r\n", power->Power_Channel_State, ina3221->Power_Data.voltage[0], ina3221->Power_Data.current[0]);
}