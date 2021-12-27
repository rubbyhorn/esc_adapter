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

#define RECEIVE_TIMEOUT 3000 // timeout in tics

static uint8_t RxBuff[MAX_BUFFER_LEN]; // received frame
static uint8_t buff; // last received byte
static size_t RxCounter = 0; //number of already received bytes
static uint32_t prevTick =  0; //tick of last Rx callback
static uint8_t frameSize = MAX_BUFFER_LEN;
static uint8_t frameReady = false;

extern UART_HandleTypeDef huart1;

void discardFrame();
void processFrame();

void esc_adapter_init(){
    HAL_UART_Receive_IT(&huart1, &buff, 1);
//    char str[80];
//    sprintf (str, "%s %d %c", "one", 2, '3'); //WTF
}

void esc_adapter_loop(){
    if (RxCounter == 1 && RxBuff[0] != PREAMBULE){
        discardFrame();
    }
    if (RxCounter == 2){
        switch (RxBuff[1]) {
            case CONFIG_REQUEST_TYPE:
            case FORCE_DEVICE_REQUEST_TYPE:
                frameSize = CONFIG_REQUEST_LENGTH; break;
            case VELOCITY_REQUEST_TYPE:
                frameSize = VELOCITY_REQUEST_LENGTH; break;
            default:
                discardFrame(); break;
        }
    }
    if (frameReady) {
        processFrame();
        frameReady = false;
        discardFrame();
    }
    if (HAL_GetTick() - prevTick  >=  RECEIVE_TIMEOUT)  {
        HAL_UART_AbortReceive_IT(&huart1);
        const char *message = "UART timeout error. Waiting new frame.\n";
        HAL_UART_Transmit_IT(&huart1, (uint8_t *)message, strlen(message));
        prevTick = HAL_GetTick();
        discardFrame();
    }
}

void HAL_UART_RxCpltCallback (UART_HandleTypeDef * huart){
    if (huart == &huart1) {

        if (RxCounter < frameSize) {
            RxCounter++;
        } else {
            frameReady = true;
            return;
        }
        RxBuff[RxCounter-1] = buff;
        HAL_UART_Receive_IT(&huart1, &buff, 1);
        prevTick = HAL_GetTick();
    }
}

void discardFrame(){
    RxCounter = 0;
    frameSize = MAX_BUFFER_LEN;
    const char *message = "frame discarded.\n";
    HAL_UART_Transmit_IT(&huart1, (uint8_t *)message, strlen(message));
    HAL_UART_Receive_IT(&huart1, &buff, 1);
}

void processFrame(){
    HAL_UART_Transmit_IT(&huart1, (uint8_t *)RxBuff, sizeof RxBuff);


}