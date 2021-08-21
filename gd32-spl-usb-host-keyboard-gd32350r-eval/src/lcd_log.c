/*!
    \file    lcd_log.c
    \brief   this file provides all the LCD Log firmware functions

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

#include "lcd_log.h"
#include "usb_lcd_conf.h"
#include <stdio.h>

LCD_LOG_line LCD_CacheBuffer[LCD_CACHE_DEPTH]; 
uint32_t LCD_LineColor;
uint16_t LCD_CacheBuffer_xptr;
uint16_t LCD_CacheBuffer_yptr_top;
uint16_t LCD_CacheBuffer_yptr_bottom;

uint16_t LCD_CacheBuffer_yptr_top_bak;
uint16_t LCD_CacheBuffer_yptr_bottom_bak;

ControlStatus LCD_CacheBuffer_yptr_invert;
ControlStatus LCD_ScrollActive;
ControlStatus LCD_Lock;
ControlStatus LCD_Scrolled;
uint16_t LCD_ScrollBackStep;

/*!
    \brief      initialize the LCD Log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_init (void)
{
    lcd_log_deinit();

    lcd_clear(LCD_COLOR_BLACK);
}

/*!
    \brief      de-initialize the LCD Log module
    \param[in]  none
    \param[out] none
    \retval     none
*/
void lcd_log_deinit (void)
{
    LCD_LineColor = LCD_LOG_TEXT_COLOR;
    LCD_CacheBuffer_xptr = 0U;
    LCD_CacheBuffer_yptr_top = 0U;
    LCD_CacheBuffer_yptr_bottom = 0U;

    LCD_CacheBuffer_yptr_top_bak = 0U;
    LCD_CacheBuffer_yptr_bottom_bak = 0U;

    LCD_CacheBuffer_yptr_invert= ENABLE;
    LCD_ScrollActive = DISABLE;
    LCD_Lock = DISABLE;
    LCD_Scrolled = DISABLE;
    LCD_ScrollBackStep = 0U;
}

/*!
    \brief      display the application header (title) on the LCD screen 
    \param[in]  ptitle: pointer to the string to be displayed
    \param[in]  start_x: x location
    \param[out] none
    \retval     none
*/
void lcd_log_header_set (uint8_t *ptitle, uint16_t start_x)
{
    lcd_font_set(&font8x16);

    lcd_text_color_set(LCD_COLOR_BLUE);

    lcd_rectangle_fill(LCD_HEADER_X, LCD_HEADER_Y, LCD_FLAG_WIDTH, LCD_FLAG_HEIGHT);

    lcd_background_color_set(LCD_COLOR_BLUE);
    lcd_text_color_set(LCD_COLOR_RED);

    lcd_vertical_string_display(LCD_HEADER_LINE, start_x, ptitle);

    lcd_background_color_set(LCD_COLOR_BLACK);
}

/*!
    \brief      display the application footer (status) on the LCD screen
    \param[in]  pstatus: pointer to the string to be displayed
    \param[in]  start_x: x location
    \param[out] none
    \retval     none
*/
void lcd_log_footer_set (uint8_t *pstatus, uint16_t start_x)
{
    lcd_text_color_set(LCD_COLOR_BLUE);

    lcd_rectangle_fill(LCD_FOOTER_X, LCD_FOOTER_Y, LCD_FLAG_WIDTH, LCD_FLAG_HEIGHT);

    lcd_background_color_set(LCD_COLOR_BLUE);
    lcd_text_color_set(LCD_COLOR_RED);

    lcd_vertical_string_display(LCD_FOOTER_LINE, start_x, pstatus);

    lcd_background_color_set(LCD_COLOR_BLACK);
}

/*!
    \brief      clear the text zone
    \param[in]  start_x, start_y, width, height: zone location
    \param[out] none
    \retval     none
*/
void lcd_log_textzone_clear (uint16_t start_x,
                             uint16_t start_y,
                             uint16_t width,
                             uint16_t height)
{
    lcd_rectangle_fill(start_x, start_y, width, height);
}

/*!
    \brief      redirect the printf to the LCD
    \param[in]  ch: character to be displayed
    \param[in]  f: output file pointer
    \param[out] none
    \retval     none
*/
LCD_LOG_PUTCHAR
{
    font_struct *cFont = lcd_font_get();
    uint32_t idx;

    if (DISABLE == LCD_Lock) {
        if (ENABLE == LCD_ScrollActive) {
            LCD_CacheBuffer_yptr_bottom = LCD_CacheBuffer_yptr_bottom_bak;
            LCD_CacheBuffer_yptr_top = LCD_CacheBuffer_yptr_top_bak;
            LCD_ScrollActive = DISABLE;
            LCD_Scrolled = DISABLE;
            LCD_ScrollBackStep = 0;
        }

        if ((LCD_CacheBuffer_xptr < LCD_FLAG_HEIGHT / cFont->width) && ('\n' != ch)) {
            LCD_CacheBuffer[LCD_CacheBuffer_yptr_bottom].line[LCD_CacheBuffer_xptr++] = (uint16_t)ch;
        } else {
            if (LCD_CacheBuffer_yptr_top >= LCD_CacheBuffer_yptr_bottom) {
                if (DISABLE == LCD_CacheBuffer_yptr_invert) {
                    LCD_CacheBuffer_yptr_top++;

                    if (LCD_CACHE_DEPTH == LCD_CacheBuffer_yptr_top) {
                        LCD_CacheBuffer_yptr_top = 0U;
                    }
                } else {
                    LCD_CacheBuffer_yptr_invert= DISABLE;
                }
            }

            for (idx = LCD_CacheBuffer_xptr; idx < LCD_FLAG_HEIGHT / cFont->width; idx++) {
                LCD_CacheBuffer[LCD_CacheBuffer_yptr_bottom].line[LCD_CacheBuffer_xptr++] = ' ';
            }

            LCD_CacheBuffer[LCD_CacheBuffer_yptr_bottom].color = LCD_LineColor;
            LCD_CacheBuffer_xptr = 0U;
            LCD_LOG_UpdateDisplay();
            LCD_CacheBuffer_yptr_bottom ++; 

            if (LCD_CACHE_DEPTH == LCD_CacheBuffer_yptr_bottom) {
                LCD_CacheBuffer_yptr_bottom = 0U;
                LCD_CacheBuffer_yptr_top = 1U;
                LCD_CacheBuffer_yptr_invert = ENABLE;
            }

            if ('\n' != ch) {
                LCD_CacheBuffer[LCD_CacheBuffer_yptr_bottom].line[LCD_CacheBuffer_xptr++] = (uint16_t)ch;
            }
        }
    }

    return ch;
}

/*!
    \brief      update the text area display
    \param[in]  none
    \param[out] none
    \retval     none
*/
void LCD_LOG_UpdateDisplay (void)
{
    uint8_t cnt = 0U ;
    uint16_t length = 0U ;
    uint16_t ptr = 0U, index = 0U;

    font_struct *cFont = lcd_font_get();

    if ((LCD_CacheBuffer_yptr_bottom < (YWINDOW_SIZE - 1U)) && (LCD_CacheBuffer_yptr_bottom >= LCD_CacheBuffer_yptr_top)) {
        lcd_text_color_set (LCD_CacheBuffer[cnt + LCD_CacheBuffer_yptr_bottom].color);
        lcd_vertical_string_display ((YWINDOW_MIN + LCD_CacheBuffer_yptr_bottom) * cFont->height, 0U, (uint8_t *)(LCD_CacheBuffer[cnt + LCD_CacheBuffer_yptr_bottom].line));
    } else {
        if (LCD_CacheBuffer_yptr_bottom < LCD_CacheBuffer_yptr_top) {
            /* Virtual length for rolling */
            length = LCD_CACHE_DEPTH + LCD_CacheBuffer_yptr_bottom ;
        } else {
            length = LCD_CacheBuffer_yptr_bottom;
        }

        ptr = length - YWINDOW_SIZE + 1U;

        for (cnt = 0U; cnt < YWINDOW_SIZE; cnt ++) {
            index = (cnt + ptr ) % LCD_CACHE_DEPTH;

            lcd_text_color_set (LCD_CacheBuffer[index].color);
            lcd_vertical_string_display ((cnt + YWINDOW_MIN) * cFont->height, 0U, (uint8_t *)(LCD_CacheBuffer[index].line));
        }
    }
}
