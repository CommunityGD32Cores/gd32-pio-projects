/*!
    \file    malloc.c
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

#include "malloc.h"

/*-----------------------------------------------------------*/
#define configTOTAL_HEAP_SIZE    (80 * 1024)
static uint8_t xHeapRegion_0[ configTOTAL_HEAP_SIZE ] __ALIGNED(16);
#if defined(CONFIG_EXTEND_MEMORY)
#define configTOTAL_HEAP1_SIZE   (64 * 1024)
static uint8_t xHeapRegion_1[ configTOTAL_HEAP1_SIZE ] __ALIGNED(16);
#endif
HeapRegion_T xHeapRegions[] =
{
    { xHeapRegion_0, sizeof(xHeapRegion_0)},
#if defined(CONFIG_EXTEND_MEMORY)
    { xHeapRegion_1, sizeof(xHeapRegion_1)},
#endif
    { NULL, 0 }
};

/*
 * Inserts a block of memory that is being freed into the correct position in
 * the list of free memory blocks.  The block being freed will be merged with
 * the block in front it and/or the block behind it if the memory blocks are
 * adjacent to each other.
 */
/*!
    \brief      insert a block into a free list
    \param[in]  pxBlockToInsert: the pointer of block
    \param[out] none
    \retval     none
*/
static void prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert );
/*!
    \brief      define and initialize heap regions
    \param[in]  pxHeapRegions: the pointer of heap region
    \param[out] none
    \retval     none
*/
void vSysDefineHeapRegions( const HeapRegion_T * const pxHeapRegions );

/*-----------------------------------------------------------*/

/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
static const size_t xHeapStructSize = ( sizeof( BlockLink_t ) + ( ( size_t ) ( portMALLOC_BYTE_ALIGNMENT - 1 ) ) )
                                    & ~( ( size_t ) portMALLOC_BYTE_ALIGNMENT_MASK );

/* Create a couple of list links to mark the start and end of the list. */
static BlockLink_t xStart, *pxEnd = NULL;

/* Keeps track of the number of free bytes remaining, but says nothing about
fragmentation. */
static size_t xFreeBytesRemaining = 0U;
static size_t xMinimumEverFreeBytesRemaining = 0U;

/* Gets set to the top bit of an size_t type.  When this bit in the xBlockSize
member of an BlockLink_t structure is set then the block belongs to the
application.  When the bit is free the block is still part of the free heap
space. */
static size_t xBlockAllocatedBit = 0;

/*
    Dump xBlock list
*/

/*!
    \brief      dump xBlock list
    \param[in]  none
    \param[out] none
    \retval     none
*/
void dump_mem_block_list(void)
{
    BlockLink_t *pxBlock = &xStart;
    int count = 0;

    while(pxBlock->pxNextFreeBlock != NULL)
    {
        printf("[%d]=0x%p, %d\r\n", count++, pxBlock, pxBlock->xBlockSize);
        pxBlock = pxBlock->pxNextFreeBlock;
    }
}

/*-----------------------------------------------------------*/

/*!
    \brief      allocate the specified size of memory
    \param[in]  xWantedSize: size of memory required
    \param[out] none
    \retval     the pointer of start memory address
*/
void *pvSysMalloc( size_t xWantedSize )
{
    BlockLink_t *pxBlock, *pxPreviousBlock, *pxNewBlockLink;
    void *pvReturn = NULL;

    /* The heap must be initialised before the first call to    malloc. */
    if (pxEnd == NULL) {
        vSysDefineHeapRegions( xHeapRegions );
    }

    DEBUG_ASSERT( pxEnd );

    sys_sched_lock();
    {
        /* Check the requested block size is not so large that the top bit is
        set.  The top bit of the block size member of the BlockLink_t structure
        is used to determine who owns the block - the application or the
        kernel, so it must be free. */
        if( ( xWantedSize & xBlockAllocatedBit ) == 0 )
        {
            /* The wanted size is increased so it can contain a BlockLink_t
            structure in addition to the requested amount of bytes. */
            if( xWantedSize > 0 )
            {
                xWantedSize += xHeapStructSize;

                /* Ensure that blocks are always aligned to the required number
                of bytes. */
                if( ( xWantedSize & portMALLOC_BYTE_ALIGNMENT_MASK ) != 0x00 )
                {
                    /* Byte alignment required. */
                    xWantedSize += ( portMALLOC_BYTE_ALIGNMENT - ( xWantedSize & portMALLOC_BYTE_ALIGNMENT_MASK ) );
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }

            if( ( xWantedSize > 0 ) && ( xWantedSize <= xFreeBytesRemaining ) )
            {
                /* Traverse the list from the start    (lowest address) block until
                one    of adequate size is found. */
                pxPreviousBlock = &xStart;
                pxBlock = xStart.pxNextFreeBlock;
                while( ( pxBlock->xBlockSize < xWantedSize ) && ( pxBlock->pxNextFreeBlock != NULL ) )
                {
                    pxPreviousBlock = pxBlock;
                    pxBlock = pxBlock->pxNextFreeBlock;
                }

                /* If the end marker was reached then a block of adequate size
                was    not found. */
                if( pxBlock != pxEnd )
                {
                    /* Return the memory space pointed to - jumping over the
                    BlockLink_t structure at its start. */
                    pvReturn = ( void *) ( ( ( uint8_t *) pxPreviousBlock->pxNextFreeBlock ) + xHeapStructSize );

                    /* This block is being returned for use so must be taken out
                    of the list of free blocks. */
                    pxPreviousBlock->pxNextFreeBlock = pxBlock->pxNextFreeBlock;

                    /* If the block is larger than required it can be split into
                    two. */
                    if( ( pxBlock->xBlockSize - xWantedSize ) > heapMINIMUM_BLOCK_SIZE )
                    {
                        /* This block is to be split into two.  Create a new
                        block following the number of bytes requested. The void
                        cast is used to prevent byte alignment warnings from the
                        compiler. */
                        pxNewBlockLink = ( void *) ( ( ( uint8_t *) pxBlock ) + xWantedSize );

                        /* Calculate the sizes of two blocks split from the
                        single block. */
                        pxNewBlockLink->xBlockSize = pxBlock->xBlockSize - xWantedSize;
                        pxBlock->xBlockSize = xWantedSize;

                        /* Insert the new block into the list of free blocks. */
                        prvInsertBlockIntoFreeList( ( pxNewBlockLink ) );
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    xFreeBytesRemaining -= pxBlock->xBlockSize;

                    if( xFreeBytesRemaining < xMinimumEverFreeBytesRemaining )
                    {
                        xMinimumEverFreeBytesRemaining = xFreeBytesRemaining;
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    /* The block is being returned - it is allocated and owned
                    by the application and has no "next" block. */
                    pxBlock->xBlockSize |= xBlockAllocatedBit;
                    pxBlock->pxNextFreeBlock = NULL;
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }

        traceMALLOC( pvReturn, xWantedSize );
    }
    ( void ) sys_sched_unlock();

    #if( configUSE_MALLOC_FAILED_HOOK == 1 )
    {
        if( pvReturn == NULL )
        {
            extern void vApplicationMallocFailedHook( void );
            vApplicationMallocFailedHook();
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }
    }
    #endif

    return pvReturn;
}
/*-----------------------------------------------------------*/

/*!
    \brief      free the allocated memory
    \param[in]  pv: the pointer of memory address
    \param[out] none
    \retval     none
*/
void vSysFree( void *pv )
{
    uint8_t *puc = ( uint8_t *) pv;
    BlockLink_t *pxLink;

    if( pv != NULL )
    {
        /* The memory being freed will have an BlockLink_t structure immediately
        before it. */
        puc -= xHeapStructSize;

        /* This casting is to keep the compiler from issuing warnings. */
        pxLink = ( void *) puc;

        /* Check the block is actually allocated. */
        DEBUG_ASSERT( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 );
        DEBUG_ASSERT( pxLink->pxNextFreeBlock == NULL );

        if( ( pxLink->xBlockSize & xBlockAllocatedBit ) != 0 )
        {
            if( pxLink->pxNextFreeBlock == NULL )
            {
                /* The block is being returned to the heap - it is no longer
                allocated. */
                pxLink->xBlockSize &= ~xBlockAllocatedBit;

                sys_sched_lock();
                {
                    /* Add this block to the list of free blocks. */
                    xFreeBytesRemaining += pxLink->xBlockSize;
                    traceFREE( pv, pxLink->xBlockSize );
                    prvInsertBlockIntoFreeList( ( ( BlockLink_t * ) pxLink ) );
                }
                ( void ) sys_sched_unlock();
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }
    }
}
/*-----------------------------------------------------------*/

/*!
    \brief      get the remaining memory size
    \param[in]  none
    \param[out] none
    \retval     the size of remaining memory
*/
size_t xSysGetFreeHeapSize( void )
{
    return xFreeBytesRemaining;
}
/*-----------------------------------------------------------*/

/*!
    \brief      get the minimum amount of total free memory
    \param[in]  none
    \param[out] none
    \retval     the size of memory
*/
size_t xSysGetMinimumEverFreeHeapSize( void )
{
    return xMinimumEverFreeBytesRemaining;
}

/*!
    \brief      get the heap minimum block size
    \param[in]  none
    \param[out] none
    \retval     the size of heap minimum block
*/
size_t xSysGetHeapMinimumBlockSize(void)
{
    return heapMINIMUM_BLOCK_SIZE;
}
/*-----------------------------------------------------------*/

/*!
    \brief      insert a block into a free list
    \param[in]  pxBlockToInsert: the pointer of block
    \param[out] none
    \retval     none
*/
static void prvInsertBlockIntoFreeList( BlockLink_t *pxBlockToInsert )
{
BlockLink_t *pxIterator;
uint8_t *puc;

    /* Iterate through the list until a block is found that has a higher address
    than the block being inserted. */
    for( pxIterator = &xStart; pxIterator->pxNextFreeBlock < pxBlockToInsert; pxIterator = pxIterator->pxNextFreeBlock )
    {
        /* Nothing to do here, just iterate to the right position. */
    }

    /* Do the block being inserted, and the block it is being inserted after
    make a contiguous block of memory? */
    puc = ( uint8_t *) pxIterator;
    if( ( puc + pxIterator->xBlockSize ) == ( uint8_t *) pxBlockToInsert )
    {
        pxIterator->xBlockSize += pxBlockToInsert->xBlockSize;
        pxBlockToInsert = pxIterator;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }

    /* Do the block being inserted, and the block it is being inserted before
    make a contiguous block of memory? */
    puc = ( uint8_t *) pxBlockToInsert;
    if( ( puc + pxBlockToInsert->xBlockSize ) == ( uint8_t *) pxIterator->pxNextFreeBlock )
    {
        if( pxIterator->pxNextFreeBlock != pxEnd )
        {
            /* Form one big block from the two blocks. */
            pxBlockToInsert->xBlockSize += pxIterator->pxNextFreeBlock->xBlockSize;
            pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock->pxNextFreeBlock;
        }
        else
        {
            pxBlockToInsert->pxNextFreeBlock = pxEnd;
        }
    }
    else
    {
        pxBlockToInsert->pxNextFreeBlock = pxIterator->pxNextFreeBlock;
    }

    /* If the block being inserted plugged a gab, so was merged with the block
    before and the block after, then it's pxNextFreeBlock pointer will have
    already been set, and should not be set here as that would make it point
    to itself. */
    if( pxIterator != pxBlockToInsert )
    {
        pxIterator->pxNextFreeBlock = pxBlockToInsert;
    }
    else
    {
        mtCOVERAGE_TEST_MARKER();
    }
}
/*-----------------------------------------------------------*/

/*!
    \brief      define and initialize heap regions
    \param[in]  pxHeapRegions: the pointer of heap region
    \param[out] none
    \retval     none
*/
void vSysDefineHeapRegions( const HeapRegion_T * const pxHeapRegions )
{
    BlockLink_t *pxFirstFreeBlockInRegion = NULL, *pxPreviousFreeBlock;
    size_t xAlignedHeap;
    size_t xTotalRegionSize, xTotalHeapSize = 0;
    int32_t xDefinedRegions = 0;
    size_t xAddress;
    const HeapRegion_T *pxHeapRegion;

    /* Can only call once! */
    DEBUG_ASSERT( pxEnd == NULL );

    pxHeapRegion = &( pxHeapRegions[ xDefinedRegions ] );

    while( pxHeapRegion->xSizeInBytes > 0 )
    {
        xTotalRegionSize = pxHeapRegion->xSizeInBytes;

        /* Ensure the heap region starts on a correctly aligned boundary. */
        xAddress = ( size_t ) pxHeapRegion->pucStartAddress;
        if( ( xAddress & portMALLOC_BYTE_ALIGNMENT_MASK ) != 0 )
        {
            xAddress += ( portMALLOC_BYTE_ALIGNMENT - 1 );
            xAddress &= ~portMALLOC_BYTE_ALIGNMENT_MASK;

            /* Adjust the size for the bytes lost to alignment. */
            xTotalRegionSize -= xAddress - ( size_t ) pxHeapRegion->pucStartAddress;
        }

        xAlignedHeap = xAddress;

        /* Set xStart if it has not already been set. */
        if( xDefinedRegions == 0 )
        {
            /* xStart is used to hold a pointer to the first item in the list of
            free blocks.  The void cast is used to prevent compiler warnings. */
            xStart.pxNextFreeBlock = ( BlockLink_t * ) xAlignedHeap;
            xStart.xBlockSize = ( size_t ) 0;
        }
        else
        {
            /* Should only get here if one region has already been added to the
            heap. */
            DEBUG_ASSERT( pxEnd != NULL );

            /* Check blocks are passed in with increasing start addresses. */
            DEBUG_ASSERT( xAddress > ( size_t ) pxEnd );
        }

        /* Remember the location of the end marker in the previous region, if
        any. */
        pxPreviousFreeBlock = pxEnd;

        /* pxEnd is used to mark the end of the list of free blocks and is
        inserted at the end of the region space. */
        xAddress = xAlignedHeap + xTotalRegionSize;
        xAddress -= xHeapStructSize;
        xAddress &= ~portMALLOC_BYTE_ALIGNMENT_MASK;
        pxEnd = ( BlockLink_t * ) xAddress;
        pxEnd->xBlockSize = 0;
        pxEnd->pxNextFreeBlock = NULL;

        /* To start with there is a single free block in this region that is
        sized to take up the entire heap region minus the space taken by the
        free block structure. */
        pxFirstFreeBlockInRegion = ( BlockLink_t * ) xAlignedHeap;
        pxFirstFreeBlockInRegion->xBlockSize = xAddress - ( size_t ) pxFirstFreeBlockInRegion;
        pxFirstFreeBlockInRegion->pxNextFreeBlock = pxEnd;

        /* If this is not the first region that makes up the entire heap space
        then link the previous region to this region. */
        if( pxPreviousFreeBlock != NULL )
        {
            pxPreviousFreeBlock->pxNextFreeBlock = pxFirstFreeBlockInRegion;
        }

        xTotalHeapSize += pxFirstFreeBlockInRegion->xBlockSize;

        /* Move onto the next HeapRegion_t structure. */
        xDefinedRegions++;
        pxHeapRegion = &( pxHeapRegions[ xDefinedRegions ] );
    }

    xMinimumEverFreeBytesRemaining = xTotalHeapSize;
    xFreeBytesRemaining = xTotalHeapSize;

    /* Check something was actually defined before it is accessed. */
    DEBUG_ASSERT( xTotalHeapSize );

    /* Work out the position of the top bit in a size_t variable. */
    xBlockAllocatedBit = ( ( size_t ) 1 ) << ( ( sizeof( size_t ) * heapBITS_PER_BYTE ) - 1 );
}

/*!
    \brief      reallocate the specified size of memory
    \param[in]  pv: the pointer of the memory address that has been acquired
    \param[in]  xWantedSize: size of memory required
    \param[out] none
    \retval     none
*/
void *pvSysReAlloc( void *pv,  size_t xWantedSize )
{
    BlockLink_t *pxLink;
    unsigned char *puc = ( unsigned char * ) pv;
    void *newArea = NULL;

    if( pv )
    {
        if( !xWantedSize )
        {
            vSysFree( pv );
            goto exit;
        }

        newArea = pvSysMalloc( xWantedSize );
        if( newArea )
        {
            int oldSize, copySize;
            /* The memory being freed will have an xBlockLink structure immediately
            before it. */
            puc -= xHeapStructSize;

            /* This casting is to keep the compiler from issuing warnings. */
            pxLink = ( void *) puc;

            oldSize =  (pxLink->xBlockSize & ~xBlockAllocatedBit) - xHeapStructSize;
            copySize = ( oldSize < xWantedSize ) ? oldSize : xWantedSize;
            memcpy( newArea, pv, copySize );

            sys_sched_lock();
            {
                /* Add this block to the list of free blocks. */
                pxLink->xBlockSize &= ~xBlockAllocatedBit;
                xFreeBytesRemaining += pxLink->xBlockSize;
                prvInsertBlockIntoFreeList( ( ( BlockLink_t * ) pxLink ) );
            }
            sys_sched_unlock();
        }
    }
    else if( xWantedSize )
        newArea = pvSysMalloc( xWantedSize );

exit:
    return newArea;
}
