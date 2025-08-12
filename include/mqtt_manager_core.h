#pragma once

#include <mbed.h>
#include "WiFiInterface.h"


bool mqtt_init_client( const char *broker,
                       int port,
                       const char *clientId );

/**
 * Publish a message to a topic.
 */
bool mqtt_publish( const char *topic, const char *message );

bool mqtt_subscribe( const char *topic);


void mqtt_yield( int timeoutMs );


void mqtt_disconnect();