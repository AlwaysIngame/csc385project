#include "mbed.h"
#include "WiFiInterface.h"
#include "wifi.h"

static bool is_connected = false;
static WiFiInterface *wifi;

#define WIFI_SSID "thinkboy"
#define WIFI_PW "hridayalgh"

WiFiInterface *get_wifi() {
    if (is_connected)
        return wifi;

    wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        printf("ERROR: No WiFiInterface found.\n");
        return nullptr;
    }

    int rc;
    printf("Connecting to WiFi: %s...\n", WIFI_SSID);
    if ((rc = wifi->connect(WIFI_SSID, nullptr)) != 0) {
        printf("WiFi connection failed! %d\n", rc);
        return nullptr;
    }
    printf("WiFi connected\n");
    is_connected = true;

    return wifi;
}