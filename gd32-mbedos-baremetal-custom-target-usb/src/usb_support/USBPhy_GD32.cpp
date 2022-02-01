/* mbed Microcontroller Library
 * Copyright (c) 2018-2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 * Copyright (c) 2018-2019 STMicroelectronics
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#if DEVICE_USBDEVICE

#include "USBPhyHw.h"
#include "pinmap.h"

/* endpoint conversion macros */
#define EP_TO_LOG(ep)       ((ep) & 0xF)
#define LOG_IN_TO_EP(log)   ((log) | 0x80)
#define LOG_OUT_TO_EP(log)  ((log) | 0x00)
#define EP_TO_IDX(ep)       ((EP_TO_LOG(ep) << 1) | ((ep) & 0x80 ? 1 : 0))
#define IDX_TO_EP(idx)      (((idx) >> 1)|((idx) & 1) << 7)

/* endpoint defines */

#if (MBED_CONF_TARGET_USB_SPEED == USE_USB_OTG_HS)

#define NUM_ENDPOINTS                6
#define MAX_PACKET_SIZE_NON_ISO      512
#define MAX_PACKET_SIZE_ISO          1023

#else

#define NUM_ENDPOINTS                4
#define MAX_PACKET_SIZE_NON_ISO      64
#define MAX_PACKET_SIZE_ISO          (256 + 128)     // Spec can go up to 1023, only ram for this though

#endif

static const uint32_t tx_ep_sizes[NUM_ENDPOINTS] = {
    MAX_PACKET_SIZE_NON_ISO,
    MAX_PACKET_SIZE_NON_ISO,
    MAX_PACKET_SIZE_NON_ISO,
#if (MBED_CONF_TARGET_USB_SPEED == USE_USB_OTG_HS)
    MAX_PACKET_SIZE_NON_ISO,
    MAX_PACKET_SIZE_NON_ISO,
#endif
    MAX_PACKET_SIZE_ISO
};

static USBPhyHw *instance;

extern "C" {
#include "usb_lib/usbd_core.h"
#include "usb_lib/usbd_transc.h"
#include "usb_lib/usbd_enum.h"
#include "usb_lib/usbd_lld_int.h"
}

usb_dev usbd_device;


/* communications device class code */
#define USB_CLASS_CDC                           0x02U

/* CDC subclass code */
#define USB_CDC_SUBCLASS_DLCM                   0x01U
#define USB_CDC_SUBCLASS_ACM                    0x02U

/* communications interface class control protocol codes */
#define USB_CDC_PROTOCOL_NONE                   0x00U
#define USB_CDC_PROTOCOL_AT                     0x01U

/* data interface class code */
#define USB_CLASS_DATA                          0x0AU

#define USB_DESCTYPE_CDC_ACM                    0x21U
#define USB_DESCTYPE_CS_INTERFACE               0x24U

#define USB_CDC_ACM_CONFIG_DESC_SIZE            0x43U

/* class-specific notification codes for PSTN subclasses */
#define USB_CDC_NOTIFY_SERIAL_STATE             0x20U

/* class-specific request codes */
#define SEND_ENCAPSULATED_COMMAND               0x00U
#define GET_ENCAPSULATED_RESPONSE               0x01U
#define SET_COMM_FEATURE                        0x02U
#define GET_COMM_FEATURE                        0x03U
#define CLEAR_COMM_FEATURE                      0x04U
#define SET_LINE_CODING                         0x20U
#define GET_LINE_CODING                         0x21U
#define SET_CONTROL_LINE_STATE                  0x22U
#define SEND_BREAK                              0x23U
#define NO_CMD                                  0xFFU

#pragma pack(1)

/* CDC ACM line coding struct */
typedef struct {
    uint32_t dwDTERate;                   /*!< data terminal rate */
    uint8_t  bCharFormat;                 /*!< stop bits */
    uint8_t  bParityType;                 /*!< parity */
    uint8_t  bDataBits;                   /*!< data bits */
} acm_line;

/* notification structure */
typedef struct {
    uint8_t bmRequestType;                /*!< type of request */
    uint8_t bNotification;                /*!< communication interface class notifications */
    uint16_t wValue;                      /*!< value of notification */
    uint16_t wIndex;                      /*!< index of interface */
    uint16_t wLength;                     /*!< length of notification data */
} acm_notification;

/* header function struct */
typedef struct {
    usb_desc_header header;               /*!< descriptor header, including type and size. */
    uint8_t   bDescriptorSubtype;         /*!< bDescriptorSubtype: header function descriptor */
    uint16_t  bcdCDC;                     /*!< bcdCDC: low byte of spec release number (CDC1.10) */
} usb_desc_header_func;

/* call managment function struct */
typedef struct {
    usb_desc_header header;               /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype:  call management function descriptor */
    uint8_t  bmCapabilities;              /*!< bmCapabilities: D0 is reset, D1 is ignored */
    uint8_t  bDataInterface;              /*!< bDataInterface: 1 interface used for call management */
} usb_desc_call_managment_func;

/* acm function struct */
typedef struct {
    usb_desc_header header;               /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype: abstract control management descriptor */
    uint8_t  bmCapabilities;              /*!< bmCapabilities: D1 */
} usb_desc_acm_func;

/* union function struct */
typedef struct {
    usb_desc_header header;               /*!< descriptor header, including type and size. */
    uint8_t  bDescriptorSubtype;          /*!< bDescriptorSubtype: union function descriptor */
    uint8_t  bMasterInterface;            /*!< bMasterInterface: communication class interface */
    uint8_t  bSlaveInterface0;            /*!< bSlaveInterface0: data class interface */
} usb_desc_union_func;

#pragma pack()
/* configuration descriptor struct */
typedef struct {
    usb_desc_config                  config;
    usb_desc_itf                     cmd_itf;
    usb_desc_header_func             cdc_header;
    usb_desc_call_managment_func     cdc_call_managment;
    usb_desc_acm_func                cdc_acm;
    usb_desc_union_func              cdc_union;
    usb_desc_ep                      cdc_cmd_endpoint;
    usb_desc_itf                     cdc_data_interface;
    usb_desc_ep                      cdc_out_endpoint;
    usb_desc_ep                      cdc_in_endpoint;
} usb_cdc_desc_config_set;

#define USB_CDC_RX_LEN      64U

typedef struct {
    uint8_t pre_packet_send;
    uint8_t packet_sent;
    uint8_t packet_receive;

    uint8_t data[USB_CDC_RX_LEN];

    uint32_t receive_length;

    acm_line line_coding;
} usb_cdc_handler;

#define USBD_VID                          0x28E9U
#define USBD_PID                          0x018AU

/* note:it should use the C99 standard when compiling the below codes */
/* USB standard device descriptor */
usb_desc_dev cdc_dev_desc =
{
    .header = 
     {
         .bLength          = USB_DEV_DESC_LEN, 
         .bDescriptorType  = USB_DESCTYPE_DEV,
     },
    .bcdUSB                = 0x0200U,
    .bDeviceClass          = USB_CLASS_CDC,
    .bDeviceSubClass       = 0x00U,
    .bDeviceProtocol       = 0x00U,
    .bMaxPacketSize0       = USBD_EP0_MAX_SIZE,
    .idVendor              = USBD_VID,
    .idProduct             = USBD_PID,
    .bcdDevice             = 0x0100U,
    .iManufacturer         = STR_IDX_MFC,
    .iProduct              = STR_IDX_PRODUCT,
    .iSerialNumber         = STR_IDX_SERIAL,
    .bNumberConfigurations = USBD_CFG_MAX_NUM,
};

/* USB device configuration descriptor */
usb_cdc_desc_config_set cdc_config_desc = 
{
    .config = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_config), 
             .bDescriptorType = USB_DESCTYPE_CONFIG,
         },
        .wTotalLength         = USB_CDC_ACM_CONFIG_DESC_SIZE,
        .bNumInterfaces       = 0x02U,
        .bConfigurationValue  = 0x01U,
        .iConfiguration       = 0x00U,
        .bmAttributes         = 0x80U,
        .bMaxPower            = 0x32U
    },

    .cmd_itf = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_itf), 
             .bDescriptorType = USB_DESCTYPE_ITF 
         },
        .bInterfaceNumber     = 0x00U,
        .bAlternateSetting    = 0x00U,
        .bNumEndpoints        = 0x01U,
        .bInterfaceClass      = USB_CLASS_CDC,
        .bInterfaceSubClass   = USB_CDC_SUBCLASS_ACM,
        .bInterfaceProtocol   = USB_CDC_PROTOCOL_AT,
        .iInterface           = 0x00U
    },

    .cdc_header = 
    {
        .header =
         {
            .bLength         = sizeof(usb_desc_header_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x00U,
        .bcdCDC              = 0x0110U
    },

    .cdc_call_managment = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_call_managment_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x01U,
        .bmCapabilities      = 0x00U,
        .bDataInterface      = 0x01U
    },

    .cdc_acm = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_acm_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x02U,
        .bmCapabilities      = 0x02U,
    },

    .cdc_union = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_union_func), 
            .bDescriptorType = USB_DESCTYPE_CS_INTERFACE
         },
        .bDescriptorSubtype  = 0x06U,
        .bMasterInterface    = 0x00U,
        .bSlaveInterface0    = 0x01U,
    },

    .cdc_cmd_endpoint = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_ep), 
            .bDescriptorType = USB_DESCTYPE_EP,
         },
        .bEndpointAddress    = CDC_CMD_EP,
        .bmAttributes        = USB_EP_ATTR_INT,
        .wMaxPacketSize      = CDC_ACM_CMD_PACKET_SIZE,
        .bInterval           = 0x0AU
    },

    .cdc_data_interface = 
    {
        .header = 
         {
            .bLength         = sizeof(usb_desc_itf), 
            .bDescriptorType = USB_DESCTYPE_ITF,
         },
        .bInterfaceNumber    = 0x01U,
        .bAlternateSetting   = 0x00U,
        .bNumEndpoints       = 0x02U,
        .bInterfaceClass     = USB_CLASS_DATA,
        .bInterfaceSubClass  = 0x00U,
        .bInterfaceProtocol  = USB_CDC_PROTOCOL_NONE,
        .iInterface          = 0x00U
    },

    .cdc_out_endpoint = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP, 
         },
        .bEndpointAddress     = CDC_OUT_EP,
        .bmAttributes         = USB_EP_ATTR_BULK,
        .wMaxPacketSize       = CDC_ACM_DATA_PACKET_SIZE,
        .bInterval            = 0x00U
    },

    .cdc_in_endpoint = 
    {
        .header = 
         {
             .bLength         = sizeof(usb_desc_ep), 
             .bDescriptorType = USB_DESCTYPE_EP 
         },
        .bEndpointAddress     = CDC_IN_EP,
        .bmAttributes         = USB_EP_ATTR_BULK,
        .wMaxPacketSize       = CDC_ACM_DATA_PACKET_SIZE,
        .bInterval            = 0x00U
    }
};

/* USB language ID descriptor */
static usb_desc_LANGID usbd_language_id_desc = 
{
    .header = 
     {
         .bLength         = sizeof(usb_desc_LANGID), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .wLANGID              = ENG_LANGID
};

/* USB manufacture string */
static usb_desc_str manufacturer_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(10U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .unicode_string = {'G', 'i', 'g', 'a', 'D', 'e', 'v', 'i', 'c', 'e'}
};

/* USB product string */
static usb_desc_str product_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(12U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     },
    .unicode_string = {'G', 'D', '3', '2', '-', 'C', 'D', 'C', '_', 'A', 'C', 'M'}
};

/* USBD serial string */
static usb_desc_str serial_string = 
{
    .header = 
     {
         .bLength         = USB_STRING_LEN(12U), 
         .bDescriptorType = USB_DESCTYPE_STR,
     }
};

/* USB string descriptor set */
uint8_t* usbd_cdc_strings[] = 
{
    [STR_IDX_LANGID]  = (uint8_t *)&usbd_language_id_desc,
    [STR_IDX_MFC]     = (uint8_t *)&manufacturer_string,
    [STR_IDX_PRODUCT] = (uint8_t *)&product_string,
    [STR_IDX_SERIAL]  = (uint8_t *)&serial_string
};

usb_desc usb_device_desc = {
    .dev_desc    = (uint8_t *)&cdc_dev_desc,
    .config_desc = (uint8_t *)&cdc_config_desc,
    .strings     = usbd_cdc_strings
};

extern usb_class usb_class_callbacks;

static uint8_t usb_device_init         (usb_dev *udev, uint8_t config_index) {
    /* initialize the data endpoints */
    usbd_ep_init(udev, EP_BUF_SNG, BULK_TX_ADDR, &(cdc_config_desc.cdc_in_endpoint));
    usbd_ep_init(udev, EP_BUF_SNG, BULK_RX_ADDR, &(cdc_config_desc.cdc_out_endpoint));

    /* initialize the command endpoint */
    usbd_ep_init(udev, EP_BUF_SNG, INT_TX_ADDR, &(cdc_config_desc.cdc_cmd_endpoint));

    udev->ep_transc[EP_ID(CDC_IN_EP)][TRANSC_IN] = usb_class_callbacks.data_in;
    udev->ep_transc[CDC_OUT_EP][TRANSC_OUT] = usb_class_callbacks.data_out;

    return USBD_OK;
}
static uint8_t usb_device_deinit       (usb_dev *udev, uint8_t config_index) {
    return USBD_OK;
}
static uint8_t usb_device_req_handler  (usb_dev *udev, usb_req *req) {
    return REQ_NOTSUPP;
}
static uint8_t usb_device_ctlx_out     (usb_dev *udev) {
    return USBD_OK;
}
static void usb_device_data_in         (usb_dev *udev, uint8_t ep_num) {}
static void usb_device_data_out        (usb_dev *udev, uint8_t ep_num) {}

usb_class usb_class_callbacks = {
    .req_cmd       = NO_CMD,

    .init          = usb_device_init,
    .deinit        = usb_device_deinit,
    .req_process   = usb_device_req_handler,
    .ctlx_out      = usb_device_ctlx_out,
    .data_in       = usb_device_data_in,
    .data_out      = usb_device_data_out
};

USBPhy *get_usb_phy()
{
    static USBPhyHw usbphy;
    return &usbphy;
}

USBPhyHw::USBPhyHw(): events(NULL), sof_enabled(false)
{

}

USBPhyHw::~USBPhyHw()
{

}

#if defined(TARGET_GD32F30X)

#include "drivers/DigitalOut.h"

void USB_reenumerate()
{
    // Force USB_DP pin (with external pull up) to 0
    mbed::DigitalOut usb_dp_pin(USB_DP, 0) ;
    wait_us(10000); // 10ms
}
#endif

void setup_usb_clock(void)
{
    uint32_t system_clock = rcu_clock_freq_get(CK_SYS);

    /* enable USB pull-up pin clock */ 
    //rcu_periph_clock_enable(RCC_AHBPeriph_GPIO_PULLUP);

    if (48000000U == system_clock) {
        rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV1);
    } else if (72000000U == system_clock) {
        rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV1_5);
    } else if (96000000U == system_clock) {
        rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV2);
    } else if (120000000U == system_clock) {
        rcu_usb_clock_config(RCU_CKUSB_CKPLL_DIV2_5);
    } else {
        /* reserved */
    }

    /* enable USB APB1 clock */
    rcu_periph_clock_enable(RCU_USBD);
}


void USBPhyHw::init(USBPhyEvents *events)
{
    setup_usb_clock();

    //printf("USBPhyHw::init() called\n");
    const PinMap *map = NULL;

    NVIC_DisableIRQ(USBHAL_IRQn);

    if (this->events == NULL) {
        sleep_manager_lock_deep_sleep();
    }
    this->events = events;
    sof_enabled = false;
    memset(epComplete, 0, sizeof(epComplete));

    //__HAL_RCC_USB_CLK_ENABLE();
    map = PinMap_USB_FS;

#if defined(TARGET_GD32F30X)
    // USB_DevConnect is empty
    USB_reenumerate();
#endif

    // Pass instance for usage inside call back
    instance = this;

    // Configure USB pins
    while (map->pin != NC) {
        pin_function(map->pin, map->function);
        map++;
    }

#if !defined(TARGET_GD32F30X)
    __HAL_RCC_PWR_CLK_ENABLE();
#endif

    /* stuff */
    usbd_init(&usbd_device, &usb_device_desc, &usb_class_callbacks);

    // Configure interrupt vector
    NVIC_SetVector(USBHAL_IRQn, (uint32_t)&_usbisr);
    NVIC_SetPriority(USBHAL_IRQn, 1);
    NVIC_EnableIRQ(USBHAL_IRQn);
}

void USBPhyHw::deinit()
{
}

bool USBPhyHw::powered()
{
    return true;
}

void USBPhyHw::connect()
{
    printf("USBPhyHw::connect() called\n");

    /* connect */
    wait_us(10000);
}

void USBPhyHw::disconnect()
{
    /* disconnect */
}

void USBPhyHw::configure()
{
    // Not needed
}

void USBPhyHw::unconfigure()
{
    // Not needed
}

void USBPhyHw::sof_enable()
{
    sof_enabled = true;
}

void USBPhyHw::sof_disable()
{
    sof_enabled = false;
}

void USBPhyHw::set_address(uint8_t address)
{
    /* todo */
}

void USBPhyHw::remote_wakeup()
{

}

const usb_ep_table_t *USBPhyHw::endpoint_table()
{
    static const usb_ep_table_t table = {
#if (MBED_CONF_TARGET_USB_SPEED != USE_USB_OTG_HS)
        512, // 512 byte for endpoint buffers but space is allocated up front
#else
        4096,
#endif
        {
            {USB_EP_ATTR_ALLOW_CTRL                         | USB_EP_ATTR_DIR_IN_AND_OUT, 0, 0},
            {USB_EP_ATTR_ALLOW_BULK | USB_EP_ATTR_ALLOW_INT | USB_EP_ATTR_DIR_IN_AND_OUT, 0, 0}, // NON ISO
            {USB_EP_ATTR_ALLOW_BULK | USB_EP_ATTR_ALLOW_INT | USB_EP_ATTR_DIR_IN_AND_OUT, 0, 0}, // NON ISO
#if (MBED_CONF_TARGET_USB_SPEED == USE_USB_OTG_HS)
            {USB_EP_ATTR_ALLOW_ALL                          | USB_EP_ATTR_DIR_IN_AND_OUT, 0, 0},
#endif
            {USB_EP_ATTR_ALLOW_ALL                          | USB_EP_ATTR_DIR_IN_AND_OUT, 0, 0},
            {0                     | USB_EP_ATTR_DIR_IN_AND_OUT,  0, 0},
            {0                     | USB_EP_ATTR_DIR_IN_AND_OUT,  0, 0},
            {0                     | USB_EP_ATTR_DIR_IN_AND_OUT,  0, 0},
            {0                     | USB_EP_ATTR_DIR_IN_AND_OUT,  0, 0},
            {0                     | USB_EP_ATTR_DIR_IN_AND_OUT,  0, 0},
            {0                     | USB_EP_ATTR_DIR_IN_AND_OUT,  0, 0},
            {0                     | USB_EP_ATTR_DIR_IN_AND_OUT,  0, 0},
            {0                     | USB_EP_ATTR_DIR_IN_AND_OUT,  0, 0},
            {0                     | USB_EP_ATTR_DIR_IN_AND_OUT,  0, 0},
            {0                     | USB_EP_ATTR_DIR_IN_AND_OUT,  0, 0},
            {0                     | USB_EP_ATTR_DIR_IN_AND_OUT,  0, 0},
#if (MBED_CONF_TARGET_USB_SPEED != USE_USB_OTG_HS)
            {0                     | USB_EP_ATTR_DIR_IN_AND_OUT,  0, 0}
#endif
        }
    };
    printf("USBPhyHw::endpoint_table() called\n");

    return &table;
}

uint32_t USBPhyHw::ep0_set_max_packet(uint32_t max_packet)
{
    // FUTURE - set endpoint 0 size and return this size
    return MAX_PACKET_SIZE_EP0;
}

// read setup packet
void USBPhyHw::ep0_setup_read_result(uint8_t *buffer, uint32_t size)
{
    if (size > MAX_PACKET_SIZE_SETUP) {
        size = MAX_PACKET_SIZE_SETUP;
    }
    //memcpy(buffer,  hpcd.Setup, size);
    //memset(hpcd.Setup, 0, MAX_PACKET_SIZE_SETUP);
}

void USBPhyHw::ep0_read(uint8_t *data, uint32_t size)
{
    //ToDo
}

uint32_t USBPhyHw::ep0_read_result()
{
    //ToDo
    return 0;
}

void USBPhyHw::ep0_write(uint8_t *buffer, uint32_t size)
{
    printf("Wanted to do an ep0_write with buffer %p size %lu\n", buffer, size);
}

void USBPhyHw::ep0_stall()
{
    endpoint_stall(0x80);
    endpoint_stall(0x00);
}

bool USBPhyHw::endpoint_add(usb_ep_t endpoint, uint32_t max_packet, usb_ep_type_t type)
{
    printf("Wanted to do an endpoin_add, endpoint = %d, max_packet = %d, type = %d \n",
           (int)endpoint, (int)max_packet, (int)type);
    //ToDo
    return false;
}

void USBPhyHw::endpoint_remove(usb_ep_t endpoint)
{
    //ToDo
}

void USBPhyHw::endpoint_stall(usb_ep_t endpoint)
{
    //ToDo
}

void USBPhyHw::endpoint_unstall(usb_ep_t endpoint)
{
    //ToDo
}

bool USBPhyHw::endpoint_read(usb_ep_t endpoint, uint8_t *data, uint32_t size)
{
    //ToDo
    return false;
}

uint32_t USBPhyHw::endpoint_read_result(usb_ep_t endpoint)
{
    //ToDo
    return 0;
}

bool USBPhyHw::endpoint_write(usb_ep_t endpoint, uint8_t *data, uint32_t size)
{
    printf("Wanted to do an endpoint_write, endpoint = %d, data = %p, size = %d \n",
           (int)endpoint, data, (int)size);

    //ToDo
    return false;
}

void USBPhyHw::endpoint_abort(usb_ep_t endpoint)
{
    //ToDo
}

void USBPhyHw::process()
{
    //HAL_PCD_IRQHandler(&instance->hpcd);
    usbd_isr();
    // Re-enable interrupt
    NVIC_ClearPendingIRQ(USBHAL_IRQn);
    NVIC_EnableIRQ(USBHAL_IRQn);
}

void USBPhyHw::_usbisr(void)
{
    NVIC_DisableIRQ(USBHAL_IRQn);
    instance->events->start_process();
}

#endif /* DEVICE_USBDEVICE */
