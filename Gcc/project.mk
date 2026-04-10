# User-editable project build configuration.

TARGET ?= STM32_RTOS

# Change MCU only for the common cases listed in Gcc/mcu_profiles.mk.
MCU ?= STM32F103C8T6

# If your exact part number is not listed, uncomment and fill these overrides.
# MCU_FAMILY ?= STM32F10X_MD
# FLASH_KB ?= 64
# RAM_KB ?= 20
# STARTUP_GROUP ?= md
# HSE_VALUE_HZ ?= 8000000

EXTRA_DEFINES ?=

include mcu_profiles.mk

CPU_FLAGS ?= -mcpu=cortex-m3 -mthumb

COMMON_FLAGS ?= $(CPU_FLAGS) $(DEFINES) $(INCLUDES) -ffunction-sections -fdata-sections -fno-common -g3 -Og
CFLAGS ?= $(COMMON_FLAGS) -std=gnu11 -Wall -Wextra -Wundef -pipe
LDFLAGS ?= $(CPU_FLAGS) -T"$(LINKER_SCRIPT_PATH)" --specs=nano.specs --specs=nosys.specs -Wl,-Map,"$(BUILD_DIR)/$(TARGET).map",--cref -Wl,--gc-sections -Wl,--print-memory-usage
LIBS ?= -Wl,--start-group -lc -lm -Wl,--end-group
