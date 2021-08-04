#include "gd32_includes.h"
#include "systick.h"

/* standard on LED2, else fallback to LED1 for boards which don't have it */
#if defined(LED2_PIN)
#define BLINKY_LED LED2
#else
#define BLINKY_LED LED1
#endif

int main(void)
{
    systick_config();

    gd_eval_led_init(BLINKY_LED);

    while (1)
    {
        gd_eval_led_toggle(BLINKY_LED);
        delay_1ms(200);
    }
}
