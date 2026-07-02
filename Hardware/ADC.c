#include "ADC.h"

uint16_t UR_data[256];
uint16_t UX_data[256];
char message[128]; // 定义一个足够大的缓冲区存放每行字符串

volatile bool UR_complete = false;
volatile bool UX_complete = false;

void DMA_Init(void)
{
    // 设置 DMA 传输参数
    // 首先是 DMA_UR
    DL_DMA_setSrcAddr(DMA, DMA_UR_CHAN_ID, (uint32_t)DL_ADC12_getFIFOAddress(ADC_UR_INST));
    DL_DMA_setDestAddr(DMA, DMA_UR_CHAN_ID, (uint32_t)&UR_data[0]);
    DL_DMA_setTransferSize(DMA, DMA_UR_CHAN_ID, 128);

    DL_DMA_enableChannel(DMA, DMA_UR_CHAN_ID);
    // 其次是 DMA_UX
    DL_DMA_setSrcAddr(DMA, DMA_UX_CHAN_ID, (uint32_t)DL_ADC12_getFIFOAddress(ADC_UX_INST));
    DL_DMA_setDestAddr(DMA, DMA_UX_CHAN_ID, (uint32_t)&UX_data[0]);
    DL_DMA_setTransferSize(DMA, DMA_UX_CHAN_ID, 128);
    
    DL_DMA_enableChannel(DMA, DMA_UX_CHAN_ID);
}

// ADC 中断服务函数
void ADC_UR_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC_UR_INST))
    {
     case DL_ADC12_IIDX_DMA_DONE:
        // 当 DMA 传输完成时，设置标志位，并关闭 DMA 传输
        DL_ADC12_disableConversions(ADC_UR_INST);
        DL_DMA_disableChannel(DMA, DMA_UR_CHAN_ID);
        UR_complete = true;
        break;
    default:
        break;
    }  
}

// ADC 中断服务函数
void ADC_UX_INST_IRQHandler(void)
{
    switch (DL_ADC12_getPendingInterrupt(ADC_UX_INST))
    {
     case DL_ADC12_IIDX_DMA_DONE:
        // 当 DMA 传输完成时，设置标志位，并关闭 DMA 传输
        //DL_Timer_stopCounter(ADC_TIM_INST);
        DL_ADC12_disableConversions(ADC_UX_INST);
        DL_DMA_disableChannel(DMA, DMA_UX_CHAN_ID);
        UX_complete = true;        
        break;
    default:
        break;
    }  
}
