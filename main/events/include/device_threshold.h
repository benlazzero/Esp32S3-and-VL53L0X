/*
 * Calibrate threshold for software based interupt
*/
#include "esp_log.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"

VL53L0X_Error calibrate_threshold(VL53L0X_Dev_t *Dev, uint32_t TotalReads, uint32_t *vThresh);
/**
 * @brief -- Uses single range to read to establish distance to static* object to base threshold
 * @param TotalReads - How many values to read for calibration (amount to be averaged)
 * @param vThresh - reference var to store base threshold value
 * May be needed in future - Ignore outlier values that may skew average
*/
