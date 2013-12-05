#Output files
EXECUTABLE=QuadrotorFlightControl.elf
BIN_IMAGE=QuadrotorFlightControl.bin

#======================================================================#
#Cross Compiler
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

CFLAGS=-g -O2 -mlittle-endian -mthumb
CFLAGS+=-mcpu=cortex-m4
CFLAGS+=-mfpu=fpv4-sp-d16
CFLAGS+=-ffreestanding -nostdlib

#Flash
CFLAGS+=-Wl,-T,stm32_flash.ld

CFLAGS+=-I./

#STM32 CMSIS
CFLAGS+=-I./Libraries/CMSIS
#STM32F4xx Peripherys including
CFLAGS+=-I./Libraries/STM32F4xx_StdPeriph_Driver/inc

#Major programs including
CFLAGS+=-I./Program_System
CFLAGS+=-I./Program_Algorithm
CFLAGS+=-I./Program_Driver
CFLAGS+=-I./Program_Module

#STM32 CMSIS
SRC=$(wildcard ./Liraries/CMSIS/*.c)
#STM32F4xx Peripherys source code
SRC+=$(wildcard ./Libraries/STM32F4xx_StdPeriph_Driver/src/*.c)

#Major programs source code
SRC+=$(wildcard ./Program_System/*.c)
SRC+=$(wildcard ./Program_Algorithm/*.c)
SRC+=$(wildcard ./Program_Driver/*.c)
SRC+=$(wildcard ./Program_Module/*.c)

#STM32 startup file
STARTUP=./Liraries/CMSIS/startup_stm32f40xx.s
#======================================================================#

#Make all
all:$(BIN_IMAGE)

$(BIM_IMAGE):$(EXECUTABLE)
	$(OBJCOPY) -O binary $^ $@

$(EXECUTABLE):$(SRC) $(STARTUP)
	$(CC) $(CFLAGS) $^ -o $@

#Make clean
clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(BIN_IMAGE)

#Make flash
flash:
	st-flash write $(BIN_IMAGE) 0x8000000
#======================================================================#

.PHONY:all clean flash
