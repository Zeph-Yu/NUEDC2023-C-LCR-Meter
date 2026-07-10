#include "IQDemod.h"

// ===================================================================
// sin/cos 查找表 (N = 40, 对应 100kHz 激励 @ 4MHz 采样率)
//
// 表值 = sin/cos(2π × k / 40)，k = 0~39
// 归一化到 [-1.0, 1.0]，直接参与电压乘法
// ===================================================================

const double IQ_cos_table[IQ_N] = {
     1.000000,  // k= 0  cos(  0°)
     0.987688,  // k= 1  cos(  9°)
     0.951057,  // k= 2  cos( 18°)
     0.891007,  // k= 3  cos( 27°)
     0.809017,  // k= 4  cos( 36°)
     0.707107,  // k= 5  cos( 45°)
     0.587785,  // k= 6  cos( 54°)
     0.453990,  // k= 7  cos( 63°)
     0.309017,  // k= 8  cos( 72°)
     0.156434,  // k= 9  cos( 81°)
     0.000000,  // k=10  cos( 90°)
    -0.156434,  // k=11  cos( 99°)
    -0.309017,  // k=12  cos(108°)
    -0.453990,  // k=13  cos(117°)
    -0.587785,  // k=14  cos(126°)
    -0.707107,  // k=15  cos(135°)
    -0.809017,  // k=16  cos(144°)
    -0.891007,  // k=17  cos(153°)
    -0.951057,  // k=18  cos(162°)
    -0.987688,  // k=19  cos(171°)
    -1.000000,  // k=20  cos(180°)
    -0.987688,  // k=21  cos(189°)
    -0.951057,  // k=22  cos(198°)
    -0.891007,  // k=23  cos(207°)
    -0.809017,  // k=24  cos(216°)
    -0.707107,  // k=25  cos(225°)
    -0.587785,  // k=26  cos(234°)
    -0.453990,  // k=27  cos(243°)
    -0.309017,  // k=28  cos(252°)
    -0.156434,  // k=29  cos(261°)
     0.000000,  // k=30  cos(270°)
     0.156434,  // k=31  cos(279°)
     0.309017,  // k=32  cos(288°)
     0.453990,  // k=33  cos(297°)
     0.587785,  // k=34  cos(306°)
     0.707107,  // k=35  cos(315°)
     0.809017,  // k=36  cos(324°)
     0.891007,  // k=37  cos(333°)
     0.951057,  // k=38  cos(342°)
     0.987688,  // k=39  cos(351°)
};

const double IQ_sin_table[IQ_N] = {
     0.000000,  // k= 0  sin(  0°)
     0.156434,  // k= 1  sin(  9°)
     0.309017,  // k= 2  sin( 18°)
     0.453990,  // k= 3  sin( 27°)
     0.587785,  // k= 4  sin( 36°)
     0.707107,  // k= 5  sin( 45°)
     0.809017,  // k= 6  sin( 54°)
     0.891007,  // k= 7  sin( 63°)
     0.951057,  // k= 8  sin( 72°)
     0.987688,  // k= 9  sin( 81°)
     1.000000,  // k=10  sin( 90°)
     0.987688,  // k=11  sin( 99°)
     0.951057,  // k=12  sin(108°)
     0.891007,  // k=13  sin(117°)
     0.809017,  // k=14  sin(126°)
     0.707107,  // k=15  sin(135°)
     0.587785,  // k=16  sin(144°)
     0.453990,  // k=17  sin(153°)
     0.309017,  // k=18  sin(162°)
     0.156434,  // k=19  sin(171°)
     0.000000,  // k=20  sin(180°)
    -0.156434,  // k=21  sin(189°)
    -0.309017,  // k=22  sin(198°)
    -0.453990,  // k=23  sin(207°)
    -0.587785,  // k=24  sin(216°)
    -0.707107,  // k=25  sin(225°)
    -0.809017,  // k=26  sin(234°)
    -0.891007,  // k=27  sin(243°)
    -0.951057,  // k=28  sin(252°)
    -0.987688,  // k=29  sin(261°)
    -1.000000,  // k=30  sin(270°)
    -0.987688,  // k=31  sin(279°)
    -0.951057,  // k=32  sin(288°)
    -0.891007,  // k=33  sin(297°)
    -0.809017,  // k=34  sin(306°)
    -0.707107,  // k=35  sin(315°)
    -0.587785,  // k=36  sin(324°)
    -0.453990,  // k=37  sin(333°)
    -0.309017,  // k=38  sin(342°)
    -0.156434,  // k=39  sin(351°)
};

// 归一化因子：I/Q 累加值 ÷ (周期数 × N/2) 得到信号幅值 (V)
// 100 个完整周期 × 40 点/周期，同频积分 Σ cos² = N_cycles × N/2 = 2000
#define IQ_NORM_FACTOR  ((double)(IQ_N_CYCLES) * IQ_N / 2.0)  // = 2000.0

// ===================================================================
// IQ 解调函数（真实电压版）
//
// 原理:
//   1. ADC 编码值 → 真实电压: V[i] = data[i] × Vref / 4096
//   2. 减去直流偏置:       V_ac[i] = V[i] - VDC_OFFSET
//   3. 乘参考信号累加:     I = Σ V_ac[i] × cos(2π × i / N)
//                          Q = Σ V_ac[i] × sin(2π × i / N)
//   4. 幅值归一化:         I /= N_cycles × N/2
//                          Q /= N_cycles × N/2
//
// 结果 I/Q 直接代表 100kHz 频率分量的幅值 (V)
//
// 注意:
// - DC 偏置在完整周期积分中自动消除（cos/sin 在一个周期内积分为零）
// - 归一化因子 = 100 × 40/2 = 2000（等于 Σ cos² 在一个周期内的和 × 周期数）
// - 双精度浮点累加，防止精度损失
// ===================================================================
IQResult IQ_Demodulate(const uint16_t data[], int count)
{
    IQResult result;
    double I_acc = 0.0;
    double Q_acc = 0.0;

    // Vref / 4096 预先计算，避免每次循环重复除法
    const double adc_to_volt = ADC_VREF / (double)ADC_MAX;

    for (int i = 0; i < count; i++)
    {
        // 1. ADC 编码 → 真实电压
        double voltage = (double)data[i] * adc_to_volt;

        // 2. 去除直流偏置（保留交流分量）
        double v_ac = voltage - VDC_OFFSET;

        // 3. 与参考信号相乘并累加
        int k = i % IQ_N;  // 查找表索引，自动循环

        I_acc += v_ac * IQ_cos_table[k];
        Q_acc += v_ac * IQ_sin_table[k];
    }

    // 4. 归一化 → 得到信号幅值 (V)
    result.I = I_acc / IQ_NORM_FACTOR;
    result.Q = Q_acc / IQ_NORM_FACTOR;

    return result;
}

// ===================================================================
// 根据 a,b,c,d 计算复阻抗（真实电压版）
//
// 公式:
//   Z_total = R_ref × Vx / VR = R_ref × (a + bj) / (c + dj)
//
//   Re(Z_total) = R_ref × (ac + bd) / (c² + d²)
//   Im(Z_total) = R_ref × (ad - bc) / (c² + d²)   感性为正，容性为负
//
//   Re(Zx) = Re(Z_total) - Rs    （Rs 串联电阻，只影响实部）
//   Im(Zx) = Im(Z_total)
//
// a, b: Vx 的同相/正交幅值 (V)
// c, d: VR 的同相/正交幅值 (V)
//
// 返回复阻抗，实部虚部单位均为 Ω
// ===================================================================
ZResult IQ_CalcImpedance(double a, double b,
                         double c, double d)
{
    // 1. 交叉乘积
    double ac = a * c;
    double bd = b * d;
    double bc = b * c;
    double ad = a * d;

    double num_real = ac + bd;   // 实部分子
    double num_imag = ad - bc;   // 虚部分子 (ad-bc: 感性为正，容性为负)

    // 分母 c² + d²
    double den = c * c + d * d;

    ZResult result;

    if (den == 0.0) {
        result.real = 0.0;
        result.imag = 0.0;
        return result;
    }

    // 2. Re(Z_total) = R_ref × num_real / den
    double re_total = R_REF * num_real / den;

    // 3. Im(Z_total) = R_ref × num_imag / den
    double im_total = R_REF * num_imag / den;

    // 4. Re(Zx) = Re(Z_total) - Rs（去掉串联电阻）
    double re_zx = re_total - R_S;

    // 保留原始符号，不再强制取绝对值
    result.real = re_zx;
    result.imag = im_total;      // Rs 不影响虚部

    return result;
}

// ===================================================================
// 根据复阻抗计算电容值和损耗角正切 D（自适应符号版）
//
// 公式:
//   C(F)  = 1 / (ω × |Im|)         ω = 2π × 100kHz
//   C(pF) = C(F) × 10^12 = CAP_CONSTANT / |Im|
//
//   D      = Re / |Im|             （无量纲）
//   D(ppm) = D × 10^6
//
// 注意:
// - 无论虚部正负，取绝对值计算（虚部为负=容性，为正=感性）
// - Im 单位 Ω，CAP_CONSTANT 已适配
// ===================================================================

// CAP_CONSTANT = 10^12 / ω = 10^12 / (2π × 100000) ≈ 1591549.43
// C(pF) = CAP_CONSTANT / |Im_Ω|
#define CAP_CONSTANT_OHM  1591549.43

CResult IQ_CalcCapacitance(const ZResult *z)
{
    CResult cr;

    // 取虚部绝对值
    double abs_imag = (z->imag >= 0.0) ? z->imag : -z->imag;

    if (abs_imag == 0.0) {
        cr.c_pF  = 0;
        cr.d_ppm = 0;
        return cr;
    }

    // 电容值 C(pF) = CAP_CONSTANT / |Im|
    cr.c_pF = (int32_t)(CAP_CONSTANT_OHM / abs_imag);

    // 损耗角 D(ppm) = |Re| × 10^6 / |Im|
    double abs_real = (z->real >= 0.0) ? z->real : -z->real;
    cr.d_ppm = (int32_t)(abs_real * 1000000.0 / abs_imag);

    return cr;
}

// ===================================================================
// 根据复阻抗计算电感值和品质因数 Q
//
// 公式:
//   L(H)  = Im / ω                ω = 2π × 100kHz
//   L(nH) = L(H) × 10^9 = L_CONSTANT × Im
//   Q     = Im / Re              （无量纲）
//   Q_ppm = Q × 10^6
//
// 注意:
// - Im > 0 为感性，Im ≤ 0 判定为非感性元件
// - Q 值计算保留符号，正 Q 表示感性元件正常
// ===================================================================

// L_CONSTANT = 10^9 / ω = 10^9 / (2π × 100000) ≈ 1591.55
// L(nH) = L_CONSTANT × Im_Ω
#define L_CONSTANT_NH   1591.549

LResult IQ_CalcInductance(const ZResult *z)
{
    LResult lr;

    // 感性元件 Im > 0（容性 Im < 0）
    if (z->imag <= 0.0) {
        lr.l_nH  = 0;
        lr.q_ppm = 0;
        return lr;
    }

    // 电感值 L(nH) = L_CONSTANT × Im
    lr.l_nH = (int32_t)(L_CONSTANT_NH * z->imag);

    // 品质因数 Q = Im / Re（保留符号）
    if (z->real != 0.0) {
        lr.q_ppm = (int32_t)(z->imag * 1000000.0 / z->real);
    } else {
        lr.q_ppm = 0;
    }

    return lr;
}
