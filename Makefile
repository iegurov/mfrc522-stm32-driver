TARGET := main

CC		 := arm-none-eabi-gcc
LD		 := arm-none-eabi-ld
AS		 := arm-none-eabi-as
OBJCOPY  := arm-none-eabi-objcopy

CPPFLAGS := -I$(CMSIS_STM32F1)/Device/ST/STM32F1xx/Include
CPPFLAGS += -I$(CMSIS_STM32F1)/Include
CPPFLAGS += -DSTM32F103xB
CFLAGS	 := -mcpu=cortex-m3 -g -std=c23 -Wall -Wextra -Wpedantic
CFLAGS   += -ffunction-sections -fdata-sections #Вынос функций, глобальных и СТАТИЧЕСКИХ переменных в отдельные секции для каждой фунцкии и каждой переменной 
CFLAGS	 += -O0

LDSCRIPT := STM32F103XB_FLASH.ld
LDFLAGS	 := -Wl,-T,$(LDSCRIPT),--gc-sections,--no-warn-rwx-segments

TEMPLATES  := $(CMSIS_STM32F1)/Device/ST/STM32F1xx/Source/Templates
STARTUP_AS := $(TEMPLATES)/gcc/startup_stm32f103xb.s
SYSTEM_C   := $(TEMPLATES)/system_stm32f1xx.c

flash: $(TARGET).bin
	st-flash write $< 0x08000000

$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(TARGET).elf: startup_stm32f103xb.o system_stm32f1xx.o $(TARGET).o stmlib.o MFRC522.o RFID_API.o
	$(CC) -o $@ $(CFLAGS) $(LDFLAGS) $^

startup_stm32f103xb.o: $(STARTUP_AS)
	$(AS) -o $@ $<

system_stm32f1xx.o: $(SYSTEM_C)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

clean:
	rm -f *.o *.elf *.bin
