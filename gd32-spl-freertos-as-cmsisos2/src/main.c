#include <gd32_include.h>
#include <printf_over_x.h>
#include <stdio.h>
#include <stdarg.h>
#include <cmsis_os2.h>

#define LEDPORT GPIOA
#define LEDPIN GPIO_PIN_1
#define LED_CLOCK RCU_GPIOA

#define STACK_SIZE (256 * 4)
uint8_t blinky_thread_stack[STACK_SIZE];
uint8_t blinky_control_block[100]; //must be >= sizeof(StaticTask_t)
osThreadId_t blinky_thread;
uint8_t print_thread_stack[STACK_SIZE];
uint8_t print_control_block[100]; //must be >= sizeof(StaticTask_t)
osThreadId_t print_thread;

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

osMutexId_t xPrintfSemaphore;
const osMutexAttr_t printfMutexAttrib = {
    "printfMutex",                         // human readable mutex name
    osMutexRecursive | osMutexPrioInherit, // attr_bits
    NULL,                                  // memory for control block (will be dynamically allocated)
    0,                                     // size for control block
};

void threadsafe_printf_init()
{
    xPrintfSemaphore = osMutexNew(&printfMutexAttrib);
}

void threadsafe_printf_lock() { osMutexAcquire(xPrintfSemaphore, osWaitForever); }
void threadsafe_printf_unlock() { osMutexRelease(xPrintfSemaphore); }

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
        osDelay(500);
        gpio_bit_reset(LEDPORT, LEDPIN);
        osDelay(500);
        threadsafe_printf("Blinky!\n");
    }
}

void vPrintTask(void *pvParameters)
{
    for (;;)
    {
        osDelay(2000);
        threadsafe_printf("Hello from thread \"%s\" (Thread ID %p)!\n",
                          osThreadGetName(osThreadGetId()), osThreadGetId());
    }
}

int main(void)
{
    init_printf_transport();
    threadsafe_printf_init();
    init_led();

    osVersion_t ver;
    char verStr[32] = {0};
    osKernelGetInfo(&ver, verStr, sizeof(verStr));
    printf("Starting FreeRTOS + CMSIS-OS2 demo! Running on \"%s\"\n", verStr);

    /* Initialize CMSIS-RTOS */
    osKernelInitialize();
    /* create threads with a pre-allocated stacks */
    const osThreadAttr_t blinkyThreadAttr = {
        .cb_mem = blinky_control_block,
        .cb_size = sizeof(blinky_control_block),
        .name = "Blinky",
        .stack_mem = blinky_thread_stack,
        .stack_size = sizeof(blinky_thread_stack)
    };
    blinky_thread = osThreadNew(vBlinkyTask, NULL, &blinkyThreadAttr);

    const osThreadAttr_t printThreadAttr = {
        .cb_mem = print_control_block,
        .cb_size = sizeof(print_control_block),
        .name = "Printer",
        .stack_mem = print_thread_stack,
        .stack_size = sizeof(print_thread_stack)
    };
    print_thread = osThreadNew(vPrintTask, NULL, &printThreadAttr);

    /* Start the scheduler itself. */
    if (osKernelGetState() == osKernelReady)
    {
        osKernelStart(); // Start thread execution
    }
    /* never reached */
    return 0;
}