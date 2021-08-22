/*!
    \file    lcd_font.h
    \brief   the header file of LCD font

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

#ifndef LCD_FONTS_H
#define LCD_FONTS_H

#include <stdint.h>

typedef struct {
    const uint16_t *table;

    uint16_t width;
    uint16_t height;
} font_struct;

typedef struct 
{
    unsigned char index[3];
    char mask[32];
} typefont_GB162;

typedef struct 
{
    unsigned char index[3];
    char mask[72];
} typefont_GB242;

extern font_struct font16x24;
extern font_struct font12x12;
extern font_struct font8x16;
extern font_struct font8x12;
extern font_struct font8x8;

extern const unsigned char ASCII16[];
extern const unsigned char sz32[];

extern const typefont_GB162 hz16[];
extern const typefont_GB242 hz24[];

#define hz16_num          100U
#define hz24_num          100U
#define LINE(x)           ((x) * (((font_struct *)lcd_font_get())->height))

#endif /* LCD_FONTS_H */
