#include "mbed.h"

#include "light_sensor.h"
#include "hum_sensor.h"
#include "temp_sensor.h"
#include "mqtt_manager_core.h"
#include "rest.h"
#include "wifi.h"
#include "ble.h"

#define CURRENT_MACHINE "sensor_hub"

static SocketAddress addr;
Thread blethread;

void setup() {
    light_init();
    
    //empty for now
    humid_init();
    temp_init();

    NetworkInterface *wifi = get_wifi();
    wifi->gethostbyname(REST_HOST, &addr);
    addr.set_port(80);
    mqtt_init_client("Happiness", "41684370ga", "test.mosquitto.org", 1883, CURRENT_MACHINE);
    //while(!get_ble()->hasInitialized());
    blethread.start(setup_ble);
}

int main() {
    setup();

    bool sensor_hub = true;

    while (true) {
        if (sensor_hub) {
            float humidity = read_humidity();
            char buf[16];
            printf("Humidity: %f\n", humidity);
            snprintf(buf, sizeof(buf), "%.2f", humidity);
            mqtt_publish("sensorhub/humidity", buf);

            float temperature = read_temperature(); 
            printf("Temperature: %f\n", temperature);
            snprintf(buf, sizeof(buf), "%.2f", temperature);
            mqtt_publish("sensorhub/temperature", buf);

            float lux = read_light();
            printf("Light: %f\n", lux);
            snprintf(buf, sizeof(buf), "%.2f", lux);
            mqtt_publish("sensorhub/light", buf);

            char json[128];
            snprintf(json, sizeof(json),
                 "{\"humidity\":%.2f,\"temperature\":%.2f,\"light\":%.2f}",
                 humidity, temperature, lux);

            rest_post_data(json, addr, "/sensor/data");
            ble_new_data(humidity, temperature, lux);
        }
        mqtt_yield(100);

        ThisThread::sleep_for(5000ms);
    }
}