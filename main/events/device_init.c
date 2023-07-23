#include <stdio.h>
#include "device_init.h"
#include "esp_log.h"
#include "driver/i2c.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"

// standard init from esp32s3 docs 
esp_err_t i2c_master_init(void) {
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

VL53L0X_Error vl53l0x_init(VL53L0X_Dev_t *pMyDevice) {
  VL53L0X_Error Status = VL53L0X_ERROR_NONE;
  uint32_t refSpadCount;
  uint8_t isApertureSpads;
  uint8_t VhvSettings;
  uint8_t PhaseCal;

  if(Status == VL53L0X_ERROR_NONE) {
    printf ("Call of VL53L0X_StaticInit\n");
    Status = VL53L0X_StaticInit(pMyDevice); 
    ESP_ERROR_CHECK(Status);
  }

  if(Status == VL53L0X_ERROR_NONE) {
    printf ("Call of VL53L0X_PerformRefSpadManagement\n");
    Status = VL53L0X_PerformRefSpadManagement(pMyDevice,
        &refSpadCount, &isApertureSpads); 
    printf ("refSpadCount = %ld, isApertureSpads = %d\n", refSpadCount, isApertureSpads);
    ESP_ERROR_CHECK(Status);
  }
  
  if(Status == VL53L0X_ERROR_NONE) {
    printf ("Call of VL53L0X_PerformRefCalibration\n");
    Status = VL53L0X_PerformRefCalibration(pMyDevice,
        &VhvSettings, &PhaseCal); 
    ESP_ERROR_CHECK(Status);
  }

  // Helps ensure a valid reading is taken
  if (Status == VL53L0X_ERROR_NONE) {
      Status = VL53L0X_SetLimitCheckEnable(pMyDevice,
          VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE, 1);
  }
  if (Status == VL53L0X_ERROR_NONE) {
      Status = VL53L0X_SetLimitCheckEnable(pMyDevice,
          VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE, 1);
  }
      
  if (Status == VL53L0X_ERROR_NONE) {
      Status = VL53L0X_SetLimitCheckValue(pMyDevice,
          VL53L0X_CHECKENABLE_SIGNAL_RATE_FINAL_RANGE,
          (FixPoint1616_t)(0.25*65536));
	}			
  if (Status == VL53L0X_ERROR_NONE) {
      Status = VL53L0X_SetLimitCheckValue(pMyDevice,
          VL53L0X_CHECKENABLE_SIGMA_FINAL_RANGE,
          (FixPoint1616_t)(18*65536));			
  }
  if (Status == VL53L0X_ERROR_NONE) {
      Status = VL53L0X_SetMeasurementTimingBudgetMicroSeconds(pMyDevice,
          50000);
  }

  printf("init done going to return\n");
  return Status;
}