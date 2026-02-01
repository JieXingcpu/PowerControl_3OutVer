#include "INA3221.h"

#include "INA3221_Register.h"
#include "i2c.h"

static void INA3221_Config(INA3221 *handle);
static INA3221_STATE INA3221_Init(INA3221 *self);
static void INA3221_ReadReg(INA3221 *handle);
static void INA3221_WriteReg(INA3221 *handle);
static void Data_Reverse(uint16_t *data) { *data = (*data >> 8) | (*data << 8); }
static void INA3221_ReadReg_DMA(INA3221 *handle);
INA3221_STATE Power_Read_Loop(INA3221 *ina3221, Power_Control *power);
INA3221_STATE ina3221_state;

void Init_Power_Read(INA3221 *ina3221)
{
  ina3221->address = CHIP_ID_REGISTER;
  ina3221->send_data_buffer = 0;
  ina3221->read_data_buffer = 0;
  ina3221->Power_Data.voltage[0] = 0.0f;
  ina3221->Power_Data.voltage[1] = 0.0f;
  ina3221->Power_Data.voltage[2] = 0.0f;
  ina3221->Power_Data.current[0] = 0.0f;
  ina3221->Power_Data.current[1] = 0.0f;
  ina3221->Power_Data.current[2] = 0.0f;
  ina3221->Init = INA3221_Init;
  ina3221->Read_Loop = Power_Read_Loop;
}

INA3221_STATE INA3221_Init(INA3221 *self)
{
  while(hi2c1.State != HAL_I2C_STATE_READY);
  HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS, 0xFF, 1, &self->read_data_buffer, 2, HAL_MAX_DELAY);
  Data_Reverse(&self->read_data_buffer);
  if(self->read_data_buffer != 0x3220)
  {
    return INA3221_STATE_ERROR;
  }
  INA3221_Config(self);  //配置INA3221，只需要配置一次
  return INA3221_STATE_READ_OK;
}
static void INA3221_Config(INA3221 *handle)
{
  while(hi2c1.State != HAL_I2C_STATE_READY);
  handle->address = CONFIG_REGISTER_ADDR;
  handle->send_data_buffer = 0x7527;
  INA3221_WriteReg(handle);
  HAL_Delay(5);
}

static void INA3221_WriteReg(INA3221 *handle)
{
  while(hi2c1.State != HAL_I2C_STATE_READY);
  Data_Reverse(&handle->send_data_buffer);
  HAL_I2C_Mem_Write(&hi2c1, I2C_ADDRESS, handle->address, 1, (uint8_t *)&handle->send_data_buffer, 2, HAL_MAX_DELAY);
  HAL_Delay(5);
}

static void INA3221_ReadReg(INA3221 *handle)
{
  while(hi2c1.State != HAL_I2C_STATE_READY);
  HAL_I2C_Mem_Read(&hi2c1, I2C_ADDRESS, handle->address, 1, (uint8_t *)&handle->read_data_buffer, 2, HAL_MAX_DELAY);
  Data_Reverse(&handle->read_data_buffer);
  HAL_Delay(5);
}

static void INA3221_ReadReg_DMA(INA3221 *handle)
{
  while(hi2c1.State != HAL_I2C_STATE_READY);
  if(HAL_I2C_Mem_Read_DMA(&hi2c1, I2C_ADDRESS, handle->address, 1, (uint8_t *)&handle->read_data_buffer, 2) == HAL_OK)
  {
    ina3221_state = INA3221_STATE_READING;
  } else
  {
    ina3221_state = INA3221_STATE_ERROR;
  }
}

static INA3221_STATE Search_Channel_To_Read(INA3221 *ina3221)
{
  const uint8_t Channel_Map[6] = 
  {
    BUS_VOLTAGE_CHANNEL_1,SHUNT_VOLTAGE_CHANNEL_1,
    BUS_VOLTAGE_CHANNEL_2,SHUNT_VOLTAGE_CHANNEL_2,
    BUS_VOLTAGE_CHANNEL_3,SHUNT_VOLTAGE_CHANNEL_3
  };
  while(ina3221->index<6)
  {
    if(ina3221->channel_state & (1 << (ina3221->index/2)))
    {
      ina3221->address = Channel_Map[ina3221->index];
      ina3221->index++;
      INA3221_ReadReg_DMA(ina3221);
      ina3221_state = INA3221_STATE_READING;
      return ina3221_state;
    }
    else 
    {
      ina3221_state=INA3221_STATE_READING;
      ina3221->index++;
      return ina3221_state;
    }
  }
  ina3221_state = INA3221_STATE_READ_OK;
  return ina3221_state;
}

/*放在main的while中*/
INA3221_STATE Power_Read_Loop(INA3221 *ina3221, Power_Control *power)
{
  if(power != NULL && power->Read_Channel_State != NULL)
  {
    ina3221->channel_state = power->Read_Channel_State(power);//通道状态
  }
  switch(ina3221_state)
  {
    case INA3221_STATE_IDLE:
      ina3221->index = 0;
      return Search_Channel_To_Read(ina3221);
      break;
    case INA3221_STATE_READING:
      return Search_Channel_To_Read(ina3221);
      break;
    case INA3221_STATE_WAIT_DMA:
      //等待DMA完成
      return INA3221_STATE_WAIT_DMA;
      break;
    case INA3221_STATE_READ_OK:
      ina3221_state = INA3221_STATE_IDLE;
      return INA3221_STATE_READ_OK;
      break;
    case INA3221_STATE_SEND_OK:
      return INA3221_STATE_SEND_OK;
      break;
    case INA3221_STATE_SENDING:
      return INA3221_STATE_SENDING;
      break;
    case INA3221_STATE_ERROR:
      return INA3221_STATE_ERROR;
      break;
    default:
      return INA3221_STATE_ERROR;
      break;
  }
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->Instance == hi2c1.Instance)
  {
    ina3221_state = INA3221_STATE_READ_OK;
  }
}
void HAL_I2C_MasterTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
  if(hi2c->Instance == hi2c1.Instance)
  {
    ina3221_state = INA3221_STATE_SEND_OK;
  }
}
