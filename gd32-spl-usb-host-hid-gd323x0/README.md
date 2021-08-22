# SPL USB Host HID example

## Description

This an example firmware adapted from `GD32F3x0_Firmware_Library_V2.1.1\Examples\USBFS\USB_Host\usb_host_hid_keyboard_mouse\`.

It allows a USB HID device (a keyboard or a mouse) to be connected to the microcontroller.

**Note**: For the keyboard demo, if `PIO_USB_HOST_HID_FULL_STATE_INFO_CALLBACK` is activated in the `platformio.ini`, a QWERTZ layout decoding is performed. Otherwise, a QWERTY layout is used.

## Example output

When connecting a keyboard and is commented out `PIO_USB_HOST_HID_FULL_STATE_INFO_CALLBACK` in the `platformio.ini`:

```
USB Host HID firmware start!
USB host library started
> Reset the USB device.
> Low speed device detected.
> Device Attached.
VID: 0x1A2C
PID: 0x2124
> HID device connected.
Manufacturer: SEM
Product: USB Keyboard
> Enumeration completed.
---------------------------------------
To start the HID class operations:
Press User Key...
> HID Demo Device : Keyboard.
> Use Keyboard to type characters:
> Keyboard pressed: 'A' (41)
> Keyboard pressed: 'B' (42)
> Keyboard pressed: 'C' (43)
> Keyboard pressed: 'D' (44)
> Keyboard pressed: 'E' (45)
> Keyboard pressed: 'F' (46)
> Keyboard pressed: 'G' (47)
> Device Disconnected.
```

When connecting a keyboard and `PIO_USB_HOST_HID_FULL_STATE_INFO_CALLBACK` is active in the `platformio.ini`

```
USB Host HID firmware start!
USB host library started
> Reset the USB device.
> Low speed device detected.
> Device Attached.
VID: 0x1A2C
PID: 0x2124
> HID device connected.
Manufacturer: SEM
Product: USB Keyboard
> Enumeration completed.
---------------------------------------
To start the HID class operations:
Press User Key...
> HID Demo Device : Keyboard.
> Use Keyboard to type characters:
Pressed Keys (scancodes): 4 5 6
Pressed Keys (symbol): A B C
Modifier Keys: lshift
< all keys released >
```

In the above mode, one can also press the user key to switch to a primitive "write text" mode. The input keys are not debounced, so double-typing characters can easily occur. For simple demonstration purposes only.

```
== ENTERED WRITE TEXT MODE ==
> Start typing on the keyboard
> And the text shall be saved and displayed.
...
Write buffer (11 bytes):
Hello world
Write buffer (12 bytes):
Hello world!
```

When connecting a mouse:

```
USB Host HID firmware start!
USB host library started
> Reset the USB device.
> Low speed device detected.
> Device Attached.
VID: 0x046D
PID: 0xC077
> HID device connected.
Manufacturer: Logitech
Product: USB Optical Mouse
> Enumeration completed.
---------------------------------------
To start the HID class operations:
Press User Key...
> HID Demo Device : Mouse.
Mouse left button pressed
Mouse left button released
Mouse right button pressed
Mouse right button released
New mouse position: X=0, Y=-1 (delta 1,-1)
New mouse position: X=0, Y=-2 (delta 1,-1)
New mouse position: X=0, Y=-3 (delta 1,-1)
New mouse position: X=0, Y=-3 (delta 1,0)
New mouse position: X=0, Y=-4 (delta 1,-1)
New mouse position: X=-1, Y=-5 (delta -1,-1)
New mouse position: X=-2, Y=-5 (delta -2,1)
New mouse position: X=-3, Y=-5 (delta -1,0)
New mouse position: X=-4, Y=-5 (delta -1,0)
New mouse position: X=-5, Y=-5 (delta -1,0)
```