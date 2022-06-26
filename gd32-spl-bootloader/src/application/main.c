#include <gd32_include.h>
#include <printf_over_x.h>
#include <stdio.h>

void delay_1ms(uint32_t count);
void systick_config(void);

int main(void)
{
    init_printf_transport();
    systick_config();

    printf("==== Application start ====\n");

    int i = 0;
    while (1)
    {
        printf("** Application is alive (%d) **\n", i++);
        delay_1ms(1000);
    }
}

volatile static uint32_t delay;

void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U))
    {
        /* capture error */
        while (1)
        {
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}

void delay_1ms(uint32_t count)
{
    delay = count;

    while (0U != delay)
    {
    }
}

void delay_decrement(void)
{
    if (0U != delay)
    {
        delay--;
    }
}

void SysTick_Handler(void)
{
    delay_decrement();
}
