 
#ifndef _CONFIG_H_
#define _CONFIG_H_

#define MODULE_ID "ESP32_Air-Mouse"
#define GATTS_TAG CONFIG_BluetoothName
#define MAX_BT_DEVICENAME_LENGTH 40

// serial port of monitor and for debugging


// indicator LED
#define INDICATOR_LED_PIN    (GPIO_NUM_2)

typedef struct config_data {
    char bt_device_name[MAX_BT_DEVICENAME_LENGTH];
    uint8_t locale;
} config_data_t;


#endif
