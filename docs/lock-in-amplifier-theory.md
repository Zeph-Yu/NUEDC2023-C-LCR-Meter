# 数字锁相放大器原理 — IQ 正交解调的数学基础

> 本文档解释 LCR 测量仪核心算法——IQ 正交解调（数字锁相放大）的数学原理。
> 核心思路：用与激励同频的两路正交参考信号做**同步检波**（乘法），
> 再通过**积分平均**消除高频分量，从直流结果中反解出信号的幅值和相位。

---

## 〇、前置概念：什么是锁相放大器？

假设你需要测量一个 $100\,\mathrm{kHz}$ 的微弱信号，但它的幅值只有几 mV，周围布满了各种频率的噪声。普通 ADC 直接采样根本看不到它。

锁相放大器（Lock-In Amplifier）的思路很巧妙——**只对参考频率的信号敏感，其他频率一律忽视**：

```
输入信号（被噪声淹没）     参考信号（100kHz，跟你想要的信号同频）
        │                          │
        └──────────×───────────────┘
                   │
             乘法输出 = 直流分量 + 高频分量
                   │
             低通滤波（积分平均，高频清零）
                   │
             干净的直流 → 反解幅值和相位
```

- **锁相**（Lock-In）：锁定在参考频率，只有同频信号能产生直流输出，其他频率混进来全是交流，被滤波器吃掉
- **放大器**（Amplifier）：噪声被过滤后，弱信号可以被大幅放大而不会饱和

**数字**锁相放大器（DLIA）就是把乘法、积分全在 MCU 代码里用 `for` 循环算完——你的 `IQ_Demodulate()` 函数就是一个软件 DLIA。

---

## 一、问题定义

假设待测信号为：

$$x(t) = A \cos(\omega t + \phi)$$

其中：

- $A$ — 信号幅值
- $\omega = 2\pi f$ — 角频率（本例中 $f = 100\,\mathrm{kHz}$）
- $\phi$ — 相位

**目标**：从 $x(t)$ 中提取 $A$ 和 $\phi$。

---

## 二、正交本振（Reference）

产生两路与激励同频的正交参考信号：

- **I 路（同相）**：$\cos(\omega t)$
- **Q 路（正交）**：$-\sin(\omega t)$

```
           cos(ωt)
              │
              ▼
 x(t) ────×──────── I 路

          -sin(ωt)
              │
              ▼
 x(t) ────×──────── Q 路
```

两路参考频率相同、相位差 $90^\circ$，构成一对正交基。

---

## 三、I 路乘法

$$I'(t) = x(t) \cdot \cos(\omega t)$$

代入 $x(t)$：

$$I'(t) = A \cos(\omega t + \phi) \cdot \cos(\omega t)$$

利用积化和差公式：

$$\cos A \cos B = \frac{1}{2}\big[\cos(A - B) + \cos(A + B)\big]$$

得：

$$I'(t) = \frac{A}{2}\big[\cos\phi + \cos(2\omega t + \phi)\big]$$

拆分为两项：

| 项 | 表达式 | 含义 |
|----|--------|------|
| 直流项 | $\frac{A}{2}\cos\phi$ | **有用信号**（0 Hz） |
| 二倍频项 | $\frac{A}{2}\cos(2\omega t + \phi)$ | 乘法产生的高频（$2\omega$） |

```
 I'(t)
  ↑
  │────────────────
  │    DC = (A/2)·cosφ
  │
  │  /\/\/\/\/\/\/\
  │  2ω 分量
  └──────────────────→ t
```

---

## 四、Q 路乘法

$$Q'(t) = -x(t) \cdot \sin(\omega t)$$

代入 $x(t)$：

$$Q'(t) = -A \cos(\omega t + \phi) \cdot \sin(\omega t)$$

利用积化和差公式：

$$\cos A \sin B = \frac{1}{2}\big[\sin(A + B) - \sin(A - B)\big]$$

得：

$$Q'(t) = -\frac{A}{2}\big[\sin(2\omega t + \phi) - \sin\phi\big]$$

整理：

$$Q'(t) = \frac{A}{2}\sin\phi - \frac{A}{2}\sin(2\omega t + \phi)$$

同样包含：

| 项 | 表达式 | 含义 |
|----|--------|------|
| 直流项 | $\frac{A}{2}\sin\phi$ | **有用信号**（0 Hz） |
| 二倍频项 | $-\frac{A}{2}\sin(2\omega t + \phi)$ | 乘法产生的高频（$2\omega$） |

---

## 五、低通滤波的作用

乘法后的频谱：

```
 乘法后
  ^
  |            *
  |
  *
  +────────────────→
 0 Hz          2f (200kHz)
```

直流分量是我们需要的，$2\omega$ 分量是乘法副产品。

低通滤波器（LPF）去除 $2\omega$ 分量，保留直流：

$$I = \mathrm{LPF}(I') = \frac{A}{2}\cos\phi$$

$$Q = \mathrm{LPF}(Q') = \frac{A}{2}\sin\phi$$

---

## 六、数字域实现：积分累加 = 低通滤波

在 MSPM0 等嵌入式系统中，**不需要设计 FIR/IIR 滤波器**。利用"完整周期内正弦波积分为零"的性质，直接累加即可。

设 ADC 采样得到离散序列：

$$x[n] = A \cos(\omega n T_s + \phi), \quad T_s = \frac{1}{4\,\mathrm{MHz}}$$

I 路计算：

$$I = \frac{1}{N} \sum_{n=0}^{N-1} x[n] \cdot \cos(\omega n T_s)$$

Q 路计算：

$$Q = -\frac{1}{N} \sum_{n=0}^{N-1} x[n] \cdot \sin(\omega n T_s)$$

**为什么累加能消除 $2\omega$ 项？**

因为 $\cos(2\omega n T_s + \phi)$ 在完整周期内的平均值为零：

$$\frac{1}{N} \sum_{n=0}^{N-1} \cos(2\omega n T_s + \phi) = 0$$

而直流项保持不变。因此 **求平均本身就是一种简单而有效的低通滤波**。

> 在你的代码中，归一化因子为 $N_\mathrm{cycles} \times N/2$（而非 $1/N$），目的是让结果直接代表信号幅值（单位 V）。

---

## 七、IQ 解调最终结果

滤波后得到直流分量：

$$I = \frac{A}{2} \cos\phi$$

$$Q = \frac{A}{2} \sin\phi$$

反解出：

**幅值**：

$$A = 2\sqrt{I^2 + Q^2}$$

**相位**：

$$\phi = \mathrm{atan2}(Q,\; I)$$

---

## 八、应用到 LCR 测量

你的系统分别对两个 ADC 通道做 IQ 解调：

$$(I_x, Q_x)$$ —— 待测支路电压 $V_x$

$$(I_r, Q_r)$$ —— 参考电阻电压 $V_r$

每路得到复数电压：

$$V_x = I_x + j Q_x = \frac{A_x}{2}(\cos\phi_x + j\sin\phi_x)$$

$$V_r = I_r + j Q_r = \frac{A_r}{2}(\cos\phi_r + j\sin\phi_r)$$

利用复阻抗公式：

$$Z_x = R_{\mathrm{ref}} \cdot \frac{V_x}{V_r} = R_{\mathrm{ref}} \cdot \frac{I_x + j Q_x}{I_r + j Q_r}$$

展开得实部和虚部：

$$\mathrm{Re}(Z_x) = R_{\mathrm{ref}} \cdot \frac{I_x I_r + Q_x Q_r}{I_r^2 + Q_r^2}$$

$$\mathrm{Im}(Z_x) = R_{\mathrm{ref}} \cdot \frac{Q_x I_r - I_x Q_r}{I_r^2 + Q_r^2}$$

> 注：实际代码中采用 $ad - bc$ 形式（$a=I_x,\; b=Q_x,\; c=I_r,\; d=Q_r$），
> 感性 Im 为正，容性 Im 为负。

由复阻抗 $Z_x$ 可计算：

**电容**（容性，$\mathrm{Im} \lt 0$）：

$$C = \frac{1}{\omega \cdot |\mathrm{Im}(Z_x)|}$$

**电感**（感性，$\mathrm{Im} \gt 0$）：

$$L = \frac{|\mathrm{Im}(Z_x)|}{\omega}$$

**损耗角正切**：

$$D = \frac{\mathrm{Re}(Z_x)}{|\mathrm{Im}(Z_x)|}$$

**品质因数**：

$$Q = \frac{|\mathrm{Im}(Z_x)|}{\mathrm{Re}(Z_x)}$$

---

## 九、总结

```
ADC 采样
  │
  ├── × cos(ωt) ──→ 累加平均 ──→ I ─┐
  │                                    ├──→ A, φ ──→ Z
  └── × (-sin(ωt)) ──→ 累加平均 ──→ Q ─┘
```

IQ 正交解调的本质就是：

1. 用两路正交参考信号做**同步检波**（乘法）
2. 用**积分累加**实现低通滤波（消除 $2\omega$ 分量）
3. 从直流分量反解幅值和相位
4. 利用 Vx 和 Vr 的复比值计算待测阻抗

整个过程不依赖复杂的模拟滤波器，完全由 ADC 采样 + 数字计算完成——这就是数字锁相放大器的核心优势。
