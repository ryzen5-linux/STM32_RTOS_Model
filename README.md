# STM32F10x FreeRTOS 开发模板

一个默认面向 STM32F103C8T6、同时覆盖多种 STM32F10x SPL 芯片组的 FreeRTOS 开发模板，内置自动化基础构建脚本、运行自检与系统级心跳支持。

## 核心特性

- **稳定内核**：基于 FreeRTOS，已配置好任务、队列、互斥量、信号量与软件定时器的基础演示。
- **开箱即用构建**：使用 GCC (arm-none-eabi-gcc) + Makefile，跨平台支持 Windows (CMD/PowerShell) 与 Linux (Bash)。自带源文件自动发现机制，无需手动修改大段 Makefile。
- **芯片快速切换**：通过修改 `Gcc/project.mk` 中的 `MCU` 一项，即可在已内置的 F100 / F103 / F105 / F107 常见型号之间切换；未预置型号也可通过手动填写 Flash/RAM/芯片组参数完成适配。
- **完备的启动自检**：上电即对 RCC、GPIO、USART、TIM、ADC、DMA、EXTI 和 RTC 等关键外设进行自动化连通性验证。
- **系统监控日志**：内置 Task3 心跳任务，默认一秒输出一次 RTOS 状态 (Tick、任务数、队列深度、空闲堆、历史最小空闲堆)。
- **规范的目录拆分**：BSP、驱动 (Peripheral/Device/Support/Resources)、应用代码 (App)、内核与外设库，天然支持业务分层。

## 环境要求

### 硬件
- 核心板：默认 STM32F103C8T6，也支持文档中列出的 STM32F10x 常见型号
- 默认调试端口：USART2 (PA2 TX / PA3 RX，波特率 115200)，用于输出自检信息和日志。

### 软件
- **编译器**：[GNU Arm Embedded Toolchain](https://developer.arm.com/downloads/-/gnu-rm) (推荐 10.3.1 / 2021.10)
  - Windows: 放在项目上级目录 `GNU Arm Embedded Toolchain\bin\arm-none-eabi-gcc.exe`
  - Linux: 需保证 `arm-none-eabi-gcc` 已加入全局 PATH
- **构建工具**：`make` (Windows 推荐使用 MSYS2 安装 `mingw32-make.exe`)

## 快速开始

### 1. 编译构建

**Windows (CMD):**
```cmd
:: 执行完整构建 (包含 elf, hex, bin 转换及输出 size)
.\Gcc\scripts\build_win.cmd all
```

**Windows (PowerShell):**
```powershell
# 执行完整构建 (包含 elf, hex, bin 转换及输出 size)
.\Gcc\scripts\build_win.ps1 all
```

**Linux (Bash):**
```bash
# 赋予执行权限后构建
chmod +x ./Gcc/scripts/build_linux.sh
./Gcc/scripts/build_linux.sh all
```

产物路径：`Gcc/build/STM32_RTOS.hex` (或 `.bin`)
编译日志：`Gcc/build/build.log`

Windows CMD 脚本同样支持常用构建动作：

```cmd
:: 清理构建产物
.\Gcc\scripts\build_win.cmd clean

:: 先清理再完整构建
.\Gcc\scripts\build_win.cmd rebuild

:: 打印自动扫描出的源文件、头文件目录和关键变量
.\Gcc\scripts\build_win.cmd print-vars
```

也可以在动作后追加 make 参数，例如：

```cmd
.\Gcc\scripts\build_win.cmd all V=1
```

### 2. 切换目标芯片

默认情况下，只需修改 `Gcc/project.mk` 中的 `MCU`：

```makefile
MCU ?= STM32F103C8T6

# 示例：切换到高密度 512 KB Flash / 64 KB RAM
# MCU ?= STM32F103RET6

# 示例：切换到互联网络系列
# MCU ?= STM32F107RCT6
```

如果目标型号未预置在 `Gcc/mcu_profiles.mk`，可在 `Gcc/project.mk` 中直接填写：

```makefile
MCU ?= CUSTOM_STM32F10X
MCU_FAMILY ?= STM32F10X_HD
FLASH_KB ?= 256
RAM_KB ?= 48
STARTUP_GROUP ?= hd
HSE_VALUE_HZ ?= 8000000
```

也可以不修改文件，直接在构建命令后追加 make 变量临时切换目标芯片：

```bash
# Linux: 直接切到 STM32F103RET6 编译
./Gcc/scripts/build_linux.sh all MCU=STM32F103RET6

# Linux: 直接切到 STM32F107RCT6 编译
./Gcc/scripts/build_linux.sh all MCU=STM32F107RCT6

# 先查看派生出的启动文件、链接脚本和芯片组宏
./Gcc/scripts/build_linux.sh print-vars MCU=STM32F107RCT6
```

```cmd
:: Windows CMD: 切到 STM32F103RET6
.\Gcc\scripts\build_win.cmd all MCU=STM32F103RET6

:: Windows CMD: 先检查变量
.\Gcc\scripts\build_win.cmd print-vars MCU=STM32F107RCT6
```

```powershell
# Windows PowerShell: 切到 STM32F107RCT6
.\Gcc\scripts\build_win.ps1 all MCU=STM32F107RCT6
```

如果你直接使用 make，建议先进入 `Gcc/` 目录再执行：

```bash
cd Gcc
make all MCU=STM32F107RCT6
```

注意：当前仓库已经验证了多 MCU 的构建切换机制，但默认演示应用本身仍然占用较多 RAM。对于 `STM32F100C8T6B` 这类 8KB RAM 的型号，可能会在链接阶段出现 RAM overflow，需要进一步裁剪任务、队列、日志或堆配置后才能落到实板。

### 3. 清理工程

**Windows:**
```powershell
.\Gcc\scripts\build_win.ps1 clean
```
```cmd
.\Gcc\scripts\build_win.cmd clean
```
**Linux:**
```bash
./Gcc/scripts/build_linux.sh clean
```

## 目录结构速览

```text
STM32_RTOS/
├── BSP/            # 板级支持与系统级启动配置
├── Driver/         # 硬件与外部驱动层
│   ├── Device/     # 外部设备、传感器驱动 (如 OLED, DHT11)
│   ├── Peripheral/ # MCU 片内外设驱动 (如 GPIO, USART)
│   ├── Resources/  # 静态资源 (LCD图片点阵、UI资源等)
│   └── Support/    # 通用协议与数据处理支持包
├── FreeRTOS/       # 操作系统封装及应用
│   ├── App/        # 应用层：包含业务任务与 app_config.h
│   └── Source/     # FreeRTOS 官方无修改源码
├── Gcc/            # 跨平台构建系统库与链接文件
├── Libraries/      # ST 标准固件库 (SPL)
└── Documents/      # 项目文档与教程
```

## 详细文档

更多详细的项目结构解释、如何开发新的外设驱动、添加新的 RTOS 任务，请参阅：
👉 **[开发模板使用教程](Documents/STM32_RTOS_开发模板使用教程.md)**

芯片组选型、启动文件和链接脚本支持说明请参阅：
👉 **[STM32F10x 支持芯片型号](Documents/STM32F10x_支持芯片型号.md)**
