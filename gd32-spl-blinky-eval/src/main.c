#include "gd32_includes.h"
#include "systick.h"



int main(void)
{
    systick_config();

    gd_eval_led_init(LED2);

    while (1)
    {
        gd_eval_led_toggle(LED2);
        delay_1ms(200);
    }
}
