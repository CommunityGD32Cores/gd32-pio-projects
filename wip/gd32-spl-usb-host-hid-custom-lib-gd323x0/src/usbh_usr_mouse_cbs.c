/*!
    \file    usbh_usr_lcd.c
    \brief   some user routines for LCD

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

#include "usbh_usr_mouse_cbs.h"
#include "usb_conf.h"
#include <stdio.h>
#include <stdbool.h>

int16_t XLoc = 0, YLoc = 0;
int16_t PrevX = 0, PrevY = 0;

/* mouse button states. false = released, true = pressed */
#define MAX_MOUSE_BUTTONS 3
bool hid_mouse_button_states[MAX_MOUSE_BUTTONS] = {false, false, false};


/*!
    \brief      the function is to handle mouse scroll to update the mouse position on display window
    \param[in]  X: USB HID mouse X co-ordinate
    \param[in]  Y: USB HID mouse Y co-ordinate
    \param[out] none
    \retval     none
*/
void HID_MOUSE_UpdatePosition (int8_t X, int8_t Y)
{
    if ((0 != X) || (0 != Y)) {
        COMPUTE_XLOC(XLoc, X);
        COMPUTE_YLOC(YLoc, Y);

        printf("New mouse position: X=%d, Y=%d (delta %d,%d)\n", (int)XLoc, (int)YLoc, (int)X, (int)Y);

        PrevX = XLoc;
        PrevY = YLoc;
    }
}

/*!
    \brief      the function is to handle mouse button press
    \param[in]  button_idx: mouse button pressed
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HID_MOUSE_ButtonPressed(uint8_t button_idx)
{
    bool old_mouse_btn_state = false;
    if(button_idx < MAX_MOUSE_BUTTONS) {
        old_mouse_btn_state = hid_mouse_button_states[button_idx];
    }
    /* only do printing if the button was previously released */
    if(old_mouse_btn_state == false) {
        /* change the color of button pressed to indicate button press */
        switch (button_idx){
        /* left button pressed */
        case HID_MOUSE_LEFTBUTTON:
            printf("Mouse left button pressed\n");
            break;

        /* right button pressed */
        case HID_MOUSE_RIGHTBUTTON:
            printf("Mouse right button pressed\n");
            break;

        /* middle button pressed */
        case HID_MOUSE_MIDDLEBUTTON:
            printf("Mouse middle button pressed\n");
            break;

        default:
            break;
        }
        //button is now pressed.
        if(button_idx < MAX_MOUSE_BUTTONS) {
            hid_mouse_button_states[button_idx] = true;
        }        
    }
}

/*!
    \brief      the function is to handle mouse button release
    \param[in]  button_idx: mouse button released
    \param[in]  none
    \param[out] none
    \retval     none
*/
void HID_MOUSE_ButtonReleased(uint8_t button_idx)
{
    bool old_mouse_btn_state = false;
    if(button_idx < MAX_MOUSE_BUTTONS) {
        old_mouse_btn_state = hid_mouse_button_states[button_idx];
    }
    /* only do printing if the button was previously released */
    if(old_mouse_btn_state == true) {
        switch (button_idx){
        /* left button pressed */
        case HID_MOUSE_LEFTBUTTON:
            printf("Mouse left button released\n");
            break;

        /* right button pressed */
        case HID_MOUSE_RIGHTBUTTON:
            printf("Mouse right button released\n");
            break;

        /* middle button pressed */
        case HID_MOUSE_MIDDLEBUTTON:
            printf("Mouse middle button released\n");
            break;

        default:
            break;
        }
        //button is now released.
        if(button_idx < MAX_MOUSE_BUTTONS) {
            hid_mouse_button_states[button_idx] = false;
        }        
    }
}
