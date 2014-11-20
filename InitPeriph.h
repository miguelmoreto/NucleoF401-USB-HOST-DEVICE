/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INIT_PERIPH_H
#define __INIT_PERIPH_H

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
#include "misc.h"


#define LED1_PORT 		GPIOA
#define LED1 			GPIO_Pin_5
#define LED1_RCC		RCC_AHB1Periph_GPIOA
#define USER_BTN_PORT	GPIOC
#define USER_BTN		GPIO_Pin_12
#define USER_BTN_RCC 	RCC_AHB1Periph_GPIOC

void MyConfigTimers(void);
void MyConfigGPIO(void);
void MyConfigUSART(void);

#endif /* __INIT_PERIPH_H */
