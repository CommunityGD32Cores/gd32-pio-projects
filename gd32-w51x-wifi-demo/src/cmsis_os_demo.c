/* --------------------------------------------------------------------------
 * Copyright (c) 2013-2017 ARM Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 *      Name:    Blinky.c
 *      Purpose: RTOS2 example program
 *
 *---------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>

#if defined(PLATFORM_OS_FREERTOS)
//#include "RTE_Components.h"
//#include CMSIS_device_header

#include "cmsis_os2.h"


osThreadId_t tid_phaseA;                /* Thread id of thread: phase_a      */
osThreadId_t tid_phaseB;                /* Thread id of thread: phase_b      */
osThreadId_t tid_phaseC;                /* Thread id of thread: phase_c      */
osThreadId_t tid_phaseD;                /* Thread id of thread: phase_d      */
osThreadId_t tid_clock;                 /* Thread id of thread: clock        */

struct phases_t {
  int_fast8_t phaseA;
  int_fast8_t phaseB;
  int_fast8_t phaseC;
  int_fast8_t phaseD;
} g_phases;


/*----------------------------------------------------------------------------
 *      Function 'signal_func' called from multiple threads
 *---------------------------------------------------------------------------*/
void signal_func (osThreadId_t tid)  {
  osThreadFlagsSet(tid_clock, 0x0100);      /* set signal to clock thread    */
  osDelay(500);                             /* delay 500ms                   */
  osThreadFlagsSet(tid_clock, 0x0100);      /* set signal to clock thread    */
  osDelay(500);                             /* delay 500ms                   */
  osThreadFlagsSet(tid, 0x0001);            /* set signal to thread 'thread' */
  osDelay(500);                             /* delay 500ms                   */
}

/*----------------------------------------------------------------------------
 *      Thread 1 'phaseA': Phase A output
 *---------------------------------------------------------------------------*/
void phaseA (void *argument) {
  for (;;) {
    osThreadFlagsWait(0x0001, osFlagsWaitAny ,osWaitForever);   /* wait for an event flag 0x0001 */
    g_phases.phaseA = 1;
    printf("A\r\n");
    signal_func(tid_phaseB);                                    /* call common signal function   */
    g_phases.phaseA = 0;
  }
}

/*----------------------------------------------------------------------------
 *      Thread 2 'phaseB': Phase B output
 *---------------------------------------------------------------------------*/
void phaseB (void *argument) {
  for (;;) {
    osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);   /* wait for an event flag 0x0001 */
    g_phases.phaseB = 1;
    printf("B\r\n");
    signal_func(tid_phaseC);                                    /* call common signal function   */
    g_phases.phaseB = 0;
  }
}

/*----------------------------------------------------------------------------
 *      Thread 3 'phaseC': Phase C output
 *---------------------------------------------------------------------------*/
void phaseC (void *argument) {
  for (;;) {
    osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);   /* wait for an event flag 0x0001 */
    g_phases.phaseC = 1;
    printf("C\r\n");
    signal_func(tid_phaseD);                                    /* call common signal function   */
    g_phases.phaseC = 0;
  }
}

/*----------------------------------------------------------------------------
 *      Thread 4 'phaseD': Phase D output
 *---------------------------------------------------------------------------*/
void phaseD (void *argument) {
  for (;;) {
    osThreadFlagsWait(0x0001, osFlagsWaitAny, osWaitForever);   /* wait for an event flag 0x0001 */
    g_phases.phaseD = 1;
    printf("D\r\n");
    signal_func(tid_phaseA);                                    /* call common signal function   */
    g_phases.phaseD = 0;
  }
}

/*----------------------------------------------------------------------------
 *      Thread 5 'clock': Signal Clock
 *---------------------------------------------------------------------------*/
void clock (void *argument) {
  for (;;) {
    osThreadFlagsWait(0x0100, osFlagsWaitAny, osWaitForever);   /* wait for an event flag 0x0100 */
    printf("clock\r\n");
    osDelay(80);                                                /* delay  80ms                   */
  }
}
#endif

/*----------------------------------------------------------------------------
 *      Blinky demo
 *---------------------------------------------------------------------------*/
void blinky_demo (void)
{
#if defined(PLATFORM_OS_FREERTOS)
  osThreadAttr_t attr;

  memset(&attr, 0, sizeof(attr));
  attr.name = "phaseA";
  tid_phaseA = osThreadNew(phaseA, NULL, &attr);
  attr.name = "phaseB";
  tid_phaseB = osThreadNew(phaseB, NULL, &attr);
  attr.name = "phaseC";
  tid_phaseC = osThreadNew(phaseC, NULL, &attr);

  attr.name = "phaseD";
  tid_phaseD = osThreadNew(phaseD, NULL, &attr);
  attr.name = "clock";
  tid_clock  = osThreadNew(clock,  NULL, &attr);
  osThreadFlagsSet(tid_phaseA, 0x0001); /* set signal to phaseA thread   */
#endif
}

