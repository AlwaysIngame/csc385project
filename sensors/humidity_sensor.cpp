#include "mbed.h"

static I2C i2c(I2C_SDA, I2C_SCL);

static const int SHT31_ADDR = 0x44 << 1;

void humid_init() {
    
}

float read_humidity() {
    char cmd[2] = {0x2C, 0x06}; 

    i2c.write(SHT31_ADDR, cmd, 2);

    ThisThread::sleep_for(20ms); 
    char data[6]; //format is 6 bytes, first 2 for temp, 3rd for temp validation, 4 and 5 for humidity, 6th for humidity validation
    i2c.read(SHT31_ADDR, data, 6);
    uint16_t humidity = (data[3] << 8) | data[4];
    return humidity;
}