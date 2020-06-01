#include "i2c.hpp"

I2C::I2C(gpio_num_t scl, gpio_num_t sda, i2c_port_t port) {
    this -> port = port;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = scl;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 400000;
    i2c_param_config(port, &conf);
    i2c_driver_install(port, conf.mode, 0, 0, 0);
}

I2C::~I2C() {
    i2c_driver_delete(port);
}

bool I2C::slave_write(uint8_t slave_addr,uint8_t reg_addr, uint8_t data) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, slave_addr << 1, 1);
    i2c_master_write_byte(cmd, reg_addr, 1);
    i2c_master_write_byte(cmd, data, 1);
    i2c_master_stop(cmd);
    int ret = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_FAIL) {
        return false;
    }
    return true;
}

bool I2C::slave_read(uint8_t slave_addr, uint8_t data, uint8_t *buf, uint32_t len) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, slave_addr << 1, 1);
    i2c_master_write_byte(cmd, data, 1);
    i2c_master_stop(cmd);
    int ret = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_FAIL) {
        return false;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, slave_addr << 1 | 1, 1);
    while(len) {
        i2c_master_read_byte(cmd, buf, (len == 1) ? I2C_MASTER_NACK : I2C_MASTER_ACK);
        buf++;
        len--;
    }
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret == ESP_FAIL) {
        return false;
    }

    return true;
}

uint8_t I2C::slave_read_byte(uint8_t slave_addr, uint8_t reg) {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, slave_addr << 1, 1);
    i2c_master_write_byte(cmd, reg, 1);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    uint8_t buf;
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, slave_addr << 1 | 1, 1);
    i2c_master_read_byte(cmd, &buf, I2C_MASTER_NACK);
    i2c_master_stop(cmd);
    i2c_master_cmd_begin(port, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return buf;
}
