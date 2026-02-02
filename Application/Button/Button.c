#include "Button.h"
#include "gpio.h"
#include "LED.h"
#include "tim.h"

static void BUTTON_1_Callback();
static void BUTTON_2_Callback();
static void BUTTON_3_Callback();

volatile ButtonState button_1_state = BUTTON_STATE_IDLE;
volatile uint16_t button_1_counter = 0;
volatile CounterState button_1_counter_state = COUNTER_STOP;

volatile ButtonState button_2_state = BUTTON_STATE_IDLE;
volatile uint16_t button_2_counter = 0;
volatile CounterState button_2_counter_state = COUNTER_STOP;

volatile ButtonState button_3_state = BUTTON_STATE_IDLE;
volatile uint16_t button_3_counter = 0;
volatile CounterState button_3_counter_state = COUNTER_STOP;

/**
  * @brief  初始化按钮状态机
  * @param  None
  * @note   自动启动TIM4中断
 */
void Button_Init()
{
  button_1_state = BUTTON_STATE_IDLE;
  button_1_counter = 0;
  button_1_counter_state = COUNTER_STOP;

  button_2_state = BUTTON_STATE_IDLE;
  button_2_counter = 0;
  button_2_counter_state = COUNTER_STOP;

  button_3_state = BUTTON_STATE_IDLE;
  button_3_counter = 0;
  button_3_counter_state = COUNTER_STOP;
  HAL_TIM_Base_Start_IT(&htim4);
}
/**
  * @brief  BUTTON 1 状态机回调函数
  * @param  None
 */
static void BUTTON_1_Callback()
{
  //判断上升沿还是下降沿
  GPIO_PinState pin_state = HAL_GPIO_ReadPin(Control_1_GPIO_Port, Control_1_Pin);
  /*上升沿*/ 
  if(pin_state==GPIO_PIN_SET)
  {
    /*处于等待长按状态*/
    if(button_1_counter_state==COUNTER_START&&button_1_state==BUTTON_STATE_WAIT_LONG)
    {
      button_1_state=BUTTON_STATE_LONG_PRESS;
      button_1_counter=0;
      button_1_counter_state=COUNTER_LONG_START;
    }
    else
    {
      button_1_state = BUTTON_STATE_PRESSED;
    }
  }
  /*下降沿*/
  else if(pin_state==GPIO_PIN_RESET)
  {

    if(button_1_state==BUTTON_STATE_PRESSED&&button_1_counter_state==COUNTER_STOP)
    {
      /*开始计时(5s)*/
      button_1_counter=0;
      button_1_counter_state = COUNTER_START;
      button_1_state = BUTTON_STATE_WAIT_LONG;

    }
    else if(button_1_state==BUTTON_STATE_LONG_PRESS&&button_1_counter_state==COUNTER_LONG_START)
    {
      /*长按计时取消*/
      button_1_counter=0;
      button_1_state=BUTTON_STATE_WAIT_LONG;
      button_1_counter_state=COUNTER_START;
      LED_Switch(LED_RUN_1, LED_TOGGLING);
    }
    else if(button_1_state==BUTTON_STATE_IDLE)
    {
      /*恢复状态*/
      button_1_state=BUTTON_STATE_IDLE;
      button_1_counter_state=COUNTER_STOP;
      button_1_counter=0;
    }
  }
}

/**
  * @brief  BUTTON 2 状态机回调函数
  * @param  None
 */
static void BUTTON_2_Callback()
{
  //判断上升沿还是下降沿
  GPIO_PinState pin_state = HAL_GPIO_ReadPin(Control_2_GPIO_Port, Control_2_Pin);
  /*上升沿*/  
  if(pin_state==GPIO_PIN_SET)
  {
    /*处于等待长按状态*/
    if(button_2_counter_state==COUNTER_START&&button_2_state==BUTTON_STATE_WAIT_LONG)
    {
      button_2_state=BUTTON_STATE_LONG_PRESS;
      button_2_counter=0;
      button_2_counter_state=COUNTER_LONG_START;
    }
    else
    {
      button_2_state = BUTTON_STATE_PRESSED;
    }
  }
  /*下降沿*/  
  else if(pin_state==GPIO_PIN_RESET)
  {

    if(button_2_state==BUTTON_STATE_PRESSED&&button_2_counter_state==COUNTER_STOP)
    {
      /*开始计时(5s)*/
      button_2_counter=0;
      button_2_counter_state = COUNTER_START;
      button_2_state = BUTTON_STATE_WAIT_LONG;

    }
    else if(button_2_state==BUTTON_STATE_LONG_PRESS&&button_2_counter_state==COUNTER_LONG_START)
    {
      /*长按计时取消*/
      button_2_counter=0;
      button_2_state=BUTTON_STATE_WAIT_LONG;
      button_2_counter_state=COUNTER_START;
      LED_Switch(LED_RUN_2, LED_TOGGLING);
    }
    else if(button_2_state==BUTTON_STATE_IDLE)
    {
      /*恢复状态*/
      button_2_state=BUTTON_STATE_IDLE;
      button_2_counter_state=COUNTER_STOP;
      button_2_counter=0;
    }
  }
}
/**
  * @brief  BUTTON 3 状态机回调函数
  * @param  None
 */
static void BUTTON_3_Callback()
{
  //判断上升沿还是下降沿
  GPIO_PinState pin_state = HAL_GPIO_ReadPin(Control_3_GPIO_Port, Control_3_Pin);
  /*上升沿*/
  if(pin_state==GPIO_PIN_SET)
  {
    /*处于等待长按状态*/
    if(button_3_counter_state==COUNTER_START&&button_3_state==BUTTON_STATE_WAIT_LONG)
    {
      button_3_state=BUTTON_STATE_LONG_PRESS;
      button_3_counter=0;
      button_3_counter_state=COUNTER_LONG_START;
    }
    else
    {
      button_3_state = BUTTON_STATE_PRESSED;
    }
  }
  /*下降沿*/
  else if(pin_state==GPIO_PIN_RESET)
  {

    if(button_3_state==BUTTON_STATE_PRESSED&&button_3_counter_state==COUNTER_STOP)
    {
      /*开始计时(5s)*/
      button_3_counter=0;
      button_3_counter_state = COUNTER_START;
      button_3_state = BUTTON_STATE_WAIT_LONG;

    }
    else if(button_3_state==BUTTON_STATE_LONG_PRESS&&button_3_counter_state==COUNTER_LONG_START)
    {
      /*长按计时取消*/
      button_3_counter=0;
      button_3_state=BUTTON_STATE_WAIT_LONG;
      button_3_counter_state=COUNTER_START;
      LED_Switch(LED_RUN_3, LED_TOGGLING);
    }
    else if(button_3_state==BUTTON_STATE_IDLE)
    {
      /*恢复状态*/
      button_3_state=BUTTON_STATE_IDLE;
      button_3_counter_state=COUNTER_STOP;
      button_3_counter=0;
    }
  }
}
/**
  * @brief  HAL库EXTI中断回调函数
  * @param  GPIO_Pin: 触发中断的引脚
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if(GPIO_Pin == Control_1_Pin)
  {
    BUTTON_1_Callback();
  }
  else if(GPIO_Pin == Control_2_Pin)
  {
    BUTTON_2_Callback();
  }
  else if(GPIO_Pin == Control_3_Pin)
  {
    BUTTON_3_Callback();
  }
}