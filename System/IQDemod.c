#include "IQDemod.h"

// sin/cos 查找表: 2πk/40, k=0..39, 归一化到 [-1.0, 1.0]
// 详细生成方法见 docs/reference-table-generation.md

const double IQ_cos_table[IQ_N] = {
     1.000000,  0.987688,  0.951057,  0.891007,  0.809017,
     0.707107,  0.587785,  0.453990,  0.309017,  0.156434,
     0.000000, -0.156434, -0.309017, -0.453990, -0.587785,
    -0.707107, -0.809017, -0.891007, -0.951057, -0.987688,
    -1.000000, -0.987688, -0.951057, -0.891007, -0.809017,
    -0.707107, -0.587785, -0.453990, -0.309017, -0.156434,
     0.000000,  0.156434,  0.309017,  0.453990,  0.587785,
     0.707107,  0.809017,  0.891007,  0.951057,  0.987688,
};

const double IQ_sin_table[IQ_N] = {
     0.000000,  0.156434,  0.309017,  0.453990,  0.587785,
     0.707107,  0.809017,  0.891007,  0.951057,  0.987688,
     1.000000,  0.987688,  0.951057,  0.891007,  0.809017,
     0.707107,  0.587785,  0.453990,  0.309017,  0.156434,
     0.000000, -0.156434, -0.309017, -0.453990, -0.587785,
    -0.707107, -0.809017, -0.891007, -0.951057, -0.987688,
    -1.000000, -0.987688, -0.951057, -0.891007, -0.809017,
    -0.707107, -0.587785, -0.453990, -0.309017, -0.156434,
};

// 归一化因子 = N_cycles × N/2 (同频积分 Σcos² = Σsin² = N_cycles × N/2)
#define IQ_NORM_FACTOR  ((double)(IQ_N_CYCLES) * IQ_N / 2.0)

// ===================================================================
// IQ 解调: ADC 编码值 → 100kHz 分量的 I/Q 幅值 (V)
//
// 原理: 同步检波 + 积分平均 (数字锁相放大器)
//   I = (2/N_total) × Σ v[n]·cos(2πn/N)
//   Q = (2/N_total) × Σ v[n]·sin(2πn/N)
// DC 偏置在完整周期积分中自动消除
// ===================================================================
IQResult IQ_Demodulate(const uint16_t data[], int count)
{
    IQResult result;
    double I_acc = 0.0, Q_acc = 0.0;
    const double adc_to_volt = ADC_VREF / (double)ADC_MAX;

    for (int i = 0; i < count; i++) {
        double v_ac = (double)data[i] * adc_to_volt - VDC_OFFSET;
        int k = i % IQ_N;
        I_acc += v_ac * IQ_cos_table[k];
        Q_acc += v_ac * IQ_sin_table[k];
    }

    result.I = I_acc / IQ_NORM_FACTOR;
    result.Q = Q_acc / IQ_NORM_FACTOR;
    return result;
}

// ===================================================================
// 复阻抗计算
//   Zx = R_ref × (Ix + jQx) / (Ir + jQr) - Rs
//   Re = R_ref × (Ix·Ir + Qx·Qr) / (Ir² + Qr²) - Rs
//   Im = R_ref × (Qx·Ir - Ix·Qr) / (Ir² + Qr²)   ← 感性为正, 容性为负
//   详细推导见 docs/lock-in-amplifier-theory.md
// ===================================================================
ZResult IQ_CalcImpedance(double Ix, double Qx, double Ir, double Qr)
{
    double den = Ir * Ir + Qr * Qr;
    ZResult result;

    if (den == 0.0) {
        result.real = 0.0;
        result.imag = 0.0;
        return result;
    }

    double re_total = R_REF * (Ix * Ir + Qx * Qr) / den;
    double im_total = R_REF * (Qx * Ir - Ix * Qr) / den;

    result.real = re_total - R_S;   // 去掉串联电阻
    result.imag = im_total;         // Rs 不影响虚部
    return result;
}

// ===================================================================
// 电容计算: C(pF) = 10¹² / (ω·|Im|), D = Re/|Im|
// ===================================================================
#define CAP_CONSTANT_OHM  1591549.43   // 10¹² / (2π·100kHz)

CResult IQ_CalcCapacitance(const ZResult *z)
{
    CResult cr;
    double abs_imag = (z->imag >= 0.0) ? z->imag : -z->imag;

    if (abs_imag == 0.0) {
        cr.c_pF = 0;
        cr.d_ppm = 0;
        return cr;
    }

    cr.c_pF = (int32_t)(CAP_CONSTANT_OHM / abs_imag);

    double abs_real = (z->real >= 0.0) ? z->real : -z->real;
    cr.d_ppm = (int32_t)(abs_real * 1000000.0 / abs_imag);
    return cr;
}

// ===================================================================
// 电感计算: L(nH) = 10⁹·Im / ω, Q = Im/Re
// Im ≤ 0 时返回 0 (非感性元件)
// ===================================================================
#define L_CONSTANT_NH   1591.549     // 10⁹ / (2π·100kHz)

LResult IQ_CalcInductance(const ZResult *z)
{
    LResult lr;

    if (z->imag <= 0.0) {
        lr.l_nH  = 0;
        lr.q_ppm = 0;
        return lr;
    }

    lr.l_nH = (int32_t)(L_CONSTANT_NH * z->imag);

    if (z->real != 0.0)
        lr.q_ppm = (int32_t)(z->imag * 1000000.0 / z->real);
    else
        lr.q_ppm = 0;

    return lr;
}
