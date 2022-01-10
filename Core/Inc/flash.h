
#ifndef FLASH_H_
#define FLASH_H_

#include "device.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_flash.h"
#include "stm32f0xx_hal_flash_ex.h"

#define  ADDR_FLASH_PAGE_127    ((uint32_t)0x08007C00)
#define  CONFIG_PAGE_ADDR ADDR_FLASH_PAGE_127

#pragma pack(push,  1)

typedef struct  {
	uint8_t  adress;
	uint8_t  reserved_slot1;  // In order for
	uint8_t  reserved_slot2;  // the structure to be
	uint8_t  reserved_slot3;  // a multiple of 32 bytes
}  flash_struct;

#pragma pack(pop)

#define AMOUNT_OF_32B_WORDS  sizeof(flash_struct)/4

void  FLASH_ReadSettings  (device_settings *device_struct);
bool  FLASH_WriteSettings (device_settings *device_struct);

#endif /* FLASH_H_ */
