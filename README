# HXC-PowerControl
HXC电源管理板CMake工程


## 使用指南

本节按“上电后如何用”来说明，重点包含 Log 调试、保护策略和按键逻辑。

### 1. 上电后的基本流程

1. 系统初始化外设后，会初始化电源控制、INA3221 采样、按键、LED、蜂鸣器、CAN 发送周期和日志串口。
2. 默认会先打开 `OUT_1` 和 `OUT_2`（用于快速联调），`OUT_3` 保持关闭。
3. 主循环每 1ms 执行一次：
	- 处理按键状态机与输出切换。
	- 轮询 INA3221 读取三路电压/电流。
	- 进行滤波与保护判断。
	- 按需打印串口 Log。

### 2. Log 功能使用

Log 通过 `USART2` 串口控制，适合调试电压电流与通道状态。

1. 打开串口工具（如 VS Code Serial Monitor），连接到板子的 `USART2`。
	- 串口参数：`115200, 8N1, 无流控`。
2. 向串口发送 `on`（大小写均可），开启连续日志输出。
3. 向串口发送 `off`（大小写均可），关闭日志输出。

输出格式为一行 6 个数据：

```text
V1,V2,V3,I1,I2,I3
```

示例（单位：V 和 A，保留 3 位小数）：

```text
24.315,24.287,0.000,12.420,11.905,0.000
```

说明：

1. 已关闭通道会被清零显示（通常为 `0.000`）。
2. Log 和串口接收使用互斥状态，避免“正在解析命令”时输出干扰。
3. 建议联调时先发送 `on`，观察稳定后再发送 `off`，降低串口占用。

### 3. 保护策略简介

系统基于 INA3221 的实时采样做保护，触发后会联动蜂鸣器和对应通道状态。

#### 3.1 电压保护

1. 过压保护：电压 `> 26.0V` 且持续约 `20s`，关闭对应输出通道并报警。
2. 低压预警：电压 `< 23.0V` 且 `> 5.0V`，触发蜂鸣器预警（不立即断电）。
3. 电压突降急停：相对历史最高值单次下降 `>= 15.0V`，立即关闭对应通道。

#### 3.2 电流保护

1. 过流保护：电流在 `31.0A ~ 50.0A` 区间且持续约 `20s`，关闭对应通道。
2. 突变电流保护：电流 `>= 50.0A` 持续约 `0.3s`，关闭对应通道。

#### 3.3 滤波策略

1. 采样使用 `FILTER_BUFFER_SIZE=11` 的去极值平均思路。
2. 每路数据先排序，再去掉最大值和最小值后取平均。
3. 滤波缓冲区未装满前只累计数据，不执行正式滤波与保护动作。

### 4. 按键功能简介（三路一致）

三个按键分别对应三路输出控制，流程一致：

1. 第一次短按松开后，进入“等待二次长按”窗口（最长 `5s`）。
2. 在 5s 窗口内再次按住，进入长按计时。
3. 长按超过约 `1.2s` 后，触发该路输出开/关切换。
4. 若等待超时或提前松手，会退出本次操作并恢复空闲状态。

LED 指示：

1. 进入等待窗口时，RUN 灯慢闪。
2. 长按计时阶段，RUN 灯进入流水效果。
3. 保护触发导致断电时，对应 ERR 灯点亮。

## 前置环境

本工程建议以 VS Code 扩展作为主要开发环境，先安装以下扩展：

必须：
1. STM32CubeIDE for Visual Studio Code（`stmicroelectronics.stm32-vscode-extension`）
	- 用途：配置/编译 CMake 工程，直接使用 Presets 构建。
	- 配置：使用时会提示安装工具链，务必全部安装，不清楚的话跟着下面的视频来

可选扩展：

1. Arm Assembly（`dan-c-underwood.arm`）
	- 用途：汇编文件语法高亮。
2. Serial Monitor（`ms-vscode.vscode-serial-monitor`）
	- 用途：串口日志查看。
### 本地环境路径说明（必须修改，否则代码提示会报错）

`.clangd` 里包含本机安装路径，**请改成你自己的用户名和安装目录**。  
例如下面路径中的 `Cindy` 需要替换为你的 Windows 用户目录：

- `C:/Users/Cindy/AppData/Local/stm32cube/bundles/st-arm-clang/21.1.1+st.7/lib/clang-runtimes/newlib/arm-none-eabi/include/c++/v1`
- `C:/Users/Cindy/AppData/Local/stm32cube/bundles/st-arm-clang/21.1.1+st.7/lib/clang-runtimes/newlib/arm-none-eabi/include`

可先在 `.clangd` 中全局搜索：
- `C:/Users/Cindy/AppData/Local/stm32cube/`

编译有问题就修改根目录下的`CMakeLists.txt`

然后替换为你自己的实际路径。

### 禁用 IntelliSense（推荐）

为避免与 clangd 冲突，建议在 VS Code 工作区设置中禁用微软 C/C++ IntelliSense 引擎。

在 `.vscode/settings.json` 中增加：

```json
{
  "C_Cpp.intelliSenseEngine": "disabled"
}
```

补充说明：

1. 如果已安装 `ms-vscode.cpptools`（C/C++ 扩展），保留也可以，但建议关闭其 IntelliSense。
2. 代码补全、跳转、诊断建议统一由 `clangd` 提供。

## 视频教程链接

说明：以下为教程入口（搜索页），可按更新时间和播放量筛选。

1.  STM32CubeIDE for VS Code操作指南（Bilibili）
	- https://www.bilibili.com/video/BV1tvXoBgEni/?spm_id_from=333.337.search-card.all.click

## CMakeLists 路径说明（必须检查，否则编译无法通过）

当你新增/调整用户代码目录时，除了修改 `.clangd`，还需要同步修改根目录下的 `CMakeLists.txt` 中两类路径：

1. 源文件编译路径（`USER_SOURCES`）
	- 对应 `file(GLOB_RECURSE USER_SOURCES ...)`
	- 需要保证你自己的 `*.c` / `*.cpp` 目录都被覆盖，否则文件不会参与编译。

2. 头文件包含路径（`USER_INCLUDES`）
	- 对应 `file(GLOB_RECURSE USER_INCLUDES ...)`
	- 需要保证你自己的 `Inc` 目录被包含，否则会出现找不到头文件。

请重点检查目录名和大小写是否与本机实际目录一致（例如 `Middleware` / `MiddleWare`），路径不一致会导致“源码未编译”或“头文件找不到”的问题。
