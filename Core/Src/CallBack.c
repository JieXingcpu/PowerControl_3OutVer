#include "CallBack.h"

#include "Button.h"
#include "INA3221.h"
#include "LED.h"
#include "Message.h"
#include "Power.h"
#include "tim.h"

extern Power_Control power;
extern INA3221 power_read;

#define LONG_PRESS_TIME 2000
#define MAX_WAITING_TIME 5000
static void print_callback(Power_Control *power, INA3221 *ina3221);
static void Send_Message_Callback(Power_Control *power, INA3221 *ina3221);

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /*10Hz*/
  if(htim->Instance == TIM1)
  {
    // print_callback(&power, &power_read);
    Send_Message_Callback(&power, &power_read);
  }
  /*10kHz*/
  if(htim->Instance == TIM3)
  {
    /*按键优化*/
  }
  /*1kHz*/
  if(htim->Instance == TIM4)
  {
    /*BUTTON_1*/
    /*进入等待长按计时状态*/
    if(button_1_counter_state == COUNTER_START && button_1_state == BUTTON_STATE_WAIT_LONG)
    {
      button_1_counter++;
      if(button_1_counter % 500 == 0) LED_Switch(LED_RUN_1, LED_TOGGLING);
      if(button_1_counter >= MAX_WAITING_TIME)
      {
        /*达到等待最长时间5s*/
        button_1_counter_state = COUNTER_STOP;
        button_1_counter = 0;
        button_1_state = BUTTON_STATE_IDLE;
      }
    } else if(button_1_counter_state == COUNTER_LONG_START)
    {
      /*进入长按计时状态,并且LED切换为流水灯*/
      button_1_counter++;
      if(button_1_counter > LONG_PRESS_TIME)  //达到长按时间3s,开启对应输出口
      {
        button_1_counter_state = COUNTER_LONG_STOP;
        button_1_counter = 0;
        // button_1_state=BUTTON_STATE_IDLE;
        button_1_state = BUTTON_STATE_OPEN;
      }
    }
    /*BUTTON_2*/
    /*进入等待长按计时状态*/
    if(button_2_counter_state == COUNTER_START && button_2_state == BUTTON_STATE_WAIT_LONG)
    {
      button_2_counter++;
      if(button_2_counter % 500 == 0) LED_Switch(LED_RUN_2, LED_TOGGLING);
      if(button_2_counter >= MAX_WAITING_TIME)
      {
        /*达到等待最长时间5s*/
        button_2_counter_state = COUNTER_STOP;
        button_2_counter = 0;
        button_2_state = BUTTON_STATE_IDLE;
      }
    } else if(button_2_counter_state == COUNTER_LONG_START)
    {
      /*进入长按计时状态,并且LED切换为流水灯*/
      button_2_counter++;
      if(button_2_counter > LONG_PRESS_TIME)  //达到长按时间3s,开启对应输出口
      {
        button_2_counter_state = COUNTER_LONG_STOP;
        button_2_counter = 0;
        button_2_state = BUTTON_STATE_OPEN;
      }
    }
    /*BUTTON_3*/
    /*进入等待长按计时状态*/
    if(button_3_counter_state == COUNTER_START && button_3_state == BUTTON_STATE_WAIT_LONG)
    {
      button_3_counter++;
      if(button_3_counter % 500 == 0) LED_Switch(LED_RUN_3, LED_TOGGLING);
      if(button_3_counter >= MAX_WAITING_TIME)
      {
        /*达到等待最长时间5s*/
        button_3_counter_state = COUNTER_STOP;
        button_3_counter = 0;
        button_3_state = BUTTON_STATE_IDLE;
      }
    } else if(button_3_counter_state == COUNTER_LONG_START)
    {
      /*进入长按计时状态,并且LED切换为流水灯*/
      button_3_counter++;
      if(button_3_counter > LONG_PRESS_TIME)
      {
        button_3_counter_state = COUNTER_LONG_STOP;
        button_3_counter = 0;
        button_3_state = BUTTON_STATE_OPEN;
      }
    }
  }
}
/**
 * @brief 发送CAN消息的回调函数,10Hz,将当前电源状态和电压电流数据发送出去
 * @param power 电源控制结构体指针,包含当前电源状态和控制函数
 * @param ina3221 INA3221结构体指针,包含当前电压电流数据和读取函数
 * @note 电压电流数据进行线性映射,将电压映射到0-4095的整数(12bit),电流映射到0-65535的整数(16bit)
 * @note 数据结构: DATA[0]:高4bit电源状态,低4bit电压高4bit; DATA[1]:电压低8bit; DATA[2-3]:电流1高8bit和低8bit; DATA[4-5]:电流2高8bit和低8bit; DATA[6-7]:电流3高8bit和低8bit
 */
static void Send_Message_Callback(Power_Control *power, INA3221 *ina3221)
{
  if(power == NULL || ina3221 == NULL) return;
  if(ina3221->read_data_mutex == true) return;  //如果正在读取数据,则不发送消息
  ina3221->read_data_mutex = true;              //发送消息时锁定数据,防止在发送过程中被修改
  uint8_t send_data[8] = {0};
  uint16_t current_data_1 = 0, current_data_2 = 0, current_data_3 = 0;
  /*电源状态*/
  send_data[0] = power->Power_Channel_State << 4;  //高4bit表示电源状态
  /*电压只发送一个输出口，且仅占用12bit，电流发送所有输出口*/
  uint16_t voltage_data = (uint16_t)(ina3221->Power_Data.voltage[0] * (4095.0f / MAX_POWER_VOLTAGE));  //将电压转换为0-4095的整数,保留三位小数
  send_data[0] |= (voltage_data >> 8) & 0x0F;                                                          //电压高4bit放在第一个字节的低4bit
  send_data[1] = (voltage_data & 0xFF);                                                                //电压低8bit放在第二个字节
  if(power->Power_Channel_State != POWER_ALL_OFF)
  {
    current_data_1 = (uint16_t)(ina3221->Power_Data.current[0] * (65535.0f / MAX_POWER_CURRENT));  //将电流转换为0-65535的整数,保留三位小数
    current_data_2 = (uint16_t)(ina3221->Power_Data.current[1] * (65535.0f / MAX_POWER_CURRENT));
    current_data_3 = (uint16_t)(ina3221->Power_Data.current[2] * (65535.0f / MAX_POWER_CURRENT));
    send_data[2] = (current_data_1 >> 8) & 0xFF;  //电流1高8bit放在第三个字节
    send_data[3] = current_data_1 & 0xFF;         //电流1低8bit放在第四个字节
    send_data[4] = (current_data_2 >> 8) & 0xFF;
    send_data[5] = current_data_2 & 0xFF;
    send_data[6] = (current_data_3 >> 8) & 0xFF;
    send_data[7] = current_data_3 & 0xFF;
  }
  Send_CAN_Message(&send_data);
  ina3221->read_data_mutex = false;  //发送消息完成后解锁数据
}

static void print_callback(Power_Control *power, INA3221 *ina3221)
{
  // printf("Power State: %d\r\n", power->Power_Channel_State);
}