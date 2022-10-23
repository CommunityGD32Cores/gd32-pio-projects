#include "gd32w51x.h"
#include "wrapper_os.h"
#include "wifi_netlink.h"
#include "app_type.h"
#include "wifi_management.h"
#include "arm_math.h"
#include "wifi_version.h"
#include "nspe_region.h"
#include "mbl_uart.h"

#define LEDPORT     GPIOB
#define LEDPIN      GPIO_PIN_11
#define LED_CLOCK   RCU_GPIOB

void systick_config(void);
void delay_1ms(uint32_t count);

int main(void)
{
    /* Clear SRAM used by rom variables */
    memset((uint32_t *)SRAM_BASE_NS, 0, RE_MBL_DATA_START);

    /* Initialize UART for trace print */
    //log_uart_init();
    //uart_config(USART1);
    printf("GIGA DEVICE\r\n");

    /* Initialize flash for reading system info */
    //flash_init();

    platform_init();

    DEBUGPRINT("SDK git revision: "WIFI_GIT_REVISION" \r\n");
    DEBUGPRINT("SDK version: V%d.%d.%d\r\n", (RE_NSPE_VERSION >> 24), ((RE_NSPE_VERSION & 0xFF0000) >> 16), (RE_NSPE_VERSION & 0xFFFF));
    DEBUGPRINT("SDK build date: "BUILD_DATE" \r\n");
    while(1) {}
    return 0;
}