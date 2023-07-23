// Mostly from https://github.com/kylehendricks/esp32-vl53l0x/blob/master/platform/src/vl53l0x_platform.c

#include "vl53l0x_platform.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"

#define ACK_CHECK_EN true

VL53L0X_Error esp_to_vl53l0x_error(esp_err_t esp_err) {
  switch (esp_err) {
    case ESP_OK:
      return VL53L0X_ERROR_NONE;
    case ESP_ERR_INVALID_ARG:
      return VL53L0X_ERROR_INVALID_PARAMS;
    case ESP_FAIL:
    case ESP_ERR_INVALID_STATE:
      return VL53L0X_ERROR_CONTROL_INTERFACE;
    case ESP_ERR_TIMEOUT:
      return VL53L0X_ERROR_TIME_OUT;
    default:
      return VL53L0X_ERROR_UNDEFINED;
  }
}

VL53L0X_Error VL53L0X_LockSequenceAccess(VL53L0X_DEV Dev){
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    return Status;
}

VL53L0X_Error VL53L0X_UnlockSequenceAccess(VL53L0X_DEV Dev){
    VL53L0X_Error Status = VL53L0X_ERROR_NONE;

    return Status;
}

// write byte buffer to device
// github.com/kylehendricks
VL53L0X_Error VL53L0X_WriteMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count){
  // make command link
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  i2c_master_start(cmd);
  
  // write I2C address
  i2c_master_write_byte(cmd, ( Dev->i2c_address << 1 ) | I2C_MASTER_WRITE, ACK_CHECK_EN);
  
  // write register
  i2c_master_write_byte(cmd, index, ACK_CHECK_EN);

  // Data
  // Note: Needed to use i2c_master_write_byte as i2c_master_write will not expect an ack
  // after each byte
  for (int i = 0; i < count; i++)
  {
      i2c_master_write_byte(cmd, *(pdata + i), ACK_CHECK_EN);
  }
  
  i2c_master_stop(cmd);
  esp_err_t ret = i2c_master_cmd_begin(Dev->i2c_port_num, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);

  return esp_to_vl53l0x_error(ret);
}

// Reads the requested number of bytes from the device
// github.com/kylehendricks
VL53L0X_Error VL53L0X_ReadMulti(VL53L0X_DEV Dev, uint8_t index, uint8_t *pdata, uint32_t count){
  
  // I2C write steps
  i2c_cmd_handle_t cmd = i2c_cmd_link_create();
  ESP_ERROR_CHECK(i2c_master_start(cmd));
    
  // write I2C address and register
  i2c_master_write_byte(cmd, ( Dev->i2c_address << 1 ) | I2C_MASTER_WRITE, ACK_CHECK_EN);
  i2c_master_write_byte(cmd, index, ACK_CHECK_EN);
  
  // start condition, change flow to read
  ESP_ERROR_CHECK(i2c_master_start(cmd));
  
  // write I2C address
  ESP_ERROR_CHECK(i2c_master_write_byte(cmd, ( Dev->i2c_address << 1 ) | I2C_MASTER_READ, ACK_CHECK_EN));
  
  // read from register
  ESP_ERROR_CHECK(i2c_master_read(cmd, pdata, count, I2C_MASTER_LAST_NACK));

  ESP_ERROR_CHECK(i2c_master_stop(cmd));
  esp_err_t ret = i2c_master_cmd_begin(Dev->i2c_port_num, cmd, 1000 / portTICK_PERIOD_MS);
  i2c_cmd_link_delete(cmd);

  return esp_to_vl53l0x_error(ret);
}


VL53L0X_Error VL53L0X_WrByte(VL53L0X_DEV Dev, uint8_t index, uint8_t data){
  return VL53L0X_WriteMulti(Dev, index, &data, 1);
}

VL53L0X_Error VL53L0X_WrWord(VL53L0X_DEV Dev, uint8_t index, uint16_t data){
    uint8_t buffer[2]; // 2
    buffer[0] = (uint8_t)(data >> 8);
    buffer[1] = (uint8_t)(data &  0x00FF);

    return VL53L0X_WriteMulti(Dev, index, buffer, 2);
}

VL53L0X_Error VL53L0X_WrDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t data){
    uint8_t buffer[4]; // 4

    buffer[0] = (uint8_t) (data >> 24);
    buffer[1] = (uint8_t)((data &  0x00FF0000) >> 16);
    buffer[2] = (uint8_t)((data &  0x0000FF00) >> 8);
    buffer[3] = (uint8_t) (data &  0x000000FF);

    return VL53L0X_WriteMulti(Dev, index, buffer, 4);
}

VL53L0X_Error VL53L0X_UpdateByte(VL53L0X_DEV Dev, uint8_t index, uint8_t AndData, uint8_t OrData){
    VL53L0X_Error status;
    uint8_t data;

    status = VL53L0X_RdByte(Dev, index, &data);

    if (status != VL53L0X_ERROR_NONE)
        return status;

    data = (data & AndData) | OrData;

    return VL53L0X_WrByte(Dev, index, data);
}

VL53L0X_Error VL53L0X_RdByte(VL53L0X_DEV Dev, uint8_t index, uint8_t *data){
  return VL53L0X_ReadMulti(Dev, index, data, 1);
}

VL53L0X_Error VL53L0X_RdWord(VL53L0X_DEV Dev, uint8_t index, uint16_t *data){
    VL53L0X_Error status;
    uint8_t  buffer[2];

    status = VL53L0X_ReadMulti(Dev, index, buffer, 2);
    *data = ((uint16_t)buffer[0]<<8) + (uint16_t)buffer[1];

    return status;
}

VL53L0X_Error  VL53L0X_RdDWord(VL53L0X_DEV Dev, uint8_t index, uint32_t *data){
    VL53L0X_Error status;
    uint8_t  buffer[4];

    status = VL53L0X_ReadMulti(Dev, index, buffer, 4);
    *data = ((uint32_t)buffer[0]<<24) + ((uint32_t)buffer[1]<<16) +
             ((uint32_t)buffer[2]<<8) + (uint32_t)buffer[3];

    return status;
}

/**
 * @brief execute delay in all polling API call
 *
 * A typical multi-thread or RTOs implementation is to sleep the task for some 5ms (with 100Hz max rate faster polling is not needed)
 * if nothing specific is need you can define it as an empty/void macro
 * @code
 * #define VL53L0X_PollingDelay(...) (void)0
 * @endcode
 * @param Dev       Device Handle
 * @param Delay     -1 = 5ms delay, 100 = 100ms
 * @return  VL53L0X_ERROR_NONE        Success
 * @return  "Other error code"    See ::VL53L0X_Error
 */
VL53L0X_Error VL53L0X_PollingDelay(VL53L0X_DEV Dev, int Delay) {
  if (Delay != -1) 
    vTaskDelay(Delay / portTICK_PERIOD_MS);

  return VL53L0X_ERROR_NONE;
}
