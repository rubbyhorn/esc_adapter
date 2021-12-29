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
#include "data.h"

#define RECEIVE_TIMEOUT 3000 // timeout in tics

static uint8_t RxBuff[MAX_BUFFER_LENGTH]; // received frame
static uint8_t buff; // last received byte
static size_t RxCounter = 0; //number of already received bytes
static uint32_t prevTick =  0; //tick of last Rx callback
static uint8_t frameSize = MAX_BUFFER_LENGTH;
static bool frameReady, frameError = false;

device_settings deviceSettings;
extern UART_HandleTypeDef huart1;

void discardFrame();
void processFrame();
void newFrame();
void processError();

void esc_adapter_init(){
    newFrame();
}

void esc_adapter_loop(){
    if (frameReady) {
        processFrame();
    }
    if (frameError) {
        discardFrame();
    }
    __disable_irq();
    if (HAL_GetTick() - prevTick  >=  RECEIVE_TIMEOUT)  {
        HAL_UART_AbortReceive_IT(&huart1);
        const char *message = "UART timeout error. Waiting new frame.\n";
        HAL_UART_Transmit(&huart1, (uint8_t *)message, strlen(message),1000);
        prevTick = HAL_GetTick();
        discardFrame();
    }
    __enable_irq();
}

void HAL_UART_RxCpltCallback (UART_HandleTypeDef * huart){
    if (huart == &huart1) {
        prevTick = HAL_GetTick();
        RxBuff[RxCounter] = buff;
        RxCounter++;
        if (RxCounter == 1 && buff != PREAMBULE){
            frameError = true;
            return;
        }
        else if (RxCounter == 2){
            switch (buff) {
                case CONFIG_REQUEST_TYPE:
                case FORCE_DEVICE_REQUEST_TYPE:
                    frameSize = CONFIG_REQUEST_LENGTH; break;
                case VELOCITY_REQUEST_TYPE:
                    frameSize = VELOCITY_REQUEST_LENGTH; break;
                default:
                    frameError = true; return;
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

void discardFrame(){
    
    const char *message = "frame discarded.\n";
    HAL_UART_Transmit_IT(&huart1, (uint8_t *)message, strlen(message));
    newFrame();
}

void newFrame(){
    RxCounter = 0;
    frameSize = MAX_BUFFER_LENGTH;
    frameReady = frameError = false;
    HAL_UART_Receive_IT(&huart1, &buff, 1);
}

void processFrame(){
    switch (RxBuff[1]) {
        case CONFIG_REQUEST_TYPE:
            if(!parse_config_package(&deviceSettings, RxBuff, false)) processError();
            break;
        case FORCE_DEVICE_REQUEST_TYPE:
            parse_config_package(&deviceSettings, RxBuff, true);
            break;
        case VELOCITY_REQUEST_TYPE:

            break;
    }
    HAL_UART_Transmit(&huart1, RxBuff, frameSize, 1000);
    newFrame();
}

void processError(){

}