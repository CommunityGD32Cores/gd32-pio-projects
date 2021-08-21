/*!
    \file    usb_lcd_conf.h
    \brief   LCD driver configuration

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

#ifndef __USB_LCD_CONF_H
#define __USB_LCD_CONF_H

#include <stdio.h>
#include "usb_conf.h"

#define LCD_LOG_TEXT_COLOR            LCD_COLOR_WHITE
#define LCD_LOG_DEFAULT_COLOR         LCD_COLOR_WHITE

#define YWINDOW_MIN                   3U
#define YWINDOW_SIZE                  11U

#define LCD_HEADER_X                  0U
#define LCD_HEADER_Y                  0U

#define LCD_FLAG_WIDTH                40U
#define LCD_FLAG_HEIGHT               240U

#define LCD_FOOTER_X                  280U
#define LCD_FOOTER_Y                  0U

#define LCD_TEXT_ZONE_X               LCD_FLAG_WIDTH
#define LCD_TEXT_ZONE_Y               0U
#define LCD_TEXT_ZONE_WIDTH           LCD_PIXEL_HEIGHT
#define LCD_TEXT_ZONE_HEIGHT          LCD_PIXEL_WIDTH - 2 * LCD_FLAG_WIDTH

#define LCD_TEXT_OFFSET               10U

#define LCD_HEADER_LINE               LCD_HEADER_Y + LCD_TEXT_OFFSET
#define LCD_FOOTER_LINE               LCD_FOOTER_X + LCD_TEXT_OFFSET

#define LCD_HINT_LINE0                230U
#define LCD_HINT_LINE1                240U
#define LCD_HINT_LINE2                260U

/* Redirect the printf to the LCD */
#ifdef __GNUC__
    /* With GCC, small printf (option LD Linker->Libraries->Small printf
       set to 'Yes') calls __io_putchar() */
    #define LCD_LOG_PUTCHAR int __io_putchar(int ch)
#else
    #define LCD_LOG_PUTCHAR int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#endif /* __USB_LCD_CONF_H */
