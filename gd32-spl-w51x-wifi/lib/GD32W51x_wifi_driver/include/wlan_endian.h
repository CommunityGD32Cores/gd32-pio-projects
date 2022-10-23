/*!
    \file    wlan_endian.h
    \brief   Endian definition for GD32W51x WiFi driver

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

#ifndef __WLAN_ENDIAN_H_
#define __WLAN_ENDIAN_H_

#include "wlan_cfg.h"

#ifdef CONFIG_LITTLE_ENDIAN
#define EF1Byte(_val)               ((uint8_t)(_val))
#define EF2Byte(_val)               ((uint16_t)(_val))
#define EF4Byte(_val)               ((uint32_t)(_val))
#else
#define EF1Byte(_val)               ((uint8_t)(_val))
#define EF2Byte(_val)               (((((uint16_t)(_val))&0x00ff)<<8)     |    \
                                    ((((uint16_t)(_val))&0xff00)>>8))
#define EF4Byte(_val)               (((((uint32_t)(_val))&0x000000ff)<<24)|    \
                                    ((((uint32_t)(_val))&0x0000ff00)<<8)  |    \
                                    ((((uint32_t)(_val))&0x00ff0000)>>8)  |    \
                                    ((((uint32_t)(_val))&0xff000000)>>24))
#endif

#define ReadEF1Byte(_ptr)           EF1Byte(*((volatile uint8_t *)(_ptr)))
#define ReadEF2Byte(_ptr)           EF2Byte(*((volatile uint16_t *)(_ptr)))
#define ReadEF4Byte(_ptr)           EF4Byte(*((volatile uint32_t *)(_ptr)))

#define WriteEF1Byte(_ptr, _val)    (*((volatile uint8_t *)(_ptr))) = EF1Byte(_val)
#define WriteEF2Byte(_ptr, _val)    (*((volatile uint16_t *)(_ptr))) = EF2Byte(_val)
#define WriteEF4Byte(_ptr, _val)    (*((volatile uint32_t *)(_ptr))) = EF4Byte(_val)

#define BIT_LEN_MASK_32(__BitLen)   (0xFFFFFFFF >> (32 - (__BitLen)))

#define BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen) \
    (BIT_LEN_MASK_32(__BitLen) << (__BitOffset))

#define LE_P4BYTE_TO_HOST_4BYTE(__pStart) \
    (EF4Byte(*((volatile uint32_t *)(__pStart))))

#define LE_BITS_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
    ( \
        ( LE_P4BYTE_TO_HOST_4BYTE(__pStart) >> (__BitOffset) ) \
        & \
        BIT_LEN_MASK_32(__BitLen) \
    )

#define LE_BITS_TO_4BYTE_32BIT(__pStart, __BitOffset, __BitLen) \
    ( \
        ( LE_P4BYTE_TO_HOST_4BYTE(__pStart)  ) \
    )

#define LE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
    ( \
        LE_P4BYTE_TO_HOST_4BYTE(__pStart) \
        & \
        ( ~ BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen) ) \
    )

#define BITS_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
    ( \
        ( (*((volatile uint32_t *)(__pStart))) >> (__BitOffset) ) \
        & \
        BIT_LEN_MASK_32(__BitLen) \
    )

#define SET_BITS_TO_4BYTE(__pStart, __BitOffset, __BitLen, __Value)  \
    *((volatile uint32_t *)(__pStart)) = \
        ( \
        ((*((volatile uint32_t *)(__pStart))) \
        & \
        ( ~ BIT_OFFSET_LEN_MASK_32(__BitOffset, __BitLen) )) \
        | \
        ( (((uint32_t)__Value) & BIT_LEN_MASK_32(__BitLen)) << (__BitOffset) ) \
        );

#define SET_BITS_TO_LE_4BYTE(__pStart, __BitOffset, __BitLen, __Value)  \
    *((volatile uint32_t *)(__pStart)) = \
        EF4Byte( \
            LE_BITS_CLEARED_TO_4BYTE(__pStart, __BitOffset, __BitLen) \
            | \
            ( (((uint32_t)__Value) & BIT_LEN_MASK_32(__BitLen)) << (__BitOffset) ) \
        );

#define BIT_LEN_MASK_16(__BitLen) \
        (0xFFFF >> (16 - (__BitLen)))

#define BIT_OFFSET_LEN_MASK_16(__BitOffset, __BitLen) \
    (BIT_LEN_MASK_16(__BitLen) << (__BitOffset))

#define LE_P2BYTE_TO_HOST_2BYTE(__pStart) \
    (EF2Byte(*((volatile uint16_t *)(__pStart))))

#define LE_BITS_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
    ( \
        ( LE_P2BYTE_TO_HOST_2BYTE(__pStart) >> (__BitOffset) ) \
        & \
        BIT_LEN_MASK_16(__BitLen) \
    )

#define LE_BITS_CLEARED_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
    ( \
        LE_P2BYTE_TO_HOST_2BYTE(__pStart) \
        & \
        ( ~ BIT_OFFSET_LEN_MASK_16(__BitOffset, __BitLen) ) \
    )

#define SET_BITS_TO_LE_2BYTE(__pStart, __BitOffset, __BitLen, __Value) \
    *((volatile uint16_t *)(__pStart)) = \
        EF2Byte( \
            LE_BITS_CLEARED_TO_2BYTE(__pStart, __BitOffset, __BitLen) \
            | \
            ( (((uint16_t)__Value) & BIT_LEN_MASK_16(__BitLen)) << (__BitOffset) ) \
        );

#define BIT_LEN_MASK_8(__BitLen) \
        (0xFF >> (8 - (__BitLen)))

#define BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen) \
    (BIT_LEN_MASK_8(__BitLen) << (__BitOffset))

#define LE_P1BYTE_TO_HOST_1BYTE(__pStart) \
    (EF1Byte(*((volatile uint8_t *)(__pStart))))

#define LE_BITS_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
    ( \
        ( LE_P1BYTE_TO_HOST_1BYTE(__pStart) >> (__BitOffset) ) \
        & \
        BIT_LEN_MASK_8(__BitLen) \
    )

#define LE_BITS_TO_1BYTE_8BIT(__pStart, __BitOffset, __BitLen) \
    LE_P1BYTE_TO_HOST_1BYTE(__pStart)

#define LE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
    ( \
        LE_P1BYTE_TO_HOST_1BYTE(__pStart) \
        & \
        ( ~ BIT_OFFSET_LEN_MASK_8(__BitOffset, __BitLen) ) \
    )

#define LE_BITS_CLEARED_TO_1BYTE_8BIT(__pStart, __BitOffset, __BitLen) \
    ( \
        LE_P1BYTE_TO_HOST_1BYTE(__pStart) \
    )

#define SET_BITS_TO_LE_1BYTE(__pStart, __BitOffset, __BitLen, __Value) \
{ \
    *((volatile uint8_t *)(__pStart)) = \
        EF1Byte( \
            LE_BITS_CLEARED_TO_1BYTE(__pStart, __BitOffset, __BitLen) \
            | \
            ( (((uint8_t)__Value) & BIT_LEN_MASK_8(__BitLen)) << (__BitOffset) ) \
        ); \
}

#define SET_BITS_TO_LE_1BYTE_8BIT(__pStart, __BitOffset, __BitLen, __Value) \
{ \
    *((volatile uint8_t *)(__pStart)) = \
        EF1Byte( \
            LE_BITS_CLEARED_TO_1BYTE_8BIT(__pStart, __BitOffset, __BitLen) \
            | \
            ((uint8_t)__Value) \
        ); \
}

/**
 * @brief This function swap the bytes for 16bit data
 * @param x input data
 * @return the swapped data
 */
__inline static uint16_t  __swap16(uint16_t x)
{
    uint16_t __x = x;
    return
    ((uint16_t)(
            (((uint16_t)(__x) & (uint16_t)0x00ffU) << 8) |
            (((uint16_t)(__x) & (uint16_t)0xff00U) >> 8) ));

}

/**
 * @brief This function swap the bytes for 32bit data
 * @param x input data
 * @return the swapped data
 */
__inline static uint32_t  __swap32(uint32_t x)
{
    uint32_t __x = (x);
    return ((uint32_t)(
        (((uint32_t)(__x) & (uint32_t)0x000000ffUL) << 24) |
        (((uint32_t)(__x) & (uint32_t)0x0000ff00UL) <<  8) |
        (((uint32_t)(__x) & (uint32_t)0x00ff0000UL) >>  8) |
        (((uint32_t)(__x) & (uint32_t)0xff000000UL) >> 24) ));
}

/**
 * @brief This function swap the bytes for 64bit data
 * @param x input data
 * @return the swapped data
 */
__inline static uint64_t  __swap64(uint64_t x)
{
    uint64_t __x = (x);

    return
    ((uint64_t)( \
        (uint64_t)(((uint64_t)(__x) & (uint64_t)0x00000000000000ffULL) << 56) | \
        (uint64_t)(((uint64_t)(__x) & (uint64_t)0x000000000000ff00ULL) << 40) | \
        (uint64_t)(((uint64_t)(__x) & (uint64_t)0x0000000000ff0000ULL) << 24) | \
        (uint64_t)(((uint64_t)(__x) & (uint64_t)0x00000000ff000000ULL) <<  8) | \
        (uint64_t)(((uint64_t)(__x) & (uint64_t)0x000000ff00000000ULL) >>  8) | \
        (uint64_t)(((uint64_t)(__x) & (uint64_t)0x0000ff0000000000ULL) >> 24) | \
        (uint64_t)(((uint64_t)(__x) & (uint64_t)0x00ff000000000000ULL) >> 40) | \
        (uint64_t)(((uint64_t)(__x) & (uint64_t)0xff00000000000000ULL) >> 56) )); \
}

#define swap16 __swap16
#define swap32 __swap32
#define swap64 __swap64

#if defined (CONFIG_LITTLE_ENDIAN)

#ifndef __LITTLE_ENDIAN
#define __LITTLE_ENDIAN 1234
#endif
#ifndef __LITTLE_ENDIAN_BITFIELD
#define __LITTLE_ENDIAN_BITFIELD
#endif

#define __constant_cpu_to_le64(x)   ((uint64_t)(x))
#define __constant_le64_to_cpu(x)   ((uint64_t)(x))
#define __constant_cpu_to_le32(x)   ((uint32_t)(x))
#define __constant_le32_to_cpu(x)   ((uint32_t)(x))
#define __constant_cpu_to_le16(x)   ((uint16_t)(x))
#define __constant_le16_to_cpu(x)   ((uint16_t)(x))
#define __cpu_to_le64(x)            ((uint64_t)(x))
#define __le64_to_cpu(x)            ((uint64_t)(x))
#define __cpu_to_le32(x)            ((uint32_t)(x))
#define __le32_to_cpu(x)            ((uint32_t)(x))
#define __cpu_to_le16(x)            ((uint16_t)(x))
#define __le16_to_cpu(x)            ((uint16_t)(x))
#define __cpu_to_be64(x)            __swap64((x))
#define __be64_to_cpu(x)            __swap64((x))
#define __cpu_to_be32(x)            __swap32((x))
#define __be32_to_cpu(x)            __swap32((x))
#define __cpu_to_be16(x)            __swap16((x))
#define __be16_to_cpu(x)            __swap16((x))
#define __cpu_to_le64p(x)           (*(uint64_t *)(x))
#define __le64_to_cpup(x)           (*(uint64_t *)(x))
#define __cpu_to_le32p(x)           (*(uint32_t *)(x))
#define __le32_to_cpup(x)           (*(uint32_t *)(x))
#define __cpu_to_le16p(x)           (*(uint16_t *)(x))
#define __le16_to_cpup(x)           (*(uint16_t *)(x))
#define __cpu_to_le64s(x)           do {} while (0)
#define __le64_to_cpus(x)           do {} while (0)
#define __cpu_to_le32s(x)           do {} while (0)
#define __le32_to_cpus(x)           do {} while (0)
#define __cpu_to_le16s(x)           do {} while (0)
#define __le16_to_cpus(x)           do {} while (0)

#elif defined (CONFIG_BIG_ENDIAN)

#ifndef __BIG_ENDIAN
#define __BIG_ENDIAN 4321
#endif
#ifndef __BIG_ENDIAN_BITFIELD
#define __BIG_ENDIAN_BITFIELD
#endif

#define __constant_htonl(x)         ((uint32_t)(x))
#define __constant_ntohl(x)         ((uint32_t)(x))
#define __constant_htons(x)         ((uint16_t)(x))
#define __constant_ntohs(x)         ((uint16_t)(x))
#define __constant_cpu_to_be64(x)   ((uint64_t)(x))
#define __constant_be64_to_cpu(x)   ((uint64_t)(x))
#define __constant_cpu_to_be32(x)   ((uint32_t)(x))
#define __constant_be32_to_cpu(x)   ((uint32_t)(x))
#define __constant_cpu_to_be16(x)   ((uint16_t)(x))
#define __constant_be16_to_cpu(x)   ((uint16_t)(x))
#define __cpu_to_le64(x)            __swap64((x))
#define __le64_to_cpu(x)            __swap64((x))
#define __cpu_to_le32(x)            __swap32((x))
#define __le32_to_cpu(x)            __swap32((x))
#define __cpu_to_le16(x)            __swap16((x))
#define __le16_to_cpu(x)            __swap16((x))
#define __cpu_to_be64(x)            ((uint64_t)(x))
#define __be64_to_cpu(x)            ((uint64_t)(x))
#define __cpu_to_be32(x)            ((uint32_t)(x))
#define __be32_to_cpu(x)            ((uint32_t)(x))
#define __cpu_to_be16(x)            ((uint16_t)(x))
#define __be16_to_cpu(x)            ((uint16_t)(x))
#define __cpu_to_be64p(x)           (*(uint64_t *)(x))
#define __be64_to_cpup(x)           (*(uint64_t *)(x))
#define __cpu_to_be32p(x)           (*(uint32_t *)(x))
#define __be32_to_cpup(x)           (*(uint32_t *)(x))
#define __cpu_to_be16p(x)           (*(uint16_t *)(x))
#define __be16_to_cpup(x)           (*(uint16_t *)(x))
#define __cpu_to_be64s(x)           do {} while (0)
#define __be64_to_cpus(x)           do {} while (0)
#define __cpu_to_be32s(x)           do {} while (0)
#define __be32_to_cpus(x)           do {} while (0)
#define __cpu_to_be16s(x)           do {} while (0)
#define __be16_to_cpus(x)           do {} while (0)

#else

#error "Must be LITTLE/BIG Endian System"

#endif

#define cpu_to_le64 __cpu_to_le64
#define le64_to_cpu __le64_to_cpu
#define cpu_to_le32 __cpu_to_le32
#define le32_to_cpu __le32_to_cpu
#define cpu_to_le16 __cpu_to_le16
#define le16_to_cpu __le16_to_cpu
#define cpu_to_be64 __cpu_to_be64
#define be64_to_cpu __be64_to_cpu
#define cpu_to_be32 __cpu_to_be32
#define be32_to_cpu __be32_to_cpu
#define cpu_to_be16 __cpu_to_be16
#define be16_to_cpu __be16_to_cpu
#define cpu_to_le64p __cpu_to_le64p
#define le64_to_cpup __le64_to_cpup
#define cpu_to_le32p __cpu_to_le32p
#define le32_to_cpup __le32_to_cpup
#define cpu_to_le16p __cpu_to_le16p
#define le16_to_cpup __le16_to_cpup
#define cpu_to_be64p __cpu_to_be64p
#define be64_to_cpup __be64_to_cpup
#define cpu_to_be32p __cpu_to_be32p
#define be32_to_cpup __be32_to_cpup
#define cpu_to_be16p __cpu_to_be16p
#define be16_to_cpup __be16_to_cpup
#define cpu_to_le64s __cpu_to_le64s
#define le64_to_cpus __le64_to_cpus
#define cpu_to_le32s __cpu_to_le32s
#define le32_to_cpus __le32_to_cpus
#define cpu_to_le16s __cpu_to_le16s
#define le16_to_cpus __le16_to_cpus
#define cpu_to_be64s __cpu_to_be64s
#define be64_to_cpus __be64_to_cpus
#define cpu_to_be32s __cpu_to_be32s
#define be32_to_cpus __be32_to_cpus
#define cpu_to_be16s __cpu_to_be16s
#define be16_to_cpus __be16_to_cpus

#undef wlan_htonl
#undef wlan_ntohl
#undef wlan_htons
#undef wlan_ntohs

extern uint32_t wlan_htonl(uint32_t);
extern uint32_t wlan_ntohl(uint32_t);
extern uint16_t wlan_htons(uint16_t);
extern uint16_t wlan_ntohs(uint16_t);

#define wlan_htonl(x) __cpu_to_be32(x)
#define wlan_ntohl(x) __be32_to_cpu(x)
#define wlan_htons(x) __cpu_to_be16(x)
#define wlan_ntohs(x) __be16_to_cpu(x)

/**
 * @brief This function read two bytes from specified address with little endian
 * @param pdata read address
 * @return read data
 */
__inline static  uint16_t le_read_cpu16(uint8_t *pdata)
{
    uint16_t a0 = 0;
    uint16_t a1 = 0;

    a0 = *pdata;
    a1 = *(pdata + 1);

#ifdef CONFIG_BIG_ENDIAN
    return (a1 |(a0 << 8));
#else
    return (a0 |(a1 << 8));
#endif
}

/**
 * @brief This function read four bytes from specified address with little endian
 * @param pdata read address
 * @return read data
 */
__inline static  uint32_t le_read_cpu32(uint8_t *pdata)
{
    uint32_t a0 = 0;
    uint32_t a1 = 0;
    uint32_t a2 = 0;
    uint32_t a3 = 0;

    a0 = *pdata;
    a1 = *(pdata + 1);
    a2 = *(pdata + 2);
    a3 = *(pdata + 3);

#ifdef CONFIG_BIG_ENDIAN
    return (a3 |(a2 << 8) | (a1 << 16) | (a0 << 24));
#else
    return (a0 |(a1 << 8) | (a2 << 16) | (a3 << 24));
#endif
}

#endif
