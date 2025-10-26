
/* Реализация простой библиотеки для модуля MFRC522 под STM32  */

#include "MFRC522.h"

void MFRC522_Init()
{
	PinMode(GPIOA, 2, OUT_PP_50MHz);
	PinReset(GPIOA, 2);
	DelayMs(50);
	PinSet(GPIOA, 2);

	MFRC522_WriteReg(0x2A<<1, 0x8D);
	MFRC522_WriteReg(0x2B<<1, 0x3E);
	MFRC522_WriteReg(0x2D<<1, 30);
	MFRC522_WriteReg(0x2C<<1, 0);

	MFRC522_WriteReg(0x15<<1, 0x40); 
	MFRC522_WriteReg(0x11<<1, 0x3D);

	MFRC522_WriteReg(TX_CTRL_REG, ANT_ON);
}

void MFRC522_SoftReset()
{
	MFRC522_WriteReg(CMD_REG, PCD_SoftReset);
}

void MFRC522_SetBitmask(uint8_t reg, uint8_t mask)
{
	uint8_t tmp = MFRC522_ReadReg(reg);
	MFRC522_WriteReg(reg, tmp | mask);
}

static bool SendREQA()
{
	uint8_t reqa = REQA;
	bool status = MFRC522_ToCard(PCD_Transceive, FRM_7BIT, &reqa, 1);
	return status;	
}

bool MFRC522_IsTagPresent()
{
	if(SendREQA() == false) return false;

	uint8_t err = MFRC522_ReadReg(ERR_REG);
	uint8_t data = MFRC522_ReadReg(FIFO_LVL_REG);

	if(err != 0 || data == 0) return false;
	return true;
}

bool MFRC522_ToCard(uint8_t command, uint8_t bitFraming, uint8_t* fifoData, uint8_t data_len)
{
	MFRC522_SetBitmask(FIFO_LVL_REG, 0x80);
	MFRC522_WriteReg(CMD_REG, PCD_Idle);
	MFRC522_WriteReg(BIT_FRM_REG, bitFraming);

	for(uint8_t i = 0; i<data_len; i++)
	{
		MFRC522_WriteReg(FIFO_DATA_REG, fifoData[i]);
	}

	MFRC522_WriteReg(CMD_REG, command);
	if(command == PCD_Transceive)
	{
		MFRC522_SetBitmask(BIT_FRM_REG, 0x80);
	}

	DelayMs(36);

	/*uint32_t start_ms = elapsed_ms;  <-- Вызывают баг, при котором вместо uid возвращаются нули
	uint32_t cur_ms = 0;
	uint8_t n;
	do
	{
		n = MFRC522_ReadReg(COM_IRQ_REG);
		cur_ms = elapsed_ms - start_ms;
	}
	while(cur_ms<36 && !(n & 0x30));

	if(cur_ms>=36) return false; //Тайм-аут
	*/
	return true;

}

bool MFRC522_ReadUID(volatile uint8_t* uid_buf)
{
	uint8_t fifoData[2] = {0x93, 0x20};
	MFRC522_ToCard(PCD_Transceive, FRM_8BIT, fifoData, 2); 

	memset(uid_buf, 0, sizeof(uid_buf));

	for(int i = 0; i<5; i++)
	{
		uid_buf[i] = MFRC522_ReadReg(FIFO_DATA_REG);
	}

	return MFRC522_CheckUID(uid_buf);
}

bool MFRC522_CheckUID(volatile uint8_t* uid_buf)
{
	for(int i = 0; i<4; i++)
	{
		if(uid_buf[4] == uid_buf[i]) return false;
	}

	uint8_t bcc = uid_buf[0] ^ uid_buf[1] ^ uid_buf[2] ^ uid_buf[3];
	if(bcc==uid_buf[4]) return true;
	return false;	
}

void MFRC522_WriteReg(uint8_t reg, uint8_t value)
{
	PinReset(GPIOA, 4);

	SPI1_Transmit(reg);
	SPI1_Transmit(value);

	PinSet(GPIOA, 4);
}

uint8_t MFRC522_ReadReg(uint8_t reg)
{	
	PinReset(GPIOA, 4);

	SPI1_Transmit(reg | READ_MSK);
	uint8_t received = SPI1_Transmit(0x00);

	PinSet(GPIOA, 4);

	return received;
}
