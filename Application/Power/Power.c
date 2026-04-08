#include "Power.h"

#include "LED.h"
#include "gpio.h"

static Power_State Power_Init(Power_Control *self);
static Power_State Power_Switch(Power_Control *self, Power_Output_Channel channel, Channel_State state, Power_Output_Channel danger_channel);
static Power_State Power_Read_Channel_State(Power_Control *self);
/**
  * @brief  初始化电源控制结构体
  * @param  power: Power_Control结构体的指针
  * @retval None
 */
void Init_Power_Control(Power_Control *power)
{
  power->Power_Channel_State = POWER_ALL_OFF;
  power->Init = Power_Init;
  power->Switch = Power_Switch;
  power->Read_Channel_State = Power_Read_Channel_State;
}
/**
  * @brief  初始化电源控制
  * @param  self: Power_Control结构体的指针
  * @note  初始化时会将所有输出通道关闭
  * @retval Power_State
 */
static Power_State Power_Init(Power_Control *self)
{
  HAL_GPIO_WritePin(OUT_1_GPIO_Port, OUT_1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(OUT_2_GPIO_Port, OUT_2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(OUT_3_GPIO_Port, OUT_3_Pin, GPIO_PIN_RESET);
  self->Power_Channel_State = POWER_ALL_OFF;
  return self->Power_Channel_State;
}
/**
  * @brief  读取当前输出通道状态
  * @param  self: Power_Control结构体的指针
  * @retval Power_State
 */
static Power_State Power_Read_Channel_State(Power_Control *self) { return self->Power_Channel_State; }
/**
  * @brief  改变输出通道状态
  * @param  self: Power_Control结构体的指针
  * @param  channel: 输出通道,可使用按位操作
  * @retval Power_State
 */
static Power_State Power_Switch(Power_Control *self, Power_Output_Channel channel, Channel_State state, Power_Output_Channel danger_channel)
{
  GPIO_PinState pin_state = (state == POWER_ON) ? GPIO_PIN_SET : GPIO_PIN_RESET;
  if(channel & POWER_OUT_1)
  {
    HAL_GPIO_WritePin(OUT_1_GPIO_Port, OUT_1_Pin, pin_state);
    if(state == POWER_ON)
    {
      self->Power_Channel_State |= POWER_ON_1;
      LED_Switch(LED_ERR_1, LED_OFF);
      LED_Switch(LED_RUN_1, LED_ON);
    } else
    {
      if(danger_channel & POWER_OUT_1)
      {
        /*如果是由于保护机制关闭的通道，则点亮对应的指示灯*/
        LED_Switch(LED_ERR_1, LED_ON);
        LED_Switch(LED_RUN_1, LED_OFF);
      } else
      {
        LED_Switch(LED_RUN_1, LED_OFF);
        LED_Switch(LED_ERR_1, LED_OFF);
      }
      self->Power_Channel_State &= ~POWER_ON_1;
    }
  }
  if(channel & POWER_OUT_2)
  {
    HAL_GPIO_WritePin(OUT_2_GPIO_Port, OUT_2_Pin, pin_state);
    if(state == POWER_ON)
    {
      self->Power_Channel_State |= POWER_ON_2;
      LED_Switch(LED_ERR_2, LED_OFF);
      LED_Switch(LED_RUN_2, LED_ON);
    } else
    {
      if(danger_channel & POWER_OUT_2)
      {
        /*如果是由于保护机制关闭的通道，则点亮对应的指示灯*/
        LED_Switch(LED_ERR_2, LED_ON);
        LED_Switch(LED_RUN_2, LED_OFF);
      } else
      {
        LED_Switch(LED_RUN_2, LED_OFF);
        LED_Switch(LED_ERR_2, LED_OFF);
      }
      self->Power_Channel_State &= ~POWER_ON_2;
    }
  }
  if(channel & POWER_OUT_3)
  {
    HAL_GPIO_WritePin(OUT_3_GPIO_Port, OUT_3_Pin, pin_state);
    if(state == POWER_ON)
    {
      self->Power_Channel_State |= POWER_ON_3;
      LED_Switch(LED_ERR_3, LED_OFF);
      LED_Switch(LED_RUN_3, LED_ON);
    } else
    {
      if(danger_channel & POWER_OUT_3)
      {
        /*如果是由于保护机制关闭的通道，则点亮对应的指示灯*/
        LED_Switch(LED_ERR_3, LED_ON);
        LED_Switch(LED_RUN_3, LED_OFF);
      } else
      {
        LED_Switch(LED_RUN_3, LED_OFF);
        LED_Switch(LED_ERR_3, LED_OFF);
      }
      self->Power_Channel_State &= ~POWER_ON_3;
    }
  }
  return self->Power_Channel_State;
}
