/*!
    \file    usbh_usr.c
    \brief   some user routines

    \version 2020-08-13, V3.0.0, firmware for GD32F3x0
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

#include "usbh_usr.h"
#include "usbh_hid_mouse.h"
#include "usbh_hid_keybd.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <qwertz_keyboard_decoding.h>
#include "drv_usb_hw.h" /* only for usb_mdelay */

typedef enum {
    DISPLAY_KEY_INFO,
    WRITE_TEXT
} ApplicationMode;

ApplicationMode curAppMode = DISPLAY_KEY_INFO;

extern int16_t XLoc, YLoc;
extern __IO int16_t PrevX, PrevY;

extern usb_core_driver usb_hid_core;

/* Points to the DEVICE_PROP structure of current device */
usbh_user_cb usr_cb =
{
    usbh_user_init,
    usbh_user_deinit,
    usbh_user_device_connected,
    usbh_user_device_reset,
    usbh_user_device_disconnected,
    usbh_user_over_current_detected,
    usbh_user_device_speed_detected,
    usbh_user_device_desc_available,
    usbh_user_device_address_assigned,
    usbh_user_configuration_descavailable,
    usbh_user_manufacturer_string,
    usbh_user_product_string,
    usbh_user_serialnum_string,
    usbh_user_enumeration_finish,
    usbh_user_userinput,
    NULL,
    usbh_user_device_not_supported,
    usbh_user_unrecovered_error
};

const uint8_t MSG_HOST_HEADER[] = "USB OTG FS HID Host";
const uint8_t MSG_HOST_FOOTER[] = "USB Host Library v2.1.0";

/*!
    \brief      user operation for host-mode initialization
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_init(void)
{
    static uint8_t startup = 0U;

    if(0U == startup){
        startup = 1U;

        /* configure the User key */
        gd_eval_key_init(KEY_USER, KEY_MODE_GPIO);

        printf("USB host library started\n");
    }
}

/*!
    \brief      user operation for device attached
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_connected(void)
{
    printf("> Device Attached.\n");
}

/*!
    \brief      user operation for unrecovered error happens
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_unrecovered_error (void)
{
    fprintf(stderr, "> UNRECOVERED ERROR STATE.\n");
}

/*!
    \brief      user operation for device disconnect event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_disconnected (void)
{
    printf("> Device Disconnected.\n");
}

/*!
    \brief      user operation for reset USB Device
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_reset(void)
{
    /* users can do their application actions here for the USB-Reset */
    printf("> Reset the USB device.\n");
}

/*!
    \brief      user operation for detecting device speed
    \param[in]  DeviceSpeed: device speed
    \param[out] none
    \retval     none
*/
void usbh_user_device_speed_detected(uint32_t device_speed)
{
    if (PORT_SPEED_HIGH == device_speed) {
        printf("> High speed device detected.\r\n");
    } else if(PORT_SPEED_FULL == device_speed) {
        printf("> Full speed device detected.\r\n");
    } else if(PORT_SPEED_LOW == device_speed) {
        printf("> Low speed device detected.\r\n");
    } else {
        fprintf(stderr, "> Device Fault.\r\n");
    }
}

/*!
    \brief      user operation when device descriptor is available
    \param[in]  DeviceDesc: device descriptor
    \param[out] none
    \retval     none
*/
void usbh_user_device_desc_available(void *device_desc)
{
    usb_desc_dev *pDevStr = device_desc;

    printf("VID: 0x%04X\n", (unsigned)pDevStr->idVendor);
    printf("PID: 0x%04X\n", (unsigned)pDevStr->idProduct);
}

/*!
    \brief      USB device is successfully assigned the address
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_address_assigned(void)
{
}

/*!
    \brief      user operation when configuration descriptor is available
    \param[in]  cfgDesc: pointer to configuration descriptor
    \param[in]  itfDesc: pointer to interface descriptor
    \param[in]  epDesc: pointer to endpoint descriptor
    \param[out] none
    \retval     none
*/
void usbh_user_configuration_descavailable(usb_desc_config *cfg_desc,
                                           usb_desc_itf *itf_desc,
                                           usb_desc_ep *ep_desc)
{
    usb_desc_itf *id = itf_desc;

    if (0x08U == (*id).bInterfaceClass) {
        printf("> Mass storage device connected.\n");
    } else if (0x03U == (*id).bInterfaceClass) {
        printf("> HID device connected.\n");
    }
}

/*!
    \brief      user operation when manufacturer string exists
    \param[in]  ManufacturerString: manufacturer string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_manufacturer_string(void *manufacturer_string)
{
    printf("Manufacturer: %s\r\n", (char *)manufacturer_string);
}

/*!
    \brief      user operation when manufacturer string exists
    \param[in]  ProductString: product string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_product_string(void *product_string)
{
    printf("Product: %s\r\n", (char *)product_string);
}

/*!
    \brief      user operation when serialnum string exists
    \param[in]  SerialNumString: serialNum string of usb device
    \param[out] none
    \retval     none
*/
void usbh_user_serialnum_string(void *serial_num_string)
{
    printf("Serial Number: %s\r\n", (char *)serial_num_string);
}

/*!
    \brief      user response request is displayed to ask for application jump to class
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_enumeration_finish(void)
{
    printf("> Enumeration completed.\n");

    printf("---------------------------------------\n");
    printf("To start the HID class operations:  \n");
    printf("Press User Key...             \n");
}

/*!
    \brief      user operation when device is not supported
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_device_not_supported(void)
{
    fprintf(stderr, "> Device not supported.\n");
}

/*!
    \brief      user action for application state entry
    \param[in]  none
    \param[out] none
    \retval     user response for user key
*/
usbh_user_status usbh_user_userinput(void)
{
    //always automatically start the demo
    //as soon as the device is connected.
    return USBH_USER_RESP_OK;
}

uint8_t old_state = RESET; 
void poll_user_key() {
    /*Key USER is in polling mode to detect user action */
    uint8_t new_state = gd_eval_key_state_get(KEY_USER);
    if(SET == new_state && old_state != new_state){
        usb_mdelay(100);
        if(SET == gd_eval_key_state_get(KEY_USER)){
            //switch app mode 
            curAppMode = curAppMode == DISPLAY_KEY_INFO 
                        ? WRITE_TEXT : DISPLAY_KEY_INFO;
            if(curAppMode == WRITE_TEXT) {
                printf("\n== ENTERED WRITE TEXT MODE ==\n");
                printf("> Start typing on the keyboard\n");
                printf("> And the text shall be saved and displayed.\n");
            } else {
                printf("\n== ENTERED DISPLAY KEY INFO MODE ==\n");
                printf("> Start typing on the keyboard\n");
                printf("> And info about the received scancodes and symbols\n");
                printf("> shall be displayed.\n");
            }
        }
    } 
    old_state = new_state;
}

/*!
    \brief      user action for device overcurrent detection event
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_over_current_detected (void)
{
    fprintf(stderr, "> Overcurrent detected.\n");
}

/*!
    \brief      init mouse window
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usr_mouse_init (void)
{
    printf("> HID Demo Device : Mouse.\n");

    HID_MOUSE_ButtonReleased(0U);
    HID_MOUSE_ButtonReleased(1U);
    HID_MOUSE_ButtonReleased(2U);

    XLoc  = 0U;
    YLoc  = 0U; 
    PrevX = 0U;
    PrevY = 0U;
}

/*!
    \brief      process mouse data
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usr_mouse_process_data (hid_mouse_info *data)
{
    //Bugfix from GD code: Use || instead of && to trigger 
    //update function in case either the X or Y delta is not 0
    if ((0U != data->x) || (0U != data->y)) {
        HID_MOUSE_UpdatePosition(data->x, data->y);
    }

    for (uint8_t index = 0U; index < 3U; index++) {
        if (data->buttons[index]) {
            HID_MOUSE_ButtonPressed(index);
        } else {
            HID_MOUSE_ButtonReleased(index);
        }
    }
}

/*!
    \brief      init keyboard window
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usr_keybrd_init (void)
{
    printf("> HID Demo Device : Keyboard.\n");
    printf("> Use Keyboard to type characters: \n");

}

/* callback for full data (made possible by a library modification) */
#ifdef PIO_USB_HOST_HID_FULL_STATE_INFO_CALLBACK
static hid_keybd_info oldKeybdState;
//1 kilobyte of write buffer
#define WRITE_BUF_LEN 1024
static char write_buf[WRITE_BUF_LEN + 1];
static int cur_write_index = 0;

void usr_keybrd_process_data_full(hid_keybd_info* k_pinfo) {
    //if new state is not old state
    if(memcmp(&oldKeybdState, k_pinfo, sizeof(hid_keybd_info)) != 0) {

        if(curAppMode == WRITE_TEXT) {
            oldKeybdState = *k_pinfo;
            for(int i=0; i < sizeof(k_pinfo->keys) / sizeof(k_pinfo->keys[0]); i++) {
                uint8_t scancode = k_pinfo->keys[i]; 
                if(scancode != KEY_NONE) {
                    bool err = false;
                    const char* szSymbol = get_str_for_scancode_qwertz(scancode, k_pinfo->lshift != 0, k_pinfo->ralt != 0, &err);
                    if(scancode == KEY_ENTER) {
                        szSymbol = "\r\n";
                        err = false;
                    } else if(scancode == KEY_SPACEBAR) {
                        szSymbol = " ";
                        err = false;
                    } else if(scancode == KEY_BACKSPACE) {
                        if(cur_write_index > 0) {
                            write_buf[cur_write_index - 1] = '\0';
                            cur_write_index--;
                        }
                        continue;
                    }
                    if(!err) {
                        //write symbol into buffer
                        //may be multiple bytes due to encoding of special chars 
                        size_t num_to_write = strlen(szSymbol);
                        size_t num_free = WRITE_BUF_LEN - cur_write_index;
                        if(num_free >= num_to_write) {
                            memcpy(write_buf + cur_write_index, szSymbol, num_to_write);
                            cur_write_index += num_to_write;
                        } else {
                            printf("\nBUFFER FULL! Clearing.\n");
                            memset(write_buf, 0, sizeof(write_buf));
                            cur_write_index = 0;
                        }                        
                    } else {
                        printf(" <unknown>");
                    }
                }
            }
            printf("Write buffer (%d bytes):\n%s\n", cur_write_index, write_buf);
        } else {
            //quick check if all keys are released
            bool atleast_one_key_pressed = false;
            for(int i=0; i < sizeof(k_pinfo->keys) / sizeof(k_pinfo->keys[0]); i++) {
                if(k_pinfo->keys[i] != KEY_NONE) {
                    atleast_one_key_pressed = true;
                }
            }
            atleast_one_key_pressed |= k_pinfo->lctrl ||k_pinfo->lshift ||k_pinfo->lalt ||k_pinfo->lgui 
                                        || k_pinfo->rctrl || k_pinfo->rshift || k_pinfo->ralt || k_pinfo->rgui;
            if(!atleast_one_key_pressed) {
                printf("< all keys released >\n");
                oldKeybdState = *k_pinfo;
                return;
            }

            //print keys
            printf("Pressed Keys (scancodes):");
            atleast_one_key_pressed = false;
            for(int i=0; i < sizeof(k_pinfo->keys) / sizeof(k_pinfo->keys[0]); i++) {
                if(k_pinfo->keys[i] != KEY_NONE) {
                    printf(" %d", (int) k_pinfo->keys[i]);
                    atleast_one_key_pressed = true;
                }
            }
            printf("\n");

            printf("Pressed Keys (symbol):");
            atleast_one_key_pressed = false;
            for(int i=0; i < sizeof(k_pinfo->keys) / sizeof(k_pinfo->keys[0]); i++) {
                if(k_pinfo->keys[i] != KEY_NONE) {
                    bool err = false;
                    const char* szSymbol = get_str_for_scancode_qwertz(k_pinfo->keys[i], k_pinfo->lshift != 0, k_pinfo->ralt != 0, &err);
                    if(!err) {
                        printf(" %s", szSymbol);
                    } else {
                        printf(" <unknown>");
                    }
                    atleast_one_key_pressed = true;
                }
            }
            printf("\n");


            //check for modifier keys
            const char* modifier_names[] = {
                "lctrl", "lshift",
                "lalt", "lgui", "rctrl",
                "rshift", "ralt", "rgui"
            };
            uint8_t modifier_vals[] = {
                k_pinfo->lctrl, k_pinfo->lshift,
                k_pinfo->lalt, k_pinfo->lgui, k_pinfo->rctrl,
                k_pinfo->rshift, k_pinfo->ralt, k_pinfo->rgui
            };
            printf("Modifier Keys:");
            for(int i=0; i < sizeof(modifier_names) / sizeof(*modifier_names); i++) {
                if(modifier_vals[i] != 0) {
                    printf(" %s", modifier_names[i]);
                }
            }
            printf("\n");
        
            oldKeybdState = *k_pinfo;
        }
    }
}
#else 
/* old behavior */
/*!
    \brief      process keyboard data
    \param[in]  data: keyboard data to be displayed
    \param[out] none
    \retval     none
*/
void usr_keybrd_process_data (uint8_t data)
{
    printf("> Keyboard pressed (assuming QWERTY): '%c' (0x%02x)\n", (char)data, (int)data);
}
#endif

/*!
    \brief      de-init user state and associated variables
    \param[in]  none
    \param[out] none
    \retval     none
*/
void usbh_user_deinit(void)
{
}
