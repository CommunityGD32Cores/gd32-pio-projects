#include "wrapper_os.h"
#include "debug_print.h"
#include "bsp_inc.h"
#include "bsp_wlan.h"
#include "tcpip.h"
#include "wifi_netif.h"
#include "wifi_netlink.h"
#include "wifi_management.h"
#include "console.h"
#include "main.h"
#include "wifi_version.h"
#include "nspe_region.h"
#ifdef CONFIG_FATFS_SUPPORT
#include "fatfs.h"
#endif
/*============================ MACROS ========================================*/
/*============================ MACRO FUNCTIONS ===============================*/
/*============================ TYPES =========================================*/
/*============================ GLOBAL VARIABLES ==============================*/
/*============================ LOCAL VARIABLES ===============================*/
/*============================ PROTOTYPES ====================================*/
/*============================ IMPLEMENTATION ================================*/

/*!
    \brief      initialize application
    \param[in]  none
    \param[out] none
    \retval     none
*/
__WEAK void app_init(void)
{
    /* User application entry */
}

/*!
    \brief      start task
    \param[in]  p_arg: the pointer of parameters
    \param[out] none
    \retval     none
*/
void start_task(void *p_arg)
{
    (void)p_arg;

    sys_reset_flag_check();
    sys_os_misc_init();
    systick_delay_init();

    wifi_management_init();
#ifdef CONFIG_FATFS_SUPPORT
    rcu_periph_clock_enable(RCU_CRC);
    fatfs_mk_mount();
#endif
#ifdef CONFIG_CONSOLE_ENABLE
    console_init();
#else
    app_init();
#endif

    sys_task_delete(NULL);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    platform_init();

    DEBUGPRINT("SDK git revision: "WIFI_GIT_REVISION" \r\n");
    DEBUGPRINT("SDK version: V%d.%d.%d\r\n", (RE_NSPE_VERSION >> 24), ((RE_NSPE_VERSION & 0xFF0000) >> 16), (RE_NSPE_VERSION & 0xFFFF));
    DEBUGPRINT("SDK build date: "BUILD_DATE" \r\n");

    sys_os_init();

    if (NULL == sys_task_create(NULL, (const uint8_t *)"start_task", NULL, START_TASK_STK_SIZE, 0,
            START_TASK_PRIO, start_task, NULL)) {
        DEBUGPRINT("ERROR: create start task failed\r\n");
    }

    sys_os_start();
}
