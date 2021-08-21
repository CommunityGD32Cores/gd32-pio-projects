/*!
    \file    usbh_usr_lcd.h
    \brief   the declarations for user routines for LCD 

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

#ifndef __USBH_USR_MOUSE_CBS_H
#define __USBH_USR_MOUSE_CBS_H

#include "usbh_hid_core.h"

/* left button: report data: 0x00*/
#define HID_MOUSE_LEFTBUTTON                0x00U

/* right button: report data: 0x01*/
#define HID_MOUSE_RIGHTBUTTON               0x01U

/* middle button: report data: 0x02*/
#define HID_MOUSE_MIDDLEBUTTON              0x02U

#define COMPUTE_XLOC(xloc, x)               (xloc) += ((x) / 2U)
#define COMPUTE_YLOC(yloc, y)               (yloc) += ((y) / 2U)

/* function declarations */
/* the function is to handle mouse scroll to update the mouse position */
void HID_MOUSE_UpdatePosition (int8_t X, int8_t Y);
/* the function is to handle mouse button press */
void HID_MOUSE_ButtonPressed (uint8_t ButtonIdx);
/* the function is to handle mouse button release */
void HID_MOUSE_ButtonReleased (uint8_t ButtonIdx);

#endif /* __USBH_USR_MOUSE_CBS_H */
