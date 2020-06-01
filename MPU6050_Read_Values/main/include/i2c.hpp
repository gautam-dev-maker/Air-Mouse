#pragma once

#include "driver/i2c.h"
#include "soc/i2c_reg.h"
#include "soc/i2c_struct.h"

class I2C {
private:
    i2c_config_t conf;
    i2c_port_t port;
public:
    I2C(gpio_num_t scl, gpio_num_t sda, i2c_port_t port);
    ~I2C();
    bool slave_write(uint8_t slave_addr,uint8_t reg_addr, uint8_t data);
    bool slave_read(uint8_t slave_addr, uint8_t data, uint8_t *buf, uint32_t len);
    uint8_t slave_read_byte(uint8_t slave_addr, uint8_t reg);
};
