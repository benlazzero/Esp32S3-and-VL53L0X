set(reqs "driver" "esp_timer" "esp_http_client" "esp_system" "esp_wifi" "esp_event" "nvs_flash")

idf_component_register(SRCS "main.c" "events/device_init.c" 
                            "events/device_camera.c" "events/device_threshold.c" 
                            "events/device_sensor_active.c" "events/device_wifi.c" 
                            "events/device_sender.c"
                    INCLUDE_DIRS "." "events" "events/include"
                    REQUIRES ${reqs})
# vl53l0x api
include_directories(${CMAKE_SOURCE_DIR}/components/vl53l0x/include)
include_directories(${CMAKE_SOURCE_DIR}/components/vl53l0x/src)

# esp camera api
include_directories(${CMAKE_SOURCE_DIR}/components/esp32cam/driver)
include_directories(${CMAKE_SOURCE_DIR}/components/esp32cam/driver/include)
include_directories(${CMAKE_SOURCE_DIR}/components/esp32cam/driver/private_include)

include_directories(${CMAKE_SOURCE_DIR}/components/esp32cam/conversions)
include_directories(${CMAKE_SOURCE_DIR}/components/esp32cam/conversions/include)
include_directories(${CMAKE_SOURCE_DIR}/components/esp32cam/conversions/private_include)

include_directories(${CMAKE_SOURCE_DIR}/components/esp32cam/sensors)
include_directories(${CMAKE_SOURCE_DIR}/components/esp32cam/sensors/private_include)

include_directories(${CMAKE_SOURCE_DIR}/components/esp32cam/target/jpeg_include)
include_directories(${CMAKE_SOURCE_DIR}/components/esp32cam/target/private_include)