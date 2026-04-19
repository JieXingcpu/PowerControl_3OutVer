#include "INA3221.h"

#include <string.h>

#include "Buzzer.h"
#include "INA3221_Register.h"
#include "i2c.h"

static INA3221_STATE INA3221_Init(INA3221 *self);
static INA3221_STATE INA3221_Config(INA3221 *handle);
static INA3221_STATE INA3221_ReadReg(INA3221 *handle);
static INA3221_STATE INA3221_WriteReg(INA3221 *handle);
static INA3221_STATE INA3221_ReadVoltage(INA3221 *handle);
static INA3221_STATE INA3221_ReadCurrent(INA3221 *handle);
INA3221_STATE Power_Read_Loop(INA3221 *ina3221, Power_Control *power);
static INA3221_STATE Search_Channel_To_Read(INA3221 *ina3221, Power_Control *power);
static Power_State Power_Voltage_Control_Loop(INA3221 *handle, Power_Control *power);
static Power_State Power_Current_Control_Loop(INA3221 *handle, Power_Control *power);
static INA3221_STATE Median_Filter(INA3221 *self, Power_Control *power);
static void Data_Reverse(uint16_t *data) { *data = (*data >> 8) | (*data << 8); }
static INA3221_STATE ina3221_state;

// float temp_v[FILTER_BUFFER_SIZE];
// float temp_c[FILTER_BUFFER_SIZE];
// static float voltage_buffer[3][FILTER_BUFFER_SIZE] = {0};
// static float current_buffer[3][FILTER_BUFFER_SIZE] = {0};
// static uint8_t buffer_index = 0;

/**
  * @brief  初始化INA3221结构体
  * @param  ina3221: INA3221结构体的指针
  * @retval None
 */
void Init_Power_Read(INA3221 *power_read)
{
  power_read->address = CHIP_ID_REGISTER;
  power_read->send_data_buffer = 0;
  power_read->read_data_buffer = 0;
  power_read->Power_Data = (Power_DataTypeDef){0};
  power_read->read_data_mutex = false;
  power_read->channel_state = 0;
  power_read->index = 0;
  for(uint8_t i = 0; i < 3; i++)
  {
    power_read->voltage_counter[i] = 0;
    power_read->current_counter[i] = 0;
    power_read->sudden_current_counter[i] = 0;
    power_read->voltage_count_flag[i] = false;
    power_read->current_count_flag[i] = false;
    power_read->sudden_current_count_flag[i] = false;
  }
  power_read->Init = INA3221_Init;
  power_read->Read_Loop = Power_Read_Loop;
  power_read->Voltage_Control_Loop = Power_Voltage_Control_Loop;
  power_read->Current_Control_Loop = Power_Current_Control_Loop;
  power_read->Median_Filter = Median_Filter;
}
/**
  * @brief  初始化INA3221
  * @param  self: INA3221结构体的指针
  * @note 该函数会检查INA3221的芯片ID以确认设备是否正常连接，并配置INA3221的配置寄存器
  * @retval INA3221_STATE
 */
static INA3221_STATE INA3221_Init(INA3221 *self)
{
  while(hi2c1.State != HAL_I2C_STATE_READY)
  {
    uint32_t timeout = HAL_GetTick();
    if(HAL_GetTick() - timeout > 1000) return INA3221_STATE_ERROR;
  }
  HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS, 0xFF, 1, (uint8_t *)&self->read_data_buffer, 2, 5);
  Data_Reverse(&self->read_data_buffer);
  if(self->read_data_buffer != 0x3220)
  {
    return INA3221_STATE_ERROR;
  }
  INA3221_Config(self);  //配置INA3221，只需要配置一次
  return INA3221_STATE_IDLE;
}
/**
  * @brief  搜索需要读取的通道并进行读取
  * @param  ina3221: INA3221结构体的指针
  * @param  power: Power_Control结构体的指针
  * @note 该函数会根据传入的Power_Control结构体中的通道状态来判断需要读取哪些通道的数据，并进行读取
  * @retval INA3221_STATE
  * @warning 该函数会修改INA3221结构体中的电压和电流数据
 */
static INA3221_STATE Search_Channel_To_Read(INA3221 *ina3221, Power_Control *power)
{
  const uint8_t Channel_Map[6] = {BUS_VOLTAGE_CHANNEL_1, SHUNT_VOLTAGE_CHANNEL_1, BUS_VOLTAGE_CHANNEL_2, SHUNT_VOLTAGE_CHANNEL_2, BUS_VOLTAGE_CHANNEL_3, SHUNT_VOLTAGE_CHANNEL_3};
  while(ina3221->index < 6)
  {
    if(ina3221->channel_state & (1 << (ina3221->index / 2)))
    {
      ina3221->address = Channel_Map[ina3221->index];
      if(ina3221->index % 2 == 0)
      {
        if(INA3221_ReadVoltage(ina3221) == INA3221_STATE_ERROR) return INA3221_STATE_ERROR;
      } else
      {
        if(INA3221_ReadCurrent(ina3221) == INA3221_STATE_ERROR) return INA3221_STATE_ERROR;
      }
      ina3221->index++;
      return INA3221_STATE_READING;
    }
    ina3221->index++;
  }
  return INA3221_STATE_READ_OK;
}
/**
  * @brief  电源控制中的检测模块INA3221读取循环
  * @param  handle: INA3221结构体的指针
  * @param  power: Power_Control结构体的指针
  * @note 该函数需要循环调用以完成对所有通道的读取，已包含电流控制功能
  * @retval INA3221_STATE
 */
INA3221_STATE Power_Read_Loop(INA3221 *ina3221, Power_Control *power)
{
  if(power != NULL && power->Read_Channel_State != NULL)
  {
    ina3221->channel_state = power->Read_Channel_State(power);  //通道状态
  }
  switch(ina3221_state)
  {
    case INA3221_STATE_IDLE:
      ina3221->index = 0;
      ina3221_state = INA3221_STATE_READING;
      break;
    case INA3221_STATE_READING:
      if(ina3221->read_data_mutex != true)
      {
        ina3221->read_data_mutex = true;  //上锁，防止在读取过程中被其他函数修改数据
        ina3221_state = Search_Channel_To_Read(ina3221, power);
        ina3221->read_data_mutex = false;
      }
      break;
    case INA3221_STATE_READ_OK:
      ina3221_state = INA3221_STATE_IDLE;
      return INA3221_STATE_READ_OK;
      break;
    case INA3221_STATE_ERROR:
      return INA3221_STATE_ERROR;
      break;
  }
  return ina3221_state;
}
/**
  * @brief  中值滤波函数
  * @param  self: INA3221结构体的指针
  * @param  power: Power_Control结构体的指针
  * @retval INA3221_STATE
  * @note 会修改INA3221结构体中的电压和电流数据为滤波后的结果
  * @note 需要初始化滤波器数据(FILTER_BUFFER_SIZE个数据)，初始阶段会直接存入数据，直到滤波器装满数据后才进行正常的滤波流程
 */
static INA3221_STATE Median_Filter(INA3221 *self, Power_Control *power)
{
  //去极值平均滤波函数
  static float voltage_buffer[3][FILTER_BUFFER_SIZE] = {0};
  static float current_buffer[3][FILTER_BUFFER_SIZE] = {0};
  static uint8_t buffer_index = 0;

  /*初始化滤波器中的数值*/
  if(self->filter_init != true)
  {
    for(uint8_t i = 0; i < 3; i++)
    {
      voltage_buffer[i][buffer_index] = self->Power_Data.voltage[i];
      current_buffer[i][buffer_index] = self->Power_Data.current[i];
    }
    buffer_index++;
    if(buffer_index >= FILTER_BUFFER_SIZE)
    {
      buffer_index = 0;
      self->filter_init = true;
    }
    return INA3221_STATE_READ_OK;
  }
  /*正常滤波流程*/
  for(uint8_t i = 0; i < 3; i++)
  {
    voltage_buffer[i][buffer_index] = self->Power_Data.voltage[i];
    current_buffer[i][buffer_index] = self->Power_Data.current[i];
  }
  buffer_index++;
  if(buffer_index >= FILTER_BUFFER_SIZE)
  {
    buffer_index = 0;
  }
  for(uint8_t i = 0; i < 3; i++)
  {
    float temp_v[FILTER_BUFFER_SIZE];
    float temp_c[FILTER_BUFFER_SIZE];
    //复制到临时数组，避免破坏原有的时序
    memcpy(temp_v, voltage_buffer[i], sizeof(float) * FILTER_BUFFER_SIZE);
    memcpy(temp_c, current_buffer[i], sizeof(float) * FILTER_BUFFER_SIZE);
    //对临时数组的数据进行冒泡排序
    for(uint8_t j = 0; j < FILTER_BUFFER_SIZE - 1; j++)
    {
      for(uint8_t k = 0; k < FILTER_BUFFER_SIZE - j - 1; k++)
      {
        if(temp_v[k] > temp_v[k + 1])
        {
          float temp = temp_v[k];
          temp_v[k] = temp_v[k + 1];
          temp_v[k + 1] = temp;
        }
        if(temp_c[k] > temp_c[k + 1])
        {
          float temp = temp_c[k];
          temp_c[k] = temp_c[k + 1];
          temp_c[k + 1] = temp;
        }
      }
    }
    //取中位数及去除极值求平均
    float sum_v = 0;
    float sum_c = 0;
    for(uint8_t count = 1; count < FILTER_BUFFER_SIZE - 1; count++)
    {
      sum_v += temp_v[count];
      sum_c += temp_c[count];
    }
    self->Power_Data.voltage[i] = sum_v / (FILTER_BUFFER_SIZE - 2);
    self->Power_Data.current[i] = sum_c / (FILTER_BUFFER_SIZE - 2);
  }
  return INA3221_STATE_READ_OK;
}

/**
  * @brief  电压保护控制
  * @param  handle: INA3221结构体的指针
  * @param  power: Power_Control结构体的指针
  * @warning 该函数会直接断开对应通道的输出
  * @note 只用于检测所有通道的电压，低于设定值则断开对应通道输出
  * @retval Power_State
 */
static Power_State Power_Voltage_Control_Loop(INA3221 *handle, Power_Control *power)
{
  Power_Output_Channel danger_channel = 0x00;
  Power_Output_Channel warning_channel = 0x00;
  static float last_voltage_max[3] = {0.0f};
  /*电压保护*/
  for(uint8_t i = 0; i < 3; i++)
  {
    /*排除未开启的通道*/
    if((power->Power_Channel_State & (1U << i)) == 0)
    {
      handle->Power_Data.voltage[i] = 0.0f;
      handle->voltage_count_flag[i] = false;
      handle->voltage_counter[i] = 0;
      continue;
    }
    /*大于最大电压*/
    if(handle->Power_Data.voltage[i] > MAX_POWER_VOLTAGE)
    {
      handle->voltage_count_flag[i] = true;
      /*持续大于最大电压*/
      if(handle->voltage_counter[i] > MAX_HOLDING_TIME * 10)
      {
        danger_channel |= (1U << i);  //标记需要断电的通道
        handle->voltage_count_flag[i] = false;
        handle->voltage_counter[i] = 0;
      }
    } else
    {
      if(handle->Power_Data.voltage[i] < MIN_POWER_VOLTAGE && handle->Power_Data.voltage[i] > 5.0f)
      {
        /*低压报警*/
        warning_channel |= (1U << i);
      }
      /*电压恢复正常，清除该通道的异常标志和计数*/
      if(handle->Power_Data.voltage[i] >= MIN_POWER_VOLTAGE)
      {
        if(handle->voltage_count_flag[i] == true)
        {
          handle->voltage_count_flag[i] = false;
          handle->voltage_counter[i] = 0;
        }
      }
    }
    /*急停通道*/
    if(last_voltage_max[i] - handle->Power_Data.voltage[i] >= 15.0f)
    {
      danger_channel |= (1U << i);
    }
    if(handle->Power_Data.voltage[i] > last_voltage_max[i]) last_voltage_max[i] = handle->Power_Data.voltage[i];
  }
  power->Power_Channel_State &(warning_channel | danger_channel) ? Buzzer_Switch(BUZZER_WARNING) : Buzzer_Switch(BUZZER_OFF);
  if(danger_channel != 0x00) power->Switch(power, danger_channel, POWER_OFF, danger_channel);
  return power->Power_Channel_State;
}
/**
  * @brief  电流保护控制
  * @param  handle: INA3221结构体的指针
  * @param  power: Power_Control结构体的指针
  * @retval Power_State
  * @warning 该函数会直接断开对应通道的输出
  * @note 放在检测完电流中调用，保证过流快速响应
  */
static Power_State Power_Current_Control_Loop(INA3221 *handle, Power_Control *power)
{
  Power_Output_Channel danger_channel = 0x00;
  /*电流保护：只检查已开启的通道，并将检测和断电逻辑合并在一次循环中完成*/
  for(uint8_t i = 0; i < 3; i++)
  {
    // 如果该通道未开启, 则跳过检测并清空所有标志与计数器
    if((power->Power_Channel_State & (1U << i)) == 0)
    {
      handle->Power_Data.current[i] = 0.0f;
      handle->current_count_flag[i] = false;
      handle->current_counter[i] = 0;
      handle->sudden_current_count_flag[i] = false;
      handle->sudden_current_counter[i] = 0;
      continue;
    }
    /*电流突变保护,如果电流突然超过MAX_CURRENT_SUDDEN*/
    if(handle->Power_Data.current[i] >= MAX_CURRENT_SUDDEN)
    {
      handle->sudden_current_count_flag[i] = true;
      danger_channel |= (1U << i);
      if(handle->sudden_current_counter[i] > SUDDEN_CURRENT_HOLDING_TIME)
      {
        /*最大电流保护*/
        power->Switch(power, (1U << i), POWER_OFF, (1U << i));
        handle->sudden_current_count_flag[i] = false;  //关闭突变计数
        handle->sudden_current_counter[i] = 0;         //重置突变计数
      }
    }
    //电流异常(MAX_POWER_CURRENT~MAX_CURRENT_SUDDEN)且持续时间超过设定值
    else if(handle->Power_Data.current[i] > MAX_POWER_CURRENT && handle->Power_Data.current[i] < MAX_CURRENT_SUDDEN)
    {
      danger_channel |= (1U << i);
      handle->current_count_flag[i] = true;
      if(handle->current_counter[i] > MAX_HOLDING_TIME * 10)
      {
        power->Switch(power, (1U << i), POWER_OFF, (1U << i));
        handle->current_count_flag[i] = false;
        handle->current_counter[i] = 0;
      }
    } else  // 电流正常，清除该通道的所有异常标志和计数
    {
      handle->current_count_flag[i] = false;
      handle->current_counter[i] = 0;
      handle->sudden_current_count_flag[i] = false;
      handle->sudden_current_counter[i] = 0;
    }
  }
  if(danger_channel != 0x00) Buzzer_Switch(BUZZER_WARNING);
  return power->Power_Channel_State;
}
/**
  * @brief  配置INA3221的配置寄存器
  * @param  handle: INA3221结构体的指针
  * @retval None
 */
static INA3221_STATE INA3221_Config(INA3221 *handle)
{
  uint32_t timeout = HAL_GetTick();
  while(hi2c1.State != HAL_I2C_STATE_READY)
  {
    if(HAL_GetTick() - timeout > 1000) return INA3221_STATE_ERROR;
  }
  handle->address = CONFIG_REGISTER_ADDR;
  handle->send_data_buffer = 0x7527;
  if(INA3221_WriteReg(handle) == INA3221_STATE_ERROR) return INA3221_STATE_ERROR;
  HAL_Delay(5);
  return INA3221_STATE_IDLE;
}
/**
  * @brief  写入INA3221寄存器
  * @param  handle: INA3221结构体的指针
  * @retval None
 */
static INA3221_STATE INA3221_WriteReg(INA3221 *handle)
{
  uint32_t timeout = HAL_GetTick();
  while(hi2c1.State != HAL_I2C_STATE_READY)
  {
    if(HAL_GetTick() - timeout > 1000) return INA3221_STATE_ERROR;
  }
  Data_Reverse(&handle->send_data_buffer);
  HAL_I2C_Mem_Write(&hi2c1, I2C_ADDRESS, handle->address, 1, (uint8_t *)&handle->send_data_buffer, 2, 5);
  return INA3221_STATE_IDLE;
}
/**
  * @brief  读取INA3221寄存器
  * @param  handle: INA3221结构体的指针
  * @retval None
 */
static INA3221_STATE INA3221_ReadReg(INA3221 *handle)
{
  uint32_t timeout = HAL_GetTick();
  while(hi2c1.State != HAL_I2C_STATE_READY)
  {
    if(HAL_GetTick() - timeout > 1000) return INA3221_STATE_ERROR;
  }
  HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS, handle->address, 1, (uint8_t *)&handle->read_data_buffer, 2, 5);
  Data_Reverse(&handle->read_data_buffer);
  return INA3221_STATE_IDLE;
}
/**
  * @brief  读取INA3221的电压值
  * @param  handle: INA3221结构体的指针
  * @retval None
 */
static INA3221_STATE INA3221_ReadVoltage(INA3221 *handle)
{
  if(INA3221_ReadReg(handle) == INA3221_STATE_ERROR) return INA3221_STATE_ERROR;
  handle->Power_Data.voltage[handle->index / 2] = (float)handle->read_data_buffer / 1000;
  return INA3221_STATE_IDLE;
}
/**
  * @brief  读取INA3221的电流值
  * @param  handle: INA3221结构体的指针
  * @retval None
 */
static INA3221_STATE INA3221_ReadCurrent(INA3221 *handle)
{
  if(INA3221_ReadReg(handle) == INA3221_STATE_ERROR) return INA3221_STATE_ERROR;
  handle->Power_Data.current[handle->index / 2] = (float)handle->read_data_buffer / 500;
  return INA3221_STATE_IDLE;
}
