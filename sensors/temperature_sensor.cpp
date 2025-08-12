#include "mbed.h"

static I2C i2c(I2C_SDA, I2C_SCL);

static const int SHT31_ADDR = 0x44 << 1;

void temp_init() {
    
}

float read_temperature() {
    char cmd[2] = {0x2C, 0x06}; 

    i2c.write(SHT31_ADDR, cmd, 2);

    ThisThread::sleep_for(20ms); 
    char data[6];
    i2c.read(SHT31_ADDR, data, 6);

    uint16_t temp_raw = (data[0] << 8) | data[1];
    float temperature = -45.0f + 175.0f * ((float)temp_raw / 65535.0f);

    return temperature;
}