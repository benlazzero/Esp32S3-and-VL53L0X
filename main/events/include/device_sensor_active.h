/*
 * State of sensor checking for difference in threshold range
*/
#include "esp_log.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"

VL53L0X_Error sensor_activate(VL53L0X_Dev_t *Dev, uint32_t BaseThreshold);
/**
 * @brief - A state in which the sensor is ranging and comparing reads against threshold range
 * @param BaseThreshold - The average of the current range or range found when threshold calibration was ran 
*/

