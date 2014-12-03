/*
 * main.c
 *
 * main.c
 * NucleoF401 USB MSC Host and Device Demo main file.
 *
 * Copyright 2014 Moreto
 *
 * This file is part of NucleoF401 USB MSC Host and Device Demo.
 *
 * NucleoF401 USB MSC Host and Device Demo is free software: you can
 * redistribute it and/or modify it under the terms of the GNU General
 * Public License as published by the Free Software Foundation, either
 * version 3 of the License, or (at your option) any later version.
 *
 * NucleoF401 USB MSC Host and Device Demo is distributed in the hope that it will
 * be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * Please consult the GNU General Public License at http://www.gnu.org/licenses/.
 *
 ************************************************************
 * NucleoF401 USB MSC Host and Device Demo Coocox project for 
 * ST NUCLEO-F401RE board with STM32F401RE controller.
 *
 * Clock is configured as:
 *   External 8MHz from STLINK V2.
 *   Main clock: 84MHz
 *   AHB clock: 84MHz
 *   APB1 clock: 42MHz
 *   APB2 clcok: 84MHz
 *   Timers clock: 84MHz
 *   SDIO clock: 48MHz
 *
 * Flash Prefetch enabled and 2 wait states (minimum for 84MHz and 3.3V).
 *
 * USART2 configured to output from printf function. USART2 is connected to
 * STLINK with 9600 baud. This way one can use the Virtual COM Port from STLINK.
 *
 * With this demo you can use your Nucleo board as a Mass Storage Device
 * that shows on your PC as a removable flash drive. The storage media is a
 * SD (or microSD) card connected with SDIO peripheral.
 *
 * You can also use it to read contents of a flash drive (thumb drive)
 * connected to USB interface of the STM32F401RE. In this case the USB
 * peripheral behaves as as USB HOST. FATFS lib is used to read from the
 * USB.
 *
 * This demo uses 2 libs from http://stm32f4-discovery.com/
 *   Lib 19: Internal RTC on STM32F4
 *   Lib 21: Read SD card with SDIO or SPI communication on STM32F4 using FatFs by Chan
 *
 *   Links:
 *   Lib 19:
 *   http://stm32f4-discovery.com/2014/07/library-19-use-internal-rtc-on-stm32f4xx-devices/
 *   Lib 21:
 *   http://stm32f4-discovery.com/2014/07/library-21-read-sd-card-fatfs-stm32f4xx-devices/
 *
 * Instead of checking continuously the function USBH_Process(&USB_OTG_Core, &USB_Host),
 * I modified the usbh_core.c to call my own callbacks when a device is connected and
 * disconnected.
 * I declared these callbacks in usbh_msc_usr.h as extern functions. The user have
 * to define these functions in their code to do something. In this case, these
 * callbacks only set some flags.
 *
 * Check BSP file for pinout used and check the timer config if you use board other
 * than Nucleo F401.
 *
 * Demo description:
 *
 * The behavior of this demo is configured by 3 defines, you have to enable
 * only one of them:
 *   DEMO_USB_HOST_ONLY    ==> enables only USB HOST
 *   DEMO_USB_DEVICE_ONLY  ==> enables only USB DEVICE
 *   DEMO_USB_BOTH         ==> enables both. User button changes between host and device.
 *
 * In DEMO_USB_HOST_ONLY the program starts the USB HOST and after that
 * it goes on with the rest of the program (showing date and time in USART2).
 * When a USB mass storage is connected the enumeration and MSC process will
 * begin. When the mass storage is ready, FATFS will be used to created
 * a file and list the contents of the flash drive.
 *
 * In DEMO_USB_DEVICE_ONLY the USB DEVICE starts and it enter in an
 * infinite loop, waiting to cable connection. If the user press the Nucleo User Button
 * the program escape from this loop and goes to the rest of the program
 * (blink the led and print date and time in USART2). Be aware that USB DEVICE
 * will be active during the rest of the program. So if you add your own code in
 * the main loop, it can be interrupted by USB when transferring data.
 *
 * To run the USB DEVICE demo, you will need a SD card connected in SDIO interface
 * or SPI (check how to configure this in TM lib fatfs sdcard lib).
 *
 * In DEMO_USB_BOTH the demo starts in USB HOST mode and when the user
 * press the button, it changes to DEVICE mode and keeps trapped in a loop
 * until the user press the button. In this case the USB changes back to
 * HOST mode. Another press in the button and it will goes back to DEVICE
 * and so on.
 *
 * Moreto
 * Florianopolis - Brazil - 2014
 */
#include "stm32f4xx.h"
#include "defines.h"
#include "InitPeriph.h"
#include "tm_stm32f4_fatfs.h"
#include "tm_stm32f4_rtc.h"

#include "usbd_msc_core.h"
#include "usbd_usr.h"
#include "usbd_desc.h"
#include "usb_conf.h"
#include "usbh_core.h"
//#include "usbh_usr.h"
#include "usbh_msc_core.h"
#include "usbh_msc_usr.h"


#include <stdio.h>
#include <string.h>

/* Choose the Demo behavior (only one):*/
//#define DEMO_USB_HOST_ONLY
//#define DEMO_USB_DEVICE_ONLY
#define DEMO_USB_BOTH

#define USE_SD_FATFS

#define MODE_USB_HOST	0
#define MODE_USB_DEVICE	1

__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_Core __ALIGN_END;
__ALIGN_BEGIN USBH_HOST               USB_Host __ALIGN_END;

#ifdef USE_SD_FATFS
    FATFS SD_Fs; //SD card fatfs object
    FIL SD_Fil;  //SD card file object
#endif

#if defined(DEMO_USB_BOTH) || defined(DEMO_USB_HOST_ONLY)
    FATFS USB_Fs;
    FIL USB_Fil;
	FRESULT fr;    /* FatFs return code */
	FIL fil;
	char line[82]; /* Line buffer */
#endif


#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Flag that is set in interrupt handler. Avoid doing slow stuff in interrupt handlers. */
volatile uint16_t one_second_flag = 0;
volatile uint8_t button_press = 0;
volatile uint8_t usb_host_connected_flag = 0;
volatile uint8_t usb_host_disconnected_flag = 0;

#if defined(DEMO_USB_BOTH) || defined(DEMO_USB_HOST_ONLY)
/* Function prototype, prints the contents of root directory */
void printdir(void);
#endif

TM_RTC_Time_t datatime;

int main()
{

    SystemInit();
    SystemCoreClockUpdate();

    //Free and total space
    uint32_t total, free;
#if defined(DEMO_USB_BOTH)
    uint8_t mode = MODE_USB_HOST;
#endif

    /* Configuring Peripherals: */
    MyConfigGPIO();
    MyConfigUSART();
    //MyConfigTimers();


	/* Initialize RTC with external 32768Hz clock */
	if (!TM_RTC_Init(TM_RTC_ClockSource_External)) {
		/* RTC was first time initialized */
		printf("\r\nRTC not set.");
		datatime.hours = 0;
		datatime.minutes = 59;
		datatime.seconds = 55;
		datatime.year = 14;
		datatime.month = 6;
		datatime.date = 30;
		datatime.day = 6;
		/* Set new time */
		TM_RTC_SetDateTime(&datatime, TM_RTC_Format_BIN);
		/* Do your stuff here */
		/* eg. set default time */
	}

	/* Set wakeup interrupt every 1 second */
	TM_RTC_Interrupts(TM_RTC_Int_1s);

#if defined(DEMO_USB_DEVICE_ONLY)
	printf("\r\n*** Starting USB demo in DEVICE MSC mode ***");
#elif defined(DEMO_USB_HOST_ONLY)
	printf("\r\n*** Starting USB demo in HOST MSC mode ***");
#elif defined(DEMO_USB_BOTH)
	printf("\r\n*** Starting USB demo HOST and DEVICE MSC mode ***");
#else
	printf("\r\n*** USB demo mode not defined ***");
#endif

#ifdef DEMO_USB_DEVICE_ONLY
	//if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9) == SET){
		USBD_Init(&USB_OTG_Core,
		#ifdef USE_USB_OTG_HS
		            USB_OTG_HS_CORE_ID,
		#else
		            USB_OTG_FS_CORE_ID,
		#endif
		            &USR_desc,
		            &USBD_MSC_cb,
		            &USR_cb);

		  GPIO_SetBits(LED1_PORT, LED1);
		  while(1){
			  /* Read user button, when pressed the rest of the program is executed
			   * and USB device keeps active.
			   */
			  if (button_press){
				  break;
			 // }
			//	  NVIC_SystemReset();
			  }
		  }
	 //}
#endif // USE_USB_DEVICE

#if defined(DEMO_USB_BOTH) || defined(DEMO_USB_HOST_ONLY)
	/* Start the demo in host mode. Init USB peripheral as HOST. */
	printf("\n\r> Initializing USB Host Full speed...");
	USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_USBH_MSC_cb);
	printf("\n\r> USB Host Full speed initialized.");
#endif // DEMO_USB_BOTH

    GPIO_SetBits(LED1_PORT, LED1);

#ifdef USE_SD_FATFS
    /* If you want to check if FATFS is working with the SD card and SDIO. */
    //Mount drive and check.
    if (f_mount(&SD_Fs, "0:", 1) == FR_OK) {
    	printf("\r\nMount ok.");

        //Try to open file
        if (f_open(&SD_Fil, "0:sd_file.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE) == FR_OK) {

			printf("\r\nFile opened.");

			TM_FATFS_DriveSize(&total, &free);
			/* Put data */
			f_printf(&SD_Fil,"This is my first file with SD card and FatFs\n");
			f_printf(&SD_Fil,"with SD card library from stm32f4-discovery.com\n");
			f_printf(&SD_Fil,"----------------------------------------------------\n");
			f_printf(&SD_Fil,"SD card total and free space:\n\n");
			/* Total space */
			f_printf(&SD_Fil,"Total: %8lu kB; %5lu MB; %2lu GB\n", total, total / 1024, total / 1048576);
			/* Free space */
			f_printf(&SD_Fil,"Free:  %8lu kB; %5lu MB; %2lu GB\n", free, free / 1024, free / 1048576);
			f_printf(&SD_Fil,"----------------------------------------------------\n");
			/* Close SD card file */
			f_close(&SD_Fil);

			//Close file, don't forget this!
			f_close(&SD_Fil);
			printf("\r\nFile written to SD card.");
        }

        //Unmount drive, don't forget this!
        f_mount(0, "0:", 1);
    }
#endif // USE_FATFS

    /* Main loop */
    while(1){

#ifdef DEMO_USB_BOTH
    	if (mode == MODE_USB_DEVICE){

    		USB_OTG_BSP_DriveVBUS(&USB_OTG_Core, 0);

    		printf("\n\r> Initializing USB Device Full speed...");
    		USBD_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USR_desc,&USBD_MSC_cb,&USR_cb);
    		printf("\n\r> USB Device Full speed initialized.");
    		GPIO_SetBits(LED1_PORT, LED1);
    		while (!button_press){
    			/* Do nothing while in usb device*/
    			/* Wait for user button to go out of the loop, after
    			 * that, enables host again.*/
    		}
			printf("\n\r> Initializing USB Host Full speed...");
			USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_USBH_MSC_cb);
			printf("\n\r> USB Host Full speed initialized.");
			mode = MODE_USB_HOST;
    		button_press = 0;
    	}
#endif

#if defined(DEMO_USB_BOTH) || defined(DEMO_USB_HOST_ONLY)
    	if (usb_host_connected_flag){ /* When a Mass Storage device is attached. */
    		printf("\r\n** A USB device was connected. **");
    		if (HCD_IsDeviceConnected(&USB_OTG_Core)){
    			while(!USBH_USR_MSC_IsReady()){
    				/* Enumerate and configure connected device.. */
    				USBH_Process(&USB_OTG_Core, &USB_Host);
    			}
    			/* When entered here USB host MSC is ready.
    			 * Mount with FATFS*/
    			if (f_mount(&USB_Fs, "1:", 1) == FR_OK) {
    				/* Mounted ok, show the contents of the flash drive: */
    				printdir();

    				/* Open a file for reading and show contents */
    				fr = f_open(&fil, "1:teste1.txt", FA_READ);
    				if (!fr){ // Test if the file was opened
    					while (f_gets(line, sizeof line, &fil))
    						printf(line);
    					/* Close the file */
    					f_close(&fil);
    					printf("\r\nFile read in USB flash drive.");
    				}else{
    					printf("\r\nError opening the file teste1.txt");
    				}
#if 1 // If you don't want to create a file in USB flash drive, change to 0.
    				/* Try to open USB file */
    				if (f_open(&USB_Fil, "1:usb_file.txt", FA_READ | FA_WRITE | FA_CREATE_ALWAYS) == FR_OK) {
    					/* Get total and free space on USB */
    					TM_FATFS_USBDriveSize(&total, &free);
    					/* Put data */
    					f_printf(&USB_Fil,"This is my first file with USB and FatFS\n");
    					f_printf(&USB_Fil,"with USB MSC HOST library from ST\n");
    					f_printf(&USB_Fil,"USB total and free space:\n");
    					/* Total space */
    					f_printf(&USB_Fil,"Total: %8lu kB; %5lu MB; %2lu GB\n", total, total / 1024, total / 1048576);
    					/* Free space */
    					f_printf(&USB_Fil,"Free:  %8lu kB; %5lu MB; %2lu GB\n", free, free / 1024, free / 1048576);
    					f_printf(&USB_Fil,"----------------------------------------------------\n");
    					/* Close USB file */
    					f_close(&USB_Fil);
    					printf("\r\nFile written to USB drive.");
    				} // f_open
#endif // 1
    				/* Unmount USB */
    				f_mount(0, "1:", 1);
    			} // f_mount usb
    		}
    		usb_host_connected_flag = 0; // Reset the flag.
    	} // usb_host_flag

    	if (usb_host_disconnected_flag){
    		printf("\r\n A USB device was disconnected. **");
    		/* Re-Initilaize Host for new Enumeration */
    		USBH_DeInit(&USB_OTG_Core, &USB_Host);
    		USB_Host.usr_cb->DeInit();
    		USB_Host.class_cb->DeInit(&USB_OTG_Core, &USB_Host.device_prop);
    		usb_host_disconnected_flag = 0;
    	}
#endif // defined(DEMO_USB_BOTH) || defined(DEMO_USB_HOST_ONLY)

    	if (one_second_flag){ /* Enter here every second (flag set in RTC interrupt) */
    		/* Toogle LED */
    		GPIO_ToggleBits(LED1_PORT,LED1);
    		/* Get time */
    		TM_RTC_GetDateTime(&datatime, TM_RTC_Format_BIN);
    		/* Write date and time to USART connected with STLINK (USART2) */
    		printf("\r\n%02d/%02d/%04d %02d:%02d:%02d",
    					datatime.date,
    					datatime.month,
    					datatime.year + 2000,
    					datatime.hours,
    					datatime.minutes,
    					datatime.seconds);

    		/* Reset flag */
    		one_second_flag = 0;
    	} // end one_second_flag

#ifdef DEMO_USB_BOTH
    	if (button_press){
    		/* When button press, change USB role. */
    		printf("\r\nButton press.");
    		if (mode == MODE_USB_DEVICE){
    			mode = MODE_USB_HOST;
    		}else{
    			mode = MODE_USB_DEVICE;
    		}
    		button_press = 0;
    	} // button press flag
#endif
    } // end main loop

} // end main

/* When a USB device is connected to the usb host this callback is executed.
 * It is asynchronous and they are called from USB interrupt. So keep it short and
 * do all your stuff in main code. */
void USBH_ConnectEventCallback(void){
	usb_host_connected_flag = 1;
}

/* When a USB device is disconnected to the usb host this callback is executed.
 * It is asynchronous and they are called from USB interrupt. So keep it short and
 * do all your stuff in main code. */
void USBH_DisconnectEventCallback(void){
	usb_host_disconnected_flag = 1;
}

/* Custom request handler function */
/* Called on wakeup interrupt */
void TM_RTC_RequestHandler() {
	one_second_flag = 1;
}

#if defined(DEMO_USB_BOTH) || defined(DEMO_USB_HOST_ONLY)
void printdir(void){
	FILINFO fno;
	DIR dir;
	FRESULT res;

	/*Reads the dir list*/
    res = f_opendir(&dir, "1:");
	if( res != FR_OK )
    {
      /*Could not open the selected directory*/
	  printf("\r\nCould not open diretory.");
      return;
    }
	printf("\r\n---Contents of the USB flash drive---");

#if _USE_LFN
	static char lfn[_MAX_LFN + 1];   /* Buffer to store the LFN */
	fno.lfname = lfn;
	fno.lfsize = sizeof lfn;
#endif

	char *fn;   /* This function is assuming non-Unicode cfg. */

    while ((HCD_IsDeviceConnected(&USB_OTG_Core)) && ( f_readdir(&dir, &fno) == FR_OK ) && fno.fname[0] != 0 )
    {
#if _USE_LFN
    	fn = *fno.lfname ? fno.lfname : fno.fname;
#else
    	fn = fno.fname;
#endif
	  printf("\r\n%s",fn);
    }
    printf("\r\n-------------------------------------");
}
#endif

/**
  * @brief  Retargets the C library printf function to the USART.
  * @param  None
  * @retval None
  */
PUTCHAR_PROTOTYPE
{
  /* Place your implementation of fputc here */
  /* e.g. write a character to the USART */
  USART_SendData(USART2, (uint8_t) ch);
  /* Loop until the end of transmission */
  while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET)
  {}
  return ch;
}
