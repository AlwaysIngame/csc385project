#pragma once
#include "mbed.h"

#define REST_HOST     "192.168.2.15"

bool rest_post_data (const char*json, SocketAddress addr, const char* path);