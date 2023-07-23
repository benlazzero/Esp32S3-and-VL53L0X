/**
 * @brief module for sending the image date via post request 
*/

#include <stdio.h>
#include <esp_http_client.h>
#include "esp_log.h"
#include "esp_err.h"
#include "esp_camera.h"


/**
 * @brief handles the frame buffer being send in a post request currenty
 * @param frb is the frame buffer from the camera sensor
*/
esp_err_t sendFrameBuffer(camera_fb_t * frb);

