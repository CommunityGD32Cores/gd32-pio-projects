/*!
    \file    flash_api.c
    \brief   Flash API for GD32W51x WiFi SDK

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

#include "bsp_inc.h"
#include "stddef.h"
#include "flash_api.h"
#include "gd32w51x.h"
#include "string.h"

// #define FLASH_MAPPED_BASE          0x90000000
#define WORD_SIZE                     (4U)

#define WRITE_ENABLE_CMD              0x06
#define WRITE_DISABLE_CMD             0x04
#define READ_STATUS_REG1_CMD          0x05
#define WRITE_STATUS_REG1_CMD         0x01
#define QUAD_IN_FAST_PROG_CMD         0x32
#define QUAD_OUT_FAST_READ_CMD        0x6B
#define SECTOR_ERASE_CMD              0x20    // 0xD8
#define HIGH_PERFORMANCE_CMD          0xA3

#define FLASH_READ16(base, addr)    *(uint16_t *)(base + addr)

uint32_t ISER[8] = {0};
uint32_t systick_ctrl;

/*!
    \brief      get flash cache enabled state
    \param[in]  none
    \param[out] none
    \retval     result of state(1: enabled, or 0: disabled)
*/
int is_flash_cache_enabled(void)
{
    if (ICACHE_CTL & ICACHE_CTL_EN)
        return 1;
    else
        return 0;
}

/*!
    \brief      disable flash cache
    \param[in]  none
    \param[out] none
    \retval     0
*/
int flash_cache_disable(void)
{
    icache_disable();
    return 0;
}

/*!
    \brief      enable flash cache
    \param[in]  none
    \param[out] none
    \retval     0
*/
int flash_cache_enable(void)
{
    icache_enable();
    return 0;
}

/*!
    \brief      check memory structure is SIP Flash or EXT Flash
    \param[in]  none
    \param[out] none
    \retval     memory structure(1: FMC mode: SIP Flash, 0: EXT Flash)
*/
int is_sip_flash(void)
{
    return (OBSTAT_NQSPI() == SET);
}

/*!
    \brief      get flash offset valid state
    \param[in]  offset: flash offset
    \param[out] none
    \retval     result of state(1: offset is valid, or 0: offset is invalid)
*/
int is_valid_flash_offset(uint32_t offset)
{
    if (offset < flash_total_size()) {
        return 1;
    }
    return 0;
}

/*!
    \brief      get flash addr valid state
    \param[in]  addr: flash address
    \param[out] none
    \retval     result of state(1: addr is valid, or 0: addr is not valid)
*/
int is_valid_flash_addr(uint32_t addr)
{
    int bvalid = 0;
    if ((addr >= FLASH_BASE) && (addr < (FLASH_BASE + flash_total_size()))) {
        bvalid = 1;
    }
    return bvalid;
}

/*!
    \brief      get flash total size
    \param[in]  none
    \param[out] none
    \retval     flash total size
*/
uint32_t flash_total_size(void)
{
    return FLASH_TOTAL_SIZE;
}

/*!
    \brief      get flash erase size
    \param[in]  none
    \param[out] none
    \retval     flash erase size. sip flash page size or qspi sector size
*/
uint32_t flash_erase_size(void)
{
    if (is_sip_flash()) {
        return FLASH_SIP_PAGE_SIZE;
    } else {
        return FLASH_QSPI_SECTOR_SIZE;
    }
}

/*!
    \brief      configure no real time decrypt areas for flash
    \param[in]  nd_idx: no decrypt register index
    \param[in]  start_page: start page of no real time decrypt area
    \param[in]  end_page: end page of no real time decrypt area
    \param[out] none
    \retval     none
*/
void flash_nodec_config(uint32_t nd_idx, uint32_t start_page, uint32_t end_page)
{
    if (is_sip_flash()) {
        /* unlock the flash program erase controller */
        fmc_unlock();
        /* unlock the option byte operation (include SECWM/HDP/WRP/NODEC/OFRG/OFVR) */
        ob_unlock();
        /* clear pending flags */
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR);

        /* set no OTFDEC region for sip flash */
        fmc_no_rtdec_config(start_page, end_page, nd_idx);

        /* lock the option byte operation */
        ob_lock();
        /* lock the flash program erase controller */
        fmc_lock();
    }
}

/*!
    \brief      config flash offset region and value
    \param[in]  of_spage: start page of offset region, 0~0x1FFF
    \param[in]  of_epage: end page of offset region, 0~0x1FFF
    \param[in]  of_value: offset value, 0~0x1FFF
    \param[out] none
    \retval     none
*/
void flash_offset_mapping(uint32_t of_spage, uint32_t of_epage, uint32_t of_value)
{
    fmc_unlock();
    ob_unlock();
    fmc_offset_region_config(of_spage, of_epage);
    fmc_offset_value_config(of_value);
    ob_lock();
    fmc_lock();
}

/*!
    \brief      initialize flash
    \param[in]  none
    \param[out] none
    \retval     0
*/
int flash_init(void)
{
    if (!is_sip_flash()) {
        qspi_flash_config(3);
    }
    return 0;
}

/*!
    \brief      lock flash write
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void flash_write_lock(void)
{
    uint8_t i;

    for (i = 0; i < 8; i++) {
        ISER[i] = NVIC->ISER[i];
        NVIC->ICER[i] = 0xffffffff;  // disable interrupts
        NVIC->ICPR[i] = 0xffffffff;  // clear pending interrupts
    }

    systick_ctrl = SysTick->CTRL;    // disable systick
    SysTick->CTRL = 0;
}

/*!
    \brief      unlock flash write
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void flash_write_unlock(void)
{
    uint8_t i;

    for (i = 0; i < 8; i++) {
        NVIC->ISER[i] = ISER[i];  // restore interrupts
    }

    SysTick->CTRL = systick_ctrl;  // restore systick
}

/*!
    \brief      configure QSPI GPIO
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void qspi_flash_gpio_config(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);

    gpio_af_set(GPIOA, GPIO_AF_3, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);

    gpio_af_set(GPIOB, GPIO_AF_3, GPIO_PIN_3 | GPIO_PIN_4);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO_PIN_3 | GPIO_PIN_4);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_25MHZ, GPIO_PIN_3 | GPIO_PIN_4);
}

/*!
    \brief      enable qspi flash write
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void qspi_flash_write_enable(void)
{
    qspi_command_struct sCommand;
    qspi_autopolling_struct sConfig;

    /* Enable write operations ------------------------------------------ */
    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = WRITE_ENABLE_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_NONE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = 0;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_NONE;
    sCommand.nbdata              = 0;
    // sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;

    qspi_command(&sCommand);

    /* Configure automatic polling mode to wait for write enabling ---- */
    sConfig.match                = 0x02;
    sConfig.mask                 = 0x02;
    sConfig.matchmode            = QSPI_MATCH_MODE_AND;
    sConfig.statusbytessize      = 1;
    sConfig.interval             = 0x10;
    sConfig.automaticstop        = QSPI_AUTOMATIC_STOP_ENABLE;

    sCommand.instruction         = READ_STATUS_REG1_CMD;
    sCommand.datamode            = QSPI_DATA_1_LINE;

    qspi_autopolling(&sCommand, &sConfig);
}

/*!
    \brief      configure automatic polling mode to wait for memory ready
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void qspi_flash_autopolling_ready(void)
{
    qspi_command_struct sCommand;
    qspi_autopolling_struct sConfig;

    /* Configure automatic polling mode to wait for memory ready ------ */
    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = READ_STATUS_REG1_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_NONE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = 0;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_1_LINE;
    sCommand.nbdata              = 0;
    // sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;

    sConfig.match                = 0x00;
    sConfig.mask                 = 0x01;
    sConfig.matchmode            = QSPI_MATCH_MODE_AND;
    sConfig.statusbytessize      = 1;
    sConfig.interval             = 0x10;
    sConfig.automaticstop        = QSPI_AUTOMATIC_STOP_ENABLE;

    qspi_autopolling(&sCommand, &sConfig);
}

/*!
    \brief      configure qspi flash to quad mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void qspi_flash_set_quad(void)
{
    uint32_t id = 0;
    uint8_t mode_s = 0;
    uint16_t mode = 0;
    qspi_command_struct sCommand;

    qspi_flash_read_id(&id);

    qspi_flash_write_enable();

    sCommand.instruction        = READ_STATUS_REG1_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_NONE;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_1_LINE;
    sCommand.nbdata              = 1;
    qspi_command(&sCommand);
    qspi_receive(&mode_s);

    mode |= (uint16_t)mode_s;

    sCommand.instruction        = READ_STATUS_REG_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_NONE;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_1_LINE;
    sCommand.nbdata              = 1;
    qspi_command(&sCommand);
    qspi_receive(&mode_s);

    mode |= (uint16_t)mode_s << 8;

    if (mode & 0x0200) {
        /* quad mode, do nothing */
    } else {
        switch ((id & 0x00ff0000) >> 16) {
        case 0x16:                                       //GD32Q32
        case 0x17:                                       //GD32Q64
        case 0x18:                                       //GD32Q128
            mode = mode >> 8;
            mode |= 0x02;
            sCommand.instruction = WRITE_STATUS_REG_CMD;//write flash status[s15-s8]
            sCommand.addressmode = QSPI_ADDRESS_NONE;
            sCommand.address     = 0;
            sCommand.datamode    = QSPI_DATA_1_LINE;
            sCommand.nbdata      = 1;
            sCommand.dummycycles = 0;
            qspi_command(&sCommand);
            qspi_transmit((uint8_t *)&mode);
            break;
        case 0x14:                                       //GD32Q80
        case 0x15:                                       //GD32Q16
        case 0x19:                                       //GD32Q256
        default:
            mode |= (0x0200);
            sCommand.instruction = WRITE_STATUS_REG1_CMD;//write flash status[s15-s0]
            sCommand.addressmode = QSPI_ADDRESS_NONE;
            sCommand.address     = 0;
            sCommand.datamode    = QSPI_DATA_1_LINE;
            sCommand.nbdata      = 2;
            sCommand.dummycycles = 0;
            qspi_command(&sCommand);
            qspi_transmit((uint8_t *)&mode);
            break;
        }

        qspi_flash_autopolling_ready();
    }
}

/*!
    \brief      configure qspi flash
    \param[in]  clock_prescaler: prescaler of qspi clock
    \param[out] none
    \retval     none
*/
void qspi_flash_config(uint32_t clock_prescaler)
{
    qspi_init_struct Init;
    qspi_command_struct sCommand;

    // rcu_periph_clock_enable(RCU_GTZC);
    // tzgpc_tzspc_peripheral_attributes_config(TZGPC_PERIPH_QSPI_FLASHREG, TZGPC_TZSPC_PERIPH_SEC);

    qspi_flash_gpio_config();

    // qspi_deinit();
    rcu_periph_clock_enable(RCU_QSPI);

    Init.prescaler               = clock_prescaler;  /* QSPI clock = AHBCLK/(ClockPrescaler+1) */
    Init.fifothreshold           = 4;
    Init.sampleshift             = QSPI_SAMPLE_SHIFTING_HALFCYCLE;  // QSPI_SAMPLE_SHIFTING_NONE;
    Init.flashsize               = 23;  /* 2^(FlashSize+1) ***** number of address bits = FlashSize + 1*/
    Init.chipselecthightime      = QSPI_CS_HIGH_TIME_1_CYCLE;
    Init.clockmode               = QSPI_CLOCK_MODE_0;
    qspi_init(&Init);

    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = 0;
    sCommand.addressmode         = QSPI_ADDRESS_NONE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = 0;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_NONE;
    sCommand.nbdata              = 0;
    // sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;

    qspi_flash_set_quad();

}

/*!
    \brief      read qspi flash device id
    \param[in]  None
    \param[out] None
    \retval     0
*/
int32_t qspi_flash_read_id(void *id)
{
    qspi_command_struct sCommand = {0};

    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = CHIP_READ_ID_CMD;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_1_LINE;
    sCommand.nbdata              = 3;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command(&sCommand);
    qspi_receive(id);

    return 0;
}

/*!
    \brief      erase qspi flash sector
    \param[in]  address: qspi flash address
    \param[out] none
    \retval     0
*/
int32_t qspi_flash_sector_erase(uint32_t address)
{
    qspi_command_struct sCommand;

    qspi_flash_write_enable();
    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = SECTOR_ERASE_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_1_LINE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = address;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_NONE;
    sCommand.nbdata              = 0;
    // sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command(&sCommand);
    qspi_flash_autopolling_ready();

    return 0;
}

/*!
    \brief      erase qspi flash full chip
    \param[in]  none
    \param[out] none
    \retval     0
*/
int32_t qspi_flash_chip_erase(void)
{
    qspi_command_struct sCommand;

    qspi_flash_write_enable();
    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = CHIP_ERASE_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_NONE;
    sCommand.addresssize         = QSPI_ADDRESS_8_BITS;
    sCommand.address             = 0;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_NONE;
    sCommand.nbdata              = 0;
    // sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command(&sCommand);
    qspi_flash_autopolling_ready();

    return 0;
}

/*!
    \brief      read qspi flash
    \param[in]  adress: flash's internal address to read from
    \param[out] data: pointer to the buffer that receives the data read from the flash
    \param[in]  size: size of data read from flash
    \retval     0
*/
int32_t qspi_flash_read(uint32_t address, void *data, uint32_t size)
{
    qspi_command_struct sCommand;

    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = QUAD_OUT_FAST_READ_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_1_LINE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = address;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_24_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 8;
    sCommand.datamode            = QSPI_DATA_4_LINES;
    sCommand.nbdata              = size;
    // sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command(&sCommand);
    qspi_receive(data);

    return 0;
}

/*!
    \brief      program qspi flash page
    \param[in]  adress: flash's internal address to write to
    \param[in]  data: pointer to the buffer that receives the data read from the flash
    \param[in]  size: size of data read from flash
    \param[out] none
    \retval     0
*/
int32_t qspi_flash_page_program(uint32_t address, const uint8_t *data, uint32_t size)
{
    qspi_command_struct sCommand;

    /* Writing Sequence 4 Line------------------------------------------ */
    qspi_flash_write_enable();
    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = QUAD_IN_FAST_PROG_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_1_LINE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = address;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_4_LINES;
    sCommand.nbdata              = size;
    // sCommand.DdrMode             = QSPI_DDR_MODE_DISABLE;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command(&sCommand);
    qspi_transmit((uint8_t *)data);
    qspi_flash_autopolling_ready();

    return 0;
}

/*!
    \brief      read flash
    \param[in]  offset: flash offset
    \param[out] data: pointer to the buffer store flash read data
    \param[in]  len: length of data read from flash
    \retval     result of read flash(0: read ok, or -1: read error)
*/
int flash_read(uint32_t offset, void *data, int len)
{
    uint32_t i;
    uint32_t dst = (uint32_t)data;
    uint32_t left;

    if (!is_valid_flash_offset(offset) || data == NULL
        || len <= 0 || !is_valid_flash_offset(offset + len - 1)) {
        return -1;
    }

    if ((offset & 3) || (dst & 3)) {
        for (i = 0; i < len; i++) {
            ((uint8_t *)data)[i] = *(uint8_t *)(FLASH_BASE + offset + i);
        }
    } else {
        left = (len & 3);
        len -= left;
        for (i = 0; i < len; i += 4) {
            *(uint32_t *)(dst + i) = *(uint32_t *)(FLASH_BASE + offset + i);
        }
        if (left > 0)
            memcpy((uint8_t *)(dst + len), (uint8_t *)(FLASH_BASE + offset + len), left);
    }
    return 0;
}

/*!
    \brief      read flash indirectly
                consider encrypted image, two flash read api will be needed,
                one is directly read and flash value is decrypted, another is indirect
                read and no decryption is done.
    \param[in]  offset: flash offset
    \param[out] data: pointer to the buffer store flash read data
    \param[in]  len: length of data read from flash
    \retval     result of read flash(0: read ok, or -1: read error)
*/
int flash_indirect_read(uint32_t offset, void *data, int len)
{
    int cache_enabled = 0;
    int ret;

    if (is_flash_cache_enabled()) {
        flash_cache_disable();
        cache_enabled = 1;
    }

    if (is_sip_flash()) {
        ret = flash_read(offset, data, len);

    } else {
        if (!is_valid_flash_offset(offset) || data == NULL
            || len <= 0 || !is_valid_flash_offset(offset + len - 1)) {
            ret = -1;
            goto Exit;
        }
        __disable_irq();
        ret = qspi_flash_read(FLASH_START_QSPI + offset, data, len);
        __enable_irq();
    }

Exit:
    if (cache_enabled)
        flash_cache_enable();
    return ret;
}

/*!
    \brief      write flash
    \param[in]  offset: flash offset
    \param[in]  data: pointer to the data write to flash
    \param[in]  len: length of data write to flash
    \param[out] none
    \retval     result of write flash(0: write ok, or -1: write error)
*/
int flash_write(uint32_t offset, const void *data, int len)
{
    uint8_t *data_u8 = (uint8_t *)data;
    uint32_t base_addr;

    if (!is_valid_flash_offset(offset) || data == NULL
        || len <= 0 || !is_valid_flash_offset(offset + len - 1)) {
        return -1;
    }

    if (is_sip_flash()) {
        uint32_t offset_align, val32;
        int vb, act_len, i;
        uint8_t val[4] = {0xFF, 0xFF, 0xFF, 0xFF};

        base_addr = FLASH_BASE;
        /* unlock the flash program erase controller */
        fmc_unlock();
        /* clear pending flags */
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR);

        offset_align = (offset & ~0x3);

        /* if offset is not 4-byte alignment */
        vb = (offset & 0x3);
        if (vb > 0) {
            act_len = ((4 - vb) > len) ? len : (4 - vb);
            for (i = 0; i < act_len; i++) {
                val[vb + i] = *(data_u8 + i);
            }
            fmc_word_program((base_addr + offset_align), *(uint32_t *)val);
            offset_align += 4;
            data_u8 += act_len;
            len -= act_len;
        }

        /* word program */
        while (len >= 4) {
            fmc_word_program((base_addr + offset_align), *(uint32_t *)data_u8);
            offset_align += 4;
            data_u8 += 4;
            len -= 4;
        }

        /* if len is not 4-byte alignment */
        val32 = 0xFFFFFFFF;
        if (len > 0) {
            while (len-- > 0) {
                val32 = (val32 << 8);
                val32 |= *(data_u8 + len);
            }
            fmc_word_program((base_addr + offset_align), val32);
        }

        /* lock the flash program erase controller */
        fmc_lock();
    } else {
        uint32_t page_offset;
        uint32_t size_to_program;

        base_addr = FLASH_START_QSPI;
        page_offset = (offset & (FLASH_QSPI_PAGE_SIZE - 1));
        if (page_offset != 0) {
            size_to_program = (len > FLASH_QSPI_PAGE_SIZE - page_offset) ? (FLASH_QSPI_PAGE_SIZE - page_offset) : len;
            __disable_irq();
            qspi_flash_page_program((base_addr + offset), data_u8, size_to_program);
            __enable_irq();
            offset += size_to_program;
            data_u8 += size_to_program;
            len -= size_to_program;
        }

        while (len > 0) {
            size_to_program = (len > FLASH_QSPI_PAGE_SIZE) ? FLASH_QSPI_PAGE_SIZE : len;
            __disable_irq();
            qspi_flash_page_program((base_addr + offset), data_u8, size_to_program);
            __enable_irq();
            offset += size_to_program;
            data_u8 += size_to_program;
            len -= size_to_program;
        }
    }
    return 0;
}

/*!
    \brief      write flash fast
    \param[in]  offset: flash offset
    \param[in]  data: pointer to the data write to flash
    \param[in]  len: length of data write to flash
    \param[out] none
    \retval     result of write flash(0: write ok, or -1: write error)
*/
int flash_fast_write(uint32_t offset, const void *data, int len)
{
    uint8_t *data_u8 = (uint8_t *)data;
    uint32_t base_addr;
    int ret = 0;

    if (!is_valid_flash_offset(offset) || data == NULL
        || len <= 0 || !is_valid_flash_offset(offset + len - 1)) {
        return -1;
    }

    if (is_sip_flash()) {
        uint8_t r, rr;
        if (len <= 4)
            return flash_write(offset, data, len);

        r = 4 - (offset & 0x3);
        rr = (offset + len) & 0x3;
        r = r == 4 ? 0 : r;
        if (r) {
            ret = flash_write(offset, data, r);
            if (ret)
                return ret;
        }

        /* unlock the flash program erase controller */
        fmc_unlock();
        /* clear pending flags */
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_WPERR);

        /* prevent interrupt handler from reading flash, it will disrupt the flash continuous programming pipeline */
        __disable_irq();
        ret = fmc_continuous_program(FLASH_BASE + offset + r, (uint32_t *)((uint8_t *)data + r), len - r - rr);
        __enable_irq();

        /* lock the flash program erase controller */
        fmc_lock();

        if (rr) {
            ret = flash_write(offset + len - rr, ((uint8_t *)data + len - rr), rr);
            if (ret)
                return ret;
        }
    } else {
        uint32_t page_offset;
        uint32_t size_to_program;

        base_addr = FLASH_START_QSPI;
        page_offset = (offset & (FLASH_QSPI_PAGE_SIZE - 1));
        if (page_offset != 0) {
            size_to_program = (len > FLASH_QSPI_PAGE_SIZE - page_offset) ? (FLASH_QSPI_PAGE_SIZE - page_offset) : len;
            __disable_irq();
            qspi_flash_page_program((base_addr + offset), data_u8, size_to_program);
            __enable_irq();
            offset += size_to_program;
            data_u8 += size_to_program;
            len -= size_to_program;
        }

        while (len > 0) {
            size_to_program = (len > FLASH_QSPI_PAGE_SIZE) ? FLASH_QSPI_PAGE_SIZE : len;
            __disable_irq();
            qspi_flash_page_program((base_addr + offset), data_u8, size_to_program);
            __enable_irq();
            offset += size_to_program;
            data_u8 += size_to_program;
            len -= size_to_program;
        }
    }
    return ret;
}

/*!
    \brief      erase flash
    \param[in]  offset: flash offset
    \param[in]  len: flash erase length
    \param[out] none
    \retval     result of erase flash(0: erase ok, or -1: erase error)
*/
int flash_erase(uint32_t offset, int len)
{
    int ret;
    uint32_t erase_sz = flash_erase_size();
    uint32_t page_start, sector_start;

    if (!is_valid_flash_offset(offset)
        || len <= 0 || !is_valid_flash_offset(offset + len - 1)) {
        return -1;
    }

    if (is_sip_flash()) {
        /* Get page start */
        page_start = FLASH_BASE + (offset & (~(erase_sz - 1)));

        /* unlock the flash program erase controller */
        fmc_unlock();

        /* clear pending flags */
        fmc_flag_clear(FMC_FLAG_END | FMC_FLAG_OBERR | FMC_FLAG_WPERR);

        while (len > 0) {
            /* erase page */
            ret = fmc_page_erase(page_start);
            if (ret != FMC_READY)
                return -2;
            page_start += erase_sz;
            len -= erase_sz;
        }
        /* lock the flash program erase controller */
        fmc_lock();
    } else {
        sector_start = FLASH_START_QSPI + (offset & (~(erase_sz - 1)));
        while (len > 0) {
            __disable_irq();
            qspi_flash_sector_erase(sector_start);
            __enable_irq();
            sector_start += erase_sz;
            len -= erase_sz;
        }
    }
    return 0;
}
