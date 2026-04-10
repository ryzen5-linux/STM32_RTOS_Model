# STM32F10x 支持的芯片型号

本文档说明本项目模板（基于 ST Standard Peripheral Library V3.5.0）所支持的全部 STM32F10x 芯片型号，以及切换目标芯片时需要修改的配置项。

---

## 1. 芯片型号分组概览

STM32F10x SPL V3.5.0 按 Flash 容量和产品系列将芯片划分为 8 个组，每个组对应一个编译宏：

| 编译宏 | 系列 | Flash 容量 | 典型封装/型号举例 | 备注 |
|--------|------|-----------|-----------------|------|
| `STM32F10X_LD` | F101 / F102 / F103 | 16 – 32 KB | F103C4T6, F103C6T6 | Low density |
| `STM32F10X_LD_VL` | F100 | 16 – 32 KB | F100C4T6, F100C6T6 | Low density Value Line |
| `STM32F10X_MD` | F101 / F102 / F103 | 64 – 128 KB | **F103C8T6** ★, F103CBT6, F103RBT6 | Medium density，**当前项目** |
| `STM32F10X_MD_VL` | F100 | 64 – 128 KB | F100C8T6, F100CBT6, F100RBT6 | Medium density Value Line |
| `STM32F10X_HD` | F101 / F103 | 256 – 512 KB | F103RCT6, F103RET6, F103VET6, F103ZET6 | High density |
| `STM32F10X_HD_VL` | F100 | 256 – 512 KB | F100RCT6, F100RET6, F100VEH6 | High density Value Line |
| `STM32F10X_XL` | F101 / F103 | 512 KB – 1 MB | F103VGT6, F103ZGT6, F103RGT6 | XL density |
| `STM32F10X_CL` | F105 / F107 | 64 – 256 KB | F105RBT6, F105VCT6, F107RCT6, F107VCT6 | Connectivity Line |

> STM32F10x SPL **不支持** STM32F0 / F2 / F3 / F4 / L0 / L4 / H7 等系列。

---

## 2. 各组详细说明

### 2.1 STM32F10X_LD — Low Density（低密度）

- **系列**：STM32F101xx、STM32F102xx、STM32F103xx
- **Flash**：16 KB (x4) / 32 KB (x6)
- **RAM**：6 KB
- **主要特性**：F103 含 USB FS + CAN；F102 仅含 USB FS；F101 为基础型
- **典型型号**：

  | 型号 | Flash | RAM | 封装 | 引脚数 |
  |------|-------|-----|------|--------|
  | STM32F103C4T6 | 16 KB | 6 KB | LQFP48 | 48 |
  | STM32F103C6T6 | 32 KB | 10 KB | LQFP48 | 48 |
  | STM32F103T6U6 | 32 KB | 10 KB | VFQFPN36 | 36 |

---

### 2.2 STM32F10X_LD_VL — Low Density Value Line（低密度超值系列）

- **系列**：STM32F100xx
- **Flash**：16 KB / 32 KB
- **RAM**：4 KB
- **主要特性**：最高 24 MHz，无 USB/CAN，含 DAC，成本更低
- **典型型号**：

  | 型号 | Flash | RAM | 封装 |
  |------|-------|-----|------|
  | STM32F100C4T6B | 16 KB | 4 KB | LQFP48 |
  | STM32F100C6T6B | 32 KB | 4 KB | LQFP48 |

---

### 2.3 STM32F10X_MD — Medium Density（中密度）★ 当前项目

- **系列**：STM32F101xx、STM32F102xx、STM32F103xx
- **Flash**：64 KB (x8) / 128 KB (xB)
- **RAM**：20 KB
- **主要特性**：F103 含 USB FS + CAN，72 MHz，3 个定时器 + 高级定时器
- **典型型号**：

  | 型号 | Flash | RAM | 封装 | 引脚数 | 备注 |
  |------|-------|-----|------|--------|------|
  | **STM32F103C8T6** | **64 KB** | **20 KB** | **LQFP48** | **48** | **本项目默认目标** |
  | STM32F103CBT6 | 128 KB | 20 KB | LQFP48 | 48 | 与 C8T6 引脚兼容 |
  | STM32F103R8T6 | 64 KB | 20 KB | LQFP64 | 64 | |
  | STM32F103RBT6 | 128 KB | 20 KB | LQFP64 | 64 | |
  | STM32F103V8T6 | 64 KB | 20 KB | LQFP100 | 100 | |
  | STM32F103VBT6 | 128 KB | 20 KB | LQFP100 | 100 | |

---

### 2.4 STM32F10X_MD_VL — Medium Density Value Line（中密度超值系列）

- **系列**：STM32F100xx
- **Flash**：64 KB / 128 KB
- **RAM**：8 KB（部分型号 4 KB）
- **主要特性**：最高 24 MHz，无 USB/CAN，含 DAC / 电机控制 PWM
- **典型型号**：

  | 型号 | Flash | RAM | 封装 |
  |------|-------|-----|------|
  | STM32F100C8T6B | 64 KB | 8 KB | LQFP48 |
  | STM32F100CBT6B | 128 KB | 8 KB | LQFP48 |
  | STM32F100RBT6B | 128 KB | 8 KB | LQFP64 |

---

### 2.5 STM32F10X_HD — High Density（高密度）

- **系列**：STM32F101xx、STM32F103xx
- **Flash**：256 KB (xC) / 384 KB (xD) / 512 KB (xE)
- **RAM**：48 KB – 64 KB
- **主要特性**：USB FS + CAN，FSMC（LCD/SRAM 接口），2 个基本定时器，DMA 扩展
- **典型型号**：

  | 型号 | Flash | RAM | 封装 | 引脚数 |
  |------|-------|-----|------|--------|
  | STM32F103RCT6 | 256 KB | 48 KB | LQFP64 | 64 |
  | STM32F103RET6 | 512 KB | 64 KB | LQFP64 | 64 |
  | STM32F103VCT6 | 256 KB | 48 KB | LQFP100 | 100 |
  | STM32F103VET6 | 512 KB | 64 KB | LQFP100 | 100 |
  | STM32F103ZCT6 | 256 KB | 48 KB | LQFP144 | 144 |
  | STM32F103ZET6 | 512 KB | 64 KB | LQFP144 | 144 |

---

### 2.6 STM32F10X_HD_VL — High Density Value Line（高密度超值系列）

- **系列**：STM32F100xx
- **Flash**：256 KB / 384 KB / 512 KB
- **RAM**：32 KB
- **主要特性**：最高 24 MHz，含 DAC，无 USB/CAN
- **典型型号**：

  | 型号 | Flash | RAM | 封装 |
  |------|-------|-----|------|
  | STM32F100RCT6B | 256 KB | 24 KB | LQFP64 |
  | STM32F100RET6B | 512 KB | 32 KB | LQFP64 |
  | STM32F100VEH6 | 512 KB | 32 KB | LQFP100 |

---

### 2.7 STM32F10X_XL — XL Density（超大密度）

- **系列**：STM32F101xx、STM32F103xx
- **Flash**：768 KB (xF) / 1024 KB (xG)
- **RAM**：80 KB – 96 KB
- **主要特性**：双 Flash Bank 支持 Bank 切换更新，其余外设与 HD 相同
- **典型型号**：

  | 型号 | Flash | RAM | 封装 | 引脚数 |
  |------|-------|-----|------|--------|
  | STM32F103RFT6 | 768 KB | 96 KB | LQFP64 | 64 |
  | STM32F103RGT6 | 1024 KB | 96 KB | LQFP64 | 64 |
  | STM32F103VGT6 | 1024 KB | 96 KB | LQFP100 | 100 |
  | STM32F103ZGT6 | 1024 KB | 96 KB | LQFP144 | 144 |

---

### 2.8 STM32F10X_CL — Connectivity Line（互联网络系列）

- **系列**：STM32F105xx、STM32F107xx
- **Flash**：64 KB (x8) / 128 KB (xB) / 256 KB (xC)
- **RAM**：64 KB
- **主要特性**：USB OTG FS（F105）/ USB OTG FS + HS（F107），2× CAN，以太网 MAC（F107），I²S
- **注意**：HSE 默认频率为 **25 MHz**（其他组为 8 MHz），需同步修改 `system_stm32f10x.c` 中的 PLL 配置
- **典型型号**：

  | 型号 | Flash | RAM | 封装 | USB | CAN | ETH |
  |------|-------|-----|------|-----|-----|-----|
  | STM32F105R8T6 | 64 KB | 64 KB | LQFP64 | OTG FS | 2× | — |
  | STM32F105RBT6 | 128 KB | 64 KB | LQFP64 | OTG FS | 2× | — |
  | STM32F105RCT6 | 256 KB | 64 KB | LQFP64 | OTG FS | 2× | — |
  | STM32F105VCT6 | 256 KB | 64 KB | LQFP100 | OTG FS | 2× | — |
  | STM32F107RBT6 | 128 KB | 64 KB | LQFP64 | OTG FS/HS | 2× | ✓ |
  | STM32F107RCT6 | 256 KB | 64 KB | LQFP64 | OTG FS/HS | 2× | ✓ |
  | STM32F107VCT6 | 256 KB | 64 KB | LQFP100 | OTG FS/HS | 2× | ✓ |

---

## 3. 型号命名规则

STM32F10x 型号命名格式：

```
STM32 F 103 C 8 T 6
│     │ │   │ │ │ └─ 温度范围：6 = -40~85°C  7 = -40~105°C
│     │ │   │ │ └─── 封装：T = LQFP  U = UFQFPN  H = TFBGA
│     │ │   │ └───── Flash 容量：4=16K  6=32K  8=64K  B=128K
│     │ │   │                    C=256K  D=384K  E=512K  G=1024K
│     │ │   └─────── 引脚数：C=48  R=64  V=100  Z=144  T=36
│     │ └─────────── 子系列：101 102 103 105 107
│     └───────────── 系列：F = 通用型
└─────────────────── 品牌：STM32
```

---

## 4. 切换目标芯片的方法

当前工程已经把芯片组选型入口统一到了 `Gcc/project.mk`。常规情况下，只需要修改 `MCU` 一项即可完成切换，无需再手工改 `DEFINES`、启动文件和链接脚本。

### 4.1 直接切换预置型号（推荐）

编辑 `Gcc/project.mk`：

```makefile
# 默认：STM32F103C8T6
MCU ?= STM32F103C8T6

# 示例 1：切换到 STM32F103RET6（高密度，512 KB Flash / 64 KB RAM）
# MCU ?= STM32F103RET6

# 示例 2：切换到 STM32F107RCT6（互联网络系列）
# MCU ?= STM32F107RCT6
```

工程会自动派生以下配置：

- `DEFINES`：自动注入对应芯片组宏、`USE_STDPERIPH_DRIVER` 和 `HSE_VALUE`
- `STARTUP_FILE`：自动选中匹配芯片组的 `startup_stm32f10x_*.s`
- `LINKER_SCRIPT`：自动选中匹配 Flash/RAM 容量的 `Gcc/ld/STM32F10X_*K_*K_FLASH.ld`

当前内置的 `MCU` 预置型号覆盖本文档表格中的常见示例型号，包括：

- F103 低/中/高/XL 密度常见封装型号
- F100 低/中/高密度 Value Line 常见型号
- F105 / F107 互联网络系列常见型号

### 4.2 手动定义未预置型号

如果目标芯片没有写入 `Gcc/mcu_profiles.mk`，可在 `Gcc/project.mk` 中直接填写芯片组和内存参数：

```makefile
MCU ?= CUSTOM_STM32F10X
MCU_FAMILY ?= STM32F10X_HD
FLASH_KB ?= 256
RAM_KB ?= 48
STARTUP_GROUP ?= hd
HSE_VALUE_HZ ?= 8000000
```

其中芯片组宏与 `STARTUP_GROUP` 的对应关系如下：

| 目标芯片 | DEFINES 宏 |
|---------|------------|
| STM32F103x4/x6 | `-DSTM32F10X_LD` |
| STM32F100x4/x6 | `-DSTM32F10X_LD_VL` |
| STM32F103x8/xB | `-DSTM32F10X_MD`（默认） |
| STM32F100x8/xB | `-DSTM32F10X_MD_VL` |
| STM32F103xC/xD/xE | `-DSTM32F10X_HD` |
| STM32F100xC/xD/xE | `-DSTM32F10X_HD_VL` |
| STM32F103xF/xG | `-DSTM32F10X_XL` |
| STM32F105/107 | `-DSTM32F10X_CL` |

对应的 `STARTUP_GROUP` 分别为 `ld`、`ld_vl`、`md`、`md_vl`、`hd`、`hd_vl`、`xl`、`cl`。

### 4.3 链接脚本支持现状

当前工程已经补齐常见容量组合的通用链接脚本，位于 `Gcc/ld/` 目录，例如：

- `STM32F10X_16K_4K_FLASH.ld`
- `STM32F10X_64K_20K_FLASH.ld`
- `STM32F10X_256K_48K_FLASH.ld`
- `STM32F10X_512K_64K_FLASH.ld`
- `STM32F10X_1024K_96K_FLASH.ld`

旧的 `Gcc/STM32F103C8Tx_FLASH.ld` 仍然保留，内部会转发到新的通用脚本，兼容旧工程用法。

如果遇到本文未覆盖的特殊内存布局，可继续新增对应容量的 `.ld` 文件，或在 `project.mk` 中直接指定自定义 `LINKER_SCRIPT`。

可通过以下方式获取：
- [STM32CubeMX](https://www.st.com/en/development-tools/stm32cubemx.html) 生成对应芯片的链接脚本
- 参考 [STM32CubeF1](https://github.com/STMicroelectronics/STM32CubeF1) 仓库中的模板

### 4.4 启动文件支持现状

`BSP/CMSIS/` 目录下现已补齐以下芯片组启动文件，并由构建系统按 `STARTUP_GROUP` 自动选择：

| 芯片组 | 启动文件 |
|--------|---------|
| LD | `startup_stm32f10x_ld.s` |
| LD_VL | `startup_stm32f10x_ld_vl.s` |
| MD | `startup_stm32f10x_md.s`（当前） |
| MD_VL | `startup_stm32f10x_md_vl.s` |
| HD | `startup_stm32f10x_hd.s` |
| HD_VL | `startup_stm32f10x_hd_vl.s` |
| XL | `startup_stm32f10x_xl.s` |
| CL | `startup_stm32f10x_cl.s` |

这些文件共用一套复位入口和默认中断处理实现，只在中断向量表上按芯片组区分。

### 4.5 互联网络系列（F105/F107）额外注意事项

切换到 `STM32F10X_CL` 时，还需要：

1. `project.mk` 已自动把 `HSE_VALUE_HZ` 设为 `25000000`，并传递给编译宏
2. `BSP/Core/system_stm32f10x.c` 已包含 `STM32F10X_CL` 的独立时钟配置分支，无需额外改 PLL 代码
3. `CPU_FLAGS` 仍保持 `-mcpu=cortex-m3 -mthumb`，无需修改
4. 若使用以太网功能（F107），需在 `BSP/Core/stm32f10x_conf.h` 中取消对应外设头文件的注释

---

## 5. 当前项目配置

| 配置项 | 当前值 | 说明 |
|--------|--------|------|
| 目标芯片 | STM32F103C8T6 | LQFP48，48 引脚 |
| Flash | 64 KB | 使用率约 42%（27 KB / 64 KB） |
| RAM | 20 KB | Cortex-M3，72 MHz |
| 芯片组宏 | `STM32F10X_MD` | 中密度 |
| 选型入口 | `MCU=STM32F103C8T6` | 在 `Gcc/project.mk` 中配置 |
| 链接脚本 | `ld/STM32F10X_64K_20K_FLASH.ld` | Flash 起始 0x08000000，64 KB |
| 启动文件 | `startup_stm32f10x_md.s` | 中密度通用 |
| HSE | `8000000` | 通过编译宏传入 |
| 内核 | Cortex-M3 | `-mcpu=cortex-m3 -mthumb` |

---

*本文档基于 ST Standard Peripheral Library V3.5.0（2011-03-11）整理。*
