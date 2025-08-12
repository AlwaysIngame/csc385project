#include "mbed.h"
#include "MQTTNetwork.h"
#include "MQTTmbed.h"
#include "MQTTClient.h"
#include "wifi.h"

static MQTTNetwork *mqttNetwork;
static MQTT::Client<MQTTNetwork, Countdown> *mqttClient;

void messageArrived(MQTT::MessageData &md) {
    MQTT::Message &message = md.message;
    printf("Message arrived on topic %.*s: %.*s\n",
           md.topicName.lenstring.len,
           md.topicName.lenstring.data,
           message.payloadlen,
           (char *)message.payload);
}

bool mqtt_init_client(const char *ssid,
                      const char *password,
                      const char *broker,
                      int port,
                      const char *clientId) {
    
    NetworkInterface *wifi = get_wifi();

    mqttNetwork = new MQTTNetwork(wifi);

    mqttClient = new MQTT::Client<MQTTNetwork, Countdown>(*mqttNetwork);
    printf("Connecting to %s:%d\r\n", broker, port);
    int rc = mqttNetwork->connect(broker, port);
    if (rc != 0)
        printf("rc from TCP connect is %d\r\n", rc);

    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = "csc385tester";
    if ((rc = mqttClient->connect(data)) != 0)
        printf("rc from MQTT connect is %d\r\n", rc);

    printf("MQTT connected!\n");
    return true;
}

bool mqtt_publish(const char *topic, const char *message) {
    MQTT::Message msg;
    msg.qos = MQTT::QOS0;
    msg.retained = false;
    msg.dup = false;
    msg.payload = (void *)message;
    msg.payloadlen = strlen(message);
    int rc;
    if ((rc=mqttClient->publish(topic, msg)) != 0) {
        printf("ERROR: MQTT publish failed %d\n", rc);
        return false;
    }
    return true;
}

bool mqtt_subscribe(const char *topic) {
    if (mqttClient->subscribe(topic, MQTT::QOS0, messageArrived) != 0) {
        printf("ERROR: MQTT subscribe failed\n");
        return false;
    }
    printf("Subscribed to topic: %s\n", topic);
    return true;
}

void mqtt_yield(int timeoutMs) {
    mqttClient->yield(timeoutMs);
}

void mqtt_disconnect() {
    mqttClient->disconnect();
    mqttNetwork->disconnect();
    delete mqttClient;
    delete mqttNetwork;
}
