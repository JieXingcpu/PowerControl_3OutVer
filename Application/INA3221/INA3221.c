#include "INA3221.h"

#include "Buzzer.h"
#include "INA3221_Register.h"
#include "i2c.h"

static INA3221_STATE INA3221_Init(INA3221 *self);
static INA3221_STATE INA3221_Config(INA3221 *handle);
static INA3221_STATE INA3221_ReadReg(INA3221 *handle);
static INA3221_STATE INA3221_WriteReg(INA3221 *handle);
static INA3221_STATE INA3221_ReadVoltage(INA3221 *handle);
static INA3221_STATE INA3221_ReadCurrent(INA3221 *handle);
static void Data_Reverse(uint16_t *data) { *data = (*data >> 8) | (*data << 8); }
static INA3221_STATE Search_Channel_To_Read(INA3221 *ina3221, Power_Control *power);
static INA3221_STATE ina3221_state;
INA3221_STATE Power_Read_Loop(INA3221 *ina3221, Power_Control *power);
static Power_State Power_Voltage_Control_Loop(INA3221 *handle, Power_Control *power);
static Power_State Power_Current_Control_Loop(INA3221 *handle, Power_Control *power);

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
  power_read->Init = INA3221_Init;
  power_read->Read_Loop = Power_Read_Loop;
  power_read->Voltage_Control_Loop = Power_Voltage_Control_Loop;
  power_read->Current_Control_Loop = Power_Current_Control_Loop;
}

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
        ina3221->read_data_mutex = true;
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
  /*电压保护*/
  if(handle->Power_Data.voltage[0] > MAX_POWER_VOLTAGE)
  {
    danger_channel |= POWER_OUT_1;
  } else if(handle->Power_Data.voltage[0] < MIN_POWER_VOLTAGE && handle->Power_Data.voltage[0] > 0)
  {
    warning_channel |= POWER_OUT_1;
  }
  if(handle->Power_Data.voltage[1] > MAX_POWER_VOLTAGE)
  {
    danger_channel |= POWER_OUT_2;
  } else if(handle->Power_Data.voltage[1] < MIN_POWER_VOLTAGE && handle->Power_Data.voltage[1] > 0)
  {
    warning_channel |= POWER_OUT_2;
  }
  if(handle->Power_Data.voltage[2] > MAX_POWER_VOLTAGE)
  {
    danger_channel |= POWER_OUT_3;
  } else if(handle->Power_Data.voltage[2] < MIN_POWER_VOLTAGE && handle->Power_Data.voltage[2] > 0)
  {
    warning_channel |= POWER_OUT_3;
  }
  power->Power_Channel_State &warning_channel ? Buzzer_Switch(BUZZER_WARNING) : Buzzer_Switch(BUZZER_OFF);
  //如果电压异常且持续时间超过设定值,则断电
  if((power->Power_Channel_State & danger_channel) && handle->holding_time > MAX_HOLDING_TIME * 10)
  {
    return power->Switch(power, danger_channel, POWER_OFF, danger_channel);
  }
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
  /*电流保护*/
  if(handle->Power_Data.current[0] > MAX_POWER_CURRENT)
  {
    danger_channel |= POWER_OUT_1;
  }
  if(handle->Power_Data.current[1] > MAX_POWER_CURRENT)
  {
    danger_channel |= POWER_OUT_2;
  }
  if(handle->Power_Data.current[2] > MAX_POWER_CURRENT)
  {
    danger_channel |= POWER_OUT_3;
  }
  //如果电流异常且持续时间超过设定值,则断电
  if(handle->holding_time > MAX_HOLDING_TIME * 10 && danger_channel != 0x00)
  {
    Buzzer_Switch(BUZZER_WARNING);
    return power->Switch(power, danger_channel, POWER_OFF, danger_channel);
  }
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
