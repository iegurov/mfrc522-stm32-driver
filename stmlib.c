
/* Реализация библиотеки stmlib */

#include "stmlib.h"

volatile uint32_t elapsed_ms = 0;
const a = 4;
int b;

void __libc_init_array()
{
	/* Заглушка для newlib */
}

void GPIO_Enable(uint32_t bitmask)
{
	RCC->APB2ENR |= bitmask;
}

void DelayMs(uint32_t ms)
{
	elapsed_ms = 0;
	while(elapsed_ms<ms) {};
}

void SysTick_Handler(void)
{
	elapsed_ms++;
}

void PinMode(GPIO_TypeDef* gpio, uint8_t pin, uint8_t mode)
{
	volatile uint32_t* reg = (pin<8) ? &gpio->CRL : &gpio->CRH;
	uint8_t shift = (pin % 8) * 4;

	*reg &= ~(0xF << shift);
	*reg |= (mode << shift);
}

void PinSet(GPIO_TypeDef* gpio, uint8_t pin) //Важно: пин должен быть сконфигурирован как output GP_PP
{
	gpio->ODR |= (1 << pin);
}

void PinReset(GPIO_TypeDef* gpio, uint8_t pin)
{
	gpio->ODR &= ~(1 << pin);
}
/*
bool PinRead()
{
	return true;
}
*/

void USART3_Enable()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPBEN;
	RCC->APB1ENR |= RCC_APB1ENR_USART3EN;
	
	PinMode(GPIOB, 10, AF_PP_50MHz); //TX
	PinMode(GPIOB, 11, IN_FL); //RX

	USART3->BRR = 0x0341;
	USART3->CR1 |= USART_CR1_TE;
	USART3->CR1 |= USART_CR1_RE;
	USART3->CR1 |= USART_CR1_RXNEIE;
	USART3->CR1 |= USART_CR1_UE;
}

void USART3_Send(uint8_t tx_data)
{
	while((USART3->SR & USART_SR_TXE) == 0) {};
	USART3->DR = tx_data;
}

uint8_t USART3_Receive()
{
	while((USART3->SR & USART_SR_RXNE) == 0) {};
	return USART3->DR;
}

void SPI1_Enable()
{
	RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;
	RCC->APB2ENR |= RCC_APB2ENR_SPI1EN;

	PinMode(GPIOA, 4, OUT_PP_50MHz); //Настройка PA4 (SS) - Управление вручную
	PinSet(GPIOA, 4);

	PinMode(GPIOA, 5, AF_PP_50MHz); //Настройка PA5 (SCLK)
	PinMode(GPIOA, 6, IN_FL); //Настройка PA6 (MISO) - в этот раз другой конфиг, до этого был GP_FI
	PinMode(GPIOA, 7, AF_PP_50MHz); //PA7 (MOSI)

	//Настройка регистров SPI
	SPI1->CR1 = 0;
	uint32_t cr1 = SPI1->CR1;

	cr1 |= SPI_CR1_MSTR;
	cr1 |= SPI_CR1_BR_2;
	cr1 |= SPI_CR1_SSI | SPI_CR1_SSM;
	cr1 |= SPI_CR1_SPE;
	
	SPI1->CR1 = cr1;

}

uint8_t SPI1_Transmit(uint8_t tx_data)
{
	while ((SPI1->SR & SPI_SR_TXE) == 0) { };
	SPI1->DR = tx_data;

	while((SPI1->SR & SPI_SR_RXNE) == 0) { };
	uint8_t rx_data  = SPI1->DR;

	return rx_data;
}

void TIM2_PWM_Enable()
{
	RCC->APB1ENR |= RCC_APB1ENR_TIM2EN;

	PinMode(GPIOA, 0, AF_PP_50MHz);

	TIM2->PSC = 8 - 1;
	TIM2->ARR = 2500 - 1;

	TIM2->CCR1 = 500;

	TIM2->CCMR1 |= (6<<4);
	TIM2->CCMR1 |= TIM_CCMR1_OC1PE;

	TIM2->CCER |= TIM_CCER_CC1E;
	TIM2->CR1  |= TIM_CR1_ARPE;
	TIM2->EGR  |= TIM_EGR_UG;
	TIM2->CR1  |= TIM_CR1_CEN;
}



