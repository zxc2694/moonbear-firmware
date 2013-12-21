#Output files
EXECUTABLE=QuadrotorFlightControl.elf
BIN_IMAGE=QuadrotorFlightControl.bin
#======================================================================#

#Cross Compiler
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

#FreeRTOS
ARCH=CM4F
FREERTOS_SRC=./Libraries/FreeRTOS/Source
FREERTOS_INC=./Libraries/FreeRTOS/Source/include
FREERTOS_PORT_INC=./Libraries/FreeRTOS/Source/portable/GCC/ARM_$(ARCH)

ST_LIB = ./Libraries/STM32F4xx_StdPeriph_Driver
PROGRAM_ALG = ./Program_Algorithm
PROGRAM_DIR = ./Program_Driver
PROGRAM_MOD = ./Program_Module
PROGRAM_SYS = ./Program_System
_PROJECTFC_ = ./ProjectFC

TOOLCHAIN_PATH ?= /usr/local/csl/arm-2012.03/arm-none-eabi
C_LIB=$(TOOLCHAIN_PATH)/lib/thumb2

CFLAGS=-g -mlittle-endian -mthumb
CFLAGS+=-mcpu=cortex-m4
CFLAGS+=-mfpu=fpv4-sp-d16 -mfloat-abi=softfp
CFLAGS+=-ffreestanding -nostdlib -Wall

#Flash
CFLAGS+=-Wl,-T,stm32_flash.ld

CFLAGS+=-I./

#STM32 CMSIS
CFLAGS+= \
	-D STM32F40XX \
	-D USE_STDPERIPH_DRIVER \
	-D __FPU_PRESENT=1 \
	-D ARM_MATH_CM4
	# __FPU_USED=1
	#__CC_ARM
CFLAGS+=-I./Libraries/CMSIS

#FreeRTOS including
CFLAGS+=-I$(FREERTOS_INC)
CFLAGS+=-I$(FREERTOS_PORT_INC)

#STM32F4xx Peripherys including
CFLAGS+=-I./Libraries/STM32F4xx_StdPeriph_Driver/inc

#Major programs including
CFLAGS+=-I./Program_System
CFLAGS+=-I./Program_Algorithm
CFLAGS+=-I./Program_Driver
CFLAGS+=-I./Program_Module

LDFLAGS+=-lm -lc -lgcc

#STM32 CMSIS
SRC=$(wildcard ./Libraries/CMSIS/*.c)

#STM32F4xx Peripherys source code
#SRC+=$(wildcard ./Libraries/STM32F4xx_StdPeriph_Driver/src/*.c)
SRC+=./Libraries/CMSIS/system_stm32f4xx.c \
	$(ST_LIB)/src/misc.c \
	$(ST_LIB)/src/stm32f4xx_rcc.c \
	$(ST_LIB)/src/stm32f4xx_dma.c \
	$(ST_LIB)/src/stm32f4xx_flash.c \
	$(ST_LIB)/src/stm32f4xx_gpio.c \
	$(ST_LIB)/src/stm32f4xx_usart.c \
	$(ST_LIB)/src/stm32f4xx_tim.c\
	$(ST_LIB)/src/stm32f4xx_spi.c\
	$(ST_LIB)/src/stm32f4xx_i2c.c

#C Standard Lib
SRC+=./string.c

#Major programs source code
SRC+=$(PROGRAM_ALG)/algorithm_*.c \
		$(PROGRAM_DIR)/stm32f4_*.c \
		$(PROGRAM_MOD)/module_*.c \
		$(PROGRAM_SYS)/QCopterFC_*.c \
		Program_System/QCopterFC.c \
		Program_System/sys_manager.c \
		Libraries/CMSIS/FastMathFunctions/arm_cos_f32.c \
		Libraries/CMSIS/FastMathFunctions/arm_sin_f32.c \
		test.c

INC_HEADERS =$(PROGRAM_ALG)/*.h \
		$(PROGRAM_DIR)/*.h \
		$(PROGRAM_MOD)/*.h \
		$(PROGRAM_SYS)/*.h \
		*.h



#FreeRTOS source code
SRC+=$(FREERTOS_SRC)/croutine.c\
	$(FREERTOS_SRC)/list.c\
	$(FREERTOS_SRC)/queue.c\
	$(FREERTOS_SRC)/tasks.c\
	$(FREERTOS_SRC)/timers.c\
	$(FREERTOS_SRC)/portable/MemMang/heap_2.c\
	$(FREERTOS_SRC)/portable/GCC/ARM_$(ARCH)/port.c\

#STM32 startup file
STARTUP=./Libraries/CMSIS/startup_stm32f4xx.s
#======================================================================#

#Make all
all:$(BIN_IMAGE)

$(BIN_IMAGE):$(EXECUTABLE)
	$(OBJCOPY) -O binary $^ $@

STARTUP_OBJ = startup_stm32f4xx.o

$(STARTUP_OBJ): $(STARTUP) 
	$(CC) $(CFLAGS) $^ -c $(STARTUP)

$(EXECUTABLE):$(SRC) $(STARTUP_OBJ) ${INC_HEADERS}
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

#Make clean
clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(BIN_IMAGE)

#Make flash
flash:
	st-flash write $(BIN_IMAGE) 0x8000000
#======================================================================#
openocd: flash
	openocd -s /opt/openocd/share/openocd/scripts/
gdbauto:
	arm-none-eabi-gdbtui -x openocd_gdb.gdb

.PHONY:all clean flash openocd gdbauto
