# LCR 测量原理 — 自平衡半桥 + 复阻抗计算

> 本文档为项目专属原理说明，通用 IQ 解调数学基础见 [锁相放大器原理](./lock-in-amplifier-theory.md)。

---

## 1. 测量拓扑：自平衡半桥

```
信号源 (100kHz) ─── Rs ─── DUT ─── R_ref ─── GND
                    │               │
                   Vx              Vr
                (ADC1 采)       (ADC0 采)
```

- **Rs**：串联限流电阻（物理焊接在 PCB 上）
- **DUT**：待测元件（电容或电感）
- **R_ref**：参考电阻（已知精确阻值）
- **Vx**：Rs + DUT 两端电压（交流分量）
- **Vr**：R_ref 两端电压（交流分量）

同一电流 $I$ 流过整个串联回路，因此：

$$\frac{V_x}{V_r} = \frac{R_s + Z_{DUT}}{R_{ref}}$$

$$\Longrightarrow \quad Z_{DUT} = R_{ref} \cdot \frac{V_x}{V_r} - R_s$$

其中 $R_s$ 通过短路标定消除（见第 5 节）。

---

## 2. 从时域波形到复电压：IQ 解调

ADC 采到的是时域波形，但我们只关心 **100kHz 频率分量**的幅值和相位。

IQ 解调的本质是**数字锁相放大器**——用与激励同频的两路正交参考信号做同步检波：

$$I = \frac{2}{N} \sum_{n=0}^{N-1} v[n] \cdot \cos\!\left(2\pi \frac{n}{N_0}\right)$$

$$Q = \frac{2}{N} \sum_{n=0}^{N-1} v[n] \cdot \sin\!\left(2\pi \frac{n}{N_0}\right)$$

其中 $N_0 = 40$（每激励周期采样点数），$N$ 为参与解调的总采样点数。

解调结果 $I$、$Q$ 直接代表 100kHz 分量的同相和正交幅值（单位 V）：

$$V_{100kHz}(t) = I \cdot \cos(\omega t) + Q \cdot \sin(\omega t)$$

> 详细数学推导见 [锁相放大器原理文档](./lock-in-amplifier-theory.md)。

---

## 3. 复阻抗计算

对 Vx 和 Vr 分别做 IQ 解调，得到两组 $(I, Q)$：

| 通道 | I（同相） | Q（正交） | 复数表示 |
|------|-----------|-----------|----------|
| Vx（Rs + DUT） | $a$ | $b$ | $V_x = a + bj$ |
| Vr（参考电阻） | $c$ | $d$ | $V_r = c + dj$ |

代入半桥关系：

$$Z_{total} = R_{ref} \cdot \frac{V_x}{V_r} = R_{ref} \cdot \frac{a + bj}{c + dj}$$

复数除法展开：

$$\begin{aligned}
\mathrm{Re}(Z_{total}) &= R_{ref} \cdot \frac{ac + bd}{c^2 + d^2} \\[8pt]
\mathrm{Im}(Z_{total}) &= R_{ref} \cdot \frac{ad - bc}{c^2 + d^2}
\end{aligned}$$

减去串联电阻得到 DUT 阻抗：

$$\begin{aligned}
\mathrm{Re}(Z_{DUT}) &= \mathrm{Re}(Z_{total}) - R_s \\
\mathrm{Im}(Z_{DUT}) &= \mathrm{Im}(Z_{total})
\end{aligned}$$

> **符号约定**：采用 $ad - bc$ 形式，感性元件 $\mathrm{Im} \gt 0$，容性元件 $\mathrm{Im} \lt 0$。
> 这一约定取决于 IQ 解调时参考信号的相位定义，不同实现可能符号相反。

---

## 4. 从阻抗到元件值

### 4.1 电容（$\mathrm{Im} \lt 0$）

$$C = \frac{1}{\omega \cdot |\mathrm{Im}(Z_{DUT})|}
= \frac{1}{2\pi \cdot 100\text{kHz} \cdot |\mathrm{Im}|}$$

电容值常数（代码中使用）：

$$C(\text{pF}) = \frac{1591549}{|\mathrm{Im}(\Omega)|}$$

损耗角正切：

$$D = \frac{\mathrm{Re}(Z_{DUT})}{|\mathrm{Im}(Z_{DUT})|}$$

### 4.2 电感（$\mathrm{Im} \gt 0$）

$$L = \frac{\mathrm{Im}(Z_{DUT})}{\omega}
= \frac{\mathrm{Im}(Z_{DUT})}{2\pi \cdot 100\text{kHz}}$$

电感值常数（代码中使用）：

$$L(\text{nH}) = 1591.55 \times \mathrm{Im}(\Omega)$$

品质因数：

$$Q = \frac{\mathrm{Im}(Z_{DUT})}{\mathrm{Re}(Z_{DUT})}$$

---

## 5. 标定方法

系统存在固定的测量偏置（PCB 走线、夹具寄生参数、ADC 通道间相位差等），通过短路标定消除。

### 步骤

1. 在 `IQDemod.h` 中设 `Z_RE_OFFSET = 0`、`Z_IM_OFFSET = 0`
2. 编译烧录，用导线短接 DUT 端子
3. 按测量键，记录 OLED 第 4 行显示的 **Re** 和 **Im**
4. 多次测量取均值，填入 `Z_RE_OFFSET` 和 `Z_IM_OFFSET`
5. 重新编译烧录即生效

### 原理

- 短路时 $\mathrm{Re}$ 反映 $R_s$ + 引线电阻
- 短路时 $\mathrm{Im}$ 反映系统相位偏移（理想为 0）
- 这两个偏移是**固定的系统误差**，对所有测量影响一致，减法消除

---

## 6. 系统参数

| 参数 | 值 | 说明 |
|------|-----|------|
| $f_{exc}$ | 100 kHz | 激励信号频率 |
| $f_s$ | 4 MHz | ADC 采样率 |
| $N_0$ | 40 | 每周期采样点数（$f_s / f_{exc}$） |
| 采样周期数 | 100 | 参与解调的有效周期 |
| ADC 分辨率 | 12-bit | 0 ~ 4095 |
| $V_{ref}$ | 3.3V | ADC 参考电压 |
| DC 偏置 | ~1.5V | 模拟前端直流偏置（ADC_DC_OFFSET ≈ 1857） |
| $R_{ref}$ | 51Ω / 1300Ω | 电感版 / 电容版（根据量程选择） |
| $R_s$ | 51Ω / 1000Ω | 电感版 / 电容版 |
| DMA | 双通道 | ADC FIFO → 内存数组，CPU 零干预 |
| 触发 | TIMA0 零溢出 | 双 ADC 同步采样 |

---

## 7. 信号处理全链路

```
┌──────────┐    ┌──────────┐    ┌──────────────┐    ┌──────────────┐
│ ADC 采样 │───▶│ DMA 搬运 │───▶│  IQ 解调     │───▶│ 复阻抗计算    │
│ 4MHz/12b │    │ 双通道   │    │ I/Q = 同相/  │    │ Z = Rref·Vx/Vr│
│          │    │          │    │ 正交幅值(V)  │    │              │
└──────────┘    └──────────┘    └──────────────┘    └──────┬───────┘
                                                           │
                                                  ┌────────▼───────┐
                                                  │ 标定修正         │
                                                  │ Re -= Z_RE_OFFSET│
                                                  │ Im -= Z_IM_OFFSET│
                                                  └────────┬───────┘
                                                           │
                                              ┌────────────▼───────┐
                                              │ 容性 (Im<0):        │
                                              │   C = 1591549/|Im|  │
                                              │   D = Re/|Im|       │
                                              │ 感性 (Im>0):        │
                                              │   L = 1591.55×Im    │
                                              │   Q = Im/Re         │
                                              └────────────────────┘
```

---

## 8. 误差来源与局限

| 误差来源 | 影响 | 缓解方法 |
|----------|------|----------|
| ADC 量化噪声（12-bit） | 小信号时信噪比下降 | 增加采样周期数（积分平均） |
| Rs / R_ref 比值 | 影响相位分辨率 | Rs ≈ R_ref 且不宜过大（见下文） |
| ADC 通道间采样偏差 | 相位误差 → D/Q 不准 | 双 ADC 同步触发、短路标定 |
| DC 偏置漂移 | 低频分量混入 | 完整周期解调（Σcos = 0 自动消除） |
| Q/D 精度 | Re 通常极小（mΩ 级） | 受限于 ADC 分辨率，需 PGA 或过采样 |

### 关于 Rs / R_ref 选值

- Rs 过大 → $\omega L / R_s$ 很小 → 相位角极小 → Im 信噪比差
- Rs = R_ref 时，短路态 Vx = Vr，$ad - bc$ 数值脆弱，但正常工作态（DUT 接入）相位角最大
- 本项目电感版最终选用 Rs = R_ref = 51Ω，相位角从 2.2° 提升至 36.8°（60.7µH）

---

## 9. 代码架构

```
firmware/
├── LCR2.0.c                 主程序入口 + 状态机主循环
├── LCR2.0.syscfg            TI SysConfig（ADC/DMA/Timer 配置）
├── System/
│   ├── IQDemod.h/c          IQ 解调 + 复阻抗计算 + C/L/D/Q 计算
│   └── System_State.h/c     状态机 + 按键中断 + OLED 显示 + VOFA+
├── Hardware/
│   ├── ADC.h/c              DMA 初始化 + 双 ADC ISR
│   ├── oled.h/c             SH1106 OLED 驱动（软件 SPI）
│   └── oledfont.h           字库
└── targetConfigs/           MSPM0G3507 调试配置
```

---

## 10. 参考文献

- Keysight Impedance Measurement Handbook — 自平衡电桥原理
- 数字锁相放大器原理 — IQ 正交解调的数学基础
- 开源 LCR 表项目（Elektor LCR Meter, RLC-2 等）

---

> 📝 本项目为 2026 电子竞赛备赛作品  
> 🛠️ MCU: TI MSPM0G3507 | 开发环境: Code Composer Studio Theia  
> 📐 测量频率: 100 kHz | 采样率: 4 MHz | ADC: 12-bit SAR
