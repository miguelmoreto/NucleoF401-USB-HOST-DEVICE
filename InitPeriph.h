/*
 * InitPeriph.c
 * Peripheral configuration functions - header file.
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
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __INIT_PERIPH_H
#define __INIT_PERIPH_H

#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"


#define LED1_PORT 		GPIOA
#define LED1 			GPIO_Pin_5
#define LED1_RCC		RCC_AHB1Periph_GPIOA
#define USER_BTN_PORT	GPIOC
#define USER_BTN		GPIO_Pin_13
#define USER_BTN_RCC 	RCC_AHB1Periph_GPIOC

void MyConfigTimers(void);
void MyConfigGPIO(void);
void MyConfigUSART(void);

#endif /* __INIT_PERIPH_H */
