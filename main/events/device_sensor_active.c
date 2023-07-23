/*
 * State of sensor checking for difference in threshold range
*/
#include "esp_log.h"
#include "vl53l0x_api.h"
#include "vl53l0x_platform.h"
#include "vl53l0x_api_strings.h"
#include "device_threshold.h"
#include "math.h"

void print_pal_error(VL53L0X_Error Status){
  char buf[VL53L0X_MAX_STRING_LENGTH];
  VL53L0X_GetPalErrorString(Status, buf);
  printf("API Status: %i : %s\n", Status, buf);
}

VL53L0X_Error WaitStopCompleted(VL53L0X_DEV Dev) {
  VL53L0X_Error Status = VL53L0X_ERROR_NONE;
  uint32_t StopCompleted=0;
  uint32_t LoopNb;

  if (Status == VL53L0X_ERROR_NONE) {
    LoopNb = 0;
    do {
        Status = VL53L0X_GetStopCompletedStatus(Dev, &StopCompleted);
        if ((StopCompleted == 0x00) || Status != VL53L0X_ERROR_NONE) {
            break;
        }
        LoopNb = LoopNb + 1;
        VL53L0X_PollingDelay(Dev, -1);
    } while (LoopNb < VL53L0X_DEFAULT_MAX_LOOP);

    if (LoopNb >= VL53L0X_DEFAULT_MAX_LOOP) {
        Status = VL53L0X_ERROR_TIME_OUT;
    }

  }
  return Status;
}

VL53L0X_Error WaitMeasurementDataReady(VL53L0X_DEV Dev) {
  VL53L0X_Error Status = VL53L0X_ERROR_NONE;
  uint8_t NewDatReady=0;
  uint32_t LoopNb;

  // Wait until it finished
  // use timeout to avoid deadlock
  if (Status == VL53L0X_ERROR_NONE) {
    LoopNb = 0;
    do {
        Status = VL53L0X_GetMeasurementDataReady(Dev, &NewDatReady);
        if ((NewDatReady == 0x01) || Status != VL53L0X_ERROR_NONE) {
            break;
        }
        LoopNb = LoopNb + 1;
        VL53L0X_PollingDelay(Dev, -1);
    } while (LoopNb < VL53L0X_DEFAULT_MAX_LOOP);

    if (LoopNb >= VL53L0X_DEFAULT_MAX_LOOP) {
        Status = VL53L0X_ERROR_TIME_OUT;
    }
  }

  return Status;
}

VL53L0X_Error sensor_activate(VL53L0X_Dev_t *Dev, uint32_t BaseThreshold) {
  VL53L0X_Error Status = VL53L0X_ERROR_NONE;
  VL53L0X_RangingMeasurementData_t RangingMeasurementData;
  VL53L0X_RangingMeasurementData_t *pRangingMeasurementData = &RangingMeasurementData;
  uint32_t measurement;
  
  if(Status == VL53L0X_ERROR_NONE) {
    uint32_t no_of_measurements = 10;
    size_t numOfMeas = 0; 
    int deg_of_free = 15;
    // Dynamic bounds for threshold based on vl53l0x ~3% acc
    // if <15.5 then the init value of 15 is used
    if (BaseThreshold * .03 > 15.5) {
      double result = BaseThreshold * .03;
      result = ceil(result);  
      printf("dynamic bounds: %f",result);
      deg_of_free = result;
    }


    // main measurment loop, better results when single range vs polling
    while(1) {
      uint32_t sumOfMeas = 0;
      for(measurement=0; measurement<no_of_measurements; measurement++) {
        if(Status == VL53L0X_ERROR_NONE) {
          Status = VL53L0X_PerformSingleRangingMeasurement(Dev, pRangingMeasurementData);

          // using the check, determine if reading is valid
          while (Status != VL53L0X_ERROR_NONE || pRangingMeasurementData->RangeStatus != 0) {
              printf("Measurement failed, retrying...\n");
              Status = VL53L0X_PerformSingleRangingMeasurement(Dev, pRangingMeasurementData);
          }

          sumOfMeas = sumOfMeas + pRangingMeasurementData->RangeMilliMeter;
          VL53L0X_PollingDelay(Dev, 100);
          printf("Single Range = %u\n", pRangingMeasurementData->RangeMilliMeter);
        } else {
          break;
        }
      }

      
      if(Status == VL53L0X_ERROR_NONE) {
        uint32_t avgOfMeas;
        numOfMeas += 1;
        printf("avg by: %lu / %lu\n", sumOfMeas, no_of_measurements);
        avgOfMeas = sumOfMeas / no_of_measurements;
        printf("average for last %lu: %lu\n", no_of_measurements, avgOfMeas);
        // take average and check bounds
        if (avgOfMeas > BaseThreshold + deg_of_free || avgOfMeas < BaseThreshold - deg_of_free) {
          printf("GECKO DETECTED!!!!\n");
          break;
        }

        // periodicly recalibrating threshold combats the VL53L0X's drift
        // little hacky but works great
        if (numOfMeas > 500) {
          Status = calibrate_threshold(Dev, 30, &BaseThreshold);
          printf("500 reached, recalibrate complete: %lu", BaseThreshold);
          if (BaseThreshold * .03 > 15.5) {
            double result = BaseThreshold * .03;
            result = ceil(result);  
            printf("dynamic bounds: %f",result);
            deg_of_free = result;
          }
          numOfMeas = 0;
        }
        printf("Total reads: %u\n", numOfMeas);
      }
    }
  }
  
  // Cleanup 
  if(Status == VL53L0X_ERROR_NONE) {
    printf ("Call of VL53L0X_StopMeasurement\n");
    Status = VL53L0X_StopMeasurement(Dev);
  }

  if(Status == VL53L0X_ERROR_NONE) {
    printf ("Wait Stop to be competed\n");
    Status = WaitStopCompleted(Dev);
  }

  if(Status == VL53L0X_ERROR_NONE) {
    Status = VL53L0X_ClearInterruptMask(Dev,
    VL53L0X_REG_SYSTEM_INTERRUPT_GPIO_NEW_SAMPLE_READY);
  }

  return Status;
};