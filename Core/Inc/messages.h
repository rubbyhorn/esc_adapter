#ifndef __MESSAGES_H
#define __MESSAGES_H

//#define CONFIG_TYPE 0x03
//#define FORCE_CONFIG_TYPE 0x03
//#define VELOCITY_TYPE 0x03
//#define FORCE_VELOCITY_TYPE 0x04
//#define ERROR_TYPE 0x05
//#define INFO_TYPE 0x06
//#define FORCE_INFO_TYPE 0x07

#define MAGIC 0xAA

#define MAX_BUFFER_LENGTH 20
//#define CONFIG_REQUEST_LENGTH 5
//#define CONFIG_RESPONSE_LENGTH 5
//#define VELOCITY_REQUEST_LENGTH 5
//#define VELOCITY_RESPONSE_LENGTH 5
//#define INFO_REQUEST_LENGTH 4
//#define INFO_RESPONSE_LENGTH 5
//#define ERROR_LENGTH 5

#define RECEIVE_TIMEOUT                     100

#define NORMAL_REQUEST_TYPE                 0x01
#define TERMINAL_REQUEST_TYPE               0x02
#define CONFIG_TYPE                 0x03
#define FORCE_CONFIG_TYPE           0x03
#define FIRMWARE_REQUEST_TYPE               0x04
#define VELOCITY_TYPE               0xAA
#define FORCE_VELOCITY_TYPE         0xAA

#define NORMAL_REQUEST_LENGTH               5
#define NORMAL_RESPONSE_LENGTH              9
#define TERMINAL_REQUEST_LENGTH             15
#define TERMINAL_RESPONSE_LENGTH            16
#define CONFIG_REQUEST_LENGTH               18
#define VELOCITY_REQUEST_LENGTH              7
#define VELOCITY_RESPONSE_LENGTH             10

#include <stdint.h>

#pragma pack(push, 1)

struct VelocityRequest
{
    uint8_t AA1;
    uint8_t AA2;
    uint8_t address;
    uint8_t setting;
    uint8_t velocity1;
    uint8_t velocity2;
    uint8_t crc;
};

struct VelocityResponse
{
    uint8_t AA;
    uint8_t address;
    uint8_t errors;
    uint16_t current1;
    uint16_t current2;
    uint8_t velocity1;
    uint8_t velocity2;
    uint8_t crc;
};

struct ConfigRequest
{
  uint8_t  magic;
  uint8_t  type;               // 0x03
  uint8_t  update_firmware;    // (bool) go to bootloader and update firmware
  uint8_t  forse_setting;      // (bool) set new address or update firmware even if old address doesn't equal BLDC address
  uint8_t  old_address;
  uint8_t  new_address;
  uint16_t high_threshold;
  uint16_t low_threshold;
  uint16_t average_threshold;
  uint8_t  update_correction;
  uint16_t clockwise_speed_k;
  uint16_t counterclockwise_speed_k;
  uint8_t  crc;
};

struct ConfigResponse
{
    uint8_t magic;
    uint8_t address;
    uint8_t errors;
    uint16_t current1;
    uint16_t current2;
    uint8_t velocity1;
    uint8_t velocity2;
    uint8_t crc;
};

struct ErrorResponse
{
    uint8_t magic;          //MAGIC
    uint8_t  type;          // 0x05
    uint8_t  address;
    uint8_t  error_type;
    uint8_t  crc;
};

struct InfoRequest
{
    uint8_t magic;          //MAGIC
    uint8_t  type;          // 0x06 or 0x07
    uint8_t  address;
    uint8_t  crc;
};

struct InfoResponse
{
    uint8_t magic;          //MAGIC
    uint8_t  type;          // 0x06 or 0x07
    uint8_t  address;
    uint8_t  velocity;
    uint8_t  crc;
};

#pragma pack(pop)

#endif //__MESSAGES_H
