# STM32_RTOS 开发模板使用教程

## 1. 项目简介

本项目是一个面向 STM32F103C8T6 的 FreeRTOS 开发模板，目标是让你可以在一个已经可运行、可自检、可扩展的工程基础上快速开展业务开发。

模板当前具备以下特性：

- MCU 平台：STM32F103C8T6（Cortex-M3，72MHz，64KB Flash，20KB RAM）
- 系统内核：FreeRTOS（启用任务、队列、互斥量、信号量、软件定时器）
- 工程组织：按 BSP、驱动、应用、RTOS 内核分层
- 构建系统：GCC + Makefile + Windows/Linux 构建脚本
- 日志能力：统一输出到 Gcc/build/build.log，包含文件清单和编译过程信息
- 启动自检：上电后可执行多外设自检并输出结果

适用场景：

- 作为新项目起点，快速接入实际业务逻辑
- 作为学习模板，理解 STM32 + FreeRTOS 的标准工程组织方式
- 作为团队模板，复用目录规范和构建流程

---

## 2. 项目结构说明

工程顶层关键目录职责如下：

- BSP/
  - 芯片底层支持与系统启动
  - 包括启动文件、系统时钟、中断处理、延时与系统初始化
- Driver/
  - 驱动层总目录，进一步细分为以下四个子目录：
  - Driver/Peripheral/
    - MCU 片内外设驱动封装
    - 对 GPIO、USART、ADC、RTC、TIM、DMA、I2C、SPI 等提供统一的 DRV_ 前缀接口
  - Driver/Device/
    - 外部设备与传感器驱动
    - 例如 OLED 模块、温湿度传感器、加速度计、EEPROM 芯片等
    - 建议以设备型号或类别命名（如 drv_oled.c、drv_dht11.c），在此目录中统一管理
  - Driver/Resources/
    - 非可执行资源文件，例如 LCD 字符点阵数据、图标数据、UI 背景图等
    - 通常以 C 数组或 const 结构体形式存放，供 Device 层驱动直接引用
    - 注意：资源文件体积较大时需关注 Flash 占用量
  - Driver/Support/
    - 驱动层通用工具与公共支撑代码
    - 例如缓冲区管理、通信协议帧解析、CRC 校验等
    - 不依赖具体硬件，可被 Peripheral 层和 Device 层复用
- FreeRTOS/
  - RTOS 内核源码 + 应用模板
  - FreeRTOS/App 下放置业务任务与应用初始化逻辑
- Libraries/
  - ST 标准外设库源码和头文件
- Gcc/
  - 构建系统目录，包含以下关键内容：
    - Makefile：自动加载清单、跨平台构建规则
    - project.mk：用户可编辑的构建参数（目标名、链接脚本、编译选项）
    - STM32F103C8Tx_FLASH.ld：链接脚本，定义 Flash/RAM 分区
    - scripts/build_win.ps1：Windows 构建入口脚本
    - scripts/build_linux.sh：Linux 构建入口脚本
    - build/：构建产物输出目录（elf / hex / bin / map / build.log）
  - 编译工具链详情见第 4.4 节
- Documents/
  - 项目文档目录（当前教程所在位置）

建议理解顺序：

1. 先看 FreeRTOS/App/main_rtos.c（启动流程）
2. 再看 FreeRTOS/App/app.c 与 tasks/（任务与同步机制）
3. 然后看 Driver/Peripheral/（硬件接口封装）
4. 最后看 Gcc/Makefile 与 scripts/（构建机制）

---

## 3. 启动流程（模板运行机制）

应用入口会完成以下阶段：

1. 基础初始化
   - 时钟、中断分组、基础硬件初始化
2. 调试串口初始化
   - 默认使用 USART2（PA2/PA3）输出启动与运行日志
3. 外设与 RTOS 资源初始化
   - 创建互斥量、队列、信号量等对象
   - 创建 Task1 / Task2 / Task3
4. （可选）自检流程
   - 对 RCC/GPIO/USART/TIM/ADC/DMA/EXTI/RTC 等执行基础连通性验证
5. 启动调度器
   - 进入 FreeRTOS 调度，系统进入多任务运行

运行中典型日志包含：

- 自检结果：PASS / FAIL / SKIP
- 队列、互斥量、信号量联动状态
- 心跳信息（tick、任务数量、队列深度、可用堆、最小剩余堆）

---

## 4. 构建系统说明

### 4.1 构建入口

Windows：

    powershell -ExecutionPolicy Bypass -File .\Gcc\scripts\build_win.ps1 all

Linux：

    bash ./Gcc/scripts/build_linux.sh all

常用动作：

- all：完整构建（elf + hex + bin + size）
- clean：清理构建产物
- rebuild：先 clean 再 all
- print-vars：打印构建变量和自动扫描结果

### 4.2 自动文件发现机制

脚本会扫描工程内的 .c/.s/.S/.h 文件，生成自动清单：

- Gcc/build/auto_sources.mk

Makefile 从该清单中读取：

- C 源文件列表
- 汇编文件列表
- 头文件目录列表

这样可以减少手工维护文件列表的工作量。

### 4.3 编译工具链说明

本模板使用 GNU Arm Embedded Toolchain（arm-none-eabi-gcc）编译器。

#### 工具链版本

推荐使用 **GNU Arm Embedded Toolchain 10.3.1（2021.10）** 及以上版本。
不同大版本（9 / 10 / 12 / 13）在语法兼容性上差异很小，但建议团队统一版本以保证构建产物一致。

#### Windows 工具链安装位置要求

构建脚本通过固定的相对路径定位工具链目录，**不依赖系统 PATH**，规则如下：

    <工程父目录>\GNU Arm Embedded Toolchain\bin\arm-none-eabi-gcc.exe

也接受版本子目录格式：

    <工程父目录>\GNU Arm Embedded Toolchain\10 2021.10\bin\arm-none-eabi-gcc.exe

以本模板为例，工程目录为：

    D:\project\CP001\STM32_RTOS\

则工具链应放置在：

    D:\project\CP001\GNU Arm Embedded Toolchain\bin\arm-none-eabi-gcc.exe

> 注意：工具链目录名称必须为 `GNU Arm Embedded Toolchain`（大小写一致）。
> 路径中有空格是正常的，脚本已处理引号转义。

#### Linux 工具链依赖

Linux 构建脚本直接调用 `arm-none-eabi-gcc`，要求工具链已加入系统 PATH。
安装方式（以 Ubuntu/Debian 为例）：

    sudo apt install gcc-arm-none-eabi

或从 ARM 官网下载解压后手动追加 PATH：

    export PATH=$PATH:/opt/gcc-arm-none-eabi-10.3.1/bin

#### make 工具依赖（Windows）

构建脚本会依次查找 `mingw32-make.exe` → `make.exe` → `gmake.exe`。
推荐安装 **MSYS2**，然后将 `C:\msys64\mingw64\bin` 加入系统 PATH。

    winget install msys2.msys2   # 安装 MSYS2
    # 然后在 MSYS2 终端执行：
    pacman -S mingw-w64-x86_64-make

#### 关键编译选项说明

| 选项 | 说明 |
|---|---|
| `-mcpu=cortex-m3 -mthumb` | 指定 Cortex-M3 架构，Thumb 指令集 |
| `-DSTM32F10X_MD` | 中密度型号宏，影响片上外设寄存器映射 |
| `-DUSE_STDPERIPH_DRIVER` | 启用 ST 标准外设库 |
| `-ffunction-sections -fdata-sections -Wl,--gc-sections` | 链接时裁剪未使用代码，减小固件体积 |
| `-Og -g3` | 调试优化级别，保留调试信息 |
| `--specs=nano.specs --specs=nosys.specs` | 使用 nano libc，禁用半主机（semihosting）模式 |

> 重要：本模板**不启用 semihosting**，printf/串口日志通过 USART2 输出。
> 若切换为其他 specs（如 rdimon.specs），需确保硬件调试器已连接并支持半主机。

---

### 4.4 编译日志

统一日志文件：

- Gcc/build/build.log

日志内容包含：

- 本次参与编译的文件总数（C 与汇编分别统计）
- 按模块分组后的文件明细（BSP、Driver、FreeRTOS kernel、FreeRTOS app、Libraries）
- 头文件搜索目录数量与明细
- make print-vars 的核心变量
- 链接、hex/bin 转换、size 结果

---

## 5. 如何使用这个开发模板

下面给出一个推荐的二次开发流程。

### 第一步：确认模板能在本机稳定构建

1. 执行 all 构建
2. 确认生成以下产物
   - Gcc/build/STM32_RTOS.elf
   - Gcc/build/STM32_RTOS.hex
   - Gcc/build/STM32_RTOS.bin
3. 检查 build.log 是否显示成功结束

### 第二步：下载并验证基线运行

1. 将 hex 或 bin 下载到板卡
2. 打开串口日志
3. 观察是否出现：
   - 启动日志
   - 自检统计
   - Task1/Task2/Task3 日志
   - 周期心跳日志

只有基线可运行，后续改动才容易定位问题。

### 第三步：从模板扩展业务任务

在 FreeRTOS/App/tasks 下新增任务文件（例如 task_comm.c、task_control.c），并按以下原则组织：

- 任务输入输出统一通过队列/消息结构体
- 共享资源访问前先拿互斥量
- 需要事件触发时使用信号量
- 周期任务优先用 vTaskDelayUntil 保持节拍稳定

建议在 app_config.h 中集中管理：

- 是否启用任务
- 栈大小
- 优先级
- 周期参数

### 第四步：接入新外设驱动

推荐流程：

1. 在 Driver/Peripheral 新增驱动封装文件
2. 保持接口风格与现有 DRV_ 前缀一致
3. 在 main_rtos.c 自检阶段加入最小验证（读写一次或状态检查）
4. 在业务任务中调用驱动并输出关键状态

### 第五步：资源与实时性调优

重点关注以下指标：

- 任务栈深度是否充足（避免栈溢出）
- xPortGetFreeHeapSize 与 xPortGetMinimumEverFreeHeapSize
- 队列长度是否满足峰值负载
- 中断优先级分组是否满足 FreeRTOS 约束（当前模板使用 NVIC_PriorityGroup_4）

---

## 6. 推荐开发规范

- 配置集中化：所有可调参数尽量集中到 app_config.h
- 日志统一化：关键路径必须有可搜索日志关键字
- 变更小步提交：每次只改一个功能点，便于定位问题
- 先自检再业务：新驱动先通过最小自检，再进入业务联调
- 保护实时性：避免在高优先级任务中做长阻塞或重格式化输出

---

## 7. 常见问题排查

### 问题 1：调度器启动前卡住

检查项：

- 中断优先级分组是否正确
- FreeRTOSConfig 的中断优先级配置是否匹配 Cortex-M3
- 是否在启动阶段调用了会长期阻塞的函数

### 问题 2：任务偶发异常或日志中断

检查项：

- 任务栈是否过小
- 局部大数组是否应该改为 static
- 是否存在未保护的共享资源访问

### 问题 3：RTC 或 EXTI 自检失败

检查项：

- 对应中断服务函数是否已实现
- 外设初始化顺序是否正确
- 等待外设状态的逻辑是否有超时保护

### 问题 4：构建日志中源文件数量异常

检查项：

- 新增文件后是否位于允许扫描路径
- 文件后缀是否为 .c/.s/.S/.h
- 是否被过滤规则排除

---

## 8. 模板迁移到新项目的建议步骤

1. 复制本工程为新目录
2. 修改 TARGET 与链接脚本（如芯片型号变化）
3. 保留 BSP/Driver/FreeRTOS 基础层
4. 清空或替换 FreeRTOS/App/tasks 中业务任务
5. 先通过最小功能（串口 + 心跳）
6. 再逐步加入传感器、通信、控制等业务模块

这样可以最大化复用模板稳定性，同时降低迁移风险。

---

## 9. 结语

这个模板的核心价值是：

- 结构清晰
- 构建自动化
- 自检与日志完备
- 便于快速扩展业务

你可以把它当作一个“可运行的最小 RTOS 产品骨架”，在此基础上按模块迭代，逐步演进为你的正式项目。
