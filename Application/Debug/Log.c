#include "Log.h"

#include <stdio.h>
#include <string.h>

#include "INA3221.h"
#include "Power.h"
#include "usart.h"

static uint8_t log_buffer[20];
static int Vol_tx_data[3];
static int Vol_tx_data_float[3];
static int Cur_tx_data[3];
static int Cur_tx_data_float[3];
Log Log_State;
static Log_Mutex Log_Mutex_State;

void Log_Init(void) { HAL_UARTEx_ReceiveToIdle_IT(&huart2, log_buffer, sizeof(log_buffer)); }

void Log_Check(void)
{
  if(Log_State == LOG_CHECK)
  {
    Log_Mutex_State = LOG_MUTEX_LOCKED;  //锁定互斥锁,防止在检查过程中接收中断修改Log_State
    for(uint8_t i = 0; i < sizeof(log_buffer) - 3; i++)
    {
      if(log_buffer[i] == 'o' || log_buffer[i] == 'O')
      {
        if(log_buffer[i + 1] == 'n' || log_buffer[i + 1] == 'N')
        {
          Log_State = LOG_ON;
          memset(log_buffer, 0, sizeof(log_buffer));
          Log_Mutex_State = LOG_MUTEX_UNLOCKED;  //解锁互斥锁
          return;
        } else if((log_buffer[i + 1] == 'f' || log_buffer[i + 1] == 'F') && (log_buffer[i + 2] == 'f' || log_buffer[i + 2] == 'F'))
        {
          Log_State = LOG_OFF;
          memset(log_buffer, 0, sizeof(log_buffer));
          Log_Mutex_State = LOG_MUTEX_UNLOCKED;  //解锁互斥锁
          return;
        }
      }
    }
    Log_Mutex_State = LOG_MUTEX_UNLOCKED;  //解锁互斥锁
  }
}
void Log_printf(Power_Control *power, INA3221 *ina3221)
{
  if(power == NULL || ina3221 == NULL)
  {
    printf("Invalid\r\n");
    return;
  }
  if(Log_Mutex_State == LOG_MUTEX_LOCKED)  //如果互斥锁被锁定,说明正在检查Log_State,此时不进行打印,以免发生冲突
  {
    return;
  }
  Log_Mutex_State = LOG_MUTEX_LOCKED;  //锁定互斥锁,防止在打印过程中接收中断修改Log_State
  Vol_tx_data[0] = (int)(ina3221->Power_Data.voltage[0] * 1000);
  Vol_tx_data_float[0] = (int)(ina3221->Power_Data.voltage[0] * 1000) % 1000;
  Vol_tx_data[1] = (int)(ina3221->Power_Data.voltage[1] * 1000);
  Vol_tx_data_float[1] = (int)(ina3221->Power_Data.voltage[1] * 1000) % 1000;
  Vol_tx_data[2] = (int)(ina3221->Power_Data.voltage[2] * 1000);
  Vol_tx_data_float[2] = (int)(ina3221->Power_Data.voltage[2] * 1000) % 1000;
  Cur_tx_data[0] = (int)(ina3221->Power_Data.current[0] * 1000);
  Cur_tx_data_float[0] = (int)(ina3221->Power_Data.current[0] * 1000) % 1000;
  Cur_tx_data[1] = (int)(ina3221->Power_Data.current[1] * 1000);
  Cur_tx_data_float[1] = (int)(ina3221->Power_Data.current[1] * 1000) % 1000;
  Cur_tx_data[2] = (int)(ina3221->Power_Data.current[2] * 1000);
  Cur_tx_data_float[2] = (int)(ina3221->Power_Data.current[2] * 1000) % 1000;
  printf("%d.%03d,%d.%03d,%d.%03d,%d.%03d,%d.%03d,%d.%03d\r\n", Vol_tx_data[0] / 1000, Vol_tx_data_float[0], Vol_tx_data[1] / 1000, Vol_tx_data_float[1], Vol_tx_data[2] / 1000, Vol_tx_data_float[2], Cur_tx_data[0] / 1000, Cur_tx_data_float[0], Cur_tx_data[1] / 1000, Cur_tx_data_float[1], Cur_tx_data[2] / 1000, Cur_tx_data_float[2]);
  Log_Mutex_State = LOG_MUTEX_UNLOCKED;  //解锁互斥锁
}

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

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
  if(huart->Instance == USART2)
  {
    if(Log_Mutex_State == LOG_MUTEX_UNLOCKED)
    {
      Log_State = LOG_CHECK;
    }
    HAL_UARTEx_ReceiveToIdle_IT(&huart2, log_buffer, sizeof(log_buffer));
  }
}
