
/* Попытка написать простую библиотеку для модуля MFRC522 для работы с STM32  */

#ifndef MFRC522
#define MFRC522

#include "stmlib.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/* Регистры */
#define CMD_REG		  (0x01 << 1)
#define ERR_REG		  (0x06 << 1)
#define FIFO_DATA_REG (0x09 << 1)
#define FIFO_LVL_REG  (0x0A << 1)
#define TX_CTRL_REG	  (0x14 << 1)
#define CTRL_REG 	  (0x0C << 1)
#define BIT_FRM_REG   (0x0D << 1)
#define COM_IRQ_REG   (0x04 << 1)

/* Команды */
#define ANT_ON 		0x03 //Включение антенны 
#define FRM_7BIT	0x07
#define FRM_8BIT	0x00
#define REQA		0x26

/* PCD-команды */
#define PCD_SoftReset  0x0F
#define PCD_Idle	   0x00
#define PCD_Transceive 0x0C
#define PCD_Reset	   0x0F

/* Маски */
#define REG_MSK		(0x7E)
#define READ_MSK	(0x80)

/* Основные функции */
void MFRC522_SoftReset();
void MFRC522_Init();
void MFRC522_AntennaON();

bool MFRC522_ToCard(uint8_t command, uint8_t bitFraming, uint8_t* fifoData, uint8_t data_len);
bool MFRC522_IsTagPresent();
bool MFRC522_ReadUID(volatile uint8_t* uid_buf);
bool MFRC522_CheckUID(volatile uint8_t* uid_buf);

/* Дополнительные функции */
void MFRC522_WriteReg(uint8_t reg, uint8_t value);
uint8_t MFRC522_ReadReg(uint8_t reg);

extern uint32_t elapsed_ms;

/*
 
1. Сброс модуля
2. Настройка
3. Запрос на наличие метки
4. Получение ответа в виде переменной

Ну и соответственно служебные команды:

1. Чтение регистра
2. Запись в регистр

 */

#endif
