#ifndef __IQ_DEMOD_H
#define __IQ_DEMOD_H

#include <stdint.h>

// ===== 参数定义 =====
// f_s   = 4 MHz     (ADC 采样率)
// f_exc = 100 kHz   (激励信号频率)
// N     = f_s / f_exc = 40 (每激励周期采样点数)

#define IQ_N        40   // sin/cos 查找表长度
#define IQ_SAMPLES  120  // 总采样点数 = 3个完整激励周期
#define Rs          1000000 //扩展量程使用电阻为1kΩ，这里使用毫欧作为单位，精度更高
#define R_ref       1500000 //分压电阻为1.5kΩ，这里使用毫欧作为单位，精度更高

// ===== IQ 解调结果 =====
// I = Σ data[i] × cos_table[i%N]  → 信号在 cos(100k·t) 方向的投影（实部 a/c）
// Q = Σ data[i] × sin_table[i%N]  → 信号在 sin(100k·t) 方向的投影（虚部 b/d）
typedef struct
{
    int64_t I;           // 同相分量（对应实部）
    int64_t Q;           // 正交分量（对应虚部）
} IQResult;

// ===== 函数声明 =====

// 对采样数据执行 IQ 解调（乘法 + 累加）
// data:  ADC 采样原始值数组 (12-bit, 0~4095)
// count: 参与计算的采样点数 (应为 IQ_SAMPLES)
// 返回:  IQ 解调结果 (I=实部, Q=虚部)
IQResult IQ_Demodulate(const uint16_t data[], int count);

// ===== 阻抗计算结果 =====
typedef struct
{
    int32_t real;   // Re(Zx)，单位 mΩ（例如 5000 表示 5.000 Ω）
    int32_t imag;   // Im(Zx)，单位 mΩ（正值=感性，负值=容性）
} ZResult;

// 根据 IQ 解调得到的 a,b,c,d 计算待测元件复阻抗
//
// 使用宏 R_ref 和 Rs（定义在本文件顶部），单位均为 mΩ
//
// 公式：
//   Z_total = R_ref × Vx / VR = R_ref × (a+bj) / (c+dj)
//   Re(Z) = R_ref × (ac + bd) / (c² + d²)
//   Im(Z) = R_ref × (bc - ad) / (c² + d²)
//   Zx     = Z_total - Rs       （Rs 只影响实部）
//
// a, b:  Vx 的 IQ 解调结果（待测支路电压的实部和虚部）
// c, d:  VR 的 IQ 解调结果（参考电阻电压的实部和虚部）
//
// 返回 ZResult，实部虚部单位均为 mΩ
ZResult IQ_CalcImpedance(int64_t a, int64_t b,
                         int64_t c, int64_t d);

// ===== 电容测量结果 =====
typedef struct
{
    int32_t c_pF;   // 电容值，单位 pF（例如 1000000 表示 1µF）
    int32_t d_ppm;  // 损耗角正切 D，单位 10⁻⁶（例如 1000 表示 D=0.001）
} CResult;

// 根据复阻抗计算电容值和损耗角正切 D
//
// 公式：
//   C = -1 / (ω × Im(Z))     ω = 2π × 100kHz
//   D = Re(Z) / |Im(Z)|
//
// z:  IQ_CalcImpedance 的返回结果（Im 必须为负，即容性）
//
// 返回 CResult：c_pF 单位 pF，d_ppm 单位 10⁻⁶
// 若 Im(Z) >= 0（感性或纯阻），c_pF 和 d_ppm 均返回 0
CResult IQ_CalcCapacitance(const ZResult *z);

#endif // __IQ_DEMOD_H
