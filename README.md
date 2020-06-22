# Air-Mouse
===========
Esp32 Air-Mouse - Moves mouse cursor,Using MPU6050(6-axis Accelerometer,Gyroscope).
* Moves mouse cursor from the values generated using DMP(Digital Motion Processing) embedded in MPU6050.
* Uses Yaw,Pitch,Roll values generated using DMP to get the data to move the cursor.
* Completely bluetooth(BLE) supported.
* Right and Left Click is made using capacitive touch.


### Prerequisites
Install ESP-IDF : https://github.com/espressif/esp-idf

### Installation
Clone the project
```
git clone https://github.com/n1rml/esp32_airmouse.git esp32_airmouse

cd esp32_airmouse
```
Build
```
idf.py build
```
Flash
```
idf.py -p (PORT) flash monitor

```
