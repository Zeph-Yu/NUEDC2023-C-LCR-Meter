#include "ti_msp_dl_config.h"
#include <stdio.h>
#include "System_State.h"
#include "oled.h"
#include "ADC.h"
#include "Key.h"

int main(void)
{
    // 初始化系统配置
    SYSCFG_DL_init();
    // 初始化 OLED 屏幕
    OLED_Init();
    OLED_Clear();
    DDS_Init();
    // 使能模块中断
    NVIC_EnableIRQ(ADC_UR_INST_INT_IRQN);
    NVIC_EnableIRQ(ADC_UX_INST_INT_IRQN);
    NVIC_EnableIRQ(KeyScan_TIMER_INST_INT_IRQN);
    
    //开启按键扫描
    DL_Timer_startCounter(KeyScan_TIMER_INST);

    while (1)
    {
        switch(state)
        {
        case STATE_IDLE : state_idle(); break;
        case STATE_GENEXCITATION : state_genexcitation(); break;
        case STATE_STABLE_WAIT : state_stable_wait(); break;
        case STATE_INIT_SAMPLING : state_init_sampling(); break;
        case STATE_SAMPLING : state_sampling(); break;
        case STATE_CALC : state_calc(); break;
        case STATE_SHOWRESULT : state_showresult();break;
        default:break;
        }
    }
}

//按键扫描定时器
void KeyScan_TIMER_INST_IRQHandler(void)
{
    if(DL_Timer_getPendingInterrupt(KeyScan_TIMER_INST) == DL_TIMER_IIDX_ZERO)
    {
        Key_Tick();
    }
}
