/*
 * Calibrate threshold for software based interupt
*/
#include "esp_log.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include "vl53l0x_def.h"
#include "vl53l0x_api_strings.h"

/**
 * @brief -- Uses single range loop to read to establish distance to static object to set a base threshold
 * @param TotalReads - How many values to read for calibration (amount to be averaged)
 * @param vThresh - reference var to store base threshold value
*/
VL53L0X_Error calibrate_threshold(VL53L0X_Dev_t *Dev, uint32_t TotalReads, uint32_t *vThresh) {
  VL53L0X_Error Status = VL53L0X_ERROR_NONE;
  VL53L0X_RangingMeasurementData_t RangingMeasurementData;
  uint32_t SumRanges = 0;
  int i;
  
  // read from sensor
  for(i = 0; i < TotalReads; i++){
    printf("reading %u values for threshold calibration\n", i);
    Status = VL53L0X_PerformSingleRangingMeasurement(Dev, &RangingMeasurementData);

    // If the status is not successful then retry 
    while (Status != VL53L0X_ERROR_NONE || RangingMeasurementData.RangeStatus != 0) {
        printf("Measurement failed, retrying...\n");
        Status = VL53L0X_PerformSingleRangingMeasurement(Dev, &RangingMeasurementData);
    }

    SumRanges = SumRanges + RangingMeasurementData.RangeMilliMeter;
  }

  *vThresh = SumRanges / TotalReads; 
  return Status;
}