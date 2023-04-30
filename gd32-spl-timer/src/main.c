#include "gd32f30x.h"

#define LEDPORT     GPIOC
#define LEDPIN      GPIO_PIN_13
#define LED_CLOCK   RCU_GPIOC

uint32_t getTimerClkFrequency(uint32_t instance)
{
    rcu_clock_freq_enum timerclkSrc = 0;
    uint32_t APBx_PSC = 0;
    uint32_t clk_src = 0;
    if (instance != (uint32_t) 0) {
        switch ((uint32_t)instance) {
            case (uint32_t)TIMER0:
            case (uint32_t)TIMER7:
            case (uint32_t)TIMER8:
            case (uint32_t)TIMER9:
            case (uint32_t)TIMER10:
                timerclkSrc = CK_APB2;
                APBx_PSC = (RCU_CFG0 & RCU_CFG0_APB2PSC) >> 11;
                break;
            case (uint32_t)TIMER1:
            case (uint32_t)TIMER2:
            case (uint32_t)TIMER3:
            case (uint32_t)TIMER4:
            case (uint32_t)TIMER5:
            case (uint32_t)TIMER6:
            case (uint32_t)TIMER11:
            case (uint32_t)TIMER12:
            case (uint32_t)TIMER13:
                timerclkSrc = CK_APB1;
                APBx_PSC = (RCU_CFG0 & RCU_CFG0_APB1PSC) >> 8;
                break;
            default:
                break;
        }
    }
    if (0 != (APBx_PSC & 0x04)) {
        clk_src = 2 * rcu_clock_freq_get(timerclkSrc);
    } else {
        clk_src =  rcu_clock_freq_get(timerclkSrc);
    }
    return clk_src;
}

void config_timer(uint32_t timer_periph) {
    nvic_irq_enable(TIMER2_IRQn, 2, 2); // hardcode enable Timer2 interrupt in NVIC
    /* enable clock input for Timer2 peirpheral */
    rcu_periph_clock_enable(RCU_AF);
    rcu_periph_clock_enable(RCU_TIMER2);
    /* configure TIMER base function */
    timer_parameter_struct timer_initpara;
    uint16_t every_x_milliseconds = 200;
    timer_initpara.prescaler = getTimerClkFrequency(timer_periph) / 10000 - 1;
    timer_initpara.period = every_x_milliseconds * 10 - 1;
    timer_initpara.repetitioncounter = 0;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_autoreload_value_config(timer_periph, 0);
    timer_init(timer_periph, &timer_initpara);
    /* enable timer update interrupt */ 
    /* happens when timer is overflowing (period / reload value exceeded) and it is set back to 0 again */
    timer_interrupt_flag_clear(timer_periph, TIMER_INT_FLAG_UP);
    timer_interrupt_enable(timer_periph, TIMER_INT_UP);
    timer_enable(timer_periph);
}

int main(void)
{
    /* enable GPIO port */
    rcu_periph_clock_enable(LED_CLOCK);
    gpio_init(LEDPORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, LEDPIN);
    config_timer((uint32_t)TIMER2);
    while (1)
    {
        __WFI(); // wait for interrupt. not needed but debugging is easier :)
    }
}

uint8_t state = 0;
void TIMER2_IRQHandler(void)
{
    // clear interrupt request to enable next run
    if (timer_interrupt_flag_get(TIMER2, TIMER_INT_FLAG_UP) != RESET) {
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_FLAG_UP);
        /* our own reaction to timer update interrupt triggering: invert GPIO pin, blinky blink */
        gpio_bit_write(LEDPORT, LEDPIN, state);
        state ^= 1; // invert for next run
    }
}

void NMI_Handler(void) {}

void HardFault_Handler(void)
{
    while (1)
        ;
}

void MemManage_Handler(void)
{
    while (1)
        ;
}

void BusFault_Handler(void)
{
    while (1)
        ;
}

void UsageFault_Handler(void)
{
    while (1)
        ;
}

void SVC_Handler(void)
{
}

void DebugMon_Handler(void)
{
}

void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
}
