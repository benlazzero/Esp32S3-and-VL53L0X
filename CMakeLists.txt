# get IDF version for comparison
set(COMPONENT_REQUIRES "driver" "esp_timer")
set(COMPONENT_PRIV_REQUIRES "freertos" "nvs_flash")

idf_component_register(SRCS "conversions/yuv.c" "conversions/to_jpg.cpp" "conversions/to_bmp.c" "conversions/jpge.cpp"
                       "conversions/esp_jpg_decode.c" "driver/esp_camera.c" "driver/cam_hal.c" "driver/sccb.c" "driver/sensor.c"
                       "sensors/ov2640.c" "sensors/ov5640.c" "target/esp32s3/ll_cam.c" "target/tjpgd.c"
                       "sensors/bf20a6.c" "sensors/bf3005.c" "sensors/gc032a.c" "sensors/gc0308.c" "sensors/gc2145.c"
                       "sensors/nt99141.c" "sensors/ov3660.c" "sensors/ov7670.c" "sensors/ov7725.c" "sensors/sc030iot.c"
                       "sensors/sc031gs.c" "sensors/sc101iot.c"
                       INCLUDE_DIRS "." "conversions/private_include" "driver/include" "conversions/include"
                                    "driver/private_include" "sensors/private_include" "target/private_include"
                                    "target/esp32s2/private_include" "target/jpeg_include/"
                        REQUIRES ${COMPONENT_REQUIRES} ${COMPONENT_PRIV_REQUIRES})