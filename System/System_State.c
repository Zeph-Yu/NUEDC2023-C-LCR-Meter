#include "System_State.h"

volatile SystemState state = STATE_INIT_SAMPLING;

void state_init_sampling(void)
{
    OLED_Clear();
    OLED_Display_GB2312_string(0,0,"PB21 is Pressed");

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
        // 这里可以添加数据处理的代码
        for(int i = 0; i < 128; i++)
        {
            DL_UART_transmitDataBlocking(UART_ADC_INST, UX_data[i]);
            DL_UART_transmitDataBlocking(UART_ADC_INST, UR_data[i]); // 发送一个示例数据，表示采样完成
        }
        state = STATE_CALC;
    }

}

void state_calc(void)
{
    OLED_Display_GB2312_string(4,4,"Calculating...");
    
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

