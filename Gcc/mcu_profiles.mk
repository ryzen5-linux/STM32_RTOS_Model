MCU ?= STM32F103C8T6

define define_mcu_profile
ifeq ($(MCU),$(1))
MCU_FAMILY ?= $(2)
FLASH_KB ?= $(3)
RAM_KB ?= $(4)
STARTUP_GROUP ?= $(5)
HSE_VALUE_HZ ?= $(6)
endif
endef

$(eval $(call define_mcu_profile,STM32F103C4T6,STM32F10X_LD,16,6,ld,8000000))
$(eval $(call define_mcu_profile,STM32F103C6T6,STM32F10X_LD,32,10,ld,8000000))
$(eval $(call define_mcu_profile,STM32F103T6U6,STM32F10X_LD,32,10,ld,8000000))

$(eval $(call define_mcu_profile,STM32F100C4T6B,STM32F10X_LD_VL,16,4,ld_vl,8000000))
$(eval $(call define_mcu_profile,STM32F100C6T6B,STM32F10X_LD_VL,32,4,ld_vl,8000000))

$(eval $(call define_mcu_profile,STM32F103C8T6,STM32F10X_MD,64,20,md,8000000))
$(eval $(call define_mcu_profile,STM32F103CBT6,STM32F10X_MD,128,20,md,8000000))
$(eval $(call define_mcu_profile,STM32F103R8T6,STM32F10X_MD,64,20,md,8000000))
$(eval $(call define_mcu_profile,STM32F103RBT6,STM32F10X_MD,128,20,md,8000000))
$(eval $(call define_mcu_profile,STM32F103V8T6,STM32F10X_MD,64,20,md,8000000))
$(eval $(call define_mcu_profile,STM32F103VBT6,STM32F10X_MD,128,20,md,8000000))

$(eval $(call define_mcu_profile,STM32F100C8T6B,STM32F10X_MD_VL,64,8,md_vl,8000000))
$(eval $(call define_mcu_profile,STM32F100CBT6B,STM32F10X_MD_VL,128,8,md_vl,8000000))
$(eval $(call define_mcu_profile,STM32F100RBT6B,STM32F10X_MD_VL,128,8,md_vl,8000000))

$(eval $(call define_mcu_profile,STM32F103RCT6,STM32F10X_HD,256,48,hd,8000000))
$(eval $(call define_mcu_profile,STM32F103RET6,STM32F10X_HD,512,64,hd,8000000))
$(eval $(call define_mcu_profile,STM32F103VCT6,STM32F10X_HD,256,48,hd,8000000))
$(eval $(call define_mcu_profile,STM32F103VET6,STM32F10X_HD,512,64,hd,8000000))
$(eval $(call define_mcu_profile,STM32F103ZCT6,STM32F10X_HD,256,48,hd,8000000))
$(eval $(call define_mcu_profile,STM32F103ZET6,STM32F10X_HD,512,64,hd,8000000))

$(eval $(call define_mcu_profile,STM32F100RCT6B,STM32F10X_HD_VL,256,24,hd_vl,8000000))
$(eval $(call define_mcu_profile,STM32F100RET6B,STM32F10X_HD_VL,512,32,hd_vl,8000000))
$(eval $(call define_mcu_profile,STM32F100VEH6,STM32F10X_HD_VL,512,32,hd_vl,8000000))

$(eval $(call define_mcu_profile,STM32F103RFT6,STM32F10X_XL,768,96,xl,8000000))
$(eval $(call define_mcu_profile,STM32F103RGT6,STM32F10X_XL,1024,96,xl,8000000))
$(eval $(call define_mcu_profile,STM32F103VGT6,STM32F10X_XL,1024,96,xl,8000000))
$(eval $(call define_mcu_profile,STM32F103ZGT6,STM32F10X_XL,1024,96,xl,8000000))

$(eval $(call define_mcu_profile,STM32F105R8T6,STM32F10X_CL,64,64,cl,25000000))
$(eval $(call define_mcu_profile,STM32F105RBT6,STM32F10X_CL,128,64,cl,25000000))
$(eval $(call define_mcu_profile,STM32F105RCT6,STM32F10X_CL,256,64,cl,25000000))
$(eval $(call define_mcu_profile,STM32F105VCT6,STM32F10X_CL,256,64,cl,25000000))
$(eval $(call define_mcu_profile,STM32F107RBT6,STM32F10X_CL,128,64,cl,25000000))
$(eval $(call define_mcu_profile,STM32F107RCT6,STM32F10X_CL,256,64,cl,25000000))
$(eval $(call define_mcu_profile,STM32F107VCT6,STM32F10X_CL,256,64,cl,25000000))

REQUIRED_MCU_FIELDS := MCU_FAMILY FLASH_KB RAM_KB STARTUP_GROUP HSE_VALUE_HZ
$(foreach field,$(REQUIRED_MCU_FIELDS),$(if $(strip $($(field))),,$(error Missing $(field) for MCU '$(MCU)'. Set it in project.mk or add a profile in Gcc/mcu_profiles.mk.)))

LINKER_SCRIPT ?= ld/STM32F10X_$(FLASH_KB)K_$(RAM_KB)K_FLASH.ld
STARTUP_FILE ?= BSP/CMSIS/startup_stm32f10x_$(STARTUP_GROUP).s

MCU_DEFINES := -D$(MCU_FAMILY) -DUSE_STDPERIPH_DRIVER -DHSE_VALUE=$(HSE_VALUE_HZ)U
DEFINES ?= $(MCU_DEFINES) $(EXTRA_DEFINES)