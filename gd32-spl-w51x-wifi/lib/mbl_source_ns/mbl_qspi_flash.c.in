/*!
    \file    mbl_qspi_flash.c
    \brief   Non-secure MBL qspi flash file for GD32W51x WiFi SDK

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

#include "gd32w51x.h"
#include "mbl_qspi_flash.h"
#include "platform_def.h"

/*!
    \brief      configure qspi flash gpio
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
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;

    sConfig.match                = 0x00;
    sConfig.mask                 = 0x01;
    sConfig.matchmode            = QSPI_MATCH_MODE_AND;
    sConfig.statusbytessize      = 1;
    sConfig.interval             = 0x10;
    sConfig.automaticstop        = QSPI_AUTOMATIC_STOP_ENABLE;

    qspi_autopolling(&sCommand, &sConfig);
}

static void qspi_tcfg_fmc_set(void)
{
    qspi_command_struct sCommand;

#if ( QSPI_FLASH_MODE == QSPI_FLASH_4_LINES )
    sCommand.instructionmode     = TCFGF_IMODF(0x1);                            // 1 line
    sCommand.instruction         = TCFGF_INSTRUCTIONF(QUAD_IO_FAST_READ_CMD);   // 0xeb
    sCommand.addressmode         = TCFGF_ADDRMODF(0x3);                         // 4 lines
    sCommand.addresssize         = TCFGF_ADDRSZF(0x2);                          // 24 bits address
    sCommand.address             = 0;
    sCommand.alternatebytemode   = TCFGF_ALTEMODF(0x3);                         // 4 lines
    sCommand.alternatebytessize  = TCFGF_ALTESZF(0);                            // 8 bits
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = TCFGF_DUMYCF(0x4);                           // 4 dummy cycles
    sCommand.datamode            = TCFGF_DATAMODF(0x3);                         // 4 lines
    sCommand.nbdata              = 0;
    sCommand.sioomode            = TCFGF_SIOOF(0);
#elif ( QSPI_FLASH_MODE == QSPI_FLASH_2_LINES )
    sCommand.instructionmode     = TCFGF_IMODF(0x1);                            // 1 line
    sCommand.instruction         = TCFGF_INSTRUCTIONF(DUAL_IO_FAST_READ_CMD);   // 0xBB
    sCommand.addressmode         = TCFGF_ADDRMODF(0x2);                         // 2 lines
    sCommand.addresssize         = TCFGF_ADDRSZF(0x2);                          // 24 bits address
    sCommand.address             = 0;
    sCommand.alternatebytemode   = TCFGF_ALTEMODF(0x2);                         // 2 lines
    sCommand.alternatebytessize  = TCFGF_ALTESZF(0);                            // 8 bits
    sCommand.alternatebytes      = 0;
    /*
        according to GD25Q16C datasheet, there are 4 alternate bits and 2 dummy cycles for 0xBB cmd,
        but GD32W51x QSPI mode has alternate bits's limit to 8bits, so set sCommand.alternatebytessize
        to 8, so alternate data[3:0]'s transfer costs 2 cycles on two lines transfer, so set
        sCommand.dummycycles to 0(which not means there is no dummy cycles)
    */
    sCommand.dummycycles         = TCFGF_DUMYCF(0);                             // 0 dummy cycles
    sCommand.datamode            = TCFGF_DATAMODF(0x2);                         // 2 lines
    sCommand.nbdata              = 0;
    sCommand.sioomode            = TCFGF_SIOOF(0);
#elif ( QSPI_FLASH_MODE == QSPI_FLASH_1_LINE )
    sCommand.instructionmode     = TCFGF_IMODF(0x1);                            // 1 line
    sCommand.instruction         = TCFGF_INSTRUCTIONF(READ_DATA_BYTE_CMD);      // 0x03
    sCommand.addressmode         = TCFGF_ADDRMODF(0x1);                         // 1 line
    sCommand.addresssize         = TCFGF_ADDRSZF(0x2);                          // 24 bits address
    sCommand.address             = 0;
    sCommand.alternatebytemode   = TCFGF_ALTEMODF(0);                           // no alternate byte
    sCommand.alternatebytessize  = TCFGF_ALTESZF(0);                            // 8 bits
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = TCFGF_DUMYCF(0);                             // 0 dummy cycles
    sCommand.datamode            = TCFGF_DATAMODF(0x1);                         // 1 line
    sCommand.nbdata              = 0;
    sCommand.sioomode            = TCFGF_SIOOF(0);
#endif

    while((QSPI_CTL & QSPI_FLAG_BUSY) != 0U){
    }
    QSPI_TCFGF = sCommand.instructionmode | sCommand.instruction | sCommand.addressmode | \
           sCommand.addresssize | sCommand.address | sCommand.alternatebytemode | sCommand.alternatebytes | \
           sCommand.dummycycles | sCommand.datamode | sCommand.nbdata | sCommand.sioomode;
}

/*!
    \brief      configure qspi flash to quad mode
    \param[in]  none
    \param[out] none
    \retval     none
*/
static void qspi_flash_quad_enable_set(void)
{
    uint32_t id = 0;
    uint8_t mode_s = 0;
    uint16_t mode = 0;
    qspi_command_struct sCommand;

    qspi_flash_read_id(&id);

    qspi_flash_write_enable();

    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = 0;
    sCommand.addressmode         = QSPI_ADDRESS_NONE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = 0;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_1_LINE;
    sCommand.nbdata              = 1;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;

    sCommand.instruction         = READ_STATUS_REG1_CMD;
    qspi_command(&sCommand);
    qspi_receive(&mode_s);

    mode |= (uint16_t)mode_s;

    sCommand.instruction         = READ_STATUS_REG_CMD;
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
            qspi_command(&sCommand);
            qspi_transmit((uint8_t *)&mode);
            break;
        case 0x14:                                       //GD32Q80
        case 0x15:                                       //GD32Q16
        case 0x19:                                       //GD32Q256
            /* To fully support GD32Q256, user has to set bit8(ADS) of Status Registers
                to enable 4-bytes Address Mode, which enabled by cmd 0xB7(the Enter 4-Byte
                Address Mode command enables accessing the address length of 32-bit for
                the memory area of higher density (larger than 128Mb), The device default
                is in 24-bit address mode); and then change all qspi related operations's
                address size from QSPI_ADDRESS_24_BITS to QSPI_ADDRESS_32_BITS. */
        default:
            mode |= (0x0200);
            sCommand.instruction = WRITE_STATUS_REG1_CMD;//write flash status[s15-s0]
            sCommand.nbdata      = 2;
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

    qspi_flash_quad_enable_set();
    qspi_tcfg_fmc_set();
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
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
    qspi_command(&sCommand);
    qspi_flash_autopolling_ready();

    return 0;
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
    \brief      read qspi flash
    \param[in]  adress: flash's internal address to read from
    \param[out] data: pointer to the buffer that receives the data read from the flash
    \param[in]  size: size of data read from flash
    \retval     0
*/
int32_t qspi_flash_read(uint32_t address, void *data, uint32_t size)
{
    qspi_command_struct sCommand;

#if ( QSPI_FLASH_MODE == QSPI_FLASH_4_LINES )
    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = QUAD_IO_FAST_READ_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_4_LINES;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = address;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_4_LINES;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 4;
    sCommand.datamode            = QSPI_DATA_4_LINES;
    sCommand.nbdata              = size;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
#elif ( QSPI_FLASH_MODE == QSPI_FLASH_2_LINES )
    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = DUAL_IO_FAST_READ_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_2_LINES;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = address;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_2_LINES;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    /*
        according to GD25Q16C datasheet, there are 4 alternate bits and 2 dummy cycles for 0xBB cmd,
        but GD32W51x QSPI mode has alternate bits's limit to 8bits, so set sCommand.alternatebytessize
        to 8, so alternate data[3:0]'s transfer costs 2 cycles on two lines transfer, so set
        sCommand.dummycycles to 0(which not means there is no dummy cycles)
    */
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_2_LINES;
    sCommand.nbdata              = size;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
#elif ( QSPI_FLASH_MODE == QSPI_FLASH_1_LINE )
    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = READ_DATA_BYTE_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_1_LINE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = address;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_1_LINE;
    sCommand.nbdata              = size;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
#endif

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

#if ( QSPI_FLASH_MODE == QSPI_FLASH_4_LINES )
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
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
#elif ( QSPI_FLASH_MODE == QSPI_FLASH_2_LINES ) || ( QSPI_FLASH_MODE == QSPI_FLASH_1_LINE )
    sCommand.instructionmode     = QSPI_INSTRUCTION_1_LINE;
    sCommand.instruction         = PAGE_PROG_CMD;
    sCommand.addressmode         = QSPI_ADDRESS_1_LINE;
    sCommand.addresssize         = QSPI_ADDRESS_24_BITS;
    sCommand.address             = address;
    sCommand.alternatebytemode   = QSPI_ALTERNATE_BYTES_NONE;
    sCommand.alternatebytessize  = QSPI_ALTERNATE_BYTES_8_BITS;
    sCommand.alternatebytes      = 0;
    sCommand.dummycycles         = 0;
    sCommand.datamode            = QSPI_DATA_1_LINE;
    sCommand.nbdata              = size;
    sCommand.sioomode            = QSPI_SIOO_INST_EVERY_CMD;
#endif
    qspi_command(&sCommand);
    qspi_transmit((uint8_t *)data);
    qspi_flash_autopolling_ready();

    return 0;
}
