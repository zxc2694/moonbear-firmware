#Output files
EXECUTABLE=QuadrotorFlightControl.elf
BIN_IMAGE=QuadrotorFlightControl.bin
#======================================================================#

#Cross Compiler
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

#FreeRTOS
ARCH=CM4F
FREERTOS_SRC=./lib/FreeRTOS/Source
FREERTOS_INC=./lib/FreeRTOS/Source/include
FREERTOS_PORT_INC=./lib/FreeRTOS/Source/portable/GCC/ARM_$(ARCH)

ST_LIB = ./lib/STM32F4xx_StdPeriph_Driver
PROGRAM_CONFIG = ./config
PROGRAM_ALG = ./algorithm
PROGRAM_DIR = ./driver
PROGRAM_MOD = ./module
PROGRAM_SYS = ./system
PROGRAM_FAT = ./lib/fatfs
PROGRAM_SHELL = ./shell
PROGRAM_DEBUG = ./debug
PROGRAM_COMMON = ./common
_PROJECTFC_ = ./ProjectFC

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
CFLAGS+=-I./lib/CMSIS

#FreeRTOS including
CFLAGS+=-I$(FREERTOS_INC)
CFLAGS+=-I$(FREERTOS_PORT_INC)

#STM32F4xx Peripherys including
CFLAGS+=-I./lib/STM32F4xx_StdPeriph_Driver/inc

#Major programs including
CFLAGS+=-I./config
CFLAGS+=-I./system
CFLAGS+=-I./algorithm
CFLAGS+=-I./driver
CFLAGS+=-I./module
CFLAGS+=-I./lib/fatfs
CFLAGS+=-I./shell
CFLAGS+=-I./debug
CFLAGS+=-I./common

LDFLAGS+=-lm -lc -lgcc

#STM32 CMSIS
SRC=$(wildcard ./lib/CMSIS/*.c)

#STM32F4xx Peripherys source code
#SRC+=$(wildcard ./lib/STM32F4xx_StdPeriph_Driver/src/*.c)
SRC+=./lib/CMSIS/system_stm32f4xx.c \
	$(ST_LIB)/src/misc.c \
	$(ST_LIB)/src/stm32f4xx_rcc.c \
	$(ST_LIB)/src/stm32f4xx_dma.c \
	$(ST_LIB)/src/stm32f4xx_flash.c \
	$(ST_LIB)/src/stm32f4xx_gpio.c \
	$(ST_LIB)/src/stm32f4xx_usart.c \
	$(ST_LIB)/src/stm32f4xx_tim.c\
	$(ST_LIB)/src/stm32f4xx_spi.c\
	$(ST_LIB)/src/stm32f4xx_i2c.c\
	$(ST_LIB)/src/stm32f4xx_sdio.c

#C Standard Lib
SRC+=$(PROGRAM_COMMON)/string.c\
	$(PROGRAM_COMMON)/stdlib.c

#Major programs source code
SRC+=$(PROGRAM_ALG)/*.c \
		$(PROGRAM_DIR)/*.c \
		$(PROGRAM_MOD)/*.c \
		$(PROGRAM_SYS)/interrupt.c \
		$(PROGRAM_SYS)/AHRS.c \
		$(PROGRAM_SYS)/remote_control.c \
		$(PROGRAM_SYS)/IMU_correct.c \
		$(PROGRAM_SYS)/main.c \
		$(PROGRAM_SYS)/global.c \
		lib/CMSIS/FastMathFunctions/arm_cos_f32.c \
		lib/CMSIS/FastMathFunctions/arm_sin_f32.c \

#Fatfs
SRC+=$(PROGRAM_FAT)/diskio.c\
	$(PROGRAM_FAT)/ff.c\
	$(PROGRAM_FAT)/syscall.c\
	$(PROGRAM_FAT)/unicode.c

INC_HEADERS =$(PROGRAM_ALG)/*.h \
		$(PROGRAM_DIR)/*.h \
		$(PROGRAM_MOD)/*.h \
		$(PROGRAM_SYS)/*.h \
		$(PROGRAM_FAT)/*.h \
		$(PROGRAM_SHELL)/*.h\
		$(PROGRAM_DEBUG)/*.h \
		$(PROGRAM_COMMON)/*.h\

#FreeRTOS source code
SRC+=$(FREERTOS_SRC)/croutine.c\
	$(FREERTOS_SRC)/list.c\
	$(FREERTOS_SRC)/queue.c\
	$(FREERTOS_SRC)/tasks.c\
	$(FREERTOS_SRC)/timers.c\
	$(FREERTOS_SRC)/portable/MemMang/heap_4.c\
	$(FREERTOS_SRC)/portable/GCC/ARM_$(ARCH)/port.c\

#Shell and linenoise
SRC+=$(PROGRAM_SHELL)/linenoise.c\
	$(PROGRAM_SHELL)/shell.c\
	$(PROGRAM_SHELL)/parser.c\
	$(PROGRAM_SHELL)/status_monitor.c\

#Debugger
SRC+=$(PROGRAM_DEBUG)/test.c

#STM32 startup file
STARTUP=./lib/CMSIS/startup_stm32f4xx.s
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
	rm -rf $(STARTUP_OBJ)

#Make flash
flash:
	st-flash write $(BIN_IMAGE) 0x8000000

#Make screen
screen:
	sudo su
	screen /dev/ttyUSB0 57600 &
#======================================================================#
openocd: flash
	openocd -s /opt/openocd/share/openocd/scripts/ -f ./debug/openocd.cfg
gdbauto:
	arm-none-eabi-gdbtui -x ./debug/openocd_gdb.gdb

.PHONY:all clean flash openocd gdbauto
