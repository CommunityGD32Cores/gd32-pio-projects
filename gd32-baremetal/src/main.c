#include "gd32f30x.h"

#define LEDPORT		GPIOC
#define LEDPIN		GPIO_PIN_13
#define LED_CLOCK	RCU_GPIOC

/* quick delay via systick implementation */
volatile static uint32_t delay;
void systick_config(void)
{
    /* setup systick timer for 1000Hz interrupts */
    if (SysTick_Config(SystemCoreClock / 1000U)){
        /* capture error */
        while (1){
        }
    }
    /* configure the systick handler priority */
    NVIC_SetPriority(SysTick_IRQn, 0x00U);
}
void delay_1ms(uint32_t count)
{
    delay = count;
    while(0U != delay){}
}

void SysTick_Handler(void)
{
    if (0U != delay){
        delay--;
    }
}

int main() {
	systick_config();
	rcu_periph_clock_enable(LED_CLOCK);
	rcu_periph_clock_enable(RCU_AF);
	gpio_init(LEDPORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_2MHZ, LEDPIN);
	/* main program loop */
	for (;;) {
		/* set led on */
		gpio_bit_set(LEDPORT, LEDPIN);
		/* delay */
		delay_1ms(500);
		/* clear led */
		gpio_bit_reset(LEDPORT, LEDPIN);
		/* delay */
		delay_1ms(500);
	}
	return 0;
}