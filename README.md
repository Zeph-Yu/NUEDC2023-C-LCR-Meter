# LCR Meter — 数字锁相放大式 LCR 测量仪

基于 TI MSPM0G3507 的自平衡半桥 LCR 表，采用 **IQ 正交解调（数字锁相放大器）** 在 100kHz 激励下同时测量电容/电感和 D/Q 值。

## 特性

- **自动识别** — 无需切换模式，接入 DUT 自动判断容性/感性
- **数字锁相放大** — 软件 DLIA，12-bit ADC 实现 L 值 < 5% 精度
- **双 ADC 同步采样** — TIMA0 触发，DMA 搬运，CPU 零等待
- **OLED 实时显示** — 128×64 SH1106，同时显示主值和 debug 阻抗
- **VOFA+ 上位机支持** — 串口发送原始波形，方便调试

## 性能指标

| 指标 | 范围 | 精度 |
|------|------|------|
| 电容 | 1 nF ~ 100 nF | < 5% |
| 损耗角 D | 0.005 ~ 1 | — |
| 电感 | 10 µH ~ 100 µH | < 5% |
| 品质因数 Q | 1 ~ 200 | ~20% |

## 硬件

```
信号源 (100kHz) ─── Rs (51Ω) ─── DUT ─── R_ref (51Ω) ─── GND
                      │                    │
                     Vx (PA15)            Vr (PA27)
                    ADC1 采样            ADC0 采样
```

| 参数 | 值 |
|------|-----|
| MCU | MSPM0G3507 (Cortex-M0+, 80MHz, 32KB SRAM) |
| ADC | 12-bit SAR × 2, 4MHz 同步采样 |
| 激励频率 | 100 kHz |
| 参考电阻 | 51Ω (Rs = R_ref) |
| 显示 | SH1106 OLED 128×64 (软件 SPI) |
| 调试接口 | VOFA+ FireWater 协议 (UART) |

## 快速开始

1. **克隆仓库**
   ```bash
   git clone https://github.com/your-username/lcr-meter.git
   ```

2. **导入工程** — 用 Code Composer Studio Theia 打开 `LCR_Meter/` 目录

3. **短路标定**
   - 将 `IQDemod.h` 中 `Z_RE_OFFSET` 和 `Z_IM_OFFSET` 设为 0
   - 编译烧录，短接 DUT 端子，按 PB21 测量
   - 记录 OLED 第 4 行的 Re/Im 值
   - 填入 `Z_RE_OFFSET` / `Z_IM_OFFSET`，重新编译

4. **测量** — 接入电容或电感，按 PB21，自动显示结果

## 目录结构

```
LCR_Meter/
├── LCR_Meter.c              主程序入口
├── LCR_Meter.syscfg         TI SysConfig 配置
├── System/
│   ├── IQDemod.h/c          IQ 解调 + 复阻抗 + C/L 自动计算
│   └── System_State.h/c     状态机 + 显示 + VOFA+
├── Hardware/
│   ├── ADC.h/c              双 ADC DMA 驱动
│   ├── Key.h/c              按键驱动 (定时器轮询)
│   └── oled.h/c/oledfont.h  OLED 驱动
└── targetConfigs/           XDS110 调试配置

docs/
├── lock-in-amplifier-theory.md    数字锁相放大器原理
├── lcr-measurement-principle.md   LCR 测量原理
└── reference-table-generation.md  sin/cos 查找表生成
```

## 原理文档

| 文档 | 内容 |
|------|------|
| [锁相放大器原理](docs/lock-in-amplifier-theory.md) | IQ 正交解调数学推导 — 同步检波 + 积分平均 |
| [LCR 测量原理](docs/lcr-measurement-principle.md) | 自平衡半桥 + 复阻抗计算 + 标定方法 |
| [参考信号表生成](docs/reference-table-generation.md) | sin/cos 查找表的计算方法 |

## 开发环境

- **IDE**: Code Composer Studio Theia (TI)
- **SDK**: mspm0_sdk_2_09_00_01
- **SysConfig**: 1.25.0
- **编译器**: TI ARM Clang
- **调试器**: XDS110

## 许可证

MIT
