/**
  ******************************************************************************
  * @file    usbh_msc_usr.c
  * @author  MCD Application Team
  * @version V1.1.0
  * @date    19-March-2012
  * @brief   This file includes the usb host library user callbacks
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2012 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include <string.h>
#include "usbh_msc_usr.h"
#include "ff.h"

/* Comment the folowing line if you don't want to print
 * debug messages with printf */
#define PRINT_DEBUG_MSG


#if 0 // This was used by the ST original demo.
UINT br;
FATFS fs;         /* Work area (file system object) for logical drive */
FIL fsrc;         /* file objects */
FRESULT res;

/* State Machine for the USBH_USR_ApplicationState */
#define USH_USR_FS_INIT       0
#define USH_USR_FS_READLIST   1
#define USH_USR_FS_WRITEFILE  2
#define USH_USR_FS_READFILE   3
#define USH_USR_FS_IDLE       4
uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;
#endif

#define USH_USR_FS_INIT       0
#define USH_USR_FS_MSC_DONE       1

//#include "lcd_log.h"

/** @addtogroup USBH_USER
* @{
*/

/** @addtogroup USBH_MSC_DEMO_USER_CALLBACKS
* @{
*/

/** @defgroup USBH_USR 
* @brief    This file includes the usb host stack user callbacks
* @{
*/ 

/** @defgroup USBH_USR_Private_TypesDefinitions
* @{
*/ 
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Defines
* @{
*/ 

/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Macros
* @{
*/ 
extern USB_OTG_CORE_HANDLE          USB_OTG_Core;
/**
* @}
*/ 


/** @defgroup USBH_USR_Private_Variables
* @{
*/ 
/* Global variables */
uint8_t USBH_USR_ApplicationState = USH_USR_FS_INIT;

/*  Points to the DEVICE_PROP structure of current device */
/*  The purpose of this register is to speed up the execution */

USBH_Usr_cb_TypeDef USR_USBH_MSC_cb =
{
  USBH_USR_Init,
  USBH_USR_DeInit,
  USBH_USR_DeviceAttached,
  USBH_USR_ResetDevice,
  USBH_USR_DeviceDisconnected,
  USBH_USR_OverCurrentDetected,
  USBH_USR_DeviceSpeedDetected,
  USBH_USR_Device_DescAvailable,
  USBH_USR_DeviceAddressAssigned,
  USBH_USR_Configuration_DescAvailable,
  USBH_USR_Manufacturer_String,
  USBH_USR_Product_String,
  USBH_USR_SerialNum_String,
  USBH_USR_EnumerationDone,
  USBH_USR_UserInput,
  USBH_USR_MSC_Application,
  USBH_USR_DeviceNotSupported,
  USBH_USR_UnrecoveredError
    
};

/** @defgroup USBH_USR_Private_Constants
* @{
*/ 

/** @defgroup USBH_USR_Private_FunctionPrototypes
* @{
*/

/** @defgroup USBH_USR_Private_Functions
* @{
*/ 


/**
* @brief  USBH_USR_Init 
*         Displays the message on LCD for host lib initialization
* @param  None
* @retval None
*/
void USBH_USR_Init(void)
{
	USBH_USR_ApplicationState = USH_USR_FS_INIT;
#ifdef PRINT_DEBUG_MSG
	printf("\r\nUSBH_USR> USB Host Init.");
#endif
}

/**
* @brief  USBH_USR_DeviceAttached 
*         Displays the message on LCD on device attached
* @param  None
* @retval None
*/
void USBH_USR_DeviceAttached(void)
{
	USBH_USR_ApplicationState = USH_USR_FS_INIT;
#ifdef PRINT_DEBUG_MSG
	printf("\r\nUSBH_USR> USB Host Device Attached.");
#endif
}


/**
* @brief  USBH_USR_UnrecoveredError
* @param  None
* @retval None
*/
void USBH_USR_UnrecoveredError (void)
{
	USBH_USR_ApplicationState = USH_USR_FS_INIT;
#ifdef PRINT_DEBUG_MSG
  printf("\r\nUSBH_USR> UNRECOVERED ERROR STATE");
#endif
}


/**
* @brief  USBH_DisconnectEvent
*         Device disconnect event
* @param  None
* @retval Staus
*/
void USBH_USR_DeviceDisconnected (void)
{
	USBH_USR_ApplicationState = USH_USR_FS_INIT;
#ifdef PRINT_DEBUG_MSG
  printf("\r\nUSBH_USR> Device Disconnected");
#endif
//  USBH_USR_ApplicationState = USH_USR_FS_INIT;
}
/**
* @brief  USBH_USR_ResetUSBDevice 
* @param  None
* @retval None
*/
void USBH_USR_ResetDevice(void)
{
	USBH_USR_ApplicationState = USH_USR_FS_INIT;
  /* callback for USB-Reset */
#ifdef PRINT_DEBUG_MSG
	printf("\r\nUSBH_USR> Device Reset");
#endif
}


/**
* @brief  USBH_USR_DeviceSpeedDetected 
*         Displays the message on LCD for device speed
* @param  Device speed
* @retval None
*/
void USBH_USR_DeviceSpeedDetected(uint8_t DeviceSpeed)
{
  if(DeviceSpeed == HPRT0_PRTSPD_HIGH_SPEED)
  {
#ifdef PRINT_DEBUG_MSG
    printf("\r\nUSBH_USR> Device Disconnected");
#endif
  }  
  else if(DeviceSpeed == HPRT0_PRTSPD_FULL_SPEED)
  {
#ifdef PRINT_DEBUG_MSG
    printf("\r\nUSBH_USR> Full speed device detected");
#endif
  }
  else if(DeviceSpeed == HPRT0_PRTSPD_LOW_SPEED)
  {
#ifdef PRINT_DEBUG_MSG
    printf("\r\nUSBH_USR> Low speed device detected");
#endif
  }
  else
  {
    printf("\r\nUSBH_USR>  Device fault");
  }
}

/**
* @brief  USBH_USR_Device_DescAvailable 
*         Displays the message on LCD for device descriptor
* @param  device descriptor
* @retval None
*/
void USBH_USR_Device_DescAvailable(void *DeviceDesc)
{
	  uint8_t temp[50];
	  USBH_DevDesc_TypeDef *hs;
	  hs = DeviceDesc;

#ifdef PRINT_DEBUG_MSG
	  printf("\r\nUSBH_USR> Device descriptor:");
	  sprintf((char *)temp , "VID          = %04Xh" , (uint16_t)(*hs).idVendor);
	//  LCD_DisplayStringLine( Line++ , temp);
	  printf("\r\nVID = %04Xh" , (uint16_t)(*hs).idVendor);

	  sprintf((char *)temp , "PID          = %04Xh" , (uint16_t)(*hs).idProduct);
	//  LCD_DisplayStringLine( Line++ , temp);
	  printf("\r\nPID = %04Xh" , (uint16_t)(*hs).idProduct);
#endif
}

/**
* @brief  USBH_USR_DeviceAddressAssigned 
*         USB device is successfully assigned the Address 
* @param  None
* @retval None
*/
void USBH_USR_DeviceAddressAssigned(void)
{
#ifdef PRINT_DEBUG_MSG
	printf("\r\nUSBH_USR> Device address assigned");
#endif
  
}


/**
* @brief  USBH_USR_Conf_Desc 
*         Displays the message on LCD for configuration descriptor
* @param  Configuration descriptor
* @retval None
*/
void USBH_USR_Configuration_DescAvailable(USBH_CfgDesc_TypeDef * cfgDesc,
                                          USBH_InterfaceDesc_TypeDef *itfDesc,
                                          USBH_EpDesc_TypeDef *epDesc)
{
	USBH_InterfaceDesc_TypeDef *id;
	id = itfDesc;
	if((*id).bInterfaceClass  == 0x08)
	{
#ifdef PRINT_DEBUG_MSG
		printf("\r\nUSBH_USR> Mass storage device connected");
#endif
	}
}

/**
* @brief  USBH_USR_Manufacturer_String 
*         Displays the message on LCD for Manufacturer String 
* @param  Manufacturer String 
* @retval None
*/
void USBH_USR_Manufacturer_String(void *ManufacturerString)
{
#ifdef PRINT_DEBUG_MSG
	printf("\r\nUSBH_USR> Manufacturer : %s", (char *)ManufacturerString);
#endif
}

/**
* @brief  USBH_USR_Product_String 
*         Displays the message on LCD for Product String
* @param  Product String
* @retval None
*/
void USBH_USR_Product_String(void *ProductString)
{
#ifdef PRINT_DEBUG_MSG
	printf("\r\nUSBH_USR> Product : %s", (char *)ProductString);
#endif
}

/**
* @brief  USBH_USR_SerialNum_String 
*         Displays the message on LCD for SerialNum_String 
* @param  SerialNum_String 
* @retval None
*/
void USBH_USR_SerialNum_String(void *SerialNumString)
{
#ifdef PRINT_DEBUG_MSG
	printf("\r\nUSBH_USR> Serial Number OK : %s", (char *)SerialNumString);
#endif
} 



/**
* @brief  EnumerationDone 
*         User response request is displayed to ask application jump to class
* @param  None
* @retval None
*/
void USBH_USR_EnumerationDone(void)
{
	/* Enumeration complete */
#ifdef PRINT_DEBUG_MSG
	printf("\r\nUSBH_USR> Enumeration completed");
#endif

} 


/**
* @brief  USBH_USR_DeviceNotSupported
*         Device is not supported
* @param  None
* @retval None
*/
void USBH_USR_DeviceNotSupported(void)
{
#ifdef PRINT_DEBUG_MSG
	printf("\r\nUSBH_USR> Device not supported!");
	printf("\r\nUSBH_USR> LUN number is %d",MSC_Machine.maxLun);
#endif
}  


/**
* @brief  USBH_USR_UserInput
*         User Action for application state entry
* @param  None
* @retval USBH_USR_Status : User response for key button
*/
USBH_USR_Status USBH_USR_UserInput(void)
{
#ifdef PRINT_DEBUG_MSG
	printf("\r\nUSBH_USR> User responded!");
#endif
	return USBH_USR_RESP_OK;
}  

/**
* @brief  USBH_USR_OverCurrentDetected
*         Over Current Detected on VBUS
* @param  None
* @retval Staus
*/
void USBH_USR_OverCurrentDetected (void)
{
#ifdef PRINT_DEBUG_MSG
  printf("\r\nUSBH_USR> Overcurrent detected.");
#endif
}


/**
* @brief  USBH_USR_MSC_Application 
*         Demo application for mass storage
* @param  None
* @retval Staus
*/
int USBH_USR_MSC_Application(void)
{
	/* When this callback is executed, the USB MSC device connected is
	 * ready to use. */
	if (USBH_USR_ApplicationState == USH_USR_FS_INIT) {
		USBH_USR_ApplicationState = USH_USR_FS_MSC_DONE;
#ifdef PRINT_DEBUG_MSG
		printf("\r\nUSBH_USR> USR application.");
#endif
	}
	return 0;
}
//	  printf("\r\n----------------------------------------1");
#if 0
	  switch(USBH_USR_ApplicationState)
	  {
	  case USH_USR_FS_INIT:

	    /* Initialises the file system */
	    f_mount(0,&fs);

	    /*Display the size of the disk*/
		printf("\r\n%s %d",MSG_DISK_SIZE,(USBH_MSC_Param.MSCapacity * USBH_MSC_Param.MSPageLength)/(1048576));
		printf("\r\n----------------------------------------1");
	    printf("\r\n----------------------------------------2");

	    if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
	    {
		  printf("\r\nWrite protected!");
		  printf("\r\n-----------------------------------------");
	    }


	    USBH_USR_ApplicationState = USH_USR_FS_READLIST;
	    break;

	  case USH_USR_FS_READLIST:

		/*Reads the dir list*/
	    res = f_opendir(&dir, "0:");
		if( res != FR_OK )
	    {
	      /*Could not open the selected directory*/
	      return(-2);
	    }
		printf("\r\n%s",MSG_ROOT_CONT);
		printf("\r\n-----------------------------------------");

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
	#if 0
	      if( Line == 13 )
	      {
	//        LCD_DisplayStringLine( Line ,"To continue 'Press Key' ");

			printf("To continue 'Press Key' ");
		    printf("\r\n");

	        /*Key in polling*/
	        while( ! ((HCD_IsDeviceConnected(&USB_OTG_FS_dev)) && \
	                 ( !GPIO_ReadInputDataBit(KEYA_PORT, KEYA_PORT_PIN) || GPIO_ReadInputDataBit(KEYB_PORT, KEYB_PORT_PIN) || \
					    GPIO_ReadInputDataBit(KEYC_PORT, KEYC_PORT_PIN)) ) );

	//        LCD_Clear(Black);
	        Line = 0;
	      }
	#endif
	    }

//		printf("\r\nTo continue 'Press Key 3' ");
//		printf("\r\n");

	    /*Key in polling*/
//	    while( !((HCD_IsDeviceConnected(&USB_OTG_Core)) && (GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2) == RESET)));

	    USBH_USR_ApplicationState = USH_USR_FS_WRITEFILE;
	    break;

	  case USH_USR_FS_WRITEFILE:

	    /* Writes a text file, HOSTDEMO.TXT in the disk*/
	     if(USBH_MSC_Param.MSWriteProtect == DISK_WRITE_PROTECTED)
	      {
			printf("\r\n%s",MSG_WR_PROTECT);
		   	printf("\r\n> ERROR HostDemo.TXT could not be created      ");
		   	printf("\r\nIndo para IDLE.");

	        USBH_USR_ApplicationState = USH_USR_FS_IDLE;
	        break;
	      }

		res = f_open( &fsrc , "0:/HostDemo.TXT" , FA_CREATE_NEW | FA_WRITE);

	    if ( res == FR_OK )
	    {
	      /* Write buffer to file */
	      res = f_write(&fsrc, textFileBuffer, sizeof(textFileBuffer), &br);
		  printf("\r\n-----------------------------------------");
		  printf("\r\nHostDemo.TXT successfully created");

	      /*close file */
	      f_close(&fsrc);
	      USBH_USR_ApplicationState = USH_USR_FS_IDLE;
	      printf("\r\nIndo para IDLE.");
	    }
	    else if ( res == FR_EXIST )
	    {
		  printf("\r\nHostDemo.TXT exist disk");
		  printf("\r\nTo recreate delete HostDemo.TXT in disk ");
		  printf("\r\nIndo para IDLE.");

	      USBH_USR_ApplicationState = USH_USR_FS_IDLE;
	    }
	    break;

	  case USH_USR_FS_IDLE:
		  break;

	  default: break;
	  }
#endif


unsigned int USBH_USR_MSC_IsReady(void){
	return USBH_USR_ApplicationState;
}

/**
* @brief  USBH_USR_DeInit
*         Deint User state and associated variables
* @param  None
* @retval None
*/
void USBH_USR_DeInit(void)
{
	USBH_USR_ApplicationState = USH_USR_FS_INIT;
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

