/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* clang-format off */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "i2c.h"
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Power.h"
#include "INA3221.h"
#include "Buzzer.h"
#include "LED.h"
#include "Button.h"
#include "Message.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// #define USE_DEBUG_PRINT  //启用调试打印,会将电压电流数据通过串口打印出来

#ifdef USE_DEBUG_PRINT
#include <stdio.h>
#endif
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
Power_Control power;
INA3221_STATE Ina3221_State;
INA3221 power_read;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifdef USE_DEBUG_PRINT
int __io_putchar(int ch)
{
  while(!(USART2->SR & (1 << 7)))
  {
  }
  USART2->DR = (uint8_t)ch;
  return ch;
}
int fputc(int ch, FILE *f)
{
  (void)f;
  return __io_putchar(ch);
}
void print_callback(Power_Control *power, INA3221 *ina3221)
{
  int voltage_1 = (int)(ina3221->Power_Data.voltage[0] * 1000);
  // int voltage_2 = (int)(ina3221->Power_Data.voltage[1] * 1000);
  // int voltage_3 = (int)(ina3221->Power_Data.voltage[2] * 1000);
  int point1=voltage_1%1000;
  // int point2=voltage_2%1000;
  // int point3=voltage_3%1000;
  
  // printf("Power State: %d\r\n", power->Power_Channel_State);
  printf("Voltage: %d.%03d V\r\n", voltage_1/1000, point1);
  // printf("Voltage: %d.%03d V\r\n", voltage_2/1000, point2);
  // printf("Voltage: %d.%03d V\r\n", voltage_3/1000, point3);
  // int current_1 = (int)(ina3221->Power_Data.current[0] * 1000);
  // int cur_ponit1 = current_1 % 1000;
  // printf("%d.%03d\r\n",current_1/1000,cur_ponit1);
}
#endif
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_CAN_Init();
  MX_I2C1_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_IWDG_Init();
  /* USER CODE BEGIN 2 */
  Init_Power_Control(&power);
  Init_Power_Read(&power_read);
  power.Init(&power);
  CAN_Connect_Init();
  Ina3221_State = power_read.Init(&power_read);
  Button_Init();
  LED_Init();
  Buzzer_Init();
  Buzzer_Switch(BUZZER_SYSTEM_INIT);
  Send_Period_Init();
  HAL_TIM_Base_Start_IT(&htim3);
  __HAL_DBGMCU_FREEZE_IWDG();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_IWDG_Refresh(&hiwdg);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* clang-format on */
    /*按键LED部分*/
    if(button_1_counter_state == COUNTER_LONG_START)
    {
      LED_Switch(LED_RUN_1, LED_FLOWING_1);
    }
    if(button_2_counter_state == COUNTER_LONG_START)
    {
      LED_Switch(LED_RUN_2, LED_FLOWING_2);
    }
    if(button_3_counter_state == COUNTER_LONG_START)
    {
      LED_Switch(LED_RUN_3, LED_FLOWING_3);
    }
    /*按键POWER部分*/
    if(button_1_state == BUTTON_STATE_CHANGE)
    {
      if(power.Power_Channel_State & POWER_ON_1)
      {
        power.Switch(&power, POWER_OUT_1, POWER_OFF, 0x00);
      } else
      {
        power.Switch(&power, POWER_OUT_1, POWER_ON, 0x00);
      }
      button_1_state = BUTTON_STATE_IDLE;
    }
    if(button_2_state == BUTTON_STATE_CHANGE)
    {
      if(power.Power_Channel_State & POWER_ON_2)
      {
        power.Switch(&power, POWER_OUT_2, POWER_OFF, 0x00);
      } else
      {
        power.Switch(&power, POWER_OUT_2, POWER_ON, 0x00);
      }
      button_2_state = BUTTON_STATE_IDLE;
    }
    if(button_3_state == BUTTON_STATE_CHANGE)
    {
      if(power.Power_Channel_State & POWER_ON_3)
      {
        power.Switch(&power, POWER_OUT_3, POWER_OFF, 0x00);
      } else
      {
        power.Switch(&power, POWER_OUT_3, POWER_ON, 0x00);
      }
      button_3_state = BUTTON_STATE_IDLE;
    }
    /*电源监测部分*/
    Ina3221_State = power_read.Read_Loop(&power_read, &power);
    if(Ina3221_State == INA3221_STATE_READ_OK)
    {
      /*进行一次电源检测*/
      power_read.Voltage_Control_Loop(&power_read, &power);
      power_read.Current_Control_Loop(&power_read, &power);
      if(power_read.holding_time > MAX_HOLDING_TIME * 10+1)  //如果持续时间超过最大值,则重置
      {
        power_read.holding_time = 0;
      }
    } else if(Ina3221_State == INA3221_STATE_ERROR)
    {
      while(1);//如果电源检测发生错误,则进入死循环,等待系统重启
    }
    #ifdef USE_DEBUG_PRINT
    print_callback(&power, &power_read);
    #endif
    /*清空对于关闭后的端口的电压和电流数据*/
    if(power_read.read_data_mutex != true)
    {
      power_read.read_data_mutex = true;
      for(uint8_t i = 0; i < 3; i++)
      {
        if((power_read.channel_state & (1U << i)) == 0U)
        {
          power_read.Power_Data.voltage[i] = 0.0f;
          power_read.Power_Data.current[i] = 0.0f;
        }
      }
      power_read.read_data_mutex = false;
    }
    if(power.Power_Channel_State == POWER_ALL_OFF)
    {
      Buzzer_Switch(BUZZER_OFF);
    }
    HAL_Delay(10);
  }
  /* clang-format off */
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
