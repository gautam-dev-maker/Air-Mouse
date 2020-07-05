#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "driver/gpio.h"
#include "config.h"
#include "hal_ble.h"
#include "esp_gap_ble_api.h"
#include "driver/gpio.h"
#include "driver/touch_pad.h"
#include "MPU6050.h"
#include "MPU6050_6Axis_MotionApps20.h"

/** mouse speed */
#define MOUSE_SPEED 15
#define MAX_CMDLEN 100
//Capacitance Touch
#define TOUCH_PAD_NO_CHANGE (-1)
#define TOUCH_THRESH_NO_USE (0)
#define TOUCH_FILTER_MODE_EN (1)
#define TOUCHPAD_FILTER_TOUCH_PERIOD (10)

//MPU Pins
#define PIN_SDA CONFIG_SDAPin
#define PIN_CLK CONFIG_CLKPin


// MPU vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorFloat gravity;    // [x, y, z]            gravity vector
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector
uint16_t packetSize = 42;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
float yaw = 0.0, pitch = 0.0, roll = 0.0;
float vertZero = 0, horzZero = 0;
float vertValue, horzValue;

//check if bluetooth is connected
static config_data_t config;
QueueHandle_t hid_ble;
struct cmdBuf{
	int State;
	int expectedBytes;
	int bufferLength;
	uint8_t buf[MAX_CMDLEN];
};
void blink_task(void *pvParameter)
{
    // Initialize GPIO pins
    gpio_pad_select_gpio(INDICATOR_LED_PIN);
    gpio_set_direction(INDICATOR_LED_PIN, GPIO_MODE_OUTPUT);
    int blinkTime;
    
    while(1) {
		
		if (halBLEIsConnected()) 
			blinkTime=1000;
		else blinkTime=250;
		
		
        /* Blink off (output low) */
        gpio_set_level(INDICATOR_LED_PIN, 0);
        vTaskDelay(blinkTime / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(INDICATOR_LED_PIN, 1);
        vTaskDelay(blinkTime / portTICK_PERIOD_MS);
    }
}

//move mouse cursor
void mpu_poll(void *pvParameter)
{
	hid_cmd_t mouseCmd;
	MPU6050 mpu = MPU6050();

	mpu.initialize();
	mpu.dmpInitialize();

	// This need to be setup individually
	mpu.setXGyroOffset(220);
	mpu.setYGyroOffset(76);
	mpu.setZGyroOffset(-85);
	mpu.setZAccelOffset(1788);

	mpu.setDMPEnabled(true);

	while (1)
	{
		mpuIntStatus = mpu.getIntStatus();
		// get current FIFO count
		fifoCount = mpu.getFIFOCount();

		if ((mpuIntStatus & 0x10) || fifoCount == 1024)
		{
			// reset so we can continue cleanly
			mpu.resetFIFO();

			// otherwise, check for DMP data ready interrupt frequently)
		}
		else if (mpuIntStatus & 0x02)
		{
			// wait for correct available data length, should be a VERY short wait
			while (fifoCount < packetSize)
				fifoCount = mpu.getFIFOCount();

			// read a packet from FIFO

			mpu.getFIFOBytes(fifoBuffer, packetSize);
			mpu.dmpGetQuaternion(&q, fifoBuffer);
			mpu.dmpGetGravity(&gravity, &q);
			mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
			yaw = ypr[2] /M_PI * 180;
            pitch = ypr[1] /M_PI * 180;
            roll = ypr[0] /M_PI * 180;
			vertValue = yaw - vertZero;
			horzValue = roll - horzZero;
			
			vertZero = yaw;
			horzZero = roll;
			
			if (halBLEIsConnected())
			{
				if (vertValue != 0 || horzValue != 0)
				{
					mouseCmd.cmd[0] = 0x01;
					mouseCmd.cmd[1] = horzValue * MOUSE_SPEED;
					mouseCmd.cmd[2] = vertValue * MOUSE_SPEED;
					xQueueSend(hid_ble,(void *)&mouseCmd, (TickType_t) 0);
				}
				
			}
		}

		//Best result is to match with DMP refresh rate
		// Its last value in components/MPU6050/MPU6050_6Axis_MotionApps20.h file line 310
		// Now its 0x13, which means DMP is refreshed with 10Hz rate
		vTaskDelay(25 / portTICK_PERIOD_MS);
	}

	vTaskDelete(NULL);
}
//Capacitance Touch
uint16_t touch_value_mlb,touch_value_mrb;                  //for raw values
uint16_t touch_filter_value_mlb,touch_filter_value_mrb;     //for filterev values
static void tp_read_task(void *pvParameter)
{
	
	while (1)
	{
		#if TOUCH_FILTER_MODE_EN
					touch_pad_read_raw_data((touch_pad_t)0,&touch_value_mlb);            //read raw data from touch 0          
		                        touch_pad_read_raw_data((touch_pad_t)2,&touch_value_mrb);            //read raw datta from touch 2
					touch_pad_read_filtered((touch_pad_t)0,&touch_filter_value_mlb);     //filters data from touch 0
					touch_pad_read_filtered((touch_pad_t)2,&touch_filter_value_mrb);     //filters data from touch 2
		#endif
		vTaskDelay(25 / portTICK_PERIOD_MS);

	}
	
	
}

static void tp_read_touch_pad_init(void)
{
		touch_pad_config((touch_pad_t)0,TOUCH_THRESH_NO_USE);
		touch_pad_config((touch_pad_t)2,TOUCH_THRESH_NO_USE);
	
}
//buttons
void button_poll(void *pvParameter)
{
	hid_cmd_t mouseCmd;
	
	bool mlb = false, mrb = false;

	while (1)
	{
		if (touch_filter_value_mlb<300 && !mlb)                                   
		{
			ESP_LOGI("MLB","click"); //for left Button
			mouseCmd.cmd[0] = 0x16;
			xQueueSend(hid_ble,(void *)&mouseCmd, (TickType_t) 0);
			mlb = true;
		}
		else if (touch_filter_value_mlb>350 && mlb)
		{
			ESP_LOGI("MLB","release");//for Release button
			mouseCmd.cmd[0] = 0x19;
			xQueueSend(hid_ble,(void *)&mouseCmd, (TickType_t) 0);
			mlb = false;
		}

		if (touch_filter_value_mrb<300 && !mrb)
		{
			ESP_LOGI("MRB","klik"); //for Right Button
			mouseCmd.cmd[0] = 0x17;
			xQueueSend(hid_ble,(void *)&mouseCmd, (TickType_t) 0);
			mrb = true;
		}
		else if (touch_filter_value_mrb>350 && mrb)
		{
			// ESP_LOGI("MRB","release");//for Release button
			mouseCmd.cmd[0] = 0x1A;
			xQueueSend(hid_ble,(void *)&mouseCmd, (TickType_t) 0);
			mrb = false;
		}
		vTaskDelay(25 / portTICK_PERIOD_MS);
	}
	vTaskDelete(NULL);
}


void task_initI2C(void *ignore) {
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = (gpio_num_t)PIN_SDA;
	conf.scl_io_num = (gpio_num_t)PIN_CLK;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = 400000;
	ESP_ERROR_CHECK(i2c_param_config(I2C_NUM_0, &conf));
	ESP_ERROR_CHECK(i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0));
	vTaskDelete(NULL);
}

extern "C" void app_main()
{
    esp_err_t ret;
    
    esp_log_level_set("*", ESP_LOG_VERBOSE);

    // Initialize NVS.
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );
	
    // Read config
    nvs_handle my_handle;
	ESP_LOGI("MAIN","loading configuration from NVS");
    ret = nvs_open("config_c", NVS_READWRITE, &my_handle);
    if(ret != ESP_OK) ESP_LOGE("MAIN","error opening NVS");
    size_t available_size = MAX_BT_DEVICENAME_LENGTH;
    strcpy(config.bt_device_name, GATTS_TAG);
    nvs_get_str (my_handle, "btname", config.bt_device_name, &available_size);
    if(ret != ESP_OK) 
    {
        ESP_LOGE("MAIN","error reading NVS - bt name, setting to default");
        strcpy(config.bt_device_name, GATTS_TAG);
    } else ESP_LOGI("MAIN","bt device name is: %s",config.bt_device_name);

    ret = nvs_get_u8(my_handle, "locale", &config.locale);
    nvs_close(my_handle);
	
	halBLEInit(0,1,0,config.bt_device_name);
    ESP_LOGI("HIDD","MAIN finished...");
    hid_ble = xQueueCreate(32,sizeof(hid_cmd_t));
    
    esp_log_level_set("*", ESP_LOG_DEBUG); 

	touch_pad_init();
	touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5,TOUCH_HVOLT_ATTEN_1V);
	tp_read_touch_pad_init();
	#if TOUCH_FILTER_MODE_EN
			touch_pad_filter_start(TOUCHPAD_FILTER_TOUCH_PERIOD);
	#endif
  
    // now start the tasks for processing input and indicator LED  
    xTaskCreate(&task_initI2C, "mpu_init", 2048, NULL, configMAX_PRIORITIES, NULL);
    xTaskCreate(&button_poll,"button_loop",4096,NULL, configMAX_PRIORITIES,NULL);
    xTaskCreate(&blink_task, "blink", 4096, NULL, configMAX_PRIORITIES, NULL);
	xTaskCreate(&tp_read_task, "tp_read_task", 2048, NULL, configMAX_PRIORITIES, NULL);
    vTaskDelay(1000/portTICK_PERIOD_MS);
	
	xTaskCreate(&mpu_poll, "mpu_loop", 8192, NULL, configMAX_PRIORITIES, NULL);

}

