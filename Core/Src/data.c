
#include <data.h>
#include <string.h>
#include "checksum.h"
#include "messages.h"
#include "device.h"
#include "main.h"
#include "flash.h"

extern UART_HandleTypeDef huart1;

bool parse_velocity_package(device_settings *device_struct,  uint8_t  *message, bool force)
{
	if  (IsChecksumm8bCorrect(message, VELOCITY_REQUEST_LENGTH))  {

		struct VelocityRequest req;

		memcpy((void*)&req,  (void*)message,  VELOCITY_REQUEST_LENGTH);

        if  (req.address  ==  device_struct->device_adress || force)  {
          struct VelocityResponse response;
          response.magic = 0xAA;
          response.type = force ? FORCE_VELOCITY_TYPE : VELOCITY_TYPE;
          response.address = device_struct->device_adress;
          response.velocity = req.velocity;
          AddChecksumm8b((uint8_t*)&response,VELOCITY_RESPONSE_LENGTH);

        	device_struct->velocity  =  req.velocity;
        	UpdateDeviceSettings(device_struct);

          HAL_UART_Transmit(&huart1, (uint8_t*)&response, VELOCITY_RESPONSE_LENGTH, 1000);
          return true;
        }
	}
    return false;
}

//void device_response(device_settings  *device_struct)
//{
//
//	struct DevicesResponse resp;
//
//	resp.preambule            = 0xAA;
//	resp.address       = device_struct->device_adress;
//	resp.velocity     = device_struct->velocity;
//
//
//	memcpy((void*)transmit_buffer,  (void*)&resp,  DEVICES_RESPONSE_LENGTH - 1);
//
//	AddChecksumm8b(transmit_buffer,  DEVICES_RESPONSE_LENGTH);
//
//    HAL_GPIO_WritePin(RS485_DIR_PORT,  RS485_DIR_PIN,  GPIO_PIN_SET);
//
//    HAL_UART_Transmit_DMA(&huart1,  transmit_buffer,  DEVICES_RESPONSE_LENGTH);
//}

bool parse_config_package(device_settings *device_struct,  uint8_t  *message, bool force)
{
	if  (IsChecksumm8bCorrect(message, CONFIG_REQUEST_LENGTH))  {
        struct ConfigRequest req;

        memcpy((void*)&req,  (void*)message,  CONFIG_REQUEST_LENGTH);

        if  (req.old_address  ==  device_struct->device_adress || force)  {
          struct ConfigResponse response;
          response.magic = 0xAA;
          response.type = force ? FORCE_CONFIG_TYPE : CONFIG_TYPE;
          response.old_address = device_struct->device_adress;
          response.new_address = req.new_address;
          AddChecksumm8b((uint8_t*)&response,CONFIG_RESPONSE_LENGTH);

          device_struct->device_adress  =  req.new_address;
          FLASH_WriteSettings(device_struct);

          HAL_UART_Transmit(&huart1, (uint8_t*)&response, CONFIG_RESPONSE_LENGTH, 1000);
        	return true;
        }
	}
	return false;
}

bool parse_info_package(device_settings *device_struct,  uint8_t  *message, bool force)
{
  if  (IsChecksumm8bCorrect(message, INFO_REQUEST_LENGTH))  {
    struct InfoRequest req;

    memcpy((void*)&req,  (void*)message,  INFO_REQUEST_LENGTH);

    if  (req.address  ==  device_struct->device_adress || force)  {
      struct InfoResponse response;
      response.magic = 0xAA;
      response.type = force ? FORCE_INFO_TYPE : INFO_TYPE;
      response.address = device_struct->device_adress;
      response.velocity = device_struct->velocity;
      AddChecksumm8b((uint8_t*)&response,INFO_RESPONSE_LENGTH);

      HAL_UART_Transmit(&huart1, (uint8_t*)&response, INFO_RESPONSE_LENGTH, 1000);
      return true;
    }
  }
  return false;
}
