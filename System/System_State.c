#include "System_State.h"

IQResult UR_IQ;
IQResult UX_IQ;
ZResult Z_Data;
CResult C_Data;
LResult L_Data;

volatile SystemState state = STATE_IDLE;

void state_idle(void)
{
    OLED_Clear();
    OLED_Display_GB2312_string(0, 0, "LCR Meter Ready");
    OLED_Display_GB2312_string(0, 2, "PA18: Excitation");
    OLED_Display_GB2312_string(0, 4, "PB21: Measure");

    while (state == STATE_IDLE)
    {
        __WFI(); // 等待按键中断改变状态
    }
}

void state_genexcitation(void)
{

}

void state_stable_wait(void)
{
    delay_cycles(CPUCLK_FREQ);
}

void state_init_sampling(void)  
{
    OLED_Clear();
    OLED_Display_GB2312_string(0,0,"Sampling...");
    
    delay_cycles(CPUCLK_FREQ);
    // 初始化 ADC 和 DMA
    DL_ADC12_enableConversions(ADC_UR_INST);
    DL_ADC12_enableConversions(ADC_UX_INST);
    DMA_Init();
    // 启动定时器
    DL_Timer_startCounter(ADC_TIM_INST);
    state = STATE_SAMPLING;
}

void state_sampling(void)
{
    if (UR_complete && UX_complete)
    {
        OLED_Display_GB2312_string(0,2,"Sampling Complete");
        UR_complete = false;
        UX_complete = false;
        delay_cycles(CPUCLK_FREQ);

        //以下是将采样的前十个周期发送到Vofa+上位机，方便观察波形
        // ============================================================
        // VOFA+ FireWater 协议：逗号分隔，换行结尾 (只用 \n)
        // 格式: UX_voltage,UR_voltage\n
        // ============================================================
        OLED_Display_GB2312_string(0, 2, "Sending to VOFA+");

        for (int i = IQ_SKIP; i < IQ_SKIP + IQ_VOFA; i++)   // 跳过第一个周期，只发前10周期到VOFA+
        {
            float ux_voltage = (UX_data[i] * 3.3f) / 4095.0f;
            float ur_voltage = (UR_data[i] * 3.3f) / 4095.0f;

            // FireWater 格式：逗号分隔，\n 结尾
            snprintf(message, sizeof(message), "%.4f,%.4f\n",
                     ux_voltage, ur_voltage);

            for (int j = 0; message[j] != '\0'; j++)
            {
                DL_UART_transmitDataBlocking(UART_ADC_INST, message[j]);
            }
        }
        OLED_Display_GB2312_string(0, 2, "VOFA+ Send Done ");
        
        state = STATE_CALC;
    }

}

void state_calc(void)
{
    OLED_Display_GB2312_string(0,4,"Calculating...");
    UX_IQ = IQ_Demodulate(&UX_data[IQ_SKIP], IQ_SAMPLES); //解调待测元件ADC数据(跳过第1周期)
    UR_IQ = IQ_Demodulate(&UR_data[IQ_SKIP], IQ_SAMPLES); //解调分压电阻ADC数据(跳过第1周期)
    Z_Data = IQ_CalcImpedance(UX_IQ.I, UX_IQ.Q, UR_IQ.I, UR_IQ.Q); //计算待测元件实部虚部
    // 短路标定：减去系统残差
    Z_Data.real -= Z_RE_OFFSET;
    Z_Data.imag -= Z_IM_OFFSET;
    // 根据 Im 符号自动判断容性/感性
    C_Data = IQ_CalcCapacitance(&Z_Data);
    L_Data = IQ_CalcInductance(&Z_Data);
    state = STATE_SHOWRESULT;
}   

void state_showresult(void)
{
    OLED_Clear();

    if (L_Data.l_nH > 0)  // Im > 0 → 感性
    {
        // 电感测量范围：10µH~100µH
        float l = L_Data.l_nH / 1000.0f;
        snprintf(message, sizeof(message), "L: %.2f uH", l);
        OLED_Display_GB2312_string(0, 0, message);

        // 品质因数 Q: 1~200
        float q = L_Data.q_ppm / 1000000.0f;
        snprintf(message, sizeof(message), "Q: %.2f", q);
        OLED_Display_GB2312_string(0, 2, message);
    }
    else if (C_Data.c_pF > 0)  // Im < 0 → 容性
    {
        // 电容测量范围：1nF~100nF
        float c = C_Data.c_pF / 1000.0f;
        snprintf(message, sizeof(message), "C: %.2f nF", c);
        OLED_Display_GB2312_string(0, 0, message);

        // 损耗角正切 D: 0.005~1
        float d = C_Data.d_ppm / 1000000.0f;
        snprintf(message, sizeof(message), "D: %.4f", d);
        OLED_Display_GB2312_string(0, 2, message);
    }
    else  // Im ≈ 0 → 无 DUT 或短路
    {
        OLED_Display_GB2312_string(0, 0, "No DUT / Short");
    }

    // DEBUG: Z 实部虚部（始终显示）
    int32_t r = (int32_t)Z_Data.real;
    int32_t i = (int32_t)Z_Data.imag;
    snprintf(message, sizeof(message), "Re:%ld Im:%ld", (long)r, (long)i);
    OLED_Display_GB2312_string(0, 4, message);

    state = STATE_STABLE_WAIT;
}

// ===== 按键中断 =====
//已弃用，改为定时器扫描
/*
void GROUP1_IRQHandler(void)
{
    switch(DL_Interrupt_getPendingGroup(DL_INTERRUPT_GROUP_1))
    {
        case DL_INTERRUPT_GROUP1_IIDX_GPIOB:
            if (!DL_GPIO_readPins(GPIOB, KEY_PB_21_PIN))
            {
                delay_cycles(CPUCLK_FREQ / 4);
                if (!DL_GPIO_readPins(GPIOB, KEY_PB_21_PIN))
                {
                    state = STATE_INIT_SAMPLING;
                }
            }
            break;
        case DL_INTERRUPT_GROUP1_IIDX_GPIOA:
            if (DL_GPIO_readPins(GPIOA, KEY_PA_18_PIN))
            {
                 delay_cycles(CPUCLK_FREQ / 4);
                 if (DL_GPIO_readPins(GPIOA, KEY_PA_18_PIN))
                 {
                    state = STATE_GENEXCITATION;
                 }
             }
             break;
         default:
             break;
    }
}
*/
