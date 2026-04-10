MAKEFILE_DIR := $(patsubst %/,%,$(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
PROJECT_ROOT := $(subst \,/,$(abspath $(MAKEFILE_DIR)/..))
BUILD_DIR ?= build
PROJECT_MK ?= project.mk
AUTO_MK ?= $(BUILD_DIR)/auto_sources.mk
HOST_OS ?= linux

REQUESTED_GOALS := $(if $(MAKECMDGOALS),$(MAKECMDGOALS),all)

-include $(PROJECT_MK)
-include $(AUTO_MK)

LINKER_SCRIPT_PATH := $(abspath $(MAKEFILE_DIR)/$(LINKER_SCRIPT))
NON_STARTUP_ASM := $(filter-out BSP/CMSIS/startup_stm32f10x_%,$(AUTO_SRC_ASM))

ifneq ($(filter-out clean help print-vars,$(REQUESTED_GOALS)),)
ifeq ($(strip $(AUTO_SRC_C)$(AUTO_SRC_ASM)$(AUTO_INC_DIRS)),)
$(error Auto source manifest missing. Run Gcc/scripts/build_win.ps1 or Gcc/scripts/build_linux.sh first.)
endif
ifeq ($(strip $(TARGET)),)
$(error TARGET is not set. Check $(PROJECT_MK).)
endif
ifeq ($(strip $(STARTUP_FILE)),)
$(error STARTUP_FILE is not set. Check $(PROJECT_MK).)
endif
endif

ifeq ($(HOST_OS),windows)
EXEEXT := .exe
CC := "$(subst \,/,$(TOOLCHAIN_BIN))/arm-none-eabi-gcc$(EXEEXT)"
OBJCOPY := "$(subst \,/,$(TOOLCHAIN_BIN))/arm-none-eabi-objcopy$(EXEEXT)"
SIZE := "$(subst \,/,$(TOOLCHAIN_BIN))/arm-none-eabi-size$(EXEEXT)"
define make-dir
	powershell -NoProfile -Command "New-Item -ItemType Directory -Force -Path '$(subst /,\\,$1)' | Out-Null"
endef
define clean-dir
	powershell -NoProfile -Command "if (Test-Path '$(subst /,\\,$(BUILD_DIR))') { Remove-Item -Recurse -Force '$(subst /,\\,$(BUILD_DIR))' }"
endef
else
CC := arm-none-eabi-gcc
OBJCOPY := arm-none-eabi-objcopy
SIZE := arm-none-eabi-size
define make-dir
	mkdir -p "$1"
endef
define clean-dir
	rm -rf "$(BUILD_DIR)"
endef
endif

INCLUDES := $(addprefix -I$(PROJECT_ROOT)/,$(AUTO_INC_DIRS))

SRC_C := $(AUTO_SRC_C)
SRC_ASM := $(NON_STARTUP_ASM) $(STARTUP_FILE)
SRC_FILES := $(addprefix ../,$(SRC_C)) $(addprefix ../,$(SRC_ASM))

SRC_C_COUNT := $(words $(SRC_C))
SRC_ASM_COUNT := $(words $(SRC_ASM))
INC_DIR_COUNT := $(words $(AUTO_INC_DIRS))
SRC_TOTAL_COUNT := $(words $(SRC_FILES))

ELF_FILE := $(BUILD_DIR)/$(TARGET).elf
HEX_FILE := $(BUILD_DIR)/$(TARGET).hex
BIN_FILE := $(BUILD_DIR)/$(TARGET).bin

.PHONY: all elf bin hex size clean help print-vars manifest

all: elf hex bin size

elf: $(ELF_FILE)

hex: $(HEX_FILE)

bin: $(BIN_FILE)

size: $(ELF_FILE)
	@echo [SIZE] $(ELF_FILE)
	@$(SIZE) $(ELF_FILE)

clean:
	@echo [CLEAN] $(BUILD_DIR)
	@$(call clean-dir)

help:
	@echo Targets:
	@echo "  all       - build elf, hex, bin and print size"
	@echo "  elf       - build elf only"
	@echo "  hex       - build hex only"
	@echo "  bin       - build bin only"
	@echo "  manifest  - show discovered source/include lists"
	@echo "  size      - print elf size"
	@echo "  clean     - remove build output"
	@echo "  print-vars- print auto discovered source/include lists"

manifest: print-vars

print-vars:
	@echo PROJECT_ROOT=$(PROJECT_ROOT)
	@echo BUILD_DIR=$(BUILD_DIR)
	@echo PROJECT_MK=$(PROJECT_MK)
	@echo HOST_OS=$(HOST_OS)
	@echo CC=$(CC)
	@echo OBJCOPY=$(OBJCOPY)
	@echo SIZE=$(SIZE)
	@echo TARGET=$(TARGET)
	@echo MCU=$(MCU)
	@echo MCU_FAMILY=$(MCU_FAMILY)
	@echo FLASH_KB=$(FLASH_KB)
	@echo RAM_KB=$(RAM_KB)
	@echo STARTUP_GROUP=$(STARTUP_GROUP)
	@echo STARTUP_FILE=$(STARTUP_FILE)
	@echo HSE_VALUE_HZ=$(HSE_VALUE_HZ)
	@echo LINKER_SCRIPT=$(LINKER_SCRIPT)
	@echo LINKER_SCRIPT_PATH=$(LINKER_SCRIPT_PATH)
	@echo CFLAGS=$(CFLAGS)
	@echo LDFLAGS=$(LDFLAGS)
	@echo LIBS=$(LIBS)
	@echo SRC_C_COUNT=$(SRC_C_COUNT)
	@echo SRC_ASM_COUNT=$(SRC_ASM_COUNT)
	@echo SRC_TOTAL_COUNT=$(SRC_TOTAL_COUNT)
	@echo INC_DIR_COUNT=$(INC_DIR_COUNT)
	@echo SRC_C=$(SRC_C)
	@echo SRC_ASM=$(SRC_ASM)
	@echo AUTO_INC_DIRS=$(AUTO_INC_DIRS)

$(ELF_FILE): $(AUTO_MK) $(SRC_FILES)
	@$(call make-dir,$(@D))
	@echo [LD] $@
	@echo [LD_CMD] $(CC) $(CFLAGS) $(SRC_FILES) $(LDFLAGS) -o $@ $(LIBS)
	@$(CC) $(CFLAGS) $(SRC_FILES) $(LDFLAGS) -o $@ $(LIBS)

$(HEX_FILE): $(ELF_FILE)
	@echo [HEX] $@
	@echo [HEX_CMD] $(OBJCOPY) -O ihex $< $@
	@$(OBJCOPY) -O ihex $< $@

$(BIN_FILE): $(ELF_FILE)
	@echo [BIN] $@
	@echo [BIN_CMD] $(OBJCOPY) -O binary $< $@
	@$(OBJCOPY) -O binary $< $@
