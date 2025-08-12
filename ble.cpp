#include "mbed.h"
#include "events/EventQueue.h"
#include "ble/BLE.h"
#include "ble.h"
#include <cstdio>

// The BLE object is the main interface for all Bluetooth operations.
BLE &bt = BLE::Instance();
events::EventQueue event_queue;

static float humidity_val;
static float temperature_val;
static float light_val;

static const UUID SENSOR_SERVICE_UUID("12345678-1234-5678-1234-56789abcdef0");
static const UUID HUMIDITY_CHAR_UUID("12345678-1234-5678-1234-56789abcdef1");
static const UUID TEMP_CHAR_UUID("12345678-1234-5678-1234-56789abcdef2");
static const UUID LIGHT_CHAR_UUID("12345678-1234-5678-1234-56789abcdef3");

ReadOnlyGattCharacteristic<float> humidityChar(HUMIDITY_CHAR_UUID, &humidity_val);
ReadOnlyGattCharacteristic<float> temperatureChar(TEMP_CHAR_UUID, &temperature_val);
ReadOnlyGattCharacteristic<float> lightChar(LIGHT_CHAR_UUID, &light_val);

GattCharacteristic *characteristics[] = { &humidityChar, &temperatureChar, &lightChar };
GattService sensorService(SENSOR_SERVICE_UUID, characteristics, sizeof(characteristics) / sizeof(characteristics[0]));

void ble_new_data(float humidity, float temperature, float light) {
    humidity_val = humidity;
    temperature_val = temperature;
    light_val = light;
}

void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context);

void on_ble_init_complete(BLE::InitializationCompleteCallbackContext *params) {

    if (params->error != BLE_ERROR_NONE) {
        printf("BLE initialization failed. Error: %u\n", params->error);
        return;
    }

    if (bt.hasInitialized()) printf("yay!\n");

    printf("BLE initialized successfully.\n");

    bt.gattServer().addService(sensorService);
    uint8_t adv_buffer[ble::LEGACY_ADVERTISING_MAX_SIZE];
    ble::AdvertisingDataBuilder adv_data_builder(adv_buffer, sizeof(adv_buffer));

    const char *device_name = "SensorHub";
    adv_data_builder.setName(device_name);

    ble_error_t error = bt.gap().setAdvertisingPayload(
        ble::LEGACY_ADVERTISING_HANDLE,
        adv_data_builder.getAdvertisingData()
    );

    if (error) {
        printf("Error setting advertising payload: %u\n", error);
        return;
    }

    ble::AdvertisingParameters adv_params(
        ble::advertising_type_t::CONNECTABLE_UNDIRECTED,
        ble::adv_interval_t(ble::millisecond_t(1000))
    );

    error = bt.gap().setAdvertisingParameters(ble::LEGACY_ADVERTISING_HANDLE, adv_params);
    if (error) {
        printf("Error setting advertising parameters: %u\n", error);
        return;
    }

    error = bt.gap().startAdvertising(ble::LEGACY_ADVERTISING_HANDLE);
    if (error) {
        printf("Error starting advertising: %u\n", error);
        return;
    }

    printf("Advertising started. Device name: %s\n", device_name);
}

void update_sensor_data() {

    bt.gattServer().write(humidityChar.getValueHandle(), (uint8_t *) &humidity_val, sizeof(humidity_val));
    bt.gattServer().write(temperatureChar.getValueHandle(), (uint8_t *) &temperature_val, sizeof(temperature_val));
    bt.gattServer().write(lightChar.getValueHandle(), (uint8_t *) &light_val, sizeof(light_val));

}

void setup_ble() {
    printf("Starting BLE float broadcaster...\n");
    bt.onEventsToProcess(schedule_ble_events);
    if(bt.init(on_ble_init_complete) != BLE_ERROR_NONE) {
        printf("BLE failed\n");
        return;
    };

    event_queue.call_every(2000ms, update_sensor_data);

    event_queue.dispatch_forever();
}

void schedule_ble_events(BLE::OnEventsToProcessCallbackContext *context) {
    event_queue.call(Callback<void()>(&bt, &BLE::processEvents));
}