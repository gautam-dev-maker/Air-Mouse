# Air-Mouse

Air-Mouse - Moves mouse cursor,Using ESP-32 & MPU6050(6-axis Accelerometer & Gyroscope) in 3D space with Bluetooth and Buttons support.

## Table of Contents

* [About the Project](#about-the-project)
  * [Tech Stack](#tech-stack)
  * [File Structure](#file-structure)
* [Getting Started](#getting-started)
  * [Prerequisites](#prerequisites)
  * [Installation](#installation)
  * [Configuration](#configuration)
* [Usage](#usage)
* [Results and Demo](#results-and-demo)
* [Troubleshooting](#troubleshooting)
* [Contributors](#contributors)
* [Acknowledgements and Resources](#acknowledgements-and-resources)
* [License](#license)

<!-- ABOUT THE PROJECT -->
## About The Project
* The Aim of the Project is to make a Mouse using the data fusion DMP(Digital Motion Processing) of MPU_6050 and ESP32 with Bluetooth support to actually make it    easier for the user to move pointer in any position they want.
* The Support for Right and Left click is also established using Capacitive touch pins of ESP32.
   
   [Air-Mouse](https://github.com/gautam-dev-maker/Air-Mouse/blob/master/docs/results/Air-Mouse.png)

### Tech Stack
The Technologies used for this project are
* [FreeRTOS](https://www.freertos.org/openrtos.html)
* [CMake](https://cmake.org)

### File Structure
    .
    ├── Components              # Contains files of specific library of functions or Hardware used
    │    ├──I2Cdev              # Library for I2C communication
    │    ├──MPU6050             # Library for MPU6050 sensor
    │    ├──CMakeLists.txt      # contains commands to include the given component in a esp-idf 
    ├── docs                    # Documentation files 
    │   ├── report.pdf          # Project report
    │   └── results             # Folder containing screenshots, gifs, videos of results
    ├── main                    # Source files (alternatively `lib` or `app`)
    │   ├──main.c               # Main Source code to be executed
    │   ├──kconfig.projbuild    # defines the entries of the menu for configuration
    │   ├──CMakeLists.txt       # contains commands to include the bluetooth library and main.c in esp-idf
    ├── CmakeLists.txt          # contains commands to include Components and main folder while executing
    ├── LICENSE
    └── README.md 
 



## Getting Started

### Prerequisites
Install ESP-IDF : https://github.com/espressif/esp-idf

### Installation
Clone the project
```
https://github.com/gautam-dev-maker/Air-Mouse.git

cd Air-Mouse
```
## Usage

Build
```
idf.py build
```
Flash
```
idf.py -p (PORT) flash monitor

```
### Configuration

```
idf.py menuconfig
```
* `Example Connection Configuration`
  * `Bluetooth Name` - Set Bluetooth Name
  
* `MPU6050 Configuration
  * `SDA Pin No.` - Set SDA Pin No.
  * `CLK Pin No.` - Set CLK Pin No.
* The default Pin configuration used to connect MPU_6050 with ESP32 in this project is shown [here](https://github.com/gautam-dev-maker/Air-Mouse/blob/master/docs/results/Esp-32%20and%20MPU6050%20pin%20connection.png)  
  
## Results and Demo
The use of Right and Left Capacitive touch pins has been demonstrated in the following videos

 [Right/left buttons](https://github.com/gautam-dev-maker/Air-Mouse/blob/master/docs/results/Right-Left%20click.mp4)
 
 ## Troubleshooting
 While Configuring for the first time if Bluetooth is not working then ,go to terminal
 
```
idf.py menuconfig
```
Then go to components/bluetooth and enable bluetooth
Press ctrl+s to save the configuration
then
```
idf.py build
```
## Contributors
* [Aman Chhaparia](https://github.com/amanchhaparia)
* [Gautam Agrawal](https://github.com/gautam-dev-maker)

## Acknowledgements and Resources
* [SRA VJTI](http://sra.vjti.info/) Eklavya 2020 
* Special thanks to [Vedant Paranjape](https://github.com/VedantParanjape)
* Jeff Rowberg for the MPU6050 library for esp-idf :
  https://github.com/jrowberg/i2cdevlib/tree/master/ESP32_ESP-IDF   
* https://github.com/nkolban/esp32-snippets
* https://github.com/VedantParanjape/idf-notes-sra

  
## License
The [License](https://github.com/gautam-dev-maker/Air-Mouse/blob/master/LICENSE) Used for this Project.
  
  
  
