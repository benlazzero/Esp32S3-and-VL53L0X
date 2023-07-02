#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"

static const char *TAG = "ESP32S3-TO-VL53L0X";

#define I2C_MASTER_SCL_IO           6 
#define I2C_MASTER_SDA_IO           5
#define I2C_MASTER_NUM              0
#define I2C_MASTER_FREQ_HZ          400000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS       1000

#define VL53L0X_SENSOR_ADDR         0x29

static esp_err_t vl53l0x_register_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_write_read_device(I2C_MASTER_NUM, VL53L0X_SENSOR_ADDR, &reg_addr, 1, data, len, I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
}

static esp_err_t i2c_master_init(void) {
  int i2c_master_port = I2C_MASTER_NUM;
  
  i2c_config_t conf = {
    .mode = I2C_MODE_MASTER,
    .sda_io_num = I2C_MASTER_SDA_IO,
    .scl_io_num = I2C_MASTER_SCL_IO,
    .sda_pullup_en = GPIO_PULLUP_ENABLE,
    .scl_pullup_en = GPIO_PULLUP_ENABLE,
    .master.clk_speed = I2C_MASTER_FREQ_HZ,
  };

  i2c_param_config(i2c_master_port, &conf);
  
  return i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}
 
void app_main(void) {
  uint8_t data[2];
  ESP_ERROR_CHECK(i2c_master_init());
  ESP_LOGI(TAG, "init worked");

  /* read from refrence register 0xC0 */
  esp_err_t ret = vl53l0x_register_read(0xC0, data, sizeof(data));
  ESP_ERROR_CHECK(ret);
  ESP_LOGI(TAG, "REGISTER DUMP = %02X%02X", data[0], data[1]);

  /* test connection */
  if (ret == ESP_OK) {
    uint16_t received_value = data[0];
    uint16_t expected_value = 0xEE; 
    if (received_value == expected_value) {
        ESP_LOGI(TAG, "I2C communication is working correctly");
    } else {
        ESP_LOGE(TAG, "I2C communication error: received 0x%04X, expected 0x%04X", received_value, expected_value);
    } 
  }

  ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
  ESP_LOGI(TAG, "I2C de-initialized successfully");
}
