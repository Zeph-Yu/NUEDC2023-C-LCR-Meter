#ifndef __IQ_DEMOD_H
#define __IQ_DEMOD_H

#include <stdint.h>

// ===== 系统参数 =====
// f_s = 4 MHz, f_exc = 100 kHz, N = f_s / f_exc = 40

#define IQ_N           40      // 每周期采样点数
#define IQ_SKIP        40      // 跳过首周期 (ADC 启动瞬态)
#define IQ_SAMPLES     4000    // 有效采样点数 (100 周期)
#define IQ_N_CYCLES    100     // 参与解调的激励周期数
#define IQ_VOFA        400     // VOFA+ 串口发送点数 (前 10 周期)
#define ADC_BITS       12
#define ADC_MAX        4096
#define ADC_VREF       3.3
#define ADC_DC_OFFSET  1857    // ADC 直流偏置 (~1.5V)

#define VDC_OFFSET      ((double)ADC_DC_OFFSET * ADC_VREF / ADC_MAX)

// 分压电阻 (Ω)
#define R_REF          51.0
#define R_S            0.0     // 通过标定修正

// 短路标定偏置 (Ω) — 短路 DUT 端子测得后填入
#define Z_RE_OFFSET    51.41   // 实部残差 [Rs=R_ref=51Ω]
#define Z_IM_OFFSET    0.33    // 虚部残差

// ===== 数据类型 =====

typedef struct {
    double I;   // 同相分量幅值 (V)
    double Q;   // 正交分量幅值 (V)
} IQResult;

typedef struct {
    double real;    // Re(Zx) (Ω), 正=阻性
    double imag;    // Im(Zx) (Ω), 正=感性, 负=容性
} ZResult;

typedef struct {
    int32_t c_pF;   // 电容值 (pF)
    int32_t d_ppm;  // 损耗角 D × 10⁶
} CResult;

typedef struct {
    int32_t l_nH;   // 电感值 (nH)
    int32_t q_ppm;  // 品质因数 Q × 10⁶
} LResult;

// ===== 函数声明 =====

// IQ 解调 — 从 ADC 采样数据中提取 100kHz 分量的同相/正交幅值
IQResult IQ_Demodulate(const uint16_t data[], int count);

// 复阻抗计算 — 根据 Vx 和 Vr 的 IQ 结果计算待测元件阻抗
//   公式: Zx = R_ref × (Ix + jQx) / (Ir + jQr) - Rs
//   Im > 0 为感性, Im < 0 为容性
ZResult IQ_CalcImpedance(double Ix, double Qx, double Ir, double Qr);

// 电容值 + 损耗角 D
CResult IQ_CalcCapacitance(const ZResult *z);

// 电感值 + 品质因数 Q (Im ≤ 0 返回 0)
LResult IQ_CalcInductance(const ZResult *z);

#endif
