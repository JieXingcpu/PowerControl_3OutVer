#include "LED.h"

#include "gpio.h"

LED_State led_state;

static uint32_t flow_tick_1 = 0;
static uint8_t flow_step_1 = 0;

static uint32_t flow_tick_2 = 0;
static uint8_t flow_step_2 = 0;

static uint32_t flow_tick_3 = 0;
static uint8_t flow_step_3 = 0;

void LED_Init(void)
{
  HAL_GPIO_WritePin(RUN_LED_1_GPIO_Port, RUN_LED_1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RUN_LED_2_GPIO_Port, RUN_LED_2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(RUN_LED_3_GPIO_Port, RUN_LED_3_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(ERR_LED_1_GPIO_Port, ERR_LED_1_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(ERR_LED_2_GPIO_Port, ERR_LED_2_Pin, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(ERR_LED_3_GPIO_Port, ERR_LED_3_Pin, GPIO_PIN_RESET);
}

void LED_Switch(LED_ID led_id, LED_State state)
{
  switch(state)
  {
    case LED_OFF:
      switch(led_id)
      {
        case LED_RUN_1:
          HAL_GPIO_WritePin(RUN_LED_1_GPIO_Port, RUN_LED_1_Pin, GPIO_PIN_RESET);
          break;
        case LED_RUN_2:
          HAL_GPIO_WritePin(RUN_LED_2_GPIO_Port, RUN_LED_2_Pin, GPIO_PIN_RESET);
          break;
        case LED_RUN_3:
          HAL_GPIO_WritePin(RUN_LED_3_GPIO_Port, RUN_LED_3_Pin, GPIO_PIN_RESET);
          break;
        case LED_ERR_1:
          HAL_GPIO_WritePin(ERR_LED_1_GPIO_Port, ERR_LED_1_Pin, GPIO_PIN_RESET);
          break;
        case LED_ERR_2:
          HAL_GPIO_WritePin(ERR_LED_2_GPIO_Port, ERR_LED_2_Pin, GPIO_PIN_RESET);
          break;
        case LED_ERR_3:
          HAL_GPIO_WritePin(ERR_LED_3_GPIO_Port, ERR_LED_3_Pin, GPIO_PIN_RESET);
          break;
      }
      break;
    case LED_ON:
      switch(led_id)
      {
        case LED_RUN_1:
          HAL_GPIO_WritePin(RUN_LED_1_GPIO_Port, RUN_LED_1_Pin, GPIO_PIN_SET);
          break;
        case LED_RUN_2:
          HAL_GPIO_WritePin(RUN_LED_2_GPIO_Port, RUN_LED_2_Pin, GPIO_PIN_SET);
          break;
        case LED_RUN_3:
          HAL_GPIO_WritePin(RUN_LED_3_GPIO_Port, RUN_LED_3_Pin, GPIO_PIN_SET);
          break;
        case LED_ERR_1:
          HAL_GPIO_WritePin(ERR_LED_1_GPIO_Port, ERR_LED_1_Pin, GPIO_PIN_SET);
          break;
        case LED_ERR_2:
          HAL_GPIO_WritePin(ERR_LED_2_GPIO_Port, ERR_LED_2_Pin, GPIO_PIN_SET);
          break;
        case LED_ERR_3:
          HAL_GPIO_WritePin(ERR_LED_3_GPIO_Port, ERR_LED_3_Pin, GPIO_PIN_SET);
          break;
      }
      break;
    case LED_TOGGLING:
      switch(led_id)
      {
        case LED_RUN_1:
          HAL_GPIO_TogglePin(RUN_LED_1_GPIO_Port, RUN_LED_1_Pin);
          break;
        case LED_RUN_2:
          HAL_GPIO_TogglePin(RUN_LED_2_GPIO_Port, RUN_LED_2_Pin);
          break;
        case LED_RUN_3:
          HAL_GPIO_TogglePin(RUN_LED_3_GPIO_Port, RUN_LED_3_Pin);
          break;
        case LED_ERR_1:
          HAL_GPIO_TogglePin(ERR_LED_1_GPIO_Port, ERR_LED_1_Pin);
          break;
        case LED_ERR_2:
          HAL_GPIO_TogglePin(ERR_LED_2_GPIO_Port, ERR_LED_2_Pin);
          break;
        case LED_ERR_3:
          HAL_GPIO_TogglePin(ERR_LED_3_GPIO_Port, ERR_LED_3_Pin);
          break;
      }
      break;
    case LED_FLOWING_1:
      /* 使用非阻塞方式实现流水灯 */
      if(HAL_GetTick() - flow_tick_1 > 250)
      {
        flow_tick_1 = HAL_GetTick();
        switch(flow_step_1)
        {
          case 0:
            HAL_GPIO_WritePin(RUN_LED_1_GPIO_Port, RUN_LED_1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(ERR_LED_1_GPIO_Port, ERR_LED_1_Pin, GPIO_PIN_RESET);
            break;
          case 1:
            HAL_GPIO_WritePin(ERR_LED_1_GPIO_Port, ERR_LED_1_Pin, GPIO_PIN_SET);
            break;
          case 2:
            HAL_GPIO_WritePin(RUN_LED_1_GPIO_Port, RUN_LED_1_Pin, GPIO_PIN_RESET);
            break;
          case 3:
            HAL_GPIO_WritePin(ERR_LED_1_GPIO_Port, ERR_LED_1_Pin, GPIO_PIN_RESET);
            break;
        }
        flow_step_1++;
        if(flow_step_1 > 3) flow_step_1 = 0;
      }
      break;
    case LED_FLOWING_2:
      /* 使用非阻塞方式实现流水灯 */
      if(HAL_GetTick() - flow_tick_2 > 250)
      {
        flow_tick_2 = HAL_GetTick();
        switch(flow_step_2)
        {
          case 0:
            HAL_GPIO_WritePin(RUN_LED_2_GPIO_Port, RUN_LED_2_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(ERR_LED_2_GPIO_Port, ERR_LED_2_Pin, GPIO_PIN_RESET);
            break;
          case 1:
            HAL_GPIO_WritePin(ERR_LED_2_GPIO_Port, ERR_LED_2_Pin, GPIO_PIN_SET);
            break;
          case 2:
            HAL_GPIO_WritePin(RUN_LED_2_GPIO_Port, RUN_LED_2_Pin, GPIO_PIN_RESET);
            break;
          case 3:
            HAL_GPIO_WritePin(ERR_LED_2_GPIO_Port, ERR_LED_2_Pin, GPIO_PIN_RESET);
            break;
        }
        flow_step_2++;
        if(flow_step_2 > 3) flow_step_2 = 0;
      }
      break;
    case LED_FLOWING_3:
      /* 使用非阻塞方式实现流水灯 */
      if(HAL_GetTick() - flow_tick_3 > 250)
      {
        flow_tick_3 = HAL_GetTick();
        switch(flow_step_3)
        {
          case 0:
            HAL_GPIO_WritePin(RUN_LED_3_GPIO_Port, RUN_LED_3_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(ERR_LED_3_GPIO_Port, ERR_LED_3_Pin, GPIO_PIN_RESET);
            break;
          case 1:
            HAL_GPIO_WritePin(ERR_LED_3_GPIO_Port, ERR_LED_3_Pin, GPIO_PIN_SET);
            break;
          case 2:
            HAL_GPIO_WritePin(RUN_LED_3_GPIO_Port, RUN_LED_3_Pin, GPIO_PIN_RESET);
            break;
          case 3:
            HAL_GPIO_WritePin(ERR_LED_3_GPIO_Port, ERR_LED_3_Pin, GPIO_PIN_RESET);
            break;
        }
        flow_step_3++;
        if(flow_step_3 > 3) flow_step_3 = 0;
      }
      break;
  }
}
