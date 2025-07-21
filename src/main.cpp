#include "mbed.h"

#include "rest.h"
#include "light_sensor.h"
#include "hum_sensor.h"


void setup() {
    light_init();
    humid_init();
}

int main() {
    setup();

    start_server();

    bool swap_light_humidity = true;

    while (true) {
        if (swap_light_humidity) {
            float humidity = read_humidity();
            printf("Humidity sensor (SHT31) humidity: %.2f%%\n", humidity);
        } else {
            float lux = read_light();
            printf("Light sensor (BH1750) lux: %.2f\n", lux);
        }




        ThisThread::sleep_for(500ms);
    }
}