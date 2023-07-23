#include <stdio.h>
#include <esp_http_client.h>
#include "device_camera.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_camera.h"
#include "device_wifi.h"
#include "device_sender.h"

/**
 * @brief handling image capture
*/
void cameraSetup(){
  
  static camera_config_t config;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_sccb_sda = SIOD_GPIO_NUM;
  config.pin_sccb_scl = SIOC_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;

  config.xclk_freq_hz = 20000000; // The clock frequency of the image sensor
  config.fb_location = CAMERA_FB_IN_PSRAM; // Set the frame buffer storage location
  config.pixel_format = PIXFORMAT_JPEG; // The pixel format of the image: PIXFORMAT_ + YUV422|GRAYSCALE|RGB565|JPEG
  config.frame_size = FRAMESIZE_UXGA; // The resolution size of the image: FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
  config.jpeg_quality = 12; // The quality of the JPEG image; ranging from 0 to 63.
  config.fb_count = 1; // The number of frame buffers to use.
  config.grab_mode = CAMERA_GRAB_WHEN_EMPTY; //  The image capture mode.
  
  esp_err_t Status = esp_camera_init(&config);
  ESP_ERROR_CHECK(Status);

  // set the exposure control
  sensor_t *s = esp_camera_sensor_get();
  s->set_exposure_ctrl(s, 1); // enable AEC
  s->set_gain_ctrl(s, 1); // enable AGC
  s->set_whitebal(s, 1); // enable AWB
};

esp_err_t takeImage(){
  // camera needs a few throw aways for calibration to set
  // i've found 1 is ok for my setup then the next frame is usable
  esp_err_t err = ESP_OK;
  for(int i = 0; i < 1; i++) {
    camera_fb_t * temp_frb = esp_camera_fb_get();
    printf("temp frames #%i\n", i);
    if (!temp_frb) {
        printf("failed to get frame buffer");
        return ESP_FAIL;
    }
    esp_camera_fb_return(temp_frb);
  }

  // get frame buffer
  camera_fb_t * frb = esp_camera_fb_get();
  printf("Just got photo--------------\n");
  if (!frb) {
    printf("failed to get frame buffer");
    return ESP_FAIL;
  }
  printf("Picture is size in bytes: %zu\n", frb->len);

  /*
  * send it
  */
  err = sendFrameBuffer(frb);
  if (err != ESP_OK) {
    printf("HTTP POST request failed: %s\n", esp_err_to_name(err));
  }
      
  // Release image buffer
  esp_camera_fb_return(frb);
  printf("image process complete\n");
  return err;
}
