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

#include "usbh_usr_lcd.h"
#include "usb_conf.h"
#include "lcd_log.h"

int16_t XLoc = 0, YLoc = 0;
int16_t PrevX = 0, PrevY = 0;

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

        if(YLoc > MOUSE_WINDOW_Y_MAX - 16) {
            YLoc = MOUSE_WINDOW_Y_MAX - 16;
        }

        if(XLoc > MOUSE_WINDOW_X_MAX - 8) {
            XLoc = MOUSE_WINDOW_X_MAX - 8;
        }

        if(YLoc < Y_MIN) {
            YLoc = Y_MIN;
        }

        if(XLoc < X_MIN) {
            XLoc = X_MIN;
        }

        lcd_text_color_set(LCD_COLOR_WHITE);
        lcd_background_color_set(LCD_COLOR_WHITE);
        lcd_vertical_char_display(MPL_Y(PrevY), MPL_X(PrevX), 'x');

        lcd_text_color_set(LCD_COLOR_BLACK);
        lcd_background_color_set(LCD_COLOR_WHITE);
        lcd_vertical_char_display(MPL_Y(YLoc), MPL_X(XLoc), 'x');

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
    /* change the color of button pressed to indicate button press */
    switch (button_idx){
    /* left button pressed */
    case HID_MOUSE_LEFTBUTTON:
        lcd_text_color_set(LCD_COLOR_MAGENTA);
        lcd_rectangle_fill(MOUSE_BUTTON_XCHORD, 
                           MOUSE_BUTTON2_YCHORD, 
                           MOUSE_BUTTON_WIDTH, 
                           MOUSE_BUTTON_HEIGHT);
        break;

    /* right button pressed */
    case HID_MOUSE_RIGHTBUTTON:
        lcd_text_color_set(LCD_COLOR_MAGENTA);
        lcd_rectangle_fill(MOUSE_BUTTON_XCHORD, 
                           MOUSE_BUTTON1_YCHORD, 
                           MOUSE_BUTTON_WIDTH, 
                           MOUSE_BUTTON_HEIGHT);
        break;

    /* middle button pressed */
    case HID_MOUSE_MIDDLEBUTTON:
        lcd_text_color_set(LCD_COLOR_MAGENTA);
        lcd_rectangle_fill(MOUSE_BUTTON_XCHORD, 
                           MOUSE_BUTTON3_YCHORD, 
                           MOUSE_BUTTON_WIDTH, 
                           MOUSE_BUTTON_HEIGHT);
        break;

    default:
        break;
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
    /* change the color of button released to default button color*/
    switch (button_idx){
    /* left button released */
    case HID_MOUSE_LEFTBUTTON:
        lcd_text_color_set(LCD_COLOR_GREY);
        lcd_rectangle_fill(MOUSE_BUTTON_XCHORD, 
                           MOUSE_BUTTON2_YCHORD, 
                           MOUSE_BUTTON_WIDTH, 
                           MOUSE_BUTTON_HEIGHT);
        break;

    /* right button released */  
    case HID_MOUSE_RIGHTBUTTON:
        lcd_text_color_set(LCD_COLOR_GREY);
        lcd_rectangle_fill(MOUSE_BUTTON_XCHORD, 
                           MOUSE_BUTTON1_YCHORD, 
                           MOUSE_BUTTON_WIDTH, 
                           MOUSE_BUTTON_HEIGHT);
        break;

    /* middle button released */  
    case HID_MOUSE_MIDDLEBUTTON:
        lcd_text_color_set(LCD_COLOR_GREY);
        lcd_rectangle_fill(MOUSE_BUTTON_XCHORD, 
                           MOUSE_BUTTON3_YCHORD, 
                           MOUSE_BUTTON_WIDTH, 
                           MOUSE_BUTTON_HEIGHT);
        break;

    default:
        break;
    }
}
