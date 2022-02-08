//
// Created by user on 27/12/2021.
//

#include <stdint.h>
#include "esc_adapter.h"
#include "stm32f0xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "messages.h"
#include <stdbool.h>
//#include "data.h"
#include "checksum.h"
#include "flash.h"
#include "communication.h"

//#define RECEIVE_TIMEOUT 3000 // timeout in tics

static uint8_t RxBuff[MAX_BUFFER_LENGTH]; // received frame
static uint8_t buff; // last received byte
static size_t RxCounter = 0; //number of already received bytes
static uint32_t prevTick =  0; //tick of last Rx callback
static uint8_t frameSize = MAX_BUFFER_LENGTH;
static bool frameReady, frameError = false;

device_settings deviceSettings;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim17;

void processFrame();
void newFrame();
//void processError();

void esc_adapter_init(){
  FLASH_ReadSettings(&deviceSettings);
  deviceSettings.velocity = 0;
  UpdateDeviceSettings(&deviceSettings);
  HAL_TIM_PWM_Start(&htim17, TIM_CHANNEL_1);
  newFrame();
}

void esc_adapter_loop(){
    if (frameReady) {
        processFrame();
    }
    if (frameError) {
        newFrame();
    }
    __disable_irq();
    if (HAL_GetTick() - prevTick  >=  RECEIVE_TIMEOUT)  {
        HAL_UART_AbortReceive(&huart1);
//        const char *message = "\nUART timeout error.\n";
//        HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message),1000);
        prevTick = HAL_GetTick();
        newFrame();
    }
    __enable_irq();
}

void HAL_UART_RxCpltCallback (UART_HandleTypeDef * huart){
    if (huart == &huart1) {
        prevTick = HAL_GetTick();
        RxBuff[RxCounter] = buff;
        RxCounter++;
        if (RxCounter == 1 && buff != MAGIC){
            frameError = true;
            return;
        }
        else if (RxCounter == 2){
            switch (buff) {
                case CONFIG_TYPE:
//                case FORCE_CONFIG_TYPE:
                    frameSize = CONFIG_REQUEST_LENGTH;
                    break;
                case VELOCITY_TYPE:
//                case FORCE_VELOCITY_TYPE:
                    frameSize = VELOCITY_REQUEST_LENGTH;
                    break;
//                case INFO_TYPE:
//                case FORCE_INFO_TYPE:
//                  frameSize = INFO_REQUEST_LENGTH;
//                  break;
                default:
                    frameError = true;
                    return;
            }
            HAL_UART_Receive_IT(&huart1, &buff, 1); // continue reciving
        }
        else if (RxCounter == frameSize) {
            frameReady = true;
        }
        else {
            HAL_UART_Receive_IT(&huart1, &buff, 1); // continue reciving
        }

    }
}

void newFrame(){
    RxCounter = 0;
    frameSize = MAX_BUFFER_LENGTH;
    frameReady = frameError = false;
    HAL_UART_Receive_IT(&huart1, &buff, 1);
}

void processFrame(){
    switch (RxBuff[1]) {
        case CONFIG_TYPE:
            if(!parse_config_package(&deviceSettings, RxBuff))
//              processError(CONFIG_TYPE);
            break;
//        case FORCE_CONFIG_TYPE:
//            if(!parse_config_package(&deviceSettings, RxBuff, true))
////              processError(CONFIG_TYPE);
//            break;
        case VELOCITY_TYPE:
            if(parse_velocity_package(&deviceSettings, RxBuff))
//              device_response(&deviceSettings);
            break;
//        case FORCE_VELOCITY_TYPE:
//          if(!parse_velocity_package(&deviceSettings, RxBuff, true))
////            processError(CONFIG_TYPE);
//          break;
//        case INFO_TYPE:
//          if(!parse_info_package(&deviceSettings, RxBuff, false))
////            processError(CONFIG_TYPE);
//          break;
//        case FORCE_INFO_TYPE:
//          if(!parse_info_package(&deviceSettings, RxBuff, true))
////            processError(CONFIG_TYPE);
//          break;

    }
    newFrame();
}

//void processError(int errType){
//    struct ErrorResponse response;
//
//
//    response.magic = 0xAA;
//    response.type = ERROR_TYPE;
//    response.address = deviceSettings.device_adress;
//    response.error_type = errType;
//    AddChecksumm8b((uint8_t*)&response,ERROR_LENGTH);
//    HAL_UART_Transmit(&huart1, (uint8_t*)&response, ERROR_LENGTH, 1000);
//}