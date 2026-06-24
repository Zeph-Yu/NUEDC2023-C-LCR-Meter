#include "System_State.h"

IQResult UR_IQ;
IQResult UX_IQ;
ZResult Z_Data;
CResult C_Data;

volatile SystemState state = STATE_INIT_SAMPLING;

void state_stable_wait(void)
{
    delay_cycles(CPUCLK_FREQ);
}

void state_init_sampling(void)  
{
    OLED_Clear();
    OLED_Display_GB2312_string(0,0,"PB21 is Pressed");
    
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
        OLED_Display_GB2312_string(2,2,"Sampling Complete");
        UR_complete = false;
        UX_complete = false;
        delay_cycles(CPUCLK_FREQ);
        
        
        for (int i = 0; i < 256; i++)
        {
            // 1. 严格按照参考公式，分别计算 UX 和 UR 的真实电压值
            // 注意：用 4095.0f 确保进行浮点数除法，防止精度丢失
            float ux_voltage = (UX_data[i] * 3.3f) / 4095.0f;
            float ur_voltage = (UR_data[i] * 3.3f) / 4095.0f;
            
            // 2. 依照参考格式，将数据打包进 message 缓冲区
            // 这里加上了索引 [i]，并在同一行输出，方便你在串口助手里对比查看
            snprintf(message, sizeof(message), "i:%d; UX_raw:%d; UX_volt:%.4fV; UR_raw:%d; UR_volt:%.4fV;\n", 
             i, UX_data[i], ux_voltage, UR_data[i], ur_voltage);
            
            // 3. 严格按照参考的 for 循环方式，逐个字符阻塞发送
            for (int j = 0; message[j] != '\0'; j++)
            {
                DL_UART_transmitDataBlocking(UART_ADC_INST, message[j]);
            }
        }   
        
        
        state = STATE_CALC;
    }

}

void state_calc(void)
{
    OLED_Display_GB2312_string(0,4,"Calculating...");
    delay_cycles(CPUCLK_FREQ);
    UX_IQ = IQ_Demodulate(UX_data, IQ_SAMPLES); //解调待测元件ADC数据
    UR_IQ = IQ_Demodulate(UR_data, IQ_SAMPLES); //解调分压电阻ADC数据
    Z_Data = IQ_CalcImpedance(UX_IQ.I, UX_IQ.Q, UR_IQ.I, UR_IQ.Q); //计算待测元件实部虚部
    C_Data = IQ_CalcCapacitance(&Z_Data); //计算电容容值和损耗角正切值
    state = STATE_SHOWRESULT;
}   

void state_showresult(void)
{
    OLED_Clear();

    if (C_Data.c_pF == 0)
    {
        // 非容性元件或测量无效
        OLED_Display_GB2312_string(0, 0, "Not Capacitive");
        OLED_Display_GB2312_string(0, 2, "or Out of Range");
    }
    else
    {
        // 电容值：根据量级自动切换单位
        if (C_Data.c_pF >= 1000000)
        {
            float c = C_Data.c_pF / 1000000.0f;
            snprintf(message, sizeof(message), "C: %.4f uF", c);
        }
        else if (C_Data.c_pF >= 1000)
        {
            float c = C_Data.c_pF / 1000.0f;
            snprintf(message, sizeof(message), "C: %.4f nF", c);
        }
        else
        {
            snprintf(message, sizeof(message), "C: %ld pF", (long)C_Data.c_pF);
        }
        OLED_Display_GB2312_string(0, 0, message);

        // 损耗角正切 D
        float d = C_Data.d_ppm / 1000000.0f;
        snprintf(message, sizeof(message), "D: %.4f", d);
        OLED_Display_GB2312_string(0, 2, message);
    }
    state = STATE_STABLE_WAIT;
}

// ===== 按键中断 =====
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

