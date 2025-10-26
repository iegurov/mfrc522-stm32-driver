
/* Попытка написать небольшую библиотеку для упрощения работы с STM32 + CMSIS */

#ifndef STMLIB_H
#define STMLIB_H

#include <stdint.h>
#include "stm32f1xx.h"

#define OUT_PP_50MHz 0b0011
#define IN_FL 		 0b0100
#define AF_PP_50MHz	 0b1011

#define PORT_A RCC_APB2ENR_IOPAEN
#define PORT_C RCC_APB2ENR_IOPCEN

struct Pin
{
	GPIO_TypeDef* gpio;
	uint8_t pin;
};

void __libc_init_array(void);

void DelayMs(uint32_t ms);

void PinMode(GPIO_TypeDef* gpio, uint8_t pin, uint8_t mode);
void PinSet(GPIO_TypeDef* gpio, uint8_t pin);
void PinReset(GPIO_TypeDef* gpio, uint8_t pin);

void GPIO_Enable(uint32_t bitmask);
void USART3_Enable();
void USART3_Send(uint8_t tx_data);
uint8_t USART3_Receive();
void SPI1_Enable();
uint8_t SPI1_Transmit(uint8_t tx_data);
void TIM2_PWM_Enable();

#endif





