#Output files
PROJECT=QuadrotorFlightControl
EXECUTABLE=$(PROJECT).elf
BIN_IMAGE=$(PROJECT).bin
#============================================================================#

#Cross Compiler
CC=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy

#============================================================================#

CFLAGS=-g -mlittle-endian -mthumb
CFLAGS+=-mcpu=cortex-m4
CFLAGS+=-mfpu=fpv4-sp-d16 -mfloat-abi=hard
CFLAGS+=-ffreestanding -nostdlib -Wall
CFLAGS+=-Wl,-T,stm32_flash.ld

CFLAGS+= \
        -D STM32F40XX \
        -D USE_STDPERIPH_DRIVER \
        -D __FPU_PRESENT=1 \
        -D ARM_MATH_CM4
        # __FPU_USED=1
        #__CC_ARM

LDFLAGS+=-lm -lc -lgcc

ARCH=CM4F
#============================================================================#
CMSIS=./lib/CMSIS

ST=./lib/STM32F4xx_StdPeriph_Driver

FREERTOS=./lib/FreeRTOS
FATFS=./lib/fatfs

CONFIG=./config
COMMON=./common
ALGORITHM=./algorithm
DRIVER=./driver
MODULE=./module
SYSTEM=./system
SHELL_=./shell
DEBUG=./debug
#============================================================================#

#INCLUDE+=./

#INCLUDE+=$(CMSIS)
SRC+=$(CMSIS)/system_stm32f4xx.c

STARTUP=./lib/CMSIS/startup_stm32f4xx.s

INCLUDE+=$(ST)/inc
SRC+=./lib/CMSIS/system_stm32f4xx.c \
        $(ST)/src/misc.c \
        $(ST)/src/stm32f4xx_rcc.c \
        $(ST)/src/stm32f4xx_dma.c \
        $(ST)/src/stm32f4xx_flash.c \
        $(ST)/src/stm32f4xx_gpio.c \
        $(ST)/src/stm32f4xx_usart.c \
        $(ST)/src/stm32f4xx_tim.c \
        $(ST)/src/stm32f4xx_spi.c \
        $(ST)/src/stm32f4xx_i2c.c \
        $(ST)/src/stm32f4xx_sdio.c

INCLUDE+=$(CONFIG)

INCLUDE+=$(COMMON)
SRC+=$(COMMON)/stdlib.c \
	$(COMMON)/string.c

INCLUDE+=$(ALGORITHM)
SRC+=$(ALGORITHM)/moving_average.c \
	$(ALGORITHM)/PID.c \
	$(ALGORITHM)/quaternion.c

INCLUDE+=$(DRIVER)
SRC+=$(DRIVER)/delay.c \
	$(DRIVER)/flash.c \
	$(DRIVER)/i2c.c \
	$(DRIVER)/sdio.c \
	$(DRIVER)/spi.c \
	$(DRIVER)/stm32f4_system.c

INCLUDE+=$(MODULE)
SRC+=$(MODULE)/ak8975.c \
	$(MODULE)/board.c \
	$(MODULE)/motor.c \
	$(MODULE)/mpu6050.c \
	$(MODULE)/mpu9150.c \
	$(MODULE)/ms5611.c \
	$(MODULE)/nrf24l01.c \
	$(MODULE)/sensor.c \
	$(MODULE)/serial.c

INCLDUE+=$(SYSTEM)
SRC+=$(SYSTEM)/AHRS.c \
	$(SYSTEM)/global.c \
	$(SYSTEM)/IMU_correct.c \
	$(SYSTEM)/interrupt.c \
	$(SYSTEM)/main.c \
	$(SYSTEM)/remote_control.c

INCLUDE+=$(SHELL_)
SRC+=$(SHELL_)/linenoise.c \
	$(SHELL_)/parser.c \
	$(SHELL_)/shell.c \
	$(SHELL_)/status_monitor.c

INCLUDE+=$(DEBUG)
SRC+=$(DEBUG)/test.c

INCLUDE+=$(FREERTOS)/Source/include \
	$(FREERTOS)/Source/portable/GCC/ARM_$(ARCH)
SRC+=$(FREERTOS)/Source/croutine.c \
	$(FREERTOS)/Source/list.c \
	$(FREERTOS)/Source/queue.c \
	$(FREERTOS)/Source/tasks.c \
	$(FREERTOS)/Source/timers.c \
	$(FREERTOS)/Source/portable/MemMang/heap_4.c \
	$(FREERTOS)/Source/portable/GCC/ARM_$(ARCH)/port.c \

INCLUDE+=$(FATFS)
SRC+=$(FATFS)/diskio.c \
	$(FATFS)/ff.c \
	$(FATFS)/syscall.c \
	$(FATFS)/unicode.c \

include $(INCLUDE)
CFLAGS+=${-I wildcard $(INCLUDE)}
#============================================================================#

#Make all
all:$(BIN_IMAGE)

$(BIN_IMAGE):$(EXECUTABLE)
	$(OBJCOPY) -O binary $^ $@

STARTUP_OBJ = startup_stm32f4xx.o

$(STARTUP_OBJ): $(STARTUP) ${INCLUDE}
	$(CC) $(CFLAGS) $^ -c $(STARTUP)

$(EXECUTABLE):$(SRC) $(STARTUP_OBJ) ${INCLUDE}
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

#Make clean
clean:
	rm -rf $(EXECUTABLE)
	rm -rf $(BIN_IMAGE)
	rm -rf $(STARTUP_OBJ)

#Make flash
flash:
	st-flash write $(BIN_IMAGE) 0x8000000

#Make openocd
openocd: flash
	openocd -s /opt/openocd/share/openocd/scripts/ -f ./debug/openocd.cfg

#Make gdbauto
gdbauto:
	arm-none-eabi-gdbtui -x ./debug/openocd_gdb.gdb
#============================================================================#

.PHONY:all clean flash openocd gdbauto
