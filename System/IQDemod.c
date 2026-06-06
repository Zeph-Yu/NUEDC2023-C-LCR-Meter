#include "IQDemod.h"

// ===================================================================
// Q15 格式 sin/cos 查找表  (N = 40, 对应 100kHz 激励 @ 4MHz 采样率)
//
// 表值 = round( sin/cos(2π × k / 40) × 32767 )
// k = 0, 1, 2, ..., 39  对应角度 0°, 9°, 18°, ..., 351°
// ===================================================================

const int16_t IQ_cos_table[IQ_N] = {
     32767,  // k= 0  cos(  0°)
     32364,  // k= 1  cos(  9°)
     31163,  // k= 2  cos( 18°)
     29196,  // k= 3  cos( 27°)
     26509,  // k= 4  cos( 36°)
     23170,  // k= 5  cos( 45°)
     19260,  // k= 6  cos( 54°)
     14876,  // k= 7  cos( 63°)
     10126,  // k= 8  cos( 72°)
      5126,  // k= 9  cos( 81°)
         0,  // k=10  cos( 90°)
     -5126,  // k=11  cos( 99°)
    -10126,  // k=12  cos(108°)
    -14876,  // k=13  cos(117°)
    -19260,  // k=14  cos(126°)
    -23170,  // k=15  cos(135°)
    -26509,  // k=16  cos(144°)
    -29196,  // k=17  cos(153°)
    -31163,  // k=18  cos(162°)
    -32364,  // k=19  cos(171°)
    -32767,  // k=20  cos(180°)
    -32364,  // k=21  cos(189°)
    -31163,  // k=22  cos(198°)
    -29196,  // k=23  cos(207°)
    -26509,  // k=24  cos(216°)
    -23170,  // k=25  cos(225°)
    -19260,  // k=26  cos(234°)
    -14876,  // k=27  cos(243°)
    -10126,  // k=28  cos(252°)
     -5126,  // k=29  cos(261°)
         0,  // k=30  cos(270°)
      5126,  // k=31  cos(279°)
     10126,  // k=32  cos(288°)
     14876,  // k=33  cos(297°)
     19260,  // k=34  cos(306°)
     23170,  // k=35  cos(315°)
     26509,  // k=36  cos(324°)
     29196,  // k=37  cos(333°)
     31163,  // k=38  cos(342°)
     32364,  // k=39  cos(351°)
};

const int16_t IQ_sin_table[IQ_N] = {
         0,  // k= 0  sin(  0°)
      5126,  // k= 1  sin(  9°)
     10126,  // k= 2  sin( 18°)
     14876,  // k= 3  sin( 27°)
     19260,  // k= 4  sin( 36°)
     23170,  // k= 5  sin( 45°)
     26509,  // k= 6  sin( 54°)
     29196,  // k= 7  sin( 63°)
     31163,  // k= 8  sin( 72°)
     32364,  // k= 9  sin( 81°)
     32767,  // k=10  sin( 90°)
     32364,  // k=11  sin( 99°)
     31163,  // k=12  sin(108°)
     29196,  // k=13  sin(117°)
     26509,  // k=14  sin(126°)
     23170,  // k=15  sin(135°)
     19260,  // k=16  sin(144°)
     14876,  // k=17  sin(153°)
     10126,  // k=18  sin(162°)
      5126,  // k=19  sin(171°)
         0,  // k=20  sin(180°)
     -5126,  // k=21  sin(189°)
    -10126,  // k=22  sin(198°)
    -14876,  // k=23  sin(207°)
    -19260,  // k=24  sin(216°)
    -23170,  // k=25  sin(225°)
    -26509,  // k=26  sin(234°)
    -29196,  // k=27  sin(243°)
    -31163,  // k=28  sin(252°)
    -32364,  // k=29  sin(261°)
    -32767,  // k=30  sin(270°)
    -32364,  // k=31  sin(279°)
    -31163,  // k=32  sin(288°)
    -29196,  // k=33  sin(297°)
    -26509,  // k=34  sin(306°)
    -23170,  // k=35  sin(315°)
    -19260,  // k=36  sin(324°)
    -14876,  // k=37  sin(333°)
    -10126,  // k=38  sin(342°)
     -5126,  // k=39  sin(351°)
};

// ===================================================================
// IQ 解调函数
//
// 原理: 采样信号 x[n] 分别与同频的 cos/sin 参考信号相乘并累加
//
//        N-1
//   I =  Σ  data[i] × cos_table[i % N]   →  信号实部（a 或 c）
//        i=0
//
//        N-1
//   Q =  Σ  data[i] × sin_table[i % N]   →  信号虚部（b 或 d）
//        i=0
//
// 注意：
// - data[i] 是 12-bit ADC 原始值 (0~4095)，含 DC 偏置
// - 累加横跨刚好 3 个完整激励周期 (120 = 3×40)
// - DC 偏置在完整周期积分中自动消除，无需手动去 DC
// - 累加器用 int64_t 防止溢出
// - 累加和本身就是滤波结果，不需要除以样本数（比值时会抵消）
// ===================================================================
IQResult IQ_Demodulate(const uint16_t data[], int count)
{
    IQResult result;
    int64_t I_acc = 0;
    int64_t Q_acc = 0;

    for (int i = 0; i < count; i++)
    {
        int k = i % IQ_N;  // 查找表索引，自动循环
        I_acc += (int32_t)data[i] * IQ_cos_table[k];
        Q_acc += (int32_t)data[i] * IQ_sin_table[k];
    }

    result.I = I_acc;
    result.Q = Q_acc;

    return result;
}

// ===================================================================
// 根据 a,b,c,d 计算复阻抗
//
// 公式:
//   Z = R_ref × (a+bj) / (c+dj) = R_ref × (a+bj)(c-dj) / (c²+d²)
//   Re(Z) = R_ref × (ac + bd) / (c² + d²)
//   Im(Z) = R_ref × (bc - ad) / (c² + d²)
//
// 为防止中间乘积溢出（a~d 可达 ~1.6e10），统一右移 SCALE 位后再乘。
// 缩放因子在比值 (ac+bd)/(c²+d²) 中完全抵消，不影响结果。
// ===================================================================
#define Z_SCALE 16   // a~d 右移 16 bit，乘积 < 2.5e10，安全

ZResult IQ_CalcImpedance(int64_t a, int64_t b,
                         int64_t c, int64_t d)
{
    // 1. 统一缩放，防止交叉乘积溢出
    a >>= Z_SCALE;
    b >>= Z_SCALE;
    c >>= Z_SCALE;
    d >>= Z_SCALE;

    // 2. 计算交叉乘积（缩放后安全）
    int64_t ac = a * c;
    int64_t bd = b * d;
    int64_t bc = b * c;
    int64_t ad = a * d;

    int64_t  num_real = ac + bd;         // 实部分子
    int64_t  num_imag = bc - ad;         // 虚部分子
    uint64_t den      = (uint64_t)c * (uint64_t)c
                      + (uint64_t)d * (uint64_t)d;   // 分母 c²+d²

    ZResult result;

    if (den == 0) {
        result.real = 0;
        result.imag = 0;
        return result;
    }

    // 3. Re(Z) = R_ref × num_real / den - Rs（单位均为 mΩ）
    {
        uint64_t un = (num_real >= 0) ? (uint64_t)num_real
                                      : (uint64_t)(-num_real);
        uint64_t val = (uint64_t)R_ref * un / den;
        int32_t  re  = (num_real >= 0) ? (int32_t)val : -(int32_t)val;

        re -= (int32_t)Rs;
        result.real = re;
    }

    // 4. Im(Z) = R_ref × num_imag / den
    {
        uint64_t un = (num_imag >= 0) ? (uint64_t)num_imag
                                      : (uint64_t)(-num_imag);
        uint64_t val = (uint64_t)R_ref * un / den;
        result.imag = (num_imag >= 0) ? (int32_t)val : -(int32_t)val;
    }

    return result;
}

// ===================================================================
// 根据复阻抗计算电容值和损耗角正切 D
//
// C = -1 / (ω × Im(Z))
//   ω = 2π × 100kHz ≈ 628318.5307 rad/s
//   Im(Z) 单位 mΩ，需转 Ω: Im_Ω = Im_mΩ / 1000
//   C(pF) = 10^12 / (ω × Im_Ω) = 10^15 / (ω × Im_mΩ)
//
//   预计算常数: 10^15 / ω = 10^10 / 2π ≈ 1591549431
//
// D = Re(Z) / |Im(Z)|，结果放大 10^6 倍输出
// ===================================================================
// C_pF = 10^15 / ω ≈ 10^10 / 2π
#define CAP_CONSTANT  1591549431LL

CResult IQ_CalcCapacitance(const ZResult *z)
{
    CResult cr;

    // 仅容性（Im < 0）时有效
    if (z->imag >= 0) {
        cr.c_pF  = 0;
        cr.d_ppm = 0;
        return cr;
    }

    // 电容值 C(pF) = CAP_CONSTANT / |Im|
    int32_t abs_imag = -z->imag;   // Im 为负，取绝对值
    if (abs_imag == 0) {
        cr.c_pF  = 0;
        cr.d_ppm = 0;
        return cr;
    }

    cr.c_pF = (int32_t)(CAP_CONSTANT / abs_imag);

    // 损耗角 D(ppm) = Re × 10^6 / |Im|
    uint32_t abs_real = (z->real >= 0) ? (uint32_t)z->real
                                       : (uint32_t)(-z->real);
    cr.d_ppm = (int32_t)((uint64_t)abs_real * 1000000ULL / (uint32_t)abs_imag);

    return cr;
}
