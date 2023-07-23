#include <stdio.h>
#include "esp_log.h"
#include "driver/i2c.h"
#include "device_init.h"
#include "device_threshold.h"
#include "device_sensor_active.h"
#include "device_camera.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "nvs_flash.h"
#include "device_wifi.h"

static const char *TAG = "ESP32S3-TO-VL53L0X";

// cooldown time for after image is taken
void coolDown() {
  printf("in cooldown\n");
  fflush(stdout);
  vTaskDelay(20000 / portTICK_PERIOD_MS); 
};

VL53L0X_Error mainProcess(VL53L0X_Dev_t *pMyDevice) {
  VL53L0X_Error Status = VL53L0X_ERROR_NONE;
  
  // initialise sensor
  Status = vl53l0x_init(pMyDevice);
  
  // initial threshold
  uint32_t BaseThreshold = 0; 
  Status = calibrate_threshold(pMyDevice, 30, &BaseThreshold);
  printf("base thresh: %lu\n", BaseThreshold);
  
  // inf loop untill threshold reached
  Status = sensor_activate(pMyDevice, BaseThreshold);
  return Status;
}

void app_main(void) {
  VL53L0X_Error Status = VL53L0X_ERROR_NONE;
  VL53L0X_Dev_t vl_dev;
  VL53L0X_Dev_t *pMyDevice = &vl_dev;

  ESP_ERROR_CHECK(i2c_master_init());
  ESP_LOGI(TAG, "I2C Init Complete");
  vTaskDelay(500 / portTICK_PERIOD_MS);
  
  // init VL53L0X after reset 
  vl_dev.i2c_address = VL53L0X_SENSOR_ADDR;
  vl_dev.i2c_port_num = I2C_MASTER_NUM;
  Status = VL53L0X_DataInit(&vl_dev);
  ESP_ERROR_CHECK(Status);
  
  // init NVS for wifi connection
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  // wifi init
  wifi_init_sta();
  
  // camera init
  cameraSetup();
  // this delay is probably not needed but gives some time for inits if needed 
  vTaskDelay(500 / portTICK_PERIOD_MS); 

  // start the main application loop
  // should exist here for life unless something goes wrong
  while (1) {
    Status = mainProcess(pMyDevice);
    ESP_ERROR_CHECK(Status);
    ret = takeImage();
    ESP_ERROR_CHECK(ret);
    coolDown();
  }
  
  // unreachable but if you want to set a stop condtion on the main process loop
  // this will cleanup the connection
  ESP_ERROR_CHECK(i2c_driver_delete(I2C_MASTER_NUM));
  ESP_LOGI(TAG, "I2C de-initialized successfully");
}
