################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL := cmd.exe
RM := rm -rf

USER_OBJS :=

LIBS := 
PROJ := 

O_SRCS := 
C_SRCS := 
S_SRCS := 
S_UPPER_SRCS := 
OBJ_SRCS := 
ASM_SRCS := 
PREPROCESSING_SRCS := 
OBJS := 
OBJS_AS_ARGS := 
C_DEPS := 
C_DEPS_AS_ARGS := 
EXECUTABLES := 
OUTPUT_FILE_PATH :=
OUTPUT_FILE_PATH_AS_ARGS :=
AVR_APP_PATH :=$$$AVR_APP_PATH$$$
QUOTE := "
ADDITIONAL_DEPENDENCIES:=
OUTPUT_FILE_DEP:=
LIB_DEP:=
LINKER_SCRIPT_DEP:=

# Every subdirectory with source files must be described here
SUBDIRS :=  \
../MotorSystems


# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS +=  \
../AT90CAN_UART.c \
../can.c \
../CANFastTransfer.c \
../commsReceive.c \
../FastTransfer.c \
../Init.c \
../InputButtons.c \
../LEDs.c \
../main.c \
../MotorSystems/Motor.c \
../MotorSystems/motorHandler.c \
../MotorSystems/MotorSubfunctions.c \
../PointerList.c \
../QueueCANtoUART.c \
../SDO.c \
../Timer.c


PREPROCESSING_SRCS += 


ASM_SRCS += 


OBJS +=  \
AT90CAN_UART.o \
can.o \
CANFastTransfer.o \
commsReceive.o \
FastTransfer.o \
Init.o \
InputButtons.o \
LEDs.o \
main.o \
MotorSystems/Motor.o \
MotorSystems/motorHandler.o \
MotorSystems/MotorSubfunctions.o \
PointerList.o \
QueueCANtoUART.o \
SDO.o \
Timer.o

OBJS_AS_ARGS +=  \
AT90CAN_UART.o \
can.o \
CANFastTransfer.o \
commsReceive.o \
FastTransfer.o \
Init.o \
InputButtons.o \
LEDs.o \
main.o \
MotorSystems/Motor.o \
MotorSystems/motorHandler.o \
MotorSystems/MotorSubfunctions.o \
PointerList.o \
QueueCANtoUART.o \
SDO.o \
Timer.o

C_DEPS +=  \
AT90CAN_UART.d \
can.d \
CANFastTransfer.d \
commsReceive.d \
FastTransfer.d \
Init.d \
InputButtons.d \
LEDs.d \
main.d \
MotorSystems/Motor.d \
MotorSystems/motorHandler.d \
MotorSystems/MotorSubfunctions.d \
PointerList.d \
QueueCANtoUART.d \
SDO.d \
Timer.d

C_DEPS_AS_ARGS +=  \
AT90CAN_UART.d \
can.d \
CANFastTransfer.d \
commsReceive.d \
FastTransfer.d \
Init.d \
InputButtons.d \
LEDs.d \
main.d \
MotorSystems/Motor.d \
MotorSystems/motorHandler.d \
MotorSystems/MotorSubfunctions.d \
PointerList.d \
QueueCANtoUART.d \
SDO.d \
Timer.d

OUTPUT_FILE_PATH +=RouterCard.elf

OUTPUT_FILE_PATH_AS_ARGS +=RouterCard.elf

ADDITIONAL_DEPENDENCIES:=

OUTPUT_FILE_DEP:= ./makedep.mk

LIB_DEP+= 

LINKER_SCRIPT_DEP+= 


# AVR32/GNU C Compiler

































./%.o: .././%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.9.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.0.106\include"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax -g2 -Wall -mmcu=at90can128 -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.0.106\gcc\dev\at90can128" -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	

MotorSystems/%.o: ../MotorSystems/%.c
	@echo Building file: $<
	@echo Invoking: AVR/GNU C Compiler : 4.9.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE)  -x c -funsigned-char -funsigned-bitfields -DDEBUG  -I"C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.0.106\include"  -O1 -ffunction-sections -fdata-sections -fpack-struct -fshort-enums -mrelax -g2 -Wall -mmcu=at90can128 -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.0.106\gcc\dev\at90can128" -c -std=gnu99 -MD -MP -MF "$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)"   -o "$@" "$<" 
	@echo Finished building: $<
	



# AVR32/GNU Preprocessing Assembler



# AVR32/GNU Assembler




ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: $(OUTPUT_FILE_PATH) $(ADDITIONAL_DEPENDENCIES)

$(OUTPUT_FILE_PATH): $(OBJS) $(USER_OBJS) $(OUTPUT_FILE_DEP) $(LIB_DEP) $(LINKER_SCRIPT_DEP)
	@echo Building target: $@
	@echo Invoking: AVR/GNU Linker : 4.9.2
	$(QUOTE)C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-gcc.exe$(QUOTE) -o$(OUTPUT_FILE_PATH_AS_ARGS) $(OBJS_AS_ARGS) $(USER_OBJS) $(LIBS) -Wl,-Map="RouterCard.map" -Wl,--start-group -Wl,-lm  -Wl,--end-group -Wl,--gc-sections -mrelax -mmcu=at90can128 -B "C:\Program Files (x86)\Atmel\Studio\7.0\Packs\atmel\ATmega_DFP\1.0.106\gcc\dev\at90can128"  
	@echo Finished building target: $@
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O ihex -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures  "RouterCard.elf" "RouterCard.hex"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -j .eeprom  --set-section-flags=.eeprom=alloc,load --change-section-lma .eeprom=0  --no-change-warnings -O ihex "RouterCard.elf" "RouterCard.eep" || exit 0
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objdump.exe" -h -S "RouterCard.elf" > "RouterCard.lss"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-objcopy.exe" -O srec -R .eeprom -R .fuse -R .lock -R .signature -R .user_signatures "RouterCard.elf" "RouterCard.srec"
	"C:\Program Files (x86)\Atmel\Studio\7.0\toolchain\avr8\avr8-gnu-toolchain\bin\avr-size.exe" "RouterCard.elf"
	
	





# Other Targets
clean:
	-$(RM) $(OBJS_AS_ARGS) $(EXECUTABLES)  
	-$(RM) $(C_DEPS_AS_ARGS)   
	rm -rf "RouterCard.elf" "RouterCard.a" "RouterCard.hex" "RouterCard.lss" "RouterCard.eep" "RouterCard.map" "RouterCard.srec" "RouterCard.usersignatures"
	