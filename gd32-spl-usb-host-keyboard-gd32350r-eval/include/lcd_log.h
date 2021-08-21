/*!
    \file    lcd_log.h
    \brief   header for the lcd_log.c file

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

#ifndef __LCD_LOG_H
#define __LCD_LOG_H

#include "usb_conf.h"
#include "usb_lcd_conf.h"
#include <string.h>

extern uint32_t LCD_LineColor;

#define  LCD_ErrLog(...)    do { \
                                 LCD_LineColor = LCD_COLOR_RED;\
                                 printf("ERROR: ") ;\
                                 printf(__VA_ARGS__);\
                                 LCD_LineColor = LCD_LOG_DEFAULT_COLOR;\
                               }while (0)

#define  LCD_UsrLog(...)    do { \
                                 LCD_LineColor = LCD_LOG_TEXT_COLOR;\
                                 printf(__VA_ARGS__);\
                               } while (0)


#define  LCD_DbgLog(...)    do { \
                                 LCD_LineColor = LCD_COLOR_RED;\
                                 printf(__VA_ARGS__);\
                                 LCD_LineColor = LCD_LOG_DEFAULT_COLOR;\
                               }while (0)

#define LCD_DevInformation(...) do { \
                                     LCD_LineColor = LCD_COLOR_BLUE; \
                                     printf(__VA_ARGS__); \
                                     LCD_LineColor = LCD_LOG_DEFAULT_COLOR; \
                                   }while(0)

#if (LCD_SCROLL_ENABLED == 1)
    #define     LCD_CACHE_DEPTH     (YWINDOW_SIZE + CACHE_SIZE)
#else
    #define     LCD_CACHE_DEPTH     YWINDOW_SIZE
#endif

typedef struct _LCD_LOG_line
{
    uint8_t  line[128];
    uint32_t color;
}LCD_LOG_line;

/* function declarations */
/* initialize the LCD Log module */
void lcd_log_init (void);
/* de-initialize the LCD Log module */
void lcd_log_deinit (void);
/* display the application header (title) on the LCD screen  */
void lcd_log_header_set (uint8_t *ptitle, uint16_t start_x);
/* display the application footer (status) on the LCD screen */
void lcd_log_footer_set (uint8_t *pstatus, uint16_t start_x);
/* clear the text zone */
void lcd_log_textzone_clear (uint16_t start_x,
                             uint16_t start_y,
                             uint16_t width,
                             uint16_t height);
/* update the text area display */
void LCD_LOG_UpdateDisplay (void);

#endif /* __LCD_LOG_H */ 
