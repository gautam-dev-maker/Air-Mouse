idf_component_register(SRCS "main.cpp" "./main.cpp" "components/ble_hid/hid_device_le_prf.c" "components/ble_hid/hal_ble.c" "components/ble_hid/hid_dev.c" "components/MPU6050/MPU6050.cpp" "components/I2Cdev/I2Cdev.cpp"
                       INCLUDE_DIRS "." "components/ble_hid" "components/I2Cdev" "components/MPU6050"                
                       REQUIRES 
                       PRIV_REQUIRES )

                                          