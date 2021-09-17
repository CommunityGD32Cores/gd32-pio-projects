/* mbed Microcontroller Library
 * Copyright (c) 2018 GigaDevice Semiconductor Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MBED_PERIPHERALNAMES_H
#define MBED_PERIPHERALNAMES_H

#include "cmsis.h"

#ifdef __cplusplus
extern "C" {
#endif

/* hacks because GD32 mbed-os code is stupid. it wants to reference peripherals that are not there and they don't do #ifdef <peripheral> checks. */

#define CAN_1 0
#define RCU_CAN1 0
#define RCU_TIMER8 0
#define RCU_TIMER9 1
#define RCU_TIMER10 2
#define RCU_TIMER11 3
#define RCU_TIMER12 4
#define RCU_TIMER13 4
#define UART_3 0
#define UART_4 1

typedef enum {
    ADC_0 = (int)ADC0,
    ADC_1 = (int)ADC1
} ADCName;

typedef enum {
    DAC_0 = (int)DAC0,
} DACName;

typedef enum {
    UART_0 = (int)USART0,
    UART_1 = (int)USART1,
    UART_2 = (int)USART2
} UARTName;

#define STDIO_UART_TX  PORTA_9
#define STDIO_UART_RX  PORTA_10
#define STDIO_UART     UART_0

typedef enum {
    SPI_0 = (int)SPI0,
    SPI_1 = (int)SPI1,
    SPI_2 = (int)SPI2
} SPIName;

typedef enum {
    I2C_0 = (int)I2C0,
    I2C_1 = (int)I2C1
} I2CName;

typedef enum {
    PWM_0 = (int)TIMER0,
    PWM_1 = (int)TIMER1,
    PWM_2 = (int)TIMER2,
    PWM_3 = (int)TIMER3,
    PWM_4 = (int)TIMER4
} PWMName;

typedef enum {
    CAN_0 = (int)CAN0
} CANName;

#ifdef __cplusplus
}
#endif

#endif
