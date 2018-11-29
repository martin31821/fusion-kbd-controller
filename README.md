# fusion-kbd-controller

This project is a tiny userspace binary, allowing you to configure the RGB Fusion keyboard of
the gigabyte AERO 15X when running linux.

## Current state

It's working (at least on my AERO 15X), but far from complete.
At the moment there is no CLI or support for d-bus etc.

Root privileges are required, since the tool has to temporarily unbind the USB device from the kernel module.
This tool uses libusb to communicate with the keyboard.

## Disclaimer

It's possible to brick your keyboard when sending bogus values here.
You should be safe when using the high level `set_mode` and `set_custom_mode` functions.

## Compiling

You need libusb and (on debian/fedora however) libusb-dev.
Afterwards compile with:

`gcc main.c $(pkg-config --libs --cflags libusb-1.0)`
