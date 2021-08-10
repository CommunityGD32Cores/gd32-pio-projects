#include <stdio.h>
#if defined(GD32F10x)
#include "gd32f10x.h"
#elif defined(GD32F1x0)
#include "gd32f1x0.h"
#elif defined(GD32F3x0)
#include <gd32f3x0.h>
#elif defined(GD32F30x)
#include "gd32f30x.h"
#elif defined(GD32F40x)
#include "gd32f4xx.h"
#elif defined(GD32E10X)
#include "gd32e10x.h"
#endif


void delay_1ms(uint32_t count);
extern void initialise_monitor_handles(void);

int main(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U))
    {
        /* capture error */
        while (1) {}
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);

    initialise_monitor_handles();

    while(1)
    {
        printf("Hello, world!\n");
        printf("This message should be delivered in the debug console window via semihosting,\n");
        printf("which only needs a SWD capable debug probe instead of a UART adapter.\n");
        printf("Some test values: %d, %p, \"%s\"\n", 123, (void*) 0x456, "abc");
        delay_1ms(500);
    }
}

volatile static uint32_t delay = 0;

void delay_1ms(uint32_t count)
{
    delay = count;

    while (0U != delay)
    {
    }
}

void SysTick_Handler(void)
{
    if (0U != delay)
    {
        delay--;
    }
}