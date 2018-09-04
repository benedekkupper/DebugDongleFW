########+++++++++++++++++++++++++++++++++++++++########
##++----  Makefile for DfuBootloader project   ----++##
########+++++++++++++++++++++++++++++++++++++++########

# optimization
OPT = -O3
DEBUG = 0

TARGET = DebugDongle

# Build path
BUILD_DIR = build_$(VID)_$(PID)

##++----  Target configuration  ----++##

CORE = m0
SERIES = STM32F0
DEVICE = STM32F042F6
BSP = BSP

C_DEFS =  \
-DUSBD_VID=0x$(VID) \
-DUSBD_PID=0x$(PID) \
-DHW_REV=$(HW_REV)


##++----  Build tool binaries  ----++##
BINPATH = /usr/bin
PREFIX = arm-none-eabi-
CC = $(BINPATH)/$(PREFIX)gcc
AS = $(BINPATH)/$(PREFIX)gcc -x assembler-with-cpp
CP = $(BINPATH)/$(PREFIX)objcopy
AR = $(BINPATH)/$(PREFIX)ar
SZ = $(BINPATH)/$(PREFIX)size
HEX = $(CP) -O ihex
BIN = $(CP) -O binary -S


##++----  MCU config  ----++##
CPU = -mcpu=cortex-$(CORE)
MCU = $(CPU) -mthumb -mfloat-abi=soft


##++----  Assembler  ----++##
AS_DEFS = 
AS_INCLUDES = 
# assembly sources
AS_SOURCES = $(wildcard $(BSP)/*.s)

# assembler flags
ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections


##++----  Compiler  ----++##
C_STANDARD = -std=gnu11

XPD_DIR = STM32_XPD
USBD_DIR = USBDevice

# All USBDevice classes are built, but only DFU is used
# C includes
C_INCLUDES =  \
-I$(BSP) \
-IApp \
-ICharger \
-ISensor \
-IVCP \
-I$(USBD_DIR)/Device \
-I$(USBD_DIR)/Class/CDC \
-I$(USBD_DIR)/Class/DFU \
-I$(USBD_DIR)/Class/HID \
-I$(USBD_DIR)/PDs/STM32_XPD \
-I$(XPD_DIR)/CMSIS/Include \
-I$(XPD_DIR)/CMSIS/Device/ST/$(SERIES)xx/Include \
-I$(XPD_DIR)/$(SERIES)_XPD/inc

# C sources
C_SOURCES =  \
$(wildcard $(BSP)/*.c) \
$(wildcard App/*.c) \
$(wildcard Charger/*.c) \
$(wildcard Sensor/*.c) \
$(wildcard VCP/*.c) \
$(wildcard $(USBD_DIR)/Device/Src/*.c) \
$(wildcard $(USBD_DIR)/Class/CDC/Src/*.c) \
$(wildcard $(USBD_DIR)/Class/DFU/Src/*.c) \
$(wildcard $(USBD_DIR)/Class/HID/Src/*.c) \
$(wildcard $(XPD_DIR)/$(SERIES)_XPD/src/*.c)

# compiler flags
CFLAGS = $(MCU) $(C_DEFS) $(C_INCLUDES) $(OPT) -Wall -fdata-sections -ffunction-sections $(C_STANDARD)

ifeq ($(DEBUG), 1)
CFLAGS += -g -gdwarf-2
endif

# Generate dependency information
CFLAGS += -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)"


##++----  Linker  ----++##
# link script
LDSCRIPT = $(wildcard $(BSP)/*.ld)

# libraries
LIBS = -lc -lm -lnosys 
LIBDIR = 
LDFLAGS = $(MCU) -specs=nano.specs -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections


##++----  Build the application  ----++##
# default action: build all
all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex

# list of objects
OBJECTS = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
vpath %.c $(sort $(dir $(C_SOURCES)))
# list of ASM program objects
OBJECTS += $(addprefix $(BUILD_DIR)/,$(notdir $(AS_SOURCES:.s=.o)))
vpath %.s $(sort $(dir $(AS_SOURCES)))

$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR) 
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:.c=.lst)) $< -o $@

$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(CFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJECTS) Makefile
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(HEX) $< $@

$(BUILD_DIR):
	mkdir $@

##++----  Clean  ----++##
clean:
	-rm -fR .dep $(BUILD_DIR)


##++----  Dependencies  ----++##
-include $(shell mkdir .dep 2>/dev/null) $(wildcard .dep/*)

# *** EOF ***