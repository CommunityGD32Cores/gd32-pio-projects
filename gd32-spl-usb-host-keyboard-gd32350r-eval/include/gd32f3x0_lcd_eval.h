/*!
    \file  gd32f3x0_lcd_eval.h
    \brief LCD driver header file

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

#ifndef GD32F3X0_LCD_EVAL_H
#define GD32F3X0_LCD_EVAL_H

#include "lcd_font.h"
#include "gd32f350r_eval.h"

/* LCD colors */
#define LCD_COLOR_WHITE               0xFFFFU
#define LCD_COLOR_BLACK               0x0000U
#define LCD_COLOR_GREY                0xF7DEU
#define LCD_COLOR_GRAY0               0xEF7DU
#define LCD_COLOR_GRAY1               0x8410U
#define LCD_COLOR_GRAY2               0x4208U
#define LCD_COLOR_BLUE                0x001FU
#define LCD_COLOR_RED                 0xF800U
#define LCD_COLOR_MAGENTA             0xF81FU
#define LCD_COLOR_GREEN               0x07e0U
#define LCD_COLOR_CYAN                0x7FFFU
#define LCD_COLOR_YELLOW              0xFFE0U

#define LCD_DEFAULT_FONT              font16x24           /*!< LCD default font */

#define CHAR_FONT_8_16                ((uint16_t)0x0000U) /*!< the font of char is 8X16 */
#define CHAR_FONT_16_24               ((uint16_t)0x0001U) /*!< the font of char is 16X24 */

#define CHAR_DIRECTION_HORIZONTAL     ((uint16_t)0x0000U) /*!< character display direction is horizontal */
#define CHAR_DIRECTION_VERTICAL       ((uint16_t)0x0001U) /*!< character display direction is vertical */

#define LCD_PIXEL_HEIGHT              (uint16_t)240U
#define LCD_PIXEL_WIDTH               (uint16_t)320U

/* PF5 tft cs */
#define LCD_CS_SET                    ((uint32_t)(GPIO_BOP(GPIOF) = GPIO_PIN_5))
#define LCD_CS_CLR                    ((uint32_t)(GPIO_BC(GPIOF) = GPIO_PIN_5))

/* PC3 tft rs/dc */
#define LCD_RS_SET                    ((uint32_t)(GPIO_BOP(GPIOC) = GPIO_PIN_3))
#define LCD_RS_CLR                    ((uint32_t)(GPIO_BC(GPIOC) = GPIO_PIN_3))

/* PC4 tft rst */
#define LCD_RST_SET                   ((uint32_t)(GPIO_BOP(GPIOC) = GPIO_PIN_4))
#define LCD_RST_CLR                   ((uint32_t)(GPIO_BC(GPIOC) = GPIO_PIN_4))

/* function declarations */
/* lcd init */
void gd_eval_lcd_init(void);
/* clear the lcd */
void lcd_clear(uint16_t color);
/* set the text font */
void lcd_font_set(font_struct *font);
/* get the text font */
font_struct *lcd_font_get(void);
/* set the text color */
void lcd_text_color_set(uint16_t color);
/* get the current text color */
uint16_t lcd_text_color_get(void);
/* set the background color */
void lcd_background_color_set(uint16_t color);
/* get the current background color */
uint16_t lcd_background_color_get(void);
/* draw a point on the lcd */
void lcd_point_set(uint16_t x,uint16_t y,uint16_t data);
/* draw a line on the LCD */
void lcd_line_draw(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t color);
/* LCD rectangle draw */
void lcd_rectangle_draw(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint16_t line_color);
/* draw a rectangle with color on the lcd */
void lcd_rectangle_fill(uint16_t x1,uint16_t y1,uint16_t width,uint16_t height);
/* draw a circle on the lcd */
void lcd_circle_draw(uint16_t x,uint16_t y,uint16_t r,uint16_t fc);
/* LCD box2 */
void lcd_box2(uint16_t x,uint16_t y,uint16_t w,uint16_t h,uint8_t mode);
/* display button down */
void display_button_down(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
/* display button up */
void display_button_up(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2);
/* draw gbk16 font on the LCD */
void lcd_draw_font_gbk16(uint16_t x,uint16_t y,char *s);
/* draw gbk24 font on the LCD */
void lcd_draw_font_gbk24(uint16_t x,uint16_t y,uint16_t fc,uint16_t bc,char *s);
/* draw num32 font on the LCD */
void lcd_draw_font_num32(uint16_t x,uint16_t y,uint16_t fc,uint16_t bc,uint16_t num);
/* display the vertical character on LCD */
void lcd_vertical_char_display(uint16_t line, uint16_t column, uint8_t ascii);
/* display the vertical string on LCD */
void lcd_vertical_string_display(uint16_t stringline, uint16_t offset, uint8_t *ptr);

#endif /* GD32F3X0_LCD_EVAL_H */
