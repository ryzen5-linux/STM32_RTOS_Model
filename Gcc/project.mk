# User-editable project build configuration.

TARGET ?= STM32_RTOS
LINKER_SCRIPT ?= STM32F103C8Tx_FLASH.ld

CPU_FLAGS ?= -mcpu=cortex-m3 -mthumb
DEFINES ?= -DSTM32F10X_MD -DUSE_STDPERIPH_DRIVER

COMMON_FLAGS ?= $(CPU_FLAGS) $(DEFINES) $(INCLUDES) -ffunction-sections -fdata-sections -fno-common -g3 -Og
CFLAGS ?= $(COMMON_FLAGS) -std=gnu11 -Wall -Wextra -Wundef -pipe
LDFLAGS ?= $(CPU_FLAGS) -T"$(LINKER_SCRIPT)" --specs=nano.specs --specs=nosys.specs -Wl,-Map,"$(BUILD_DIR)/$(TARGET).map",--cref -Wl,--gc-sections -Wl,--print-memory-usage
LIBS ?= -Wl,--start-group -lc -lm -Wl,--end-group
