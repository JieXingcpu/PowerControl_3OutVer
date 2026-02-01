#include "LED.h"

#include "gpio.h"

LED_State led_state;

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
      HAL_GPIO_WritePin(RUN_LED_1_GPIO_Port, RUN_LED_1_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(ERR_LED_1_GPIO_Port, ERR_LED_1_Pin, GPIO_PIN_RESET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(ERR_LED_1_GPIO_Port, ERR_LED_1_Pin, GPIO_PIN_SET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(RUN_LED_1_GPIO_Port, RUN_LED_1_Pin, GPIO_PIN_RESET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(ERR_LED_1_GPIO_Port, ERR_LED_1_Pin, GPIO_PIN_RESET);
      HAL_Delay(250);
      break;
    case LED_FLOWING_2:
      HAL_GPIO_WritePin(RUN_LED_2_GPIO_Port, RUN_LED_2_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(ERR_LED_2_GPIO_Port, ERR_LED_2_Pin, GPIO_PIN_RESET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(ERR_LED_2_GPIO_Port, ERR_LED_2_Pin, GPIO_PIN_SET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(RUN_LED_2_GPIO_Port, RUN_LED_2_Pin, GPIO_PIN_RESET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(ERR_LED_2_GPIO_Port, ERR_LED_2_Pin, GPIO_PIN_RESET);
      HAL_Delay(250);
      break;
    case LED_FLOWING_3:
      HAL_GPIO_WritePin(RUN_LED_3_GPIO_Port, RUN_LED_3_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(ERR_LED_3_GPIO_Port, ERR_LED_3_Pin, GPIO_PIN_RESET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(ERR_LED_3_GPIO_Port, ERR_LED_3_Pin, GPIO_PIN_SET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(RUN_LED_3_GPIO_Port, RUN_LED_3_Pin, GPIO_PIN_RESET);
      HAL_Delay(250);
      HAL_GPIO_WritePin(ERR_LED_3_GPIO_Port, ERR_LED_3_Pin, GPIO_PIN_RESET);
      HAL_Delay(250);
      break;
  }
}
