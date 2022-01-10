
#include "flash.h"
#include "device.h"

void  FLASH_ReadSettings (device_settings *device_struct)
{

	flash_struct  config;

	uint32_t  *flash_config_struct_addr  =  (uint32_t*)CONFIG_PAGE_ADDR;
	uint32_t  *config_struct_addr        =  (void *)&config;

	for (uint8_t i  =  0;  i  <  AMOUNT_OF_32B_WORDS;  i++)  {
		*config_struct_addr  =  *(volatile uint32_t*)flash_config_struct_addr;
		flash_config_struct_addr++;
		config_struct_addr++;
	}

	device_struct->device_adress  =  config.adress;
}

bool  FLASH_WriteSettings (device_settings *device_struct)
{

	flash_struct  data;

  data.adress  =  device_struct->device_adress;

	HAL_FLASH_Unlock();

	FLASH_EraseInitTypeDef errase_conf;
	errase_conf.TypeErase    =  FLASH_TYPEERASE_PAGES;
	errase_conf.PageAddress  =  CONFIG_PAGE_ADDR;
	errase_conf.NbPages      =  1;

	uint32_t page_error;
	if(HAL_FLASHEx_Erase(&errase_conf,  &page_error) != HAL_OK){
    return false;
  }

	uint32_t  *flash_config_struct_addr  =  (uint32_t*)CONFIG_PAGE_ADDR;
  uint32_t  *config_struct_addr        =  (void *)&data;

  for (uint8_t  i  =  0;  i  <  AMOUNT_OF_32B_WORDS;  i++)  {
    if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)flash_config_struct_addr,  *config_struct_addr) != HAL_OK){
      return false;
    }
    flash_config_struct_addr++;
    config_struct_addr++;
  }

  HAL_FLASH_Lock();
  return true;
}
