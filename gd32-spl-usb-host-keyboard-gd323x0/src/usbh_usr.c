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

    printf("VID: %04Xh\n", (unsigned)pDevStr->idVendor);
    printf("PID: %04Xh\n", (unsigned)pDevStr->idProduct);
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
    usbh_user_status usbh_usr_status = USBH_USER_NO_RESP;

    /*Key USER is in polling mode to detect user action */
    if(RESET == gd_eval_key_state_get(KEY_USER)){
        usbh_usr_status = USBH_USER_RESP_OK;
    }

    return usbh_usr_status;
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
#include <stdbool.h>
static hid_keybd_info oldKeybdState;

void usr_keybrd_process_data_full(hid_keybd_info* k_pinfo) {
    //if new state is not old state
    if(memcmp(&oldKeybdState, k_pinfo, sizeof(hid_keybd_info)) != 0) {

        printf("Pressed Keys:");
        //print keys
        bool atleast_one_key_pressed = false;
        for(int i=0; i < sizeof(k_pinfo->keys) / sizeof(k_pinfo->keys[0]); i++) {
            if(k_pinfo->keys[i] != KEY_NONE) {
                printf(" %d", (int) k_pinfo->keys[i]);
                atleast_one_key_pressed = true;
            }
        }
        if(!atleast_one_key_pressed) {
            printf(" (all released)");
        }
        printf("\n");
        //check for modifier keys
        const char* modifier_names[] = {
            "state", "lctrl", "lshift",
            "lalt", "lgui", "rctrl",
            "rshift", "ralt", "rgui"
        };
        uint8_t modifier_vals[] = {
            k_pinfo->state, k_pinfo->lctrl, k_pinfo->lshift,
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
    printf("> Keyboard pressed: '%c' (%02x)\n", (char)data, (int)data);
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
