#ifndef __MESSAGES_H
#define __MESSAGES_H

#define CONFIG_REQUEST_TYPE 0x01
#define FORCE_DEVICE_REQUEST_TYPE 0x02
#define VELOCITY_REQUEST_TYPE 0x03
#define ERROR_REQUEST_TYPE 0x03

#define PREAMBULE 0xAA

#define MAX_BUFFER_LENGTH 5
#define CONFIG_REQUEST_LENGTH       5
#define CONFIG_RESPONSE_LENGTH      5
#define VELOCITY_REQUEST_LENGTH     5
#define VELOCITY_RESPONSE_LENGTH    5

#include <stdint.h>

#pragma pack(push, 1)

struct VelocityRequest
{
	uint8_t preambule;      //PREAMBULE
    uint8_t  type;          // 0x03
	uint8_t address;
	uint8_t velocity;
	uint8_t crc;
};

struct VelocityResponse
{
	uint8_t preambule;      //PREAMBULE
    uint8_t  type;          // 0x03
	uint8_t address;
	uint8_t velocity;
	uint8_t crc;
};

struct ConfigRequest
{
    uint8_t preambule;      //PREAMBULE
	uint8_t  type;          // 0x01 or 0x02
	uint8_t  old_address;
	uint8_t  new_address;
	uint8_t  crc;
};

struct ConfigResponse
{
    uint8_t preambule;      //PREAMBULE
    uint8_t  type;          // 0x03 or 0x02
    uint8_t  old_address;
    uint8_t  new_address;
    uint8_t  crc;
};

struct ErrorResponse
{
    uint8_t preambule;      //PREAMBULE
    uint8_t  type;          // 0x04
    uint8_t  address;
    uint8_t  error_type;
    uint8_t  crc;
};

#pragma pack(pop)

#endif //__MESSAGES_H
