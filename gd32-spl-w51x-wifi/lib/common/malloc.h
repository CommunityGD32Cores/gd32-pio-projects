/*!
    \file    malloc.h
    \brief   Malloc function for GD32W51x WiFi SDK

    \version 2021-10-30, V1.0.0, firmware for GD32W51x
*/

/*
    Copyright (c) 2021, GigaDevice Semiconductor Inc.

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

#ifndef __MALLOC_H
#define __MALLOC_H

#include "app_cfg.h"
#include "app_type.h"
#include "wrapper_os.h"
#include "debug_print.h"

#ifndef NULL
#define NULL     0
#endif

#define portMALLOC_BYTE_ALIGNMENT            8
#define portMALLOC_BYTE_ALIGNMENT_MASK       ( 0x0007 )

/* Block sizes must not get too small. */
#define heapMINIMUM_BLOCK_SIZE           ( ( size_t ) ( xHeapStructSize << 1 ) )

/* Assumes 8bit bytes! */
#define heapBITS_PER_BYTE                ( ( size_t ) 8 )

#ifndef mtCOVERAGE_TEST_MARKER
    #define mtCOVERAGE_TEST_MARKER()
#endif

#ifndef traceMALLOC
    #define traceMALLOC( pvAddress, uiSize )
#endif

#ifndef traceFREE
    #define traceFREE( pvAddress, uiSize )
#endif

typedef struct
{
    uint8_t *pucStartAddress;
    size_t xSizeInBytes;
} HeapRegion_T;

/* Define the linked list structure.  This is used to link free blocks in order
of their memory address. */
typedef struct A_BLOCK_LINK
{
    struct A_BLOCK_LINK *pxNextFreeBlock;    /*<< The next free block in the list. */
    size_t xBlockSize;                        /*<< The size of the free block. */
} BlockLink_t;

void *pvSysMalloc(size_t xWantedSize);
void *pvSysReAlloc(void *pv, size_t xWantedSize);
void vSysFree(void *pv);
size_t xSysGetFreeHeapSize(void);
size_t xSysGetMinimumEverFreeHeapSize(void);
size_t xSysGetHeapMinimumBlockSize(void);

#endif
