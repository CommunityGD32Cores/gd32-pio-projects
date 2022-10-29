#include "gd32w51x.h"
#include "wrapper_os.h"
#include "wifi_netlink.h"
#include "app_type.h"
#include "wifi_management.h"
#include "arm_math.h"
#include "wifi_version.h"
#include "nspe_region.h"
#include "mbl_uart.h"
#include "console.h"

void start_task(void *p_arg) {
    sys_reset_flag_check();
    systick_delay_init();
    sys_os_misc_init();
    wifi_management_init();
    console_init();
    // everything else happens inside console task
    sys_task_delete(NULL);
}

int main(void)
{
    platform_init();

    DEBUGPRINT("SDK git revision: "WIFI_GIT_REVISION" \r\n");
    DEBUGPRINT("SDK version: V%d.%d.%d\r\n", (RE_NSPE_VERSION >> 24), ((RE_NSPE_VERSION & 0xFF0000) >> 16), (RE_NSPE_VERSION & 0xFFFF));
    DEBUGPRINT("SDK build date: " __DATE__ " " __TIME__" \r\n");

    sys_os_init();
    if (NULL == sys_task_create(NULL, (const uint8_t *)"start_task", NULL, START_TASK_STK_SIZE, 0,
            START_TASK_PRIO, start_task, NULL)) {
        DEBUGPRINT("ERROR: create start task failed\r\n");
    }
    sys_os_start();
    while(1);
    return 0;
}