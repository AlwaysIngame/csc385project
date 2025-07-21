#include "mbed.h"


I2C i2c_light(I2C_SDA, I2C_SCL);

const int BH1750_ADDR = 0x23 << 1; 


void light_init() {
    char cmd[1] = {0x10}; 
    i2c_light.write(BH1750_ADDR, cmd, 1);
}

float read_light() {
    char data[2];
    i2c_light.read(BH1750_ADDR, data, 2);
    int raw = (data[0] << 8) | data[1];
    return raw / 1.2f; 
}
