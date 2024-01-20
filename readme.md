# "Hello, World!" AMP for the DE10-Nano development board

## Overview

This contains asymmetric multiprocessing (AMP) version of "Hello, World!" bare-metal C program for the DE10-Nano development board.  Included is my makefile script (actually a set of scripts) to build the C sources and SD card image.

## Build requirements

Minimum to build the C sources:
- GNU C/C++ cross toolchain for ARM processors supporting Cortex-A9
- newlib library.  This is usually included with the above toolchain

### Using my script to build
To build the C sources using the script under Windows, you will need a Windows port of GNU make with some shell facilities, a good solution is xPack's Build Tools.

### Using "Eclipse IDE for Embedded C/C++ Developers" to build
To build the C sources using Eclipse, you can open with the included project file for Eclipse IDE, but you may need to setup other things first as described in my guide.

### Building the SD card image and U-Boot sources
To build these under Windows you will need to use WSL2 or Linux under a VM.  See the makefile or my guide for more information.

You can find the guide on my website:
[https://truhy.co.uk](https://truhy.co.uk)

## Notes

If you are using other development board you may have to rebuild U-Boot-SPL and the Quartus Prime project handoff files to support your development board.  As an example, on the DE10-Nano board USB-UART is wired to HPS UART0, but for another board it may be to HPS UART1.

This sample uses the UART controller registers to send a "Hello, World!" message using the HPS UART0 controller. The message is sent out to the USB-UART, i.e. the micro USB connector on the development board.  To view the serial messages, start a standard serial com terminal software (e.g. PuTTY) and set it to use the relevant com port with the following settings: 115200 baud, 8 data bits, no parity and 1 stop bit.
