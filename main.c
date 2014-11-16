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

#include <stdio.h>
#include <string.h>

#define USE_FATFS
//#define USE_USB_DEVICE

__ALIGN_BEGIN USB_OTG_CORE_HANDLE     USB_OTG_dev  __ALIGN_END ;

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

/* Flag that is set in interrupt handler. Avoid doing slow stuff in interrupt handlers. */
volatile uint16_t one_second_flag = 0;

TM_RTC_Time_t datatime;

int main()
{

    SystemInit();
    SystemCoreClockUpdate();
#ifdef USE_FATFS
    //Fatfs object
    FATFS FatFs;
    //File object
    FIL fil;
#endif
    //Free and total space
    uint32_t total, free;

    /* Configuring Peripherals: */
    MyConfigGPIO();
    MyConfigUSART();
    //MyConfigTimers();

    uint16_t counter = 0;

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
	  USBD_Init(&USB_OTG_dev,
	#ifdef USE_USB_OTG_HS
	            USB_OTG_HS_CORE_ID,
	#else
	            USB_OTG_FS_CORE_ID,
	#endif
	            &USR_desc,
	            &USBD_MSC_cb,
	            &USR_cb);

	  GPIO_SetBits(LED1_PORT, LED1);
	  while(1){}
#endif

    GPIO_SetBits(LED1_PORT, LED1);
#ifdef USE_FATFS
    //Mount drive
    if (f_mount(&FatFs, "", 1) == FR_OK) {
        //Mounted OK, turn on RED LED
    	printf("\r\nMount ok.");
        //TM_DISCO_LedOn(LED_RED);

        //Try to open file
        if (f_open(&fil, "1stfile.txt", FA_OPEN_ALWAYS | FA_READ | FA_WRITE) == FR_OK) {
            //File opened, turn off RED and turn on GREEN led
        	printf("\r\nFile opened.");
            //TM_DISCO_LedOn(LED_GREEN);
            //TM_DISCO_LedOff(LED_RED);

            //If we put more than 0 characters (everything OK)
            if (f_puts("First string in my file\n", &fil) > 0) {
                if (TM_FATFS_DriveSize(&total, &free) == FR_OK) {
                	printf("\r\nDrive size: %d", total);
                	printf("\r\nFree space: %d", free);
                    //Data for drive size are valid
                }

                //Turn on both leds
                //TM_DISCO_LedOn(LED_GREEN | LED_RED);
                printf("\r\nEverthing ok.");
            }

            //Close file, don't forget this!
            f_close(&fil);
        }

        //Unmount drive, don't forget this!
        f_mount(0, "", 1);
    }

#endif
    /* Main loop */
    while(1){

    	if (one_second_flag){
    		/* Toogle LED */
    		GPIO_ToggleBits(LED1_PORT,LED1);
    		/* Get time */
    		TM_RTC_GetDateTime(&datatime, TM_RTC_Format_BIN);

    		/* Write to USART connected with STLINK (USART2) */
    		printf("\r\n%02d.%02d.%04d %02d:%02d:%02d",
    					datatime.date,
    					datatime.month,
    					datatime.year + 2000,
    					datatime.hours,
    					datatime.minutes,
    					datatime.seconds);

    		//printf("\r\nTime elapsed is %d seconds.",counter);
    		//counter++;
    		/* Reset flag */
    		one_second_flag = 0;
    	} // end one_second_flag
    } // end main loop

} // end main


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
