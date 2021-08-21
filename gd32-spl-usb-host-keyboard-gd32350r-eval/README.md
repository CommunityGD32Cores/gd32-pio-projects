# SPL USB Host Keyboard example

## Description

This is the original GigaDevice example contained in `GD32F3x0_Firmware_Library_V2.1.1\Examples\USBFS\USB_Host\usb_host_hid_keyboard_mouse\`.

The example is only intended for a GD32350R-EVAL board and makes use of the on-board LCD screen of that board.

Original README:

>  This demo is based on the GD32350R-EVAL board, and it provides a description of 
how to use the USBFS host peripheral on the GD32F3x0 devices.
>
>  When an USB Device is attached to the Host port, the device is enumerated and checked
whether it can support HID device, if the attached device is HID one, when the user
press the user key, the mouse or the keyboard application is launched.
>
>  If a mouse has been attached, moving the mouse will print the position of the mouse and 
the state of button through the serial port.
>
>  If a keyboard has been attached, pressing the keyboard will print the state of the button
through the serial port.
>
>Note: In the USB Host HID class, two layouts are defined in the usbh_hid_keybd.h file
      and could be used (Azerty and Querty)
>
>       //#define QWERTY_KEYBOARD
>       #define AZERTY_KEYBOARD
>
>  The User can eventually add his own layout by editing the HID_KEYBRD_Key array
in the usbh_hid_keybd.c file.
