#include <gd32_include.h>
#include <printf_over_x.h>
#include <stdio.h>
#include <stdarg.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>

#define LEDPORT GPIOA
#define LEDPIN GPIO_PIN_1
#define LED_CLOCK RCU_GPIOA

#define STACK_SIZE 256
StaticTask_t xBlinkyTaskBuffer;
StackType_t xBlinkyStack[STACK_SIZE];
TaskHandle_t blinkyTaskHandle;

StaticTask_t xPrintTaskBuffer;
StackType_t xPrintStack[STACK_SIZE];
TaskHandle_t printTaskHandle;

void init_led()
{
    rcu_periph_clock_enable(LED_CLOCK);
#if defined(GD32F3x0) || defined(GD32F1x0) || defined(GD32F4xx) || defined(GD32E23x)
    gpio_mode_set(LEDPORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LEDPIN);
    gpio_output_options_set(LEDPORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LEDPIN);
#else /* valid for GD32F10x, GD32E20x, GD32F30x, GD32F403, GD32E10X */
    gpio_init(LEDPORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LEDPIN);
#endif
}

SemaphoreHandle_t xPrintfSemaphore = NULL;
StaticSemaphore_t xPrintfMutexBuffer;

void threadsafe_printf_init()
{
    xPrintfSemaphore = xSemaphoreCreateMutexStatic(&xPrintfMutexBuffer);
}

void threadsafe_printf_lock() {  xSemaphoreTake(xPrintfSemaphore, portMAX_DELAY); }
void threadsafe_printf_unlock() { xSemaphoreGive(xPrintfSemaphore); }

void threadsafe_printf(const char *pcFormat, ...)
{
    va_list arg;
    threadsafe_printf_lock();
    va_start(arg, pcFormat);
    vprintf(pcFormat, arg);
    va_end(arg);
    threadsafe_printf_unlock();
}

void vBlinkyTask(void *pvParameters)
{
    for (;;)
    {
        gpio_bit_set(LEDPORT, LEDPIN);
        vTaskDelay(pdMS_TO_TICKS(500));
        gpio_bit_reset(LEDPORT, LEDPIN);
        vTaskDelay(pdMS_TO_TICKS(500));
        threadsafe_printf("Blinky!\n");
    }
}

void vTaskManagerTask(void *pvParameters)
{
    static TaskStatus_t pxTaskStatusArray[4];
    unsigned long ulTotalRunTime, ulStatsAsPercentage;
    UBaseType_t uxArraySize = sizeof(pxTaskStatusArray) / sizeof(pxTaskStatusArray[0]);
    for (;;)
    {
        vTaskDelay(pdMS_TO_TICKS(2000));
        uxArraySize = uxTaskGetSystemState(pxTaskStatusArray,
                                           uxArraySize,
                                           &ulTotalRunTime);
        printf("Total runtime: %lu ticks\n", ulTotalRunTime);
        for (UBaseType_t x = 0; x < uxArraySize; x++)
        {
            ulStatsAsPercentage =
                  pxTaskStatusArray[ x ].ulRunTimeCounter / ulTotalRunTime;
            if( ulStatsAsPercentage > 0UL )
            {
               threadsafe_printf( "Task: \"%s\", Prio: %lu, Runtime: %lu, CPU Usage: %lu%%\n",
                                 pxTaskStatusArray[ x ].pcTaskName,
                                 pxTaskStatusArray[ x ].uxCurrentPriority,
                                 pxTaskStatusArray[ x ].ulRunTimeCounter,
                                 ulStatsAsPercentage );
            }
            else
            {
               /* If the percentage is zero here then the task has
               consumed less than 1% of the total run time. */
               threadsafe_printf( "Task: \"%s\", Prio: %lu, Runtime: %lu, CPU Usage:<1%%\n",
                                 pxTaskStatusArray[ x ].pcTaskName,
                                 pxTaskStatusArray[ x ].uxCurrentPriority,
                                 pxTaskStatusArray[ x ].ulRunTimeCounter);
            }
        }
    }
}

int main(void)
{
    init_printf_transport();
    threadsafe_printf_init();
    init_led();

    printf("Starting FreeRTOS demo!\n");

    /* Create the task without using any dynamic memory allocation. */
    blinkyTaskHandle = xTaskCreateStatic(
        vBlinkyTask,      /* Function that implements the task. */
        "Blinky",         /* Text name for the task. */
        STACK_SIZE,       /* Number of indexes in the xBlinkyStack array. */
        (void *)0,        /* Parameter passed into the task. */
        tskIDLE_PRIORITY, /* Priority at which the task is created. */
        xBlinkyStack,           /* Array to use as the task's stack. */
        &xBlinkyTaskBuffer);    /* Variable to hold the task's data structure. */

    printTaskHandle = xTaskCreateStatic(
        vTaskManagerTask,  /* Function that implements the task. */
        "TaskManager",     /* Text name for the task. */
        STACK_SIZE,        /* Number of indexes in the xBlinkyStack array. */
        (void *)0,        /* Parameter passed into the task. */
        tskIDLE_PRIORITY, /* Priority at which the task is created. */
        xPrintStack,      /* Array to use as the task's stack. */
        &xPrintTaskBuffer);    /* Variable to hold the task's data structure. */

    /* Start the scheduler itself. */
    vTaskStartScheduler();
    /* never reached */
    return 0;
}

/* service FreeRTOS timer interrupts (except for ports which do it automatically) */
#if !defined(GD32E23x) && !defined(GD32E50X)
extern void xPortSysTickHandler(void);
void SysTick_Handler(void)
{
    xPortSysTickHandler();
}
#endif