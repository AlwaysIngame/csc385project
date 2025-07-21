#include "mbed.h"
#include "rtos.h"
#include "WiFiInterface.h"
#include "TCPSocket.h"
#include "rest.h"

#define WIFI_SSID "YOUR_WIFI_SSID"
#define WIFI_PASS "YOUR_WIFI_PASSWORD"

DigitalOut led(LED1);
WiFiInterface *wifi;

// Basic HTTP Response Templates
const char* http_ok = "HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n";
const char* http_not_found = "HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n\r\nNot Found";

void handle_client(TCPSocket *client) {
    char buffer[1024] = {0};
    int len = client->recv(buffer, sizeof(buffer) - 1);

    if (len <= 0) {
        client->close();
        delete client;
        return;
    }

    buffer[len] = '\0';
    printf("Received request:\n%s\n", buffer);

    // Parse method and path
    char method[8], path[64];
    sscanf(buffer, "%s %s", method, path);

    if (strcmp(method, "GET") == 0 && strcmp(path, "/status") == 0) {
        client->send(http_ok, strlen(http_ok));
        client->send("{\"status\":\"ok\"}\n", 18);
    } else if (strcmp(method, "POST") == 0 && strcmp(path, "/led") == 0) {
        // Find body
        char *body = strstr(buffer, "\r\n\r\n");
        if (body) {
            body += 4;
            if (strstr(body, "on")) led = 1;
            else if (strstr(body, "off")) led = 0;
        }
        client->send(http_ok, strlen(http_ok));
        client->send("{\"led\":\"updated\"}\n", 21);
    } else {
        client->send(http_not_found, strlen(http_not_found));
    }

    client->close();
    delete client;
}

void start_server() {
    printf("Starting REST API on DISCO_L475VG_IOT01A...\n");

    wifi = WiFiInterface::get_default_instance();
    if (!wifi) {
        printf("ERROR: No WiFiInterface found.\n");
        return ;
    }

    printf("Connecting to WiFi...\n");
    int ret = wifi->connect(WIFI_SSID, WIFI_PASS, NSAPI_SECURITY_WPA_WPA2);
    if (ret != 0) {
        printf("WiFi connection failed: %d\n", ret);
        return ;
    }

    TCPSocket server;
    server.open(wifi);
    server.bind(80);
    server.listen(5);

    while (true) {
        TCPSocket *client = server.accept();
        handle_client(client);
    }
}