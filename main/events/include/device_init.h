/*
I2C setup and VL53L0X initialization 
*/
#include "esp_log.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"

#ifndef I2C_CONST
#define I2C_CONST

#define I2C_MASTER_SCL_IO           6
#define I2C_MASTER_SDA_IO           5
#define I2C_MASTER_NUM              0
#define I2C_MASTER_FREQ_HZ          400000
#define I2C_MASTER_TX_BUF_DISABLE   0
#define I2C_MASTER_RX_BUF_DISABLE   0
#define I2C_MASTER_TIMEOUT_MS       1000

#define VL53L0X_SENSOR_ADDR         0x29

#endif // I2C_CONST

esp_err_t i2c_master_init(void);
/**
 * @brief Config for I2C connection
 *
 * @return status: 0=ok, -1=fail
 */

VL53L0X_Error vl53l0x_init(VL53L0X_Dev_t *pMyDevice);
/**
 * @brief Calibrate sensor SPAD, Ref, Static Init
 *
 * @return status: 0=no error
 */
