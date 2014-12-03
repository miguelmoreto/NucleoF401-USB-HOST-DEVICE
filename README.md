# Readme #

NucleoF401 USB MSC Host and Device Demo Coocox project for 
ST NUCLEO-F401RE board with STM32F401RE controller.

Clock is configured as:
  External 8MHz from STLINK V2.
  Main clock: 84MHz
  AHB clock: 84MHz
  APB1 clock: 42MHz
  APB2 clcok: 84MHz
  Timers clock: 84MHz
  SDIO clock: 48MHz

Flash Prefetch enabled and 2 wait states (minimum for 84MHz and 3.3V).

USART2 configured to output from printf function. USART2 is connected to
STLINK with 9600 baud. This way one can use the Virtual COM Port from STLINK.

With this demo you can use your Nucleo board as a Mass Storage Device
that shows on your PC as a removable flash drive. The storage media is a
SD (or microSD) card connected with SDIO peripheral.

You can also use it to read contents of a flash drive (thumb drive)
connected to USB interface of the STM32F401RE. In this case the USB
peripheral behaves as as USB HOST. FATFS lib is used to read from the
USB.

This demo uses 2 libs from http://stm32f4-discovery.com/
  Lib 19: Internal RTC on STM32F4
  Lib 21: Read SD card with SDIO or SPI communication on STM32F4 using FatFs by Chan

  Links:
  Lib 19:
  http://stm32f4-discovery.com/2014/07/library-19-use-internal-rtc-on-stm32f4xx-devices/
  Lib 21:
  http://stm32f4-discovery.com/2014/07/library-21-read-sd-card-fatfs-stm32f4xx-devices/

Instead of checking continuously the function USBH_Process(&USB_OTG_Core, &USB_Host),
I modified the usbh_core.c to call my own callbacks when a device is connected and
disconnected.
I declared these callbacks in usbh_msc_usr.h as extern functions. The user have
to define these functions in their code to do something. In this case, these
callbacks only set some flags.

Check BSP file for pinout used and check the timer config if you use board other
than Nucleo F401.

Demo description:

The behavior of this demo is configured by 3 defines, you have to enable
only one of them:
  DEMO_USB_HOST_ONLY    ==> enables only USB HOST
  DEMO_USB_DEVICE_ONLY  ==> enables only USB DEVICE
  DEMO_USB_BOTH         ==> enables both. User button changes between host and device.

In DEMO_USB_HOST_ONLY the program starts the USB HOST and after that
it goes on with the rest of the program (showing date and time in USART2).
When a USB mass storage is connected the enumeration and MSC process will
begin. When the mass storage is ready, FATFS will be used to created
a file and list the contents of the flash drive.

In DEMO_USB_DEVICE_ONLY the USB DEVICE starts and it enter in an
infinite loop, waiting to cable connection. If the user press the Nucleo User Button
the program escape from this loop and goes to the rest of the program
(blink the led and print date and time in USART2). Be aware that USB DEVICE
will be active during the rest of the program. So if you add your own code in
the main loop, it can be interrupted by USB when transferring data.

To run the USB DEVICE demo, you will need a SD card connected in SDIO interface
or SPI (check how to configure this in TM lib fatfs sdcard lib).

In DEMO_USB_BOTH the demo starts in USB HOST mode and when the user
press the button, it changes to DEVICE mode and keeps traped in a loop
until the user press the button. In this case the USB changes back to
HOST mode. Another press in the button and it will goes back to DEVICE
and so on.

Moreto
Florianopolis - Brazil - 2014