/**
 * @brief Send the image data to local webserver
*/

#include <stdio.h>
#include "device_sender.h"
#include <esp_http_client.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_camera.h"

// client config for client init
// Use your local server/ post endpoint
esp_http_client_config_t client_config = {
  .url = "your local endpoint here...",  
  .method = HTTP_METHOD_POST,
  .timeout_ms = 10000,  // timeout in milliseconds
};

/**
 * @brief handles the frame buffer being send in a post request currenty
 * @param frb is the frame buffer from the camera sensor
*/
esp_err_t sendFrameBuffer(camera_fb_t * frb) {
  esp_err_t err = ESP_OK;

  // init client
  esp_http_client_handle_t client = esp_http_client_init(&client_config);
  esp_http_client_set_header(client, "Content-Type", "image/jpeg"); 
  
  // open connection
  err = esp_http_client_open(client, frb->len);
  if(err != ESP_OK) {
      printf("Error opening connection: %s\n", esp_err_to_name(err));
      return ESP_ERR_HTTP_CONNECT;
  }

  // Write the JPEG data to the client
  int write_len = esp_http_client_write(client, (const char *)frb->buf, frb->len);
  if(write_len != frb->len) {
      printf("Failed to write all data.\n");
      return ESP_ERR_HTTP_WRITE_DATA;
  }
  
  // performs the transfer based on setting above
  err = esp_http_client_perform(client);
  if (err == ESP_OK) {
  printf("HTTP POST Status = %d, content_length = %lld\n",
          esp_http_client_get_status_code(client),
          esp_http_client_get_content_length(client));
  } else {
    printf("HTTP POST request failed: %s\n", esp_err_to_name(err));
  }

  esp_http_client_cleanup(client);
  return err;
};
