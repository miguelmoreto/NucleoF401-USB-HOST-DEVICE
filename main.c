/*
 * main.c
 *
 * Example Coocox project for ST NUCLEO-F401RE boar with STM32F401RE controller.
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
 * Project based on a smaller STM32F401RC supported by Coocox. In linker configuration
 * the ram and flash size is updated to suit the STM32F401RE.
 *
 * This example configures the LED pin as output and every second the led toogle.
 * Time base is from TIM3, configured to generate an update interrupt every second.
 *
 * USART2 configured to output from printf function. USART2 is connected to
 * STLINK with 9600 baud. This way one can use the Virtual COM Port from STLINK.
 *
 * A string with a counter is printed in serial every second.
 *
 * Instead of checking continuously the function USBH_Process(&USB_OTG_Core, &USB_Host),
 * I modified the usbh_core.c to call my own callbacks when a device is connected and disconnected.
 * I defined these callbacks in usbh_msc_usr.h as extern functions. The user can declare
 * these functions in their code to do something.
 *
 * Check BSP file for pinout used and check the timer config if you use other board
 * than Nucleo F401.
 *
 * Miguel Moreto
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

#define USE_FATFS
//#define USE_USB_DEVICE
#define USE_USB_HOST

#define MODE_USB_HOST	0
#define MODE_USB_DEVICE	1

//__ALIGN_BEGIN USB_OTG_CORE_HANDLE     USB_OTG_dev  __ALIGN_END ;
__ALIGN_BEGIN USB_OTG_CORE_HANDLE    USB_OTG_Core __ALIGN_END;
__ALIGN_BEGIN USBH_HOST               USB_Host __ALIGN_END;

#ifdef USE_FATFS
    //Fatfs object
    FATFS SD_Fs;
    //File object
    FIL SD_Fil;
#ifdef USE_USB_HOST
    FATFS USB_Fs;
    FIL USB_Fil;
	FRESULT fr;    /* FatFs return code */
	FIL fil;
	char line[82]; /* Line buffer */
#endif
    int var;
    char buffer[50];

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

void printdir(void);

TM_RTC_Time_t datatime;

int main()
{

    SystemInit();
    SystemCoreClockUpdate();

    //Free and total space
    uint32_t total, free;
    uint8_t mode = MODE_USB_HOST;

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


#ifdef USE_USB_DEVICE
	if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9) == SET){
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
			  /* Read VBUS pin, when cable is disconnected
			   * system resets. */
			  //if (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_9)==RESET){
			//	  NVIC_SystemReset();
			  //}
		  }
	 }
#endif

#ifdef USE_USB_HOST
	printf("\n\r> Initializing USB Host Full speed...");
	USBH_Init(&USB_OTG_Core,USB_OTG_FS_CORE_ID,&USB_Host,&USBH_MSC_cb,&USR_USBH_MSC_cb);
	printf("\n\r> USB Host Full speed initialized.");
#endif

    GPIO_SetBits(LED1_PORT, LED1);
#ifdef USE_FATFS
    //Mount drive
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

#endif
    /* Main loop */
    while(1){
#if 1
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

#ifdef USE_USB_HOST
    	if (usb_host_connected_flag){
    		printf("\r\n** A USB device was connected. **");
    		if (HCD_IsDeviceConnected(&USB_OTG_Core)){
    			while(!USBH_USR_MSC_IsReady()){
    				/* Enumerate and configure connected device.. */
    				USBH_Process(&USB_OTG_Core, &USB_Host);
    			}
    			/* When entered here USB host MSC is ready. */
    			if (f_mount(&USB_Fs, "1:", 1) == FR_OK) {
    				/* Mounted ok */
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
#if 1
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
#endif
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
#endif
    	if (one_second_flag){
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
			//USART_SendData(USART1, 'a');
			/* Loop until the end of transmission */
			//while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET)
			//{}

    		/* Reset flag */
    		one_second_flag = 0;
    	} // end one_second_flag
#if 1
    	if (button_press){
    		printf("\r\nButton press.");
    		if (mode == MODE_USB_DEVICE){
    			mode = MODE_USB_HOST;
        		/* Re-Initilaize Host for new Enumeration */
        		//USBH_DeInit(&USB_OTG_Core, &USB_Host);
        		//USB_Host.usr_cb->DeInit();
        		//USB_Host.class_cb->DeInit(&USB_OTG_Core, &USB_Host.device_prop);
        		//usb_host_disconnected_flag = 0;

    		}else{
    			mode = MODE_USB_DEVICE;
    		}
    		button_press = 0;
    	}
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

#ifdef USE_USB_HOST
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
