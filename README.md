# STM32F103 FreeRTOS 开发模板

一个面向 STM32F103C8T6 的基础、标准、易于扩展的 FreeRTOS 开发模板，内置自动化基础构建脚本、运行自检与系统级心跳支持。

## 核心特性

- **稳定内核**：基于 FreeRTOS，已配置好任务、队列、互斥量、信号量与软件定时器的基础演示。
- **开箱即用构建**：使用 GCC (arm-none-eabi-gcc) + Makefile，跨平台支持 Windows (PowerShell) 与 Linux (Bash)。自带源文件自动发现机制，无需手动修改大段 Makefile。
- **完备的启动自检**：上电即对 RCC、GPIO、USART、TIM、ADC、DMA、EXTI 和 RTC 等关键外设进行自动化连通性验证。
- **系统监控日志**：内置 Task3 心跳任务，默认一秒输出一次 RTOS 状态 (Tick、任务数、队列深度、空闲堆、历史最小空闲堆)。
- **规范的目录拆分**：BSP、驱动 (Peripheral/Device/Support/Resources)、应用代码 (App)、内核与外设库，天然支持业务分层。

## 环境要求

### 硬件
- 核心板：STM32F103C8T6 (或管脚与资源兼容的 STM32 芯片，可通过修改配置兼容其他型号)
- 默认调试端口：USART2 (PA2 TX / PA3 RX，波特率 115200)，用于输出自检信息和日志。

### 软件
- **编译器**：[GNU Arm Embedded Toolchain](https://developer.arm.com/downloads/-/gnu-rm) (推荐 10.3.1 / 2021.10)
  - Windows: 放在项目上级目录 `GNU Arm Embedded Toolchain\bin\arm-none-eabi-gcc.exe`
  - Linux: 需保证 `arm-none-eabi-gcc` 已加入全局 PATH
- **构建工具**：`make` (Windows 推荐使用 MSYS2 安装 `mingw32-make.exe`)

## 快速开始

### 1. 编译构建

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

### 2. 清理工程

**Windows:**
```powershell
.\Gcc\scripts\build_win.ps1 clean
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
