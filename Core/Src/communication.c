
#include <communication.h>
#include <string.h>
#include "checksum.h"
#include "messages.h"
#include "device.h"
#include "main.h"
#include "flash.h"

extern UART_HandleTypeDef huart1;
extern  device_settings device_struct1;

bool parse_velocity_package(device_settings *device_struct,  uint8_t  *message)
{
  if  (IsChecksumm8bCorrect(message, VELOCITY_REQUEST_LENGTH) || 1)  {

    struct VelocityRequest req;

    memcpy((void*)&req,  (void*)message,  VELOCITY_REQUEST_LENGTH);

    if  (req.address  ==  device_struct->device_adress)  {
      device_struct->velocity  =  req.velocity;
      UpdateDeviceSettings(device_struct);
      return true;
    }
  }
  return false;
}

void device_response(device_settings  *device_struct)
{

	struct VelocityResponse resp;

	resp.AA            = 0xAA;
	resp.address       = device_struct->device_adress;
	resp.errors        = 0x00;
	resp.current1      = 0x0000;
	resp.current2      = 0x0000;
	resp.velocity1     = device_struct->velocity;
  resp.velocity2     = 0x00;
  uint8_t transmit_buffer[VELOCITY_RESPONSE_LENGTH];
	memcpy((void*)transmit_buffer,  (void*)&resp,  VELOCITY_RESPONSE_LENGTH - 1);

	AddChecksumm8b(transmit_buffer,  VELOCITY_RESPONSE_LENGTH);
  HAL_UART_Transmit(&huart1,  transmit_buffer,  VELOCITY_RESPONSE_LENGTH, 10);
}

bool parse_config_package(device_settings *device_struct,  uint8_t  *message)
{
	if  (IsChecksumm8bCorrect(message, CONFIG_REQUEST_LENGTH))  {
        struct ConfigRequest req;

        memcpy((void*)&req,  (void*)message,  CONFIG_REQUEST_LENGTH);

        if  (req.old_address  ==  device_struct->device_adress || req.forse_setting)  {
        	device_struct->device_adress  =  req.new_address;
          FLASH_WriteSettings(device_struct);
        	return true;
        }
	}
	return false;
}

