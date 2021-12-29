#ifndef __COMMUNICATION_H
#define __COMMUNICATION_H

#include <stdint.h>
#include "messages.h"
#include "device.h"
#include <stdbool.h>

bool parse_velocity_package(device_settings *device_struct,  uint8_t  *message, bool force);
bool parse_config_package(device_settings *device_struct,  uint8_t  *message, bool force);

#endif //__COMMUNICATION_H
