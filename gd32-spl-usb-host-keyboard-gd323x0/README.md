# SPL USB Host Keyboard example

## Description

This an example firmware adapted from `GD32F3x0_Firmware_Library_V2.1.1\Examples\USBFS\USB_Host\usb_host_hid_keyboard_mouse\`.

It allows a USB HID device (a keyboard or a mouse) to be connected to the microcontroller. 

## Example output

When connecting a keyboard:

```
USB Host HID firmware start!
USB host library started
> Reset the USB device.
> Low speed device detected.
> Device Attached.
VID: 1A2Ch
PID: 2124h
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

When connecting a mouse:

```
USB Host HID firmware start!
USB host library started
> Reset the USB device.
> Low speed device detected.
> Device Attached.
VID: 046Dh
PID: C077h
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