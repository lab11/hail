################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS +=  \
../src/ASF/common/services/clock/sam4l/dfll.c \
../src/ASF/common/services/clock/sam4l/osc.c \
../src/ASF/common/services/clock/sam4l/pll.c \
../src/ASF/common/services/clock/sam4l/sysclk.c \
../src/ASF/common/services/serial/usart_serial.c \
../src/ASF/common/services/sleepmgr/sam4l/sleepmgr.c \
../src/ASF/common/utils/stdio/read.c \
../src/ASF/common/utils/stdio/write.c \
../src/ASF/sam/drivers/acifc/acifc.c \
../src/ASF/sam/drivers/adcife/adcife.c \
../src/ASF/sam/drivers/aesa/aesa.c \
../src/ASF/sam/drivers/ast/ast.c \
../src/ASF/sam/drivers/bpm/bpm.c \
../src/ASF/sam/drivers/crccu/crccu.c \
../src/ASF/sam/drivers/dacc/dacc.c \
../src/ASF/sam/drivers/eic/eic.c \
../src/ASF/sam/drivers/freqm/freqm.c \
../src/ASF/sam/drivers/events/events.c \
../src/ASF/sam/drivers/flashcalw/flashcalw.c \
../src/ASF/sam/drivers/gloc/gloc.c \
../src/ASF/sam/drivers/gpio/gpio.c \
../src/ASF/sam/drivers/hmatrixb/hmatrixb.c \
../src/ASF/sam/drivers/lcdca/lcdca.c \
../src/ASF/sam/drivers/parc/parc.c \
../src/ASF/sam/drivers/pdca/pdca.c \
../src/ASF/sam/drivers/picouart/picouart.c \
../src/ASF/sam/drivers/spi/spi.c \
../src/ASF/sam/drivers/tc/tc.c \
../src/ASF/sam/drivers/twim/twim.c \
../src/ASF/sam/drivers/twis/twis.c \
../src/ASF/sam/drivers/usart/usart.c \
../src/ASF/sam/drivers/wdt/wdt_sam4l.c \
../src/ASF/common/utils/interrupt/interrupt_sam_nvic.c \
../src/ASF/sam/utils/cmsis/sam4l/source/templates/exceptions.c \
../src/ASF/sam/utils/cmsis/sam4l/source/templates/gcc/startup_sam4l.c \
../src/ASF/sam/utils/cmsis/sam4l/source/templates/system_sam4l.c \
../src/ASF/sam/utils/syscalls/gcc/syscalls.c \
../src/main.c \
../src/bootloader.c \
../src/bl_commands.c \
../src/crc32.c

OBJS +=  \
src/ASF/common/services/clock/sam4l/dfll.o \
src/ASF/common/services/clock/sam4l/osc.o \
src/ASF/common/services/clock/sam4l/pll.o \
src/ASF/common/services/clock/sam4l/sysclk.o \
src/ASF/common/services/serial/usart_serial.o \
src/ASF/common/services/sleepmgr/sam4l/sleepmgr.o \
src/ASF/common/utils/stdio/read.o \
src/ASF/common/utils/stdio/write.o \
src/ASF/sam/drivers/acifc/acifc.o \
src/ASF/sam/drivers/adcife/adcife.o \
src/ASF/sam/drivers/aesa/aesa.o \
src/ASF/sam/drivers/ast/ast.o \
src/ASF/sam/drivers/bpm/bpm.o \
src/ASF/sam/drivers/crccu/crccu.o \
src/ASF/sam/drivers/dacc/dacc.o \
src/ASF/sam/drivers/eic/eic.o \
src/ASF/sam/drivers/freqm/freqm.o \
src/ASF/sam/drivers/events/events.o \
src/ASF/sam/drivers/flashcalw/flashcalw.o \
src/ASF/sam/drivers/gloc/gloc.o \
src/ASF/sam/drivers/gpio/gpio.o \
src/ASF/sam/drivers/hmatrixb/hmatrixb.o \
src/ASF/sam/drivers/lcdca/lcdca.o \
src/ASF/sam/drivers/parc/parc.o \
src/ASF/sam/drivers/pdca/pdca.o \
src/ASF/sam/drivers/picouart/picouart.o \
src/ASF/sam/drivers/spi/spi.o \
src/ASF/sam/drivers/tc/tc.o \
src/ASF/sam/drivers/twim/twim.o \
src/ASF/sam/drivers/twis/twis.o \
src/ASF/sam/drivers/usart/usart.o \
src/ASF/sam/drivers/wdt/wdt_sam4l.o \
src/ASF/common/utils/interrupt/interrupt_sam_nvic.o \
src/ASF/sam/utils/cmsis/sam4l/source/templates/exceptions.o \
src/ASF/sam/utils/cmsis/sam4l/source/templates/gcc/startup_sam4l.o \
src/ASF/sam/utils/cmsis/sam4l/source/templates/system_sam4l.o \
src/ASF/sam/utils/syscalls/gcc/syscalls.o \
src/main.o \
src/bootloader.o \
src/bl_commands.o \
src/crc32.o \
src/jumpfunc.o

BINARY_NAME=hail_bootloader

OUTPUT_FILE_PATH +=$(BINARY_NAME).elf
CCBIN=arm-none-eabi-gcc
OBJCOPY=arm-none-eabi-objcopy
OBJDUMP=arm-none-eabi-objdump
SIZE=arm-none-eabi-size
JLINK=JLinkExe
CINCLUDES=-I"../src" -I"../src/ASF/common/boards/user_board" -I"../src/ASF/common/utils" -I"../src/ASF/sam/utils"
CINCLUDES+=-I"../src/ASF/sam/utils/cmsis/sam4l/include" -I"../src/ASF/sam/utils/cmsis/sam4l/source/templates"
CINCLUDES+=-I"../src/ASF/sam/utils/header_files" -I"../src/ASF/sam/utils/preprocessor" -I"../src/ASF/thirdparty/CMSIS/Include"
CINCLUDES+=-I"../src/ASF/thirdparty/CMSIS/Lib/GCC" -I"../src/config" -I"$(ToolchainDir)\..\..\CMSIS_Atmel"
CINCLUDES+=-I"$(ToolchainDir)\..\..\CMSIS_Atmel\CMSIS\Include" -I"$(ToolchainDir)\..\..\CMSIS_Atmel\Device\ATMEL"
CINCLUDES+=-I"$(ToolchainDir)\..\..\CMSIS_Atmel\Device\ATMEL\sam4l\include" -I"../src/ASF/sam/drivers/bpm"
CINCLUDES+=-I"../src/ASF/sam/drivers/flashcalw" -I"../src/ASF/common/services/clock" -I"../src/ASF/common/services/ioport"
CINCLUDES+=-I"../src/ASF/sam/drivers/wdt" -I"../src/ASF/common/boards" -I"../src/ASF/sam/drivers/acifc"
CINCLUDES+=-I"../src/ASF/sam/drivers/adcife" -I"../src/ASF/sam/drivers/aesa" -I"../src/ASF/sam/drivers/ast"
CINCLUDES+=-I"../src/ASF/sam/drivers/cpu" -I"../src/ASF/sam/drivers/crccu" -I"../src/ASF/sam/drivers/dacc"
CINCLUDES+=-I"../src/ASF/sam/drivers/freqm" -I"../src/ASF/sam/drivers/eic" -I"../src/ASF/sam/drivers/events"
CINCLUDES+=-I"../src/ASF/sam/drivers/gloc" -I"../src/ASF/sam/drivers/gpio" -I"../src/ASF/sam/drivers/hmatrixb"
CINCLUDES+=-I"../src/ASF/sam/drivers/lcdca" -I"../src/ASF/sam/drivers/pdca" -I"../src/ASF/sam/drivers/picouart"
CINCLUDES+=-I"../src/ASF/sam/drivers/spi" -I"../src/ASF/sam/drivers/tc" -I"../src/ASF/sam/drivers/twim"
CINCLUDES+=-I"../src/ASF/sam/drivers/twis" -I"../src/ASF/sam/drivers/usart" -I"../src/ASF/common/services/serial/sam_uart"
CINCLUDES+=-I"../src/ASF/common/services/serial" -I"../src/ASF/common/services/sleepmgr"
CINCLUDES+=-I"../src/ASF/common/utils/stdio/stdio_serial" -I"../src/ASF/sam/drivers/parc"

CWARNS=-Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int -Wmain -Wparentheses -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs -Wunused -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef -Wshadow -Wbad-function-cast -Wwrite-strings -Wsign-compare -Waggregate-return -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wredundant-decls -Wnested-externs -Wlong-long -Wunreachable-code -Wcast-align -Wall -Wstrict-prototypes -Werror-implicit-function-declaration -Wpointer-arith -Wall
CMISCFLAGS=-mthumb -D__SAM4LC8C__ -D__ATSAM4LC8C__ -DARM_MATH_CM4=true  -O1 -fdata-sections -ffunction-sections -mlong-calls -g3 -mcpu=cortex-m4 -c -pipe -fno-strict-aliasing -std=gnu99 -ffunction-sections -fdata-sections --param max-inline-insns-single=500

CPROJFLAGS=-DBOARD=USER_BOARD -Dprintf=iprintf -Dscanf=iscanf -DPARC_CALLBACK_MODE=false

CFLAGS=$(CINCLUDES) $(CWARNS) $(CMISCFLAGS) $(CPROJFLAGS)

src/ASF/common/services/clock/sam4l/%.o: ../src/ASF/common/services/clock/sam4l/%.c
	mkdir -p src/ASF/common/services/clock/sam4l
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/common/services/serial/%.o: ../src/ASF/common/services/serial/%.c
	mkdir -p src/ASF/common/services/serial
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/common/services/sleepmgr/sam4l/%.o: ../src/ASF/common/services/sleepmgr/sam4l/%.c
	mkdir -p src/ASF/common/services/sleepmgr/sam4l
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/common/utils/stdio/%.o: ../src/ASF/common/utils/stdio/%.c
	mkdir -p src/ASF/common/utils/stdio
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/acifc/%.o: ../src/ASF/sam/drivers/acifc/%.c
	mkdir -p src/ASF/sam/drivers/acifc
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/adcife/%.o: ../src/ASF/sam/drivers/adcife/%.c
	mkdir -p src/ASF/sam/drivers/adcife
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/aesa/%.o: ../src/ASF/sam/drivers/aesa/%.c
	mkdir -p src/ASF/sam/drivers/aesa
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/ast/%.o: ../src/ASF/sam/drivers/ast/%.c
	mkdir -p src/ASF/sam/drivers/ast
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/bpm/%.o: ../src/ASF/sam/drivers/bpm/%.c
	mkdir -p src/ASF/sam/drivers/bpm
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/crccu/%.o: ../src/ASF/sam/drivers/crccu/%.c
	mkdir -p src/ASF/sam/drivers/crccu
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/dacc/%.o: ../src/ASF/sam/drivers/dacc/%.c
	mkdir -p src/ASF/sam/drivers/dacc
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/eic/%.o: ../src/ASF/sam/drivers/eic/%.c
	mkdir -p src/ASF/sam/drivers/eic
	$(CCBIN) $(CFLAGS) -o "$@" "$<"

src/ASF/sam/drivers/freqm/%.o: ../src/ASF/sam/drivers/freqm/%.c
	mkdir -p src/ASF/sam/drivers/freqm
	$(CCBIN) $(CFLAGS) -o "$@" "$<"

src/ASF/sam/drivers/events/%.o: ../src/ASF/sam/drivers/events/%.c
	mkdir -p src/ASF/sam/drivers/events
	$(CCBIN) $(CFLAGS) -o "$@" "$<"

src/ASF/sam/drivers/flashcalw/%.o: ../src/ASF/sam/drivers/flashcalw/%.c
	mkdir -p src/ASF/sam/drivers/flashcalw
	$(CCBIN) $(CFLAGS) -o "$@" "$<"

src/ASF/sam/drivers/gloc/%.o: ../src/ASF/sam/drivers/gloc/%.c
	mkdir -p src/ASF/sam/drivers/gloc
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/gpio/%.o: ../src/ASF/sam/drivers/gpio/%.c
	mkdir -p src/ASF/sam/drivers/gpio
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/hmatrixb/%.o: ../src/ASF/sam/drivers/hmatrixb/%.c
	mkdir -p src/ASF/sam/drivers/hmatrixb
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/lcdca/%.o: ../src/ASF/sam/drivers/lcdca/%.c
	mkdir -p src/ASF/sam/drivers/lcdca
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/parc/%.o: ../src/ASF/sam/drivers/parc/%.c
	mkdir -p src/ASF/sam/drivers/parc
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/pdca/%.o: ../src/ASF/sam/drivers/pdca/%.c
	mkdir -p src/ASF/sam/drivers/pdca
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/picouart/%.o: ../src/ASF/sam/drivers/picouart/%.c
	mkdir -p src/ASF/sam/drivers/picouart
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/spi/%.o: ../src/ASF/sam/drivers/spi/%.c
	mkdir -p src/ASF/sam/drivers/spi
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/tc/%.o: ../src/ASF/sam/drivers/tc/%.c
	mkdir -p src/ASF/sam/drivers/tc
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/twim/%.o: ../src/ASF/sam/drivers/twim/%.c
	mkdir -p src/ASF/sam/drivers/twim
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/twis/%.o: ../src/ASF/sam/drivers/twis/%.c
	mkdir -p src/ASF/sam/drivers/twis
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/usart/%.o: ../src/ASF/sam/drivers/usart/%.c
	mkdir -p src/ASF/sam/drivers/usart
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/drivers/wdt/%.o: ../src/ASF/sam/drivers/wdt/%.c
	mkdir -p src/ASF/sam/drivers/wdt
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/%.o: ../src/%.c
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/common/boards/user_board/%.o: ../src/ASF/common/boards/user_board/%.c
	mkdir -p src/ASF/common/boards/user_board
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/common/utils/interrupt/%.o: ../src/ASF/common/utils/interrupt/%.c
	mkdir -p src/ASF/common/utils/interrupt
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/utils/cmsis/sam4l/source/templates/%.o: ../src/ASF/sam/utils/cmsis/sam4l/source/templates/%.c
	mkdir -p src/ASF/sam/utils/cmsis/sam4l/source/templates
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/utils/cmsis/sam4l/source/templates/gcc/%.o: ../src/ASF/sam/utils/cmsis/sam4l/source/templates/gcc/%.c
	mkdir -p src/ASF/sam/utils/cmsis/sam4l/source/templates/gcc
	$(CCBIN) $(CFLAGS) -o "$@" "$<"


src/ASF/sam/utils/syscalls/gcc/%.o: ../src/ASF/sam/utils/syscalls/gcc/%.c
	mkdir -p src/ASF/sam/utils/syscalls/gcc
	$(CCBIN) $(CFLAGS) -o "$@" "$<"




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif


LFILE=../src/bootloader.ld

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

src/jumpfunc.o: ../src/jumpfunc.s
	$(CCBIN) -c -o "$@" "$<"

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS)
	$(CCBIN) -o$(OUTPUT_FILE_PATH) $(OBJS) $(LIBS) -mthumb -Wl,-Map="$(BINARY_NAME).map" -Wl,--start-group -larm_cortexM4l_math -lm  -Wl,--end-group -L"../src/ASF/thirdparty/CMSIS/Lib/GCC"  -Wl,--gc-sections -mcpu=cortex-m4 -Wl,--entry=Reset_Handler -Wl,--cref -mthumb -T $(LFILE)
	$(SIZE) $(BINARY_NAME).elf
	$(OBJCOPY) -O binary $(BINARY_NAME).elf $(BINARY_NAME).bin

prog: all
	$(JLINK) prog.jlink || true

calib: all
	$(JLINK) calib.jlink || true
# Other Targets
clean:
	-$(RM) $(OBJS) $(EXECUTABLES)
	rm -f $(BINARY_NAME).elf
	rm -f $(BINARY_NAME).bin
	rm -f $(BINARY_NAME).map
	rm -rf src
