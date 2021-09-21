#if defined(GD32F10x)
#include "gd32f10x.h"
#elif defined(GD32F1x0)
#include "gd32f1x0.h"
#elif defined (GD32F20x)
#include "gd32f20x.h"
#elif defined(GD32F3x0)
#include "gd32f3x0.h"
#elif defined(GD32F30x)
#include "gd32f30x.h"
#elif defined(GD32F4xx)
#include "gd32f4xx.h"
#elif defined(GD32F403)
#include "gd32f403.h"
#elif defined(GD32E10X)
#include "gd32e10x.h"
#elif defined(GD32E23x)
#include "gd32e23x.h"
#elif defined(GD32E50X)
#include "gd32e50x.h"
#else
#error "Unknown chip series"
#endif

/* define blinky LED pin here, board specific, otherwise default PC13 */
#ifdef GD32350G_START
/* correct LED for GD32350G-START board. PA1 */
#define LEDPORT     GPIOA
#define LEDPIN      GPIO_PIN_1
#define LED_CLOCK   RCU_GPIOA
#else
#define LEDPORT     GPIOC
#define LEDPIN      GPIO_PIN_13
#define LED_CLOCK   RCU_GPIOC
#endif

class GD32Led {
public:
    GD32Led(uint32_t led_port, uint32_t led_pin, rcu_periph_enum led_clock) {
        this->m_led_port = led_port;
        this->m_led_pin = led_pin;
        this->m_led_clock = led_clock;
        this->init();
    }

    void init() {
        rcu_periph_clock_enable(m_led_clock);

        /* set output as output */
#if defined(GD32F3x0) || defined(GD32F1x0) || defined(GD32F4xx) || defined(GD32E23x)
        gpio_mode_set(m_led_port, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, m_led_pin);
        gpio_output_options_set(m_led_port, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, m_led_pin);
#else /* valid for GD32F10x, GD32E20x, GD32F30x, GD32F403, GD32E10X */
        gpio_init(m_led_port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, m_led_pin);
#endif
    }

    GD32Led &operator= (int value)
    {
        write(value);
        return *this;
    }

    void write(int value)
    {
        gpio_bit_write(m_led_port, m_led_pin, value != 0 ? SET : RESET);
    }    

private:
    uint32_t m_led_port, m_led_pin;
    rcu_periph_enum m_led_clock;
};

void systick_config(void);
void delay_1ms(uint32_t count);

/* globally construct LED object */
GD32Led myLed(LEDPORT, LEDPIN, LED_CLOCK);

int main(void)
{
    systick_config();

    /* 
     * blink LED using C++ object. "led = <int>" will set the GPIO to the desired value
     * through operator overloading.
     */
    while (1)
    {
        myLed = 1;
        delay_1ms(500);
        myLed = 0;
        delay_1ms(500);
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

/* interrupt handlers must be declared with C linkage, otherwise
*  C++ name-mangling kicks in and doesn't define the right symbol name! */
extern "C" {

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
    delay_decrement();
}
}