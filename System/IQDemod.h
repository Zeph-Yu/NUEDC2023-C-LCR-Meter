#ifndef __IQ_DEMOD_H
#define __IQ_DEMOD_H

#include <stdint.h>

// ===== 系统参数 =====
// f_s   = 4 MHz     (ADC 采样率)
// f_exc = 100 kHz   (激励信号频率)
// N     = f_s / f_exc = 40 (每激励周期采样点数)

#define IQ_N           40      // sin/cos 查找表长度
#define IQ_SKIP        40      // 舍弃第一个周期 (ADC启动瞬态)
#define IQ_SAMPLES     4000    // 有效采样点数 = 100个完整激励周期 (参与解调)
#define IQ_N_CYCLES    100     // 激励周期数 (4000点 / 40点每周期)
#define IQ_VOFA        400     // VOFA+显示前10个周期
#define ADC_BITS       12      // ADC 分辨率
#define ADC_MAX        4096    // 12-bit ADC 满量程 (2^12)
#define ADC_VREF       3.3     // ADC 参考电压 (V)
#define ADC_DC_OFFSET  1857    // ADC 直流偏置编码值 (对应约 1.5V)

// 直流偏置对应的真实电压 (V)  = 1857 / 4096 × 3.3 ≈ 1.5
#define VDC_OFFSET      ((double)ADC_DC_OFFSET * ADC_VREF / ADC_MAX)

// 分压电阻参数 (单位: Ω)
#define R_REF          51.0    // 分压电阻 R_ref = 51 Ω
#define R_S            0.0     // 扩展量程电阻 Rs (已通过标定修正)

// 短路标定参数 (单位: Ω)
// 短路 DUT 端，记录测得的 Zx 实部/虚部，作为系统残差
#define Z_RE_OFFSET    51.0    // 实部残差 = Rs
#define Z_IM_OFFSET    0.0     // 虚部残差 (系统相位偏移)

// ===== IQ 解调结果 =====
// IQ 解调将采样信号分别投影到 cos(100kHz·t) 和 sin(100kHz·t) 方向，
// 结果归一化后直接代表该频率分量的电压幅值 (V)
typedef struct
{
    double I;           // 同相分量幅值 (V) — 对应实部 a 或 c
    double Q;           // 正交分量幅值 (V) — 对应虚部 b 或 d
} IQResult;

// ===== 函数声明 =====

// 对采样数据执行 IQ 解调
// data:  ADC 采样原始值数组 (12-bit, 0~4095)
// count: 参与计算的采样点数 (应为 IQ_SAMPLES)
// 返回:  IQ 解调结果 (I=同相幅值 V, Q=正交幅值 V)
//
// 处理流程:
//   1. ADC 编码 → 真实电压: V[i] = data[i] × Vref / 4096
//   2. 去除 DC 偏置: V_ac[i] = V[i] - VDC_OFFSET
//   3. 乘参考信号累加: I = Σ V_ac[i] × cos(2π×i/N)
//                      Q = Σ V_ac[i] × sin(2π×i/N)
//   4. 归一化: I /= N_cycles × N/2   → 直接得到信号幅值 (V)
IQResult IQ_Demodulate(const uint16_t data[], int count);

// ===== 阻抗计算结果 =====
typedef struct
{
    double real;    // Re(Zx)，单位 Ω（例如 500.0 表示 500 Ω）
    double imag;    // Im(Zx)，单位 Ω（正值=感性，负值=容性）
} ZResult;

// 根据 IQ 解调得到的 a,b,c,d 计算待测元件复阻抗
//
// 参数:
//   a, b: Vx 的 IQ 解调结果（待测支路电压的实部和虚部，单位 V）
//   c, d: VR 的 IQ 解调结果（参考电阻电压的实部和虚部，单位 V）
//
// 公式:
//   Z_total = R_ref × Vx / VR = R_ref × (a+bj) / (c+dj)
//   Re(Z) = R_ref × (ac + bd) / (c² + d²)
//   Im(Z) = R_ref × (ad - bc) / (c² + d²)   ← 感性为正，容性为负
//   Zx     = Z_total - Rs       （Rs 只影响实部）
//
// 返回 ZResult，实部虚部单位均为 Ω
ZResult IQ_CalcImpedance(double a, double b,
                         double c, double d);

// ===== 电容测量结果 =====
typedef struct
{
    int32_t c_pF;   // 电容值，单位 pF（例如 1000000 表示 1µF）
    int32_t d_ppm;  // 损耗角正切 D，单位 10⁻⁶（例如 1000 表示 D=0.001）
} CResult;

// 根据复阻抗计算电容值和损耗角正切 D
//
// 公式:
//   C(pF) = 10^12 / (ω × |Im|)      ω = 2π × 100kHz
//   D(ppm) = Re × 10^6 / |Im|       (Im 取绝对值)
//
// z:  IQ_CalcImpedance 的返回结果（单位 Ω）
//
// 返回 CResult：c_pF 单位 pF，d_ppm 单位 10⁻⁶
// 若 |Im(Z)| == 0，c_pF 和 d_ppm 均返回 0
CResult IQ_CalcCapacitance(const ZResult *z);

// ===== 电感测量结果 =====
typedef struct
{
    int32_t l_nH;   // 电感值，单位 nH（例如 1000000 表示 1mH）
    int32_t q_ppm;  // 品质因数 Q × 10⁶（例如 50000 表示 Q=0.05）
} LResult;

// 根据复阻抗计算电感值和品质因数 Q
//
// 公式:
//   L(H)  = Im / ω                  ω = 2π × 100kHz
//   L(nH) = L(H) × 10^9 = L_CONSTANT × Im
//   Q     = Im / Re                （无量纲）
//   Q_ppm = Q × 10^6
//
// z:  IQ_CalcImpedance 的返回结果（单位 Ω）
//
// 返回 LResult：l_nH 单位 nH，q_ppm 单位 10⁻⁶
// 若 Im(Z) ≤ 0（非感性），l_nH 和 q_ppm 均返回 0
LResult IQ_CalcInductance(const ZResult *z);

#endif // __IQ_DEMOD_H
