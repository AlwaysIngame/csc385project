#include "mbed.h"

I2C i2c_humidity(I2C_SDA, I2C_SCL);

const int SHT31_ADDR = 0x44 << 1;

void humid_init() {
    
}

float read_humidity() {
    char cmd[2] = {0x2C, 0x06}; 

    i2c_humidity.write(SHT31_ADDR, cmd, 2);

    ThisThread::sleep_for(20ms); 
    char data[6];
    i2c_humidity.read(SHT31_ADDR, data, 6);

    uint16_t raw_humidity = (data[3] << 8) | data[4];
    float humidity = 100.0f * raw_humidity / 65535.0f;
    return humidity;
}