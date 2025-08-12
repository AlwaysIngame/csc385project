#include "mbed.h"
#include "wifi.h"
#include "rest.h"

bool rest_post_data(const char *json_payload, SocketAddress addr, const char *path) {
    TCPSocket socket;
    NetworkInterface *wifi = get_wifi();
    nsapi_error_t err = socket.open(wifi);
    if (err != NSAPI_ERROR_OK) {
        printf("Socket open failed: %d\n", err);
        return false;
    }

    if (socket.connect(addr) != NSAPI_ERROR_OK) {
        printf("Socket connection to REST server failed\n");
        socket.close();
        return false;
    }

    // Construct HTTP POST request
    char request[512];
    int len = snprintf(request, sizeof(request),
                       "POST %s HTTP/1.1\r\n"
                       "Host: %s\r\n"
                       "Content-Type: application/json\r\n"
                       "Content-Length: %d\r\n"
                       "Connection: close\r\n"
                       "\r\n"
                       "%s",
                       path, REST_HOST, strlen(json_payload), json_payload);

    if (socket.send(request, len) < 0) {
        printf("Failed to send HTTP request\n");
        socket.close();
        return false;
    }

    // Read response (optional)
    char buffer[512];
    int bytes = socket.recv(buffer, sizeof(buffer) - 1);
    if (bytes > 0) {
        buffer[bytes] = '\0';
        printf("REST response: %s\n", buffer);
    }

    socket.close();
    return true;
}