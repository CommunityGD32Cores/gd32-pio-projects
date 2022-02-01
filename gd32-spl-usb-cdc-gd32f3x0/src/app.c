/*!
    \file    app.c
    \brief   USB main routine for CDC device

    \version 2020-08-13, V3.0.0, demo for GD32F3x0
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc. 

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "drv_usb_hw.h"
#include "cdc_acm_core.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

usb_core_driver cdc_acm;

void USB_WaitDataTransmitted(usb_dev *udev) {
    //Tricky.. no status back from CDC handler or doesn't seem to be updating .. :/
    //This won't work properly without flushing the data -- buffer will be overwritten occassionally
    /*usb_cdc_handler* cdc = (usb_cdc_handler*)(udev)->dev.class_data[CDC_COM_INTERFACE];
    while(cdc->packet_sent != 1) {
        usb_mdelay(1);
    }*/
}

void USB_CDC_SendStr(usb_dev *udev, const char* str, size_t len) {
    usb_cdc_handler* cdc = (usb_cdc_handler*)(udev)->dev.class_data[CDC_COM_INTERFACE];
    //respect max 64 bytes buffer length of cdc->data[USB_CDC_RX_LEN], USB_CDC_RX_LEN = 64
    //this **truncates** data -- it does not transmit the rest if it goes over 64 bytes. 
    //to be improved with a buffer or whatever.
    size_t send_len = len >= sizeof(cdc->data) ? sizeof(cdc->data) : len; 
    //CDC driver is weird, only meant for echoing data back directly.
    //we have to write into the data buffer and set the "receive_length" to the length it's supposed
    //to send.
    //also, destroys previously sent data
    memcpy(cdc->data, str, send_len);
    cdc->receive_length = send_len;
    //trigger sending
    cdc_acm_data_send(&cdc_acm);
    //blocking wait 
    USB_WaitDataTransmitted(udev);
}

//could also be 
void USB_printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    //variable-length buffer of the required size
    char buf[vsnprintf(NULL, 0, fmt, args) + 1];
    int ret_len = vsnprintf(buf, sizeof buf, fmt, args);
    if(ret_len > 0) {
        USB_CDC_SendStr(&cdc_acm, (char*) buf, (size_t) ret_len);
    }
    va_end(args);
}

/* so that printf() goes via USB-CDC */
/* can also be written to use USART instead, see https://github.com/CommunityGD32Cores/gd32-pio-projects/blob/main/gd32-spl-usb-cdc-gd32f30x/src/printf_over_x.c */
#include <errno.h>
#include <sys/unistd.h> // STDOUT_FILENO, STDERR_FILENO
int _write(int file, char *data, int len)
{
    if ((file != STDOUT_FILENO) && (file != STDERR_FILENO))
    {
        errno = EBADF;
        return -1;
    }

    USB_CDC_SendStr(&cdc_acm, data, (size_t) len);
    USB_WaitDataTransmitted(&cdc_acm);

    // return # of bytes written - as best we can tell
    return len;
}

/*!
    \brief      main routine will construct a USB CDC device
    \param[in]  none
    \param[out] none
    \retval     none
*/
int main(void)
{
    usb_rcu_config();

    usb_timer_init();

    usbd_init (&cdc_acm, USB_CORE_ENUM_FS, &cdc_desc, &cdc_class);

    usb_intr_config();

#ifdef USE_IRC48M
    /* CTC peripheral clock enable */
    rcu_periph_clock_enable(RCU_CTC);

    /* CTC config */
    ctc_config();

    while (RESET == ctc_flag_get(CTC_FLAG_CKOK)) {
    }
#endif /* USE_IRC48M */

    /* main loop */
    while (1) {
        if (USBD_CONFIGURED == cdc_acm.dev.cur_status) {
            USB_printf("Testing 1: %d, 0x%08x\n", 123, 0x1234);
            usb_mdelay(500);
            printf("Testing 2: \"%s\"\n", "This is a test");
            usb_mdelay(500);
        }
    }
}
