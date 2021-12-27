//
// Created by user on 27/12/2021.
//

#include <stdint.h>
#include "esc_adapter.h"
#include "stm32f0xx_hal.h"
#include <string.h>
#include <stdio.h>
#include "messages.h"

#define BUFFERLEN 5

static uint8_t RxBuff[BUFFERLEN];
static uint8_t buff;
static size_t RxPointer = 0;
static uint32_t  prevTick =  0;

extern UART_HandleTypeDef huart1;

void esc_adapter_init(){
    HAL_UART_Receive_IT(&huart1, &buff, 1);
}

void esc_adapter_loop(){
//    if (HAL_GetTick() - prevTick  >=  RECEIVE_TIMEOUT)  {
//        HAL_UART_Abort_IT(&huart1);
//        const char *err_msg = "UART timeout error";
//        HAL_UART_Receive_IT(&huart1, (uint8_t*)err_msg, strlen(err_msg));
//    }
}

void HAL_UART_RxCpltCallback (UART_HandleTypeDef * huart){
    if (huart == &huart1) {
        if (RxPointer < BUFFERLEN-1) {
            RxBuff[RxPointer] = buff;
            RxPointer++;
            HAL_UART_Receive_IT(&huart1, &buff, 1);
        } else{
            RxBuff[RxPointer] = buff;
//            char str[80];
//            sprintf (str, "%s %d %c", "one", 2, '3'); //WTF
            HAL_UART_Transmit_IT(&huart1, RxBuff, strlen((char *)RxBuff));
            RxPointer = 0;
            HAL_UART_Receive_IT(&huart1, &buff, 1);
        }
        prevTick = HAL_GetTick();
    }
}